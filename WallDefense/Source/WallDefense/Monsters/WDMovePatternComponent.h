#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Monsters/WDMonsterData.h"
#include "WDMovePatternComponent.generated.h"

/**
 * Moves its owner toward a target with the sheet's pattern — one class, eight patterns,
 * all data (ArchitectureTechnique §6.4). Manual movement on the ground plane: monsters
 * advance along their spawn->target axis, the pattern modulates the rest.
 */
UCLASS(ClassGroup = (Monsters), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDMovePatternComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDMovePatternComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Configure(EWDMovePattern InPattern, float InSpeed, AActor* InTarget, float StopDistance);

	/** Repulsion wave (wall skill): loses ground along the advance axis and resumes. */
	UFUNCTION(BlueprintCallable, Category = "WD|Monster")
	void PushBack(float Distance);

	UFUNCTION(BlueprintPure, Category = "WD|Monster")
	bool HasReachedTarget() const { return bReached; }

	UFUNCTION(BlueprintPure, Category = "WD|Monster")
	float GetCurrentSpeed() const { return CurrentSpeed; }

	/** Burrower: untouchable underground phase. Hopper: airborne. */
	UFUNCTION(BlueprintPure, Category = "WD|Monster")
	bool IsBurrowed() const { return bBurrowed; }

	UFUNCTION(BlueprintPure, Category = "WD|Monster")
	bool IsAirborne() const { return CurrentHopHeight > 5.f; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Monster", meta = (ClampMin = "0.0"))
	float LateralAmplitude = 220.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Monster", meta = (ClampMin = "0.05"))
	float LateralFrequency = 0.4f;

private:
	EWDMovePattern Pattern = EWDMovePattern::Straight;
	float Speed = 200.f;
	float StopDistance = 180.f;
	TWeakObjectPtr<AActor> Target;

	FVector SpawnLocation = FVector::ZeroVector;
	FVector AimPoint = FVector::ZeroVector;  // closest point on the target's collision (wide wall -> monsters spread along the façade)
	FVector Axis = FVector::XAxisVector;     // spawn -> aim point
	FVector Lateral = FVector::YAxisVector;  // perpendicular
	float StartDistance = 1.f;
	float AlongDistance = 0.f;
	float Time = 0.f;
	float GroundZ = 0.f;
	float CurrentSpeed = 0.f;
	float CurrentHopHeight = 0.f;
	float SpiralAngle = 0.f;
	float SpiralRadius = 0.f;
	float FlankSide = 1.f;
	bool bBurrowed = false;
	bool bReached = false;
	bool bConfigured = false;
};
