#pragma once

#include "CoreMinimal.h"

class UWDHealthComponent;

/** Finds damageable actors (anything carrying a live UWDHealthComponent). Debug-scale O(n) scans. */
namespace WDTargeting
{
	/** Health component of an actor, or nullptr (dead ones excluded). */
	WALLDEFENSE_API UWDHealthComponent* GetLiveHealth(const AActor* Actor);

	/** Nearest live damageable actor within Radius of Origin, excluding Ignored. */
	WALLDEFENSE_API AActor* FindNearestDamageable(UWorld* World, const FVector& Origin, float Radius,
		const TSet<TObjectPtr<AActor>>& Ignored, AActor* AlsoIgnore = nullptr);

	/** All live damageable actors within Radius of Origin. */
	WALLDEFENSE_API TArray<AActor*> FindAllDamageable(UWorld* World, const FVector& Origin, float Radius,
		AActor* AlsoIgnore = nullptr);
}
