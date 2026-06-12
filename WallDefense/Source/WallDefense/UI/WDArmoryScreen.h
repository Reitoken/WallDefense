#pragma once

#include "CoreMinimal.h"
#include "UI/WDMenuScreen.h"
#include "Core/WDTypes.h"
#include "WDArmoryScreen.generated.h"

class UWDWeaponData;

/**
 * The armory (GDD §11): wall and weapon upgrades, paid with gold + the element's
 * materials. Event-driven: rebuilds on every Progression change, auto-saves after
 * each purchase. Buttons grey out when the player cannot afford them.
 */
UCLASS()
class WALLDEFENSE_API UWDArmoryScreen : public UWDMenuScreen
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void RebuildContent() override;

	UFUNCTION() void HandleGoldChanged(int32 NewGold) { ScheduleRebuild(); }
	UFUNCTION() void HandleResourceChanged(const FWDResourceStack& Stack) { ScheduleRebuild(); }
	UFUNCTION() void HandleWallUpgraded(int32 NewLevel) { ScheduleRebuild(); }
	UFUNCTION() void HandleWeaponLeveledUp(EWDElement Element, int32 NewLevel) { ScheduleRebuild(); }

	UFUNCTION() void HandleUpgradeWall();
	UFUNCTION() void HandleUpgradeNormal()    { TryUpgradeWeapon(EWDElement::Normal); }
	UFUNCTION() void HandleUpgradeFire()      { TryUpgradeWeapon(EWDElement::Fire); }
	UFUNCTION() void HandleUpgradeIce()       { TryUpgradeWeapon(EWDElement::Ice); }
	UFUNCTION() void HandleUpgradeLightning() { TryUpgradeWeapon(EWDElement::Lightning); }
	UFUNCTION() void HandleUpgradeWind()      { TryUpgradeWeapon(EWDElement::Wind); }
	UFUNCTION() void HandleUpgradeLight()     { TryUpgradeWeapon(EWDElement::Light); }
	UFUNCTION() void HandleUpgradeDark()      { TryUpgradeWeapon(EWDElement::Dark); }

private:
	void TryUpgradeWeapon(EWDElement Element);
	void AutoSave();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UWDWeaponData>> Arsenal;
};
