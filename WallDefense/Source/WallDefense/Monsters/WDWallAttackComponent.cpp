#include "Monsters/WDWallAttackComponent.h"
#include "Monsters/WDMonster.h"
#include "Combat/WDHealthComponent.h"
#include "Weapons/WDProjectilePoolSubsystem.h"
#include "Core/WDDebugSubsystem.h"

UWDWallAttackComponent::UWDWallAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWDWallAttackComponent::Configure(AActor* InTarget, float InDamage, float InRange, float InInterval, bool bInRanged, EWDElement InElement)
{
	Target = InTarget;
	Damage = InDamage;
	Range = InRange;
	Interval = InInterval;
	bRanged = bInRanged;
	Element = InElement;
}

void UWDWallAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Cooldown = FMath::Max(0.f, Cooldown - DeltaTime);
	bAttacking = false;

	if (Damage <= 0.f || !Target.IsValid())
	{
		return;
	}

	// Distance to the target's SURFACE, not its center: a wide wall is hit at its façade.
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	ImpactPoint = Target->GetActorLocation();
	float Distance = FVector::Dist2D(OwnerLocation, ImpactPoint);
	FVector ClosestPoint;
	if (Target->ActorGetDistanceToCollision(OwnerLocation, ECC_WorldDynamic, ClosestPoint) >= 0.f)
	{
		ImpactPoint = ClosestPoint;
		Distance = FVector::Dist2D(OwnerLocation, ClosestPoint);
	}
	if (Distance > Range)
	{
		return;
	}

	bAttacking = true;
	if (Cooldown <= 0.f)
	{
		DoAttack();
		Cooldown = Interval;
		OnAttack.Broadcast();
	}
}

void UWDWallAttackComponent::DoAttack()
{
	if (bRanged)
	{
		// Slow, readable projectile toward the wall. Never hits fellow monsters.
		if (UWDProjectilePoolSubsystem* Pool = GetWorld()->GetSubsystem<UWDProjectilePoolSubsystem>())
		{
			FWDShotParams Shot;
			Shot.Element = Element;
			Shot.Damage = Damage;
			Shot.Speed = 800.f;
			Shot.Range = Range + 300.f;
			Shot.Radius = 25.f;
			Shot.Instigator = GetOwner();
			Shot.IgnoredClass = AWDMonster::StaticClass();
			const FVector Muzzle = GetOwner()->GetActorLocation() + FVector(0, 0, 40.f);
			Pool->FireShot(Shot, Muzzle, ImpactPoint - Muzzle);
		}
	}
	else
	{
		FWDDamageEvent DamageEvent;
		DamageEvent.Amount = Damage;
		DamageEvent.Element = Element;
		DamageEvent.Instigator = GetOwner();
		DamageEvent.ImpactPoint = ImpactPoint;
		UWDHealthComponent::DamageActor(Target.Get(), DamageEvent);

		if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
		{
			Debug->DrawChain(GetOwner()->GetActorLocation(), ImpactPoint, Element, 0.25f);
		}
	}
}
