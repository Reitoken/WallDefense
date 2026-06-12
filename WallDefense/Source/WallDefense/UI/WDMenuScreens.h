#pragma once

#include "CoreMinimal.h"
#include "UI/WDMenuScreen.h"
#include "Core/WDTypes.h"
#include "Core/WDSettingsSubsystem.h" // EWDVolumeChannel
#include "WDMenuScreens.generated.h"

class UWDMonsterData;

/** Logo / presentation screen — any key or the button continues to the main menu. */
UCLASS()
class WALLDEFENSE_API UWDSplashScreen : public UWDMenuScreen
{
	GENERATED_BODY()

protected:
	virtual void RebuildContent() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void HandleContinue();
};

/** Main menu: new game, load game, options, quit. */
UCLASS()
class WALLDEFENSE_API UWDMainMenuScreen : public UWDMenuScreen
{
	GENERATED_BODY()

protected:
	virtual void RebuildContent() override;

	UFUNCTION() void HandleNewGame();
	UFUNCTION() void HandleLoadGame();
	UFUNCTION() void HandleOptions();
	UFUNCTION() void HandleQuit();
};

/** The 5 save slots. Subclasses decide: start fresh or load. */
UCLASS(Abstract)
class WALLDEFENSE_API UWDSaveSlotsScreenBase : public UWDMenuScreen
{
	GENERATED_BODY()

protected:
	virtual void RebuildContent() override;
	virtual bool IsNewGameMode() const { return false; }
	virtual FText GetSlotsTitle() const;

	void ChooseSlot(int32 SlotIndex);

	UFUNCTION() void HandleSlot0() { ChooseSlot(0); }
	UFUNCTION() void HandleSlot1() { ChooseSlot(1); }
	UFUNCTION() void HandleSlot2() { ChooseSlot(2); }
	UFUNCTION() void HandleSlot3() { ChooseSlot(3); }
	UFUNCTION() void HandleSlot4() { ChooseSlot(4); }
};

UCLASS()
class WALLDEFENSE_API UWDNewGameSlotsScreen : public UWDSaveSlotsScreenBase
{
	GENERATED_BODY()

protected:
	virtual bool IsNewGameMode() const override { return true; }
	virtual FText GetSlotsTitle() const override;
};

UCLASS()
class WALLDEFENSE_API UWDLoadGameSlotsScreen : public UWDSaveSlotsScreenBase
{
	GENERATED_BODY()
};

/** The lobby (HQ): the home between runs — adventure, armory, encyclopedia, options. */
UCLASS()
class WALLDEFENSE_API UWDLobbyScreen : public UWDMenuScreen
{
	GENERATED_BODY()

protected:
	virtual void RebuildContent() override;

	UFUNCTION() void HandleAdventure();
	UFUNCTION() void HandleArmory();
	UFUNCTION() void HandleEncyclopedia();
	UFUNCTION() void HandleOptions();
	UFUNCTION() void HandleMainMenu();
};

/** Options: graphics quality, language, volumes (stored; audio routing comes with sound). */
UCLASS()
class WALLDEFENSE_API UWDOptionsScreen : public UWDMenuScreen
{
	GENERATED_BODY()

protected:
	virtual void RebuildContent() override;

	UFUNCTION() void HandleQualityLow()    { SetQuality(0); }
	UFUNCTION() void HandleQualityMedium() { SetQuality(1); }
	UFUNCTION() void HandleQualityHigh()   { SetQuality(2); }
	UFUNCTION() void HandleQualityEpic()   { SetQuality(3); }
	UFUNCTION() void HandleLanguageFrench()  { SetLanguage(TEXT("fr")); }
	UFUNCTION() void HandleLanguageEnglish() { SetLanguage(TEXT("en")); }
	UFUNCTION() void HandleVolumeMaster(float Value);
	UFUNCTION() void HandleVolumeMusic(float Value);
	UFUNCTION() void HandleVolumeSfx(float Value);

private:
	void SetQuality(int32 QualityLevel);
	void SetLanguage(const TCHAR* Culture);
	void AddVolumeRow(const FText& Label, EWDVolumeChannel Channel, const FName& HandlerName);
};

/** Encyclopedia (GDD §2.4): the zone 1 bestiary with its ????? mystery. */
UCLASS()
class WALLDEFENSE_API UWDEncyclopediaScreen : public UWDMenuScreen
{
	GENERATED_BODY()

protected:
	virtual void RebuildContent() override;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UWDMonsterData>> Bestiary;
};
