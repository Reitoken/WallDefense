#include "LaneGrid.h"

#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"

ALaneGrid::ALaneGrid()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

FVector ALaneGrid::GetLaneForwardVector() const
{
	return GetActorForwardVector();
}

FVector ALaneGrid::GetLaneRightVector() const
{
	return GetActorRightVector();
}

float ALaneGrid::GetLaneLateralOffset(int32 LaneIndex) const
{
	const int32 Clamped = FMath::Clamp(LaneIndex, 0, GetNumLanes() - 1);
	const float Center = (GetNumLanes() - 1) * 0.5f;
	return (Clamped - Center) * LaneSpacing;
}

FVector ALaneGrid::GetLaneCenterWorld(int32 LaneIndex) const
{
	return GetActorLocation() + GetLaneRightVector() * GetLaneLateralOffset(LaneIndex);
}

FVector ALaneGrid::GetLanePointWorld(int32 LaneIndex, float AlongDistance) const
{
	return GetLaneCenterWorld(LaneIndex) + GetLaneForwardVector() * ClampAlong(AlongDistance);
}

int32 ALaneGrid::GetNearestLaneIndex(const FVector& WorldLocation) const
{
	const FVector Rel = WorldLocation - GetActorLocation();
	const float Lateral = FVector::DotProduct(Rel, GetLaneRightVector());
	const float Center = (GetNumLanes() - 1) * 0.5f;
	const int32 Index = FMath::RoundToInt(Lateral / FMath::Max(LaneSpacing, KINDA_SMALL_NUMBER) + Center);
	return FMath::Clamp(Index, 0, GetNumLanes() - 1);
}

float ALaneGrid::ClampAlong(float AlongDistance) const
{
	const float Half = LaneLength * 0.5f;
	return FMath::Clamp(AlongDistance, -Half, Half);
}

float ALaneGrid::GetColumnAlongDistance(int32 ColumnIndex) const
{
	const int32 Clamped = FMath::Clamp(ColumnIndex, 0, GetNumColumns() - 1);
	const float CellSize = LaneLength / GetNumColumns();
	return -LaneLength * 0.5f + (Clamped + 0.5f) * CellSize;
}

int32 ALaneGrid::GetNearestColumnIndex(float AlongDistance) const
{
	const float CellSize = LaneLength / GetNumColumns();
	const int32 Index = FMath::FloorToInt((AlongDistance + LaneLength * 0.5f) / FMath::Max(CellSize, KINDA_SMALL_NUMBER));
	return FMath::Clamp(Index, 0, GetNumColumns() - 1);
}

void ALaneGrid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bDrawDebug)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector ZOff(0.f, 0.f, DebugZOffset);
	const float Half = LaneLength * 0.5f;

	for (int32 Lane = 0; Lane < GetNumLanes(); ++Lane)
	{
		const FVector Start = GetLanePointWorld(Lane, -Half) + ZOff;
		const FVector End = GetLanePointWorld(Lane, Half) + ZOff;
		DrawDebugLine(World, Start, End, FColor::Green, false, -1.f, 0, 4.f);

		for (int32 Col = 0; Col < GetNumColumns(); ++Col)
		{
			const FVector Center = GetLanePointWorld(Lane, GetColumnAlongDistance(Col)) + ZOff;
			DrawDebugPoint(World, Center, 8.f, FColor::Yellow, false, -1.f);
		}
	}
}
