#include "Weapons/WDWeaponComponent.h"
#include "Weapons/WDWeaponData.h"
#include "Weapons/WDWeaponMath.h"
#include "Weapons/WDTargeting.h"
#include "Weapons/WDProjectilePoolSubsystem.h"
#include "Combat/WDHealthComponent.h"
#include "Core/WDDebugSubsystem.h"

UWDWeaponComponent::UWDWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWDWeaponComponent::SetWeapon(UWDWeaponData* InWeapon)
{
	Weapon = InWeapon;
	Cooldown = 0.f; // switching is free; the dark laser's long interval punishes spamming it, not switching
}

void UWDWeaponComponent::StartFire()
{
	bFiring = true;
}

void UWDWeaponComponent::StopFire()
{
	bFiring = false;
}

void UWDWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Cooldown = FMath::Max(0.f, Cooldown - DeltaTime);
	TickStrikes(DeltaTime);

	if (bFiring && Weapon && Cooldown <= 0.f)
	{
		FireOnce();
		Cooldown = Weapon->FireInterval;
		OnFired.Broadcast(Weapon);
	}
}

void UWDWeaponComponent::FireOnce()
{
	const FVector Muzzle = GetMuzzleLocation();
	const FVector Direction = GetAimDirection();

	switch (Weapon->FireMode)
	{
	case EWDFireMode::Projectile:     FireProjectile(Muzzle, Direction); break;
	case EWDFireMode::ContinuousCone: FireCone(Muzzle, Direction);       break;
	case EWDFireMode::BouncingRay:    FireBouncingRay(Muzzle, Direction); break;
	case EWDFireMode::TargetedStrike: FireStrike();                       break;
	}
}

void UWDWeaponComponent::FireProjectile(const FVector& Muzzle, const FVector& Direction)
{
	if (UWDProjectilePoolSubsystem* Pool = GetWorld()->GetSubsystem<UWDProjectilePoolSubsystem>())
	{
		Pool->FireShot(Weapon->MakeShotParams(GetOwner()), Muzzle, Direction);
	}
}

void UWDWeaponComponent::FireCone(const FVector& Muzzle, const FVector& Direction)
{
	UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>();

	// Cone edges, so the player reads the area even in debug.
	if (Debug)
	{
		const FVector Left = Direction.RotateAngleAxis(-Weapon->ConeHalfAngleDeg, FVector::UpVector);
		const FVector Right = Direction.RotateAngleAxis(Weapon->ConeHalfAngleDeg, FVector::UpVector);
		Debug->DrawLine(Muzzle, Muzzle + Left * Weapon->Range, Weapon->Element, 0.1f, 2.f);
		Debug->DrawLine(Muzzle, Muzzle + Right * Weapon->Range, Weapon->Element, 0.1f, 2.f);
	}

	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(Weapon->ConeHalfAngleDeg));
	for (AActor* Target : WDTargeting::FindAllDamageable(GetWorld(), Muzzle, Weapon->Range, GetOwner()))
	{
		const FVector ToTarget = (Target->GetActorLocation() - Muzzle).GetSafeNormal2D();
		if (FVector::DotProduct(Direction, ToTarget) >= CosHalfAngle)
		{
			FWDDamageEvent Damage;
			Damage.Amount = Weapon->Damage;
			Damage.Element = Weapon->Element;
			Damage.Instigator = GetOwner();
			Damage.ImpactPoint = Target->GetActorLocation();
			UWDHealthComponent::DamageActor(Target, Damage);
			if (Debug)
			{
				Debug->DrawImpact(Target->GetActorLocation(), Weapon->Element, 0.15f);
			}
		}
	}
}

void UWDWeaponComponent::FireBouncingRay(const FVector& Muzzle, const FVector& Direction)
{
	UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>();

	TSet<TObjectPtr<AActor>> AlreadyHit;
	FVector SegmentStart = Muzzle;
	FVector SegmentDirection = Direction;
	float RemainingRange = Weapon->Range;

	for (int32 Segment = 0; Segment <= Weapon->Bounce && RemainingRange > 1.f; ++Segment)
	{
		// Where does this segment stop? First blocking wall, or end of range.
		FHitResult WallHit;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WDRay), false, GetOwner());
		const FVector DesiredEnd = SegmentStart + SegmentDirection * RemainingRange;
		const bool bHitWall = GetWorld()->LineTraceSingleByChannel(WallHit, SegmentStart, DesiredEnd, ECC_WorldStatic, QueryParams);
		const FVector SegmentEnd = bHitWall ? WallHit.ImpactPoint : DesiredEnd;

		// The ray pierces EVERY damageable on the segment (low damage per target, multi-hit by design).
		TArray<FHitResult> Hits;
		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
		GetWorld()->SweepMultiByObjectType(Hits, SegmentStart, SegmentEnd, FQuat::Identity, ObjectParams,
			FCollisionShape::MakeSphere(Weapon->RayThickness), QueryParams);

		for (const FHitResult& Hit : Hits)
		{
			AActor* Victim = Hit.GetActor();
			if (!Victim || AlreadyHit.Contains(Victim) || !WDTargeting::GetLiveHealth(Victim))
			{
				continue;
			}
			AlreadyHit.Add(Victim);
			FWDDamageEvent Damage;
			Damage.Amount = Weapon->Damage;
			Damage.Element = Weapon->Element;
			Damage.Instigator = GetOwner();
			Damage.ImpactPoint = Hit.ImpactPoint;
			UWDHealthComponent::DamageActor(Victim, Damage);
			if (Debug)
			{
				Debug->DrawImpact(Hit.ImpactPoint, Weapon->Element, 0.4f);
			}
		}

		if (Debug)
		{
			Debug->DrawLine(SegmentStart, SegmentEnd, Weapon->Element, 0.5f, 6.f);
		}

		if (!bHitWall)
		{
			break;
		}
		RemainingRange -= FVector::Dist(SegmentStart, SegmentEnd);
		SegmentDirection = WDWeaponMath::Reflect(SegmentDirection, WallHit.ImpactNormal);
		SegmentStart = WallHit.ImpactPoint + WallHit.ImpactNormal * 1.f;
	}
}

void UWDWeaponComponent::FireStrike()
{
	const FVector Origin = GetOwner()->GetActorLocation();
	FVector Target = AimTarget;
	if (Target.IsNearlyZero())
	{
		Target = Origin + GetAimDirection() * Weapon->StrikeMaxDistance * 0.7f;
	}
	// Clamp to max reach.
	const FVector Offset = (Target - Origin).GetClampedToMaxSize2D(Weapon->StrikeMaxDistance);
	FPendingStrike Strike;
	Strike.Location = Origin + Offset;
	Strike.Location.Z = Origin.Z;
	Strike.TimeRemaining = Weapon->StrikeDelay;
	Strike.Radius = Weapon->StrikeRadius;
	Strike.Damage = Weapon->Damage;
	Strike.Element = Weapon->Element;
	PendingStrikes.Add(Strike);
}

void UWDWeaponComponent::TickStrikes(float DeltaTime)
{
	UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>();

	for (int32 i = PendingStrikes.Num() - 1; i >= 0; --i)
	{
		FPendingStrike& Strike = PendingStrikes[i];
		Strike.TimeRemaining -= DeltaTime;

		if (Strike.TimeRemaining > 0.f)
		{
			if (Debug)
			{
				// Telegraph shrinking toward impact.
				Debug->DrawGroundCircle(Strike.Location, Strike.Radius, Strike.Element, 0.f);
			}
			continue;
		}

		for (AActor* Victim : WDTargeting::FindAllDamageable(GetWorld(), Strike.Location, Strike.Radius, GetOwner()))
		{
			FWDDamageEvent Damage;
			Damage.Amount = Strike.Damage;
			Damage.Element = Strike.Element;
			Damage.Instigator = GetOwner();
			Damage.ImpactPoint = Victim->GetActorLocation();
			UWDHealthComponent::DamageActor(Victim, Damage);
		}
		if (Debug)
		{
			Debug->DrawGroundCircle(Strike.Location, Strike.Radius, Strike.Element, 0.5f);
			Debug->DrawLine(Strike.Location + FVector(0, 0, 1200.f), Strike.Location, Strike.Element, 0.5f, 10.f);
		}
		PendingStrikes.RemoveAt(i);
	}
}

FVector UWDWeaponComponent::GetMuzzleLocation() const
{
	// Socket convention §11: no weapon mesh yet -> data-driven fallback offset.
	const FVector Offset = Weapon ? Weapon->MuzzleOffset : FVector(60.f, 0.f, 50.f);
	return GetOwner()->GetActorLocation() + GetOwner()->GetActorRotation().RotateVector(Offset);
}

FVector UWDWeaponComponent::GetAimDirection() const
{
	return GetOwner()->GetActorForwardVector().GetSafeNormal2D();
}
