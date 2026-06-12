#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WDWeaponInventoryComponent.generated.h"

class UWDWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnWeaponSwitched, UWDWeaponData*, Weapon, int32, Index);

/**
 * The weapon slots and THE switch (one single switch = changing element, GDD §5).
 * Knows nothing about firing — the owner wires OnWeaponSwitched to the WeaponComponent.
 * With no assets assigned, builds the 7 level-1 debug weapons (debug-first).
 */
UCLASS(ClassGroup = (Weapons), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDWeaponInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "WD|Weapons")
	void NextWeapon();

	UFUNCTION(BlueprintCallable, Category = "WD|Weapons")
	void SelectWeapon(int32 Index);

	UFUNCTION(BlueprintPure, Category = "WD|Weapons")
	UWDWeaponData* GetActiveWeapon() const;

	UFUNCTION(BlueprintPure, Category = "WD|Weapons")
	int32 GetActiveIndex() const { return ActiveIndex; }

	UFUNCTION(BlueprintPure, Category = "WD|Weapons")
	int32 GetWeaponCount() const { return Weapons.Num(); }

	/** Assign real DA_Weapon assets here; empty = the 7 debug weapons are generated. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WD|Weapons")
	TArray<TObjectPtr<UWDWeaponData>> Weapons;

	UPROPERTY(BlueprintAssignable, Category = "WD|Weapons")
	FWDOnWeaponSwitched OnWeaponSwitched;

private:
	int32 ActiveIndex = 0;
};
