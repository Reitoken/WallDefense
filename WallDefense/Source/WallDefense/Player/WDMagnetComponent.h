#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDMagnetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FWDOnLootCollected, EWDLootType, Type, EWDElement, Element, EWDResourceTier, Tier, int32, Amount);

/**
 * Pulls nearby AWDLootPickup actors to the owner and collects them on contact
 * (GDD §4). The attraction radius grows with the character level (read from the
 * Progression subsystem). Broadcasts what it grabs — the GameMode tallies the run.
 */
UCLASS(ClassGroup = (Player), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDMagnetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDMagnetComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "WD|Magnet")
	float GetMagnetRadius() const;

	/** Base attraction radius (level 1). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Magnet", meta = (ClampMin = "50.0"))
	float BaseRadius = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Magnet", meta = (ClampMin = "100.0"))
	float PullSpeed = 950.f;

	/** Within this distance the drop is collected. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Magnet", meta = (ClampMin = "20.0"))
	float CollectDistance = 90.f;

	UPROPERTY(BlueprintAssignable, Category = "WD|Magnet")
	FWDOnLootCollected OnLootCollected;
};
