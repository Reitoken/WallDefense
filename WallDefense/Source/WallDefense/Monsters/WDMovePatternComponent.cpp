#include "Monsters/WDMovePatternComponent.h"
#include "Monsters/WDPatternMath.h"

UWDMovePatternComponent::UWDMovePatternComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWDMovePatternComponent::Configure(EWDMovePattern InPattern, float InSpeed, AActor* InTarget, float InStopDistance)
{
	Pattern = InPattern;
	Speed = InSpeed;
	Target = InTarget;
	StopDistance = InStopDistance;

	SpawnLocation = GetOwner()->GetActorLocation();
	GroundZ = SpawnLocation.Z;
	Time = 0.f;
	AlongDistance = 0.f;
	bReached = false;
	bConfigured = Target.IsValid();

	if (bConfigured)
	{
		// Aim at the NEAREST point of the target's collision, not its center: against the wide
		// wall, monsters spread along the façade instead of piling up in the middle (§11.3).
		AimPoint = Target->GetActorLocation();
		FVector ClosestPoint;
		if (Target->ActorGetDistanceToCollision(SpawnLocation, ECC_WorldDynamic, ClosestPoint) >= 0.f)
		{
			AimPoint = ClosestPoint;
		}

		const FVector ToTarget = (AimPoint - SpawnLocation).GetSafeNormal2D();
		Axis = ToTarget.IsNearlyZero() ? FVector::XAxisVector : ToTarget;
		Lateral = FVector::CrossProduct(FVector::UpVector, Axis);
		StartDistance = FMath::Max(1.f, FVector::Dist2D(SpawnLocation, AimPoint));
		SpiralRadius = StartDistance;
		SpiralAngle = FMath::Atan2(SpawnLocation.Y - Target->GetActorLocation().Y, SpawnLocation.X - Target->GetActorLocation().X);
		FlankSide = (FMath::RandBool() ? 1.f : -1.f);
	}
}

void UWDMovePatternComponent::PushBack(float Distance)
{
	if (!bConfigured || Distance <= 0.f)
	{
		return;
	}
	AlongDistance = FMath::Max(0.f, AlongDistance - Distance);
	SpiralRadius = FMath::Min(StartDistance, SpiralRadius + Distance);
	bReached = false;
}

void UWDMovePatternComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bConfigured || bReached || !Target.IsValid())
	{
		CurrentSpeed = 0.f;
		return;
	}

	Time += DeltaTime;
	const FVector TargetLocation = Target->GetActorLocation(); // spiral orbits the center
	const float DistanceToTarget = FVector::Dist2D(GetOwner()->GetActorLocation(), AimPoint);

	if (DistanceToTarget <= StopDistance)
	{
		bReached = true;
		CurrentSpeed = 0.f;
		return;
	}

	float SpeedMultiplier = 1.f;
	float LateralOffset = 0.f;
	CurrentHopHeight = 0.f;
	bBurrowed = false;

	switch (Pattern)
	{
	case EWDMovePattern::Sinusoidal:
		LateralOffset = WDPatternMath::SinusoidOffset(Time, LateralAmplitude, LateralFrequency);
		break;
	case EWDMovePattern::Zigzag:
		LateralOffset = WDPatternMath::ZigzagOffset(Time, LateralAmplitude, LateralFrequency * 1.6f);
		break;
	case EWDMovePattern::ChargePause:
		SpeedMultiplier = WDPatternMath::ChargePauseSpeed(Time, 1.1f, 0.7f);
		break;
	case EWDMovePattern::Hopper:
		CurrentHopHeight = WDPatternMath::HopHeight(Time, 0.9f, 140.f);
		break;
	case EWDMovePattern::Burrower:
		bBurrowed = WDPatternMath::IsBurrowed(Time, 1.5f, 1.2f);
		SpeedMultiplier = bBurrowed ? 1.8f : 1.f; // races forward underground
		break;
	case EWDMovePattern::Flanker:
		LateralOffset = FlankSide * LateralAmplitude * 2.f * WDPatternMath::FlankerBlend(DistanceToTarget, 600.f, StartDistance);
		break;
	case EWDMovePattern::Spiral:
	{
		// Orbits the target while closing in.
		SpiralRadius = FMath::Max(StopDistance, SpiralRadius - Speed * 0.7f * DeltaTime);
		SpiralAngle += (Speed * 0.9f / FMath::Max(150.f, SpiralRadius)) * DeltaTime;
		const FVector Orbit = TargetLocation + FVector(FMath::Cos(SpiralAngle), FMath::Sin(SpiralAngle), 0.f) * SpiralRadius;
		FVector NewLocation = Orbit;
		NewLocation.Z = GroundZ;
		CurrentSpeed = FVector::Dist2D(GetOwner()->GetActorLocation(), NewLocation) / FMath::Max(DeltaTime, 0.001f);
		GetOwner()->SetActorLocation(NewLocation);
		GetOwner()->SetActorRotation((TargetLocation - NewLocation).GetSafeNormal2D().Rotation());
		if (SpiralRadius <= StopDistance + 1.f)
		{
			bReached = true;
		}
		return;
	}
	default:
		break;
	}

	// Advance along the axis, sway on the lateral, hop on Z.
	AlongDistance += Speed * SpeedMultiplier * DeltaTime;
	FVector NewLocation = SpawnLocation + Axis * AlongDistance + Lateral * LateralOffset;
	NewLocation.Z = GroundZ + CurrentHopHeight;

	CurrentSpeed = FVector::Dist2D(GetOwner()->GetActorLocation(), NewLocation) / FMath::Max(DeltaTime, 0.001f);
	const FVector MoveDirection = (NewLocation - GetOwner()->GetActorLocation()).GetSafeNormal2D();
	GetOwner()->SetActorLocation(NewLocation);
	if (!MoveDirection.IsNearlyZero())
	{
		GetOwner()->SetActorRotation(MoveDirection.Rotation());
	}
}
