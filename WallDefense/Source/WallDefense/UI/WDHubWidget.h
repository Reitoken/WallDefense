#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WDTypes.h"
#include "WDHubWidget.generated.h"

class UVerticalBox;
class UWDWeaponData;
class UWDMonsterData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnStartStageRequested, int32, StageNumber, EWDDifficulty, Mode);

/**
 * The minimal hub (GDD §11): wallet, wall & weapon upgrades, stage & mode select
 * (with the unlock chain), encyclopedia with its ????? mystery, save slots,
 * quality & language, launch. Event-driven: subscribes to the Progression and
 * rebuilds its content on any change (ArchitectureTechnique §7 binding pattern).
 * Debug-first code-built tree; widgets do zero game logic — clicks call subsystems.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API UWDHubWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "WD|UI")
	FWDOnStartStageRequested OnStartStageRequested;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION() void HandleGoldChanged(int32 NewGold) { ScheduleRefresh(); }
	UFUNCTION() void HandleXPChanged(int32 NewXP, int32 NewLevel) { ScheduleRefresh(); }
	UFUNCTION() void HandleResourceChanged(const FWDResourceStack& Stack) { ScheduleRefresh(); }
	UFUNCTION() void HandleWallUpgraded(int32 NewLevel) { ScheduleRefresh(); }
	UFUNCTION() void HandleWeaponLeveledUp(EWDElement Element, int32 NewLevel) { ScheduleRefresh(); }
	UFUNCTION() void HandleSlotLoaded(int32 SlotIndex) { ScheduleRefresh(); }

	UFUNCTION() void HandleStartStage();
	UFUNCTION() void HandleUpgradeWall();

	UFUNCTION() void HandleUpgradeNormal()    { TryUpgradeWeapon(EWDElement::Normal); }
	UFUNCTION() void HandleUpgradeFire()      { TryUpgradeWeapon(EWDElement::Fire); }
	UFUNCTION() void HandleUpgradeIce()       { TryUpgradeWeapon(EWDElement::Ice); }
	UFUNCTION() void HandleUpgradeLightning() { TryUpgradeWeapon(EWDElement::Lightning); }
	UFUNCTION() void HandleUpgradeWind()      { TryUpgradeWeapon(EWDElement::Wind); }
	UFUNCTION() void HandleUpgradeLight()     { TryUpgradeWeapon(EWDElement::Light); }
	UFUNCTION() void HandleUpgradeDark()      { TryUpgradeWeapon(EWDElement::Dark); }

	UFUNCTION() void HandleSlot0() { SelectSlot(0); }
	UFUNCTION() void HandleSlot1() { SelectSlot(1); }
	UFUNCTION() void HandleSlot2() { SelectSlot(2); }
	UFUNCTION() void HandleSlot3() { SelectSlot(3); }
	UFUNCTION() void HandleSlot4() { SelectSlot(4); }

	UFUNCTION() void HandleStage1() { SelectStage(1); }
	UFUNCTION() void HandleStage2() { SelectStage(2); }
	UFUNCTION() void HandleStage3() { SelectStage(3); }
	UFUNCTION() void HandleStage4() { SelectStage(4); }
	UFUNCTION() void HandleStage5() { SelectStage(5); }

	UFUNCTION() void HandleModeNormal() { SelectMode(EWDDifficulty::Normal); }
	UFUNCTION() void HandleModeHard()   { SelectMode(EWDDifficulty::Hard); }
	UFUNCTION() void HandleModeHell()   { SelectMode(EWDDifficulty::Hell); }

	UFUNCTION() void HandleQualityLow()    { SetQuality(0); }
	UFUNCTION() void HandleQualityMedium() { SetQuality(1); }
	UFUNCTION() void HandleQualityHigh()   { SetQuality(2); }
	UFUNCTION() void HandleQualityEpic()   { SetQuality(3); }

	UFUNCTION() void HandleLanguageFrench()  { SetLanguage(TEXT("fr")); }
	UFUNCTION() void HandleLanguageEnglish() { SetLanguage(TEXT("en")); }

private:
	void TryUpgradeWeapon(EWDElement Element);
	void SelectSlot(int32 SlotIndex);
	void SelectStage(int32 StageNumber);
	void SelectMode(EWDDifficulty Mode);
	void SetQuality(int32 QualityLevel);
	void SetLanguage(const TCHAR* Culture);
	void ScheduleRefresh();
	void RebuildContent();

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ContentColumn;

	/** Transient previews: weapon names/elements + zone 1 bestiary for the encyclopedia. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UWDWeaponData>> Arsenal;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UWDMonsterData>> Bestiary;

	int32 SelectedStage = 1;
	EWDDifficulty SelectedMode = EWDDifficulty::Normal;
	bool bRefreshScheduled = false;
};
