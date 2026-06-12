#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDSpecialAbilityComponent.generated.h"

class UWDWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnSpecialCast, UWDWeaponData*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnSpecialCooldown, float, Remaining, float, Total);

/**
 * The Special (GDD §5.1): one charged nova per weapon, ONE shared cooldown gauge.
 * Effect = a ring of empowered piercing shots of the active element, scaled by the
 * weapon's meta level (the level-100 effect, weaker early). Wind shoves everything.
 * OnSpecialCast is the future video hook (UISubsystem plays the cinematic when
 * media assets exist) — debug-first: a fat ground circle announces it.
 */
UCLASS(ClassGroup = (Weapons), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDSpecialAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDSpecialAbilityComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Owner wires this on weapon switch. The cooldown does NOT reset (one gauge). */
	UFUNCTION(BlueprintCallable, Category = "WD|Special")
	void SetWeapon(UWDWeaponData* InWeapon);

	UFUNCTION(BlueprintCallable, Category = "WD|Special")
	void TryActivate();

	UFUNCTION(BlueprintPure, Category = "WD|Special")
	float GetCooldownRemaining() const { return CooldownRemaining; }

	UFUNCTION(BlueprintPure, Category = "WD|Special")
	bool IsReady() const { return CooldownRemaining <= 0.f; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Special", meta = (ClampMin = "1.0"))
	float Cooldown = 30.f;

	UPROPERTY(BlueprintAssignable, Category = "WD|Special")
	FWDOnSpecialCast OnSpecialCast;

	UPROPERTY(BlueprintAssignable, Category = "WD|Special")
	FWDOnSpecialCooldown OnSpecialCooldownChanged;

private:
	UPROPERTY(Transient)
	TObjectPtr<UWDWeaponData> Weapon;

	float CooldownRemaining = 0.f;
};
