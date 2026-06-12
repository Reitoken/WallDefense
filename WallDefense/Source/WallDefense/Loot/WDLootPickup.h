#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/WDTypes.h"
#include "WDLootPickup.generated.h"

class UStaticMeshComponent;

/**
 * One drop on the ground (GDD §7): gold, XP or an elemental resource.
 * Pops out of the dead monster with a small bounce, then lives 10 s and expires,
 * blinking the last 3 s — pick up or keep killing, the real moment-to-moment decision.
 * Not collectable while airborne (the bounce scatters them, making the grab earned).
 * No collision: it is pure loot, projectiles fly through it.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDLootPickup : public AActor
{
	GENERATED_BODY()

public:
	AWDLootPickup();

	virtual void Tick(float DeltaSeconds) override;

	void Init(EWDLootType InType, EWDElement InElement, EWDResourceTier InTier, int32 InAmount);

	/** Launches the spawn bounce: a decaying hop from here to the landing spot. */
	void StartBounceTo(const FVector& LandingSpot);

	/** The magnet only grabs landed drops. */
	bool IsCollectable() const { return !bBouncing && !bCollected; }

	/** True exactly once — the magnet that wins the race gets the loot. */
	bool TryCollect();

	UFUNCTION(BlueprintPure, Category = "WD|Loot")
	EWDLootType GetLootType() const { return LootType; }

	UFUNCTION(BlueprintPure, Category = "WD|Loot")
	EWDElement GetElement() const { return Element; }

	UFUNCTION(BlueprintPure, Category = "WD|Loot")
	EWDResourceTier GetTier() const { return Tier; }

	UFUNCTION(BlueprintPure, Category = "WD|Loot")
	int32 GetAmount() const { return Amount; }

	/** Seconds on the ground before expiring (GDD §4 proposal: 10 s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Loot", meta = (ClampMin = "1.0"))
	float LifeTime = 10.f;

	/** Blink warning during the last seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Loot", meta = (ClampMin = "0.0"))
	float BlinkTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Loot", meta = (ClampMin = "0.1"))
	float BounceDuration = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Loot", meta = (ClampMin = "0.0"))
	float BounceHeight = 110.f;

	/** The visual sphere — child of a scene root so bobbing stays LOCAL (never moves the actor). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Loot")
	TObjectPtr<UStaticMeshComponent> Body;

private:
	UPROPERTY(VisibleAnywhere, Category = "WD|Loot")
	TObjectPtr<USceneComponent> SceneRoot;

	EWDLootType LootType = EWDLootType::Gold;
	EWDElement Element = EWDElement::Normal;
	EWDResourceTier Tier = EWDResourceTier::Fragments;
	int32 Amount = 1;

	FVector BounceStart = FVector::ZeroVector;
	FVector BounceLand = FVector::ZeroVector;
	float BounceTime = 0.f;
	float Age = 0.f;
	bool bBouncing = false;
	bool bCollected = false;
};
