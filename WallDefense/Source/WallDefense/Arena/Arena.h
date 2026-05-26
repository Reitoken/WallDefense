#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arena.generated.h"

class USceneComponent;
class UHierarchicalInstancedStaticMeshComponent;
class UStaticMesh;

UCLASS(Blueprintable)
class WALLDEFENSE_API AArena : public AActor
{
	GENERATED_BODY()

public:
	AArena();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Arena")
	void RebuildGrid();

	UFUNCTION(BlueprintPure, Category = "Arena")
	int32 GetCellIndex(int32 X, int32 Y) const;

	UFUNCTION(BlueprintPure, Category = "Arena")
	FVector GetCellLocalLocation(int32 X, int32 Y) const;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arena|Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arena|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena|Grid", meta = (ClampMin = "1"))
	int32 GridSizeX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena|Grid", meta = (ClampMin = "1"))
	int32 GridSizeY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena|Grid", meta = (ClampMin = "0.01"))
	float CellSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena|Grid")
	bool bCenterOnActor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena|Mesh")
	TObjectPtr<UStaticMesh> CellMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena|Mesh")
	FVector InstanceScale = FVector::OneVector;
};
