#include "Weapons/WDProjectile.h"
#include "Weapons/WDProjectilePoolSubsystem.h"
#include "Weapons/WDWeaponMath.h"
#include "Weapons/WDTargeting.h"
#include "Combat/WDHealthComponent.h"
#include "Core/WDDebugSubsystem.h"

AWDProjectile::AWDProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(false);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AWDProjectile::Launch(const FWDShotParams& InParams, const FVector& Start, const FVector& Direction)
{
	Params = InParams;
	SetActorLocation(Start);
	Velocity = Direction.GetSafeNormal2D() * Params.Speed;
	TraveledDistance = 0.f;
	PierceLeft = Params.Pierce;
	BounceLeft = Params.Bounce;
	HitActors.Reset();
	bInFlight = true;
	SetActorTickEnabled(true);
}

void AWDProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bInFlight)
	{
		return;
	}

	if (Params.bHoming)
	{
		if (AActor* Target = WDTargeting::FindNearestDamageable(GetWorld(), GetActorLocation(), 1500.f, HitActors, Params.Instigator))
		{
			const FVector Desired = Target->GetActorLocation() - GetActorLocation();
			Velocity = WDWeaponMath::ComputeHomingDirection(Velocity, Desired, Params.HomingTurnSpeed * DeltaSeconds) * Params.Speed;
		}
	}

	const FVector Start = GetActorLocation();
	const float StepLength = Params.Speed * DeltaSeconds;
	const FVector End = Start + Velocity.GetSafeNormal() * StepLength;

	// One swept sphere per tick; hits are processed in order along the path.
	TArray<FHitResult> Hits;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WDProjectile), false, this);
	if (Params.Instigator)
	{
		QueryParams.AddIgnoredActor(Params.Instigator);
	}
	GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjectParams,
		FCollisionShape::MakeSphere(Params.Radius), QueryParams);

	FVector NewLocation = End;
	bool bConsumed = false;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && WDTargeting::GetLiveHealth(HitActor))
		{
			if (HitActors.Contains(HitActor))
			{
				continue;
			}
			ApplyHit(HitActor, Hit.ImpactPoint);
			if (PierceLeft-- > 0)
			{
				continue; // through, next victim on the path
			}
			Consume(Hit.ImpactPoint, /*bSpawnFragments=*/true);
			bConsumed = true;
			break;
		}

		if (Hit.bBlockingHit)
		{
			// World geometry: bounce or die.
			if (BounceLeft-- > 0)
			{
				Velocity = WDWeaponMath::Reflect(Velocity.GetSafeNormal(), Hit.ImpactNormal) * Params.Speed;
				NewLocation = Hit.ImpactPoint + Hit.ImpactNormal * Params.Radius;
				if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
				{
					Debug->DrawImpact(Hit.ImpactPoint, Params.Element, 0.4f);
				}
			}
			else
			{
				Consume(Hit.ImpactPoint, /*bSpawnFragments=*/true);
				bConsumed = true;
			}
			break;
		}
	}

	if (bConsumed)
	{
		return;
	}

	SetActorLocation(NewLocation);
	TraveledDistance += StepLength;

	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		Debug->DrawLine(Start, NewLocation, Params.Element, 0.08f, FMath::Max(2.f, Params.Radius * 0.15f));
	}

	if (TraveledDistance >= Params.Range)
	{
		Consume(NewLocation, /*bSpawnFragments=*/false); // fizzles out, no fragments
	}
}

void AWDProjectile::ApplyHit(AActor* Victim, const FVector& ImpactPoint)
{
	HitActors.Add(Victim);

	FWDDamageEvent Damage;
	Damage.Amount = Params.Damage;
	Damage.Element = Params.Element;
	Damage.Instigator = Params.Instigator;
	Damage.ImpactPoint = ImpactPoint;
	UWDHealthComponent::DamageActor(Victim, Damage);

	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		Debug->DrawImpact(ImpactPoint, Params.Element, 0.4f);
	}

	if (Params.ChainCount > 0)
	{
		ApplyChain(Victim);
	}
}

void AWDProjectile::ApplyChain(AActor* FirstVictim)
{
	AActor* Source = FirstVictim;
	for (int32 i = 0; i < Params.ChainCount; ++i)
	{
		AActor* Next = WDTargeting::FindNearestDamageable(GetWorld(), Source->GetActorLocation(), Params.ChainRange,
			HitActors, Params.Instigator);
		if (!Next)
		{
			break;
		}
		HitActors.Add(Next);

		FWDDamageEvent Damage;
		Damage.Amount = Params.Damage * Params.ChainDamageFraction;
		Damage.Element = Params.Element;
		Damage.Instigator = Params.Instigator;
		Damage.ImpactPoint = Next->GetActorLocation();
		UWDHealthComponent::DamageActor(Next, Damage);

		if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
		{
			Debug->DrawChain(Source->GetActorLocation(), Next->GetActorLocation(), Params.Element, 0.4f);
		}
		Source = Next;
	}
}

void AWDProjectile::Consume(const FVector& Location, bool bSpawnFragments)
{
	bInFlight = false;
	SetActorTickEnabled(false);

	UWDProjectilePoolSubsystem* Pool = GetWorld()->GetSubsystem<UWDProjectilePoolSubsystem>();

	if (bSpawnFragments && Params.FragmentCount > 0 && Pool)
	{
		FWDShotParams Fragment = Params;
		Fragment.Damage = Params.Damage * Params.FragmentDamageFraction;
		Fragment.Range = 350.f;
		Fragment.Radius = FMath::Max(8.f, Params.Radius * 0.5f);
		Fragment.Pierce = 0;
		Fragment.Bounce = 0;
		Fragment.FragmentCount = 0; // fragments never re-fragment
		Fragment.ChainCount = 0;

		for (const FVector& Direction : WDWeaponMath::ComputeRingDirections(Params.FragmentCount))
		{
			Pool->FireShot(Fragment, Location, Direction);
		}
	}

	if (Pool)
	{
		Pool->Release(this);
	}
}

