#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDPauseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWDOnPauseChoice);

/**
 * The pause menu (GDD §11): resume or abandon — abandoning counts as a defeat,
 * the run loot is KEPT. The widget only announces choices; the controller acts.
 * Debug-first: builds its own tree when no Blueprint design exists.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API UWDPauseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "WD|UI")
	FWDOnPauseChoice OnResumeRequested;

	UPROPERTY(BlueprintAssignable, Category = "WD|UI")
	FWDOnPauseChoice OnAbandonRequested;

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleResumeClicked();

	UFUNCTION()
	void HandleAbandonClicked();
};
