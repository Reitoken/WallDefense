#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"
#include "WDUISubsystem.generated.h"

class UWDMenuScreen;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnScreenChanged, UWDMenuScreen*, NewScreen);

/**
 * The menu screen stack (ArchitectureTechnique §5): ONE screen on display at a time,
 * game-style navigation — enter a screen, go back with the stack (B / Back buttons).
 * Screens never stack themselves; everything goes through here.
 */
UCLASS()
class WALLDEFENSE_API UWDUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Replaces the current screen; the current one goes on the back stack. */
	UFUNCTION(BlueprintCallable, Category = "WD|UI")
	UWDMenuScreen* ShowScreen(TSubclassOf<UWDMenuScreen> ScreenClass, bool bRememberCurrent = true);

	/** New navigation root (splash, main menu, lobby after loading a save). */
	UFUNCTION(BlueprintCallable, Category = "WD|UI")
	UWDMenuScreen* ShowScreenAndClearStack(TSubclassOf<UWDMenuScreen> ScreenClass);

	UFUNCTION(BlueprintCallable, Category = "WD|UI")
	void GoBack();

	UFUNCTION(BlueprintPure, Category = "WD|UI")
	bool CanGoBack() const { return BackStack.Num() > 0; }

	UPROPERTY(BlueprintAssignable, Category = "WD|UI")
	FWDOnScreenChanged OnScreenChanged;

private:
	UPROPERTY(Transient)
	TObjectPtr<UWDMenuScreen> CurrentScreen;

	TArray<TSubclassOf<UWDMenuScreen>> BackStack;
};
