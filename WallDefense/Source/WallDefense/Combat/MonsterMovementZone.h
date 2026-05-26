#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterMovementZone.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class WALLDEFENSE_API AMonsterMovementZone : public AActor
{
	GENERATED_BODY()

public:
	AMonsterMovementZone();

	UFUNCTION(BlueprintPure, Category = "MovementZone")
	bool IsLocationInside(const FVector& Location) const;

	UFUNCTION(BlueprintCallable, Category = "MovementZone")
	FVector ClampToZone(const FVector& Location) const;

	UFUNCTION(BlueprintPure, Category = "MovementZone")
	FVector GetRandomPointInZone() const;

	UFUNCTION(BlueprintPure, Category = "MovementZone")
	FBox GetWorldBounds() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementZone")
	TObjectPtr<UBoxComponent> Box;

	/** If true, the zone is rendered in PIE for debug purposes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementZone")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementZone")
	FLinearColor DebugColor = FLinearColor(0.1f, 0.6f, 1.f, 0.2f);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
