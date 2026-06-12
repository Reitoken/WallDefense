#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WDTypes.h"
#include "WDStageSummaryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWDOnSummaryChoice);

/**
 * The end-of-stage dopamine screen (GDD §11): victory OR defeat, stars,
 * detailed loot × multiplier, XP. Buttons: replay or back to the hub.
 * Debug-first code-built tree; a BP subclass animates it later.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API UWDStageSummaryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Builds the content. Call right after CreateWidget, before AddToViewport. */
	void InitSummary(bool bVictory, int32 Stars, const FWDLootBundle& Granted, float Multiplier, int32 BonusGold, int32 CharacterLevel);

	UPROPERTY(BlueprintAssignable, Category = "WD|UI")
	FWDOnSummaryChoice OnReplayRequested;

	UPROPERTY(BlueprintAssignable, Category = "WD|UI")
	FWDOnSummaryChoice OnMenuRequested;

protected:
	UFUNCTION()
	void HandleReplayClicked();

	UFUNCTION()
	void HandleMenuClicked();
};
