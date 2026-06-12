#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDMenuScreen.generated.h"

class UVerticalBox;
class UButton;
class UWDUISubsystem;
class UWDProgressionSubsystem;

/**
 * Base class of every menu screen (game-style navigation, GDD §11): a dark fullscreen
 * backdrop + a centered column rebuilt from RebuildContent(). Back = Escape / gamepad B
 * / the «Retour» button, all through the UISubsystem stack. The first button gets the
 * keyboard/gamepad focus so pads navigate the menus. Debug-first code-built trees;
 * BP subclasses restyle later.
 */
UCLASS(Abstract)
class WALLDEFENSE_API UWDMenuScreen : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	/** Fill ContentColumn (already cleared). The screen's whole body lives here. */
	virtual void RebuildContent() {}

	void Rebuild();
	void ScheduleRebuild();

	// --- Building helpers (all append to ContentColumn) ---
	void AddTitle(const FText& Title);
	void AddText(const FText& Text, int32 FontSize = 15, const FLinearColor& Color = FLinearColor(0.62f, 0.66f, 0.8f), float PadBottom = 6.f);
	UButton* AddButton(const FText& Label, const FName& HandlerName, int32 FontSize = 20, float PadBottom = 10.f);
	void AddBackButton();

	UWDUISubsystem* GetUI() const;
	UWDProgressionSubsystem* GetProgression() const;

	UFUNCTION()
	void HandleBack();

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ContentColumn;

	/** First button built this rebuild — receives focus (gamepad menu navigation). */
	UPROPERTY(Transient)
	TObjectPtr<UButton> FirstFocus;

private:
	bool bRebuildScheduled = false;
};
