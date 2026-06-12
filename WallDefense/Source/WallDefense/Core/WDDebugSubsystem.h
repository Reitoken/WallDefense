#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/WDTypes.h"
#include "WDDebugSubsystem.generated.h"

/**
 * Centralized debug drawing, colored by element (debug-first rule, ArchitectureTechnique §5).
 * Everything gameplay draws through here; one console variable rules it all:
 *   wd.Debug.Draw 0/1
 * No-op when disabled — callers never check anything.
 */
UCLASS()
class WALLDEFENSE_API UWDDebugSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "WD|Debug")
	bool IsDebugDrawEnabled() const;

	/** Projectile path segment (trace, laser bounce...). */
	UFUNCTION(BlueprintCallable, Category = "WD|Debug")
	void DrawLine(const FVector& Start, const FVector& End, EWDElement Element, float Duration = 1.f, float Thickness = 2.f);

	/** Projectile / pickup / spawn point. */
	UFUNCTION(BlueprintCallable, Category = "WD|Debug")
	void DrawSphere(const FVector& Center, float Radius, EWDElement Element, float Duration = 1.f);

	/** Area of effect on the ground (top-down friendly flat circle). */
	UFUNCTION(BlueprintCallable, Category = "WD|Debug")
	void DrawGroundCircle(const FVector& Center, float Radius, EWDElement Element, float Duration = 1.f);

	/** Impact point. */
	UFUNCTION(BlueprintCallable, Category = "WD|Debug")
	void DrawImpact(const FVector& Point, EWDElement Element, float Duration = 1.f);

	/** Chain/propagation between two targets (lightning chain, mark spread...). */
	UFUNCTION(BlueprintCallable, Category = "WD|Debug")
	void DrawChain(const FVector& From, const FVector& To, EWDElement Element, float Duration = 1.f);

	/** Floating debug text (damage numbers before UI exists). */
	UFUNCTION(BlueprintCallable, Category = "WD|Debug")
	void DrawText(const FVector& Location, const FString& Text, EWDElement Element, float Duration = 1.f);
};
