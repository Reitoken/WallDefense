#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDLoadingScreenWidget.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * The loading screen (ArchitectureTechnique §7): subscribes to the PreloadSubsystem,
 * fills its bar with OnPreloadProgress, removes itself on OnPreloadFinished.
 * Debug-first: with no Blueprint design, it builds its own minimal tree in code
 * (dark fullscreen + title + progress bar). A BP subclass only restyles the visuals
 * through the pushed events — the binding pattern of every widget.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API UWDLoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed visual events — the Blueprint (if any) only does looks. */
	UFUNCTION(BlueprintImplementableEvent, Category = "WD|UI")
	void OnLoadingProgress(float Progress);

	UFUNCTION(BlueprintImplementableEvent, Category = "WD|UI")
	void OnLoadingFinished();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandlePreloadProgress(float Progress);

	UFUNCTION()
	void HandlePreloadFinished();

private:
	void BuildDebugTree();

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> DebugProgressBar;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DebugTitle;
};
