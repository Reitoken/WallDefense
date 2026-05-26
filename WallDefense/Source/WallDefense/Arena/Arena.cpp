#include "Arena.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"

AArena::AArena()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Grid = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Grid"));
	Grid->SetupAttachment(Root);
	Grid->SetMobility(EComponentMobility::Static);
	Grid->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Grid->SetCollisionObjectType(ECC_WorldStatic);
}

void AArena::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RebuildGrid();
}

void AArena::RebuildGrid()
{
	if (!Grid)
	{
		return;
	}

	Grid->ClearInstances();
	Grid->SetStaticMesh(CellMesh);

	if (!CellMesh)
	{
		return;
	}

	const float OffsetX = bCenterOnActor ? -((GridSizeX - 1) * CellSize) * 0.5f : 0.f;
	const float OffsetY = bCenterOnActor ? -((GridSizeY - 1) * CellSize) * 0.5f : 0.f;

	TArray<FTransform> InstanceTransforms;
	InstanceTransforms.Reserve(GridSizeX * GridSizeY);

	for (int32 Y = 0; Y < GridSizeY; ++Y)
	{
		for (int32 X = 0; X < GridSizeX; ++X)
		{
			const FVector Location(OffsetX + X * CellSize, OffsetY + Y * CellSize, 0.f);
			InstanceTransforms.Emplace(FRotator::ZeroRotator, Location, InstanceScale);
		}
	}

	Grid->AddInstances(InstanceTransforms, /*bShouldReturnIndices*/ false);
}

int32 AArena::GetCellIndex(int32 X, int32 Y) const
{
	if (X < 0 || X >= GridSizeX || Y < 0 || Y >= GridSizeY)
	{
		return INDEX_NONE;
	}
	return Y * GridSizeX + X;
}

FVector AArena::GetCellLocalLocation(int32 X, int32 Y) const
{
	const float OffsetX = bCenterOnActor ? -((GridSizeX - 1) * CellSize) * 0.5f : 0.f;
	const float OffsetY = bCenterOnActor ? -((GridSizeY - 1) * CellSize) * 0.5f : 0.f;
	return FVector(OffsetX + X * CellSize, OffsetY + Y * CellSize, 0.f);
}
