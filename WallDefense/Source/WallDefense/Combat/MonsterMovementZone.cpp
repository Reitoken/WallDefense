#include "MonsterMovementZone.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AMonsterMovementZone::AMonsterMovementZone()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);
	Box->SetBoxExtent(FVector(1000.f, 1000.f, 200.f));
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Box->SetHiddenInGame(true);
}

void AMonsterMovementZone::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(bDrawDebug);
}

void AMonsterMovementZone::Tick(float /*DeltaSeconds*/)
{
	if (bDrawDebug && Box)
	{
		const FBox Bounds = GetWorldBounds();
		DrawDebugBox(GetWorld(), Bounds.GetCenter(), Bounds.GetExtent(), GetActorQuat(), DebugColor.ToFColor(true), false, -1.f, 0, 4.f);
	}
}

bool AMonsterMovementZone::IsLocationInside(const FVector& Location) const
{
	if (!Box)
	{
		return false;
	}
	const FBox Bounds = GetWorldBounds();
	return Bounds.IsInsideOrOn(Location);
}

FVector AMonsterMovementZone::ClampToZone(const FVector& Location) const
{
	if (!Box)
	{
		return Location;
	}
	const FBox Bounds = GetWorldBounds();
	return FVector(
		FMath::Clamp(Location.X, Bounds.Min.X, Bounds.Max.X),
		FMath::Clamp(Location.Y, Bounds.Min.Y, Bounds.Max.Y),
		FMath::Clamp(Location.Z, Bounds.Min.Z, Bounds.Max.Z));
}

FVector AMonsterMovementZone::GetRandomPointInZone() const
{
	if (!Box)
	{
		return GetActorLocation();
	}
	const FBox Bounds = GetWorldBounds();
	return FVector(
		FMath::FRandRange(Bounds.Min.X, Bounds.Max.X),
		FMath::FRandRange(Bounds.Min.Y, Bounds.Max.Y),
		FMath::FRandRange(Bounds.Min.Z, Bounds.Max.Z));
}

FBox AMonsterMovementZone::GetWorldBounds() const
{
	if (!Box)
	{
		return FBox(ForceInit);
	}
	const FVector Extent = Box->GetScaledBoxExtent();
	const FVector Center = Box->GetComponentLocation();
	return FBox(Center - Extent, Center + Extent);
}
