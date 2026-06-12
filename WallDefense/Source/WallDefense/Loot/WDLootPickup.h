#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/WDTypes.h"
#include "WDLootPickup.generated.h"

class UStaticMeshComponent;

/**
 * One drop on the ground (GDD §7): gold, XP or an elemental resource.
 * Lives 10 s then expires, blinking the last 3 s — pick up or keep killing,
 * the real moment-to-moment decision. Pulled in by the heroine's magnet.
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Loot")
	TObjectPtr<UStaticMeshComponent> Body;

private:
	EWDLootType LootType = EWDLootType::Gold;
	EWDElement Element = EWDElement::Normal;
	EWDResourceTier Tier = EWDResourceTier::Fragments;
	int32 Amount = 1;
	float Age = 0.f;
	bool bCollected = false;
};
