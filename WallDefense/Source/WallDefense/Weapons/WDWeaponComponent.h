#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDWeaponComponent.generated.h"

class UWDWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnFired, UWDWeaponData*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnWeaponCooldown, float, Remaining, float, Total);

/**
 * Fires the active weapon according to its data — the four fire modes of the 7 weapons
 * (projectile, continuous cone, bouncing ray, targeted strike). Aim comes from the owner's
 * facing; the strike point from SetAimTarget. Muzzle = fallback offset until meshes/sockets land.
 */
UCLASS(ClassGroup = (Weapons), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDWeaponComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "WD|Weapons")
	void SetWeapon(UWDWeaponData* InWeapon);

	UFUNCTION(BlueprintCallable, Category = "WD|Weapons")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "WD|Weapons")
	void StopFire();

	UFUNCTION(BlueprintPure, Category = "WD|Weapons")
	bool IsFiring() const { return bFiring; }

	/** Where a targeted strike lands (set by the controller: cursor point or aim direction). */
	UFUNCTION(BlueprintCallable, Category = "WD|Weapons")
	void SetAimTarget(const FVector& WorldLocation) { AimTarget = WorldLocation; }

	UPROPERTY(BlueprintAssignable, Category = "WD|Weapons")
	FWDOnFired OnFired;

private:
	void FireOnce();
	void FireProjectile(const FVector& Muzzle, const FVector& Direction);
	void FireCone(const FVector& Muzzle, const FVector& Direction);
	void FireBouncingRay(const FVector& Muzzle, const FVector& Direction);
	void FireStrike();
	void TickStrikes(float DeltaTime);

	FVector GetMuzzleLocation() const;
	FVector GetAimDirection() const;

	UPROPERTY(Transient)
	TObjectPtr<UWDWeaponData> Weapon;

	struct FPendingStrike
	{
		FVector Location = FVector::ZeroVector;
		float TimeRemaining = 0.f;
		float Radius = 0.f;
		float Damage = 0.f;
		EWDElement Element = EWDElement::Normal;
	};
	TArray<FPendingStrike> PendingStrikes;

	FVector AimTarget = FVector::ZeroVector;
	float Cooldown = 0.f;
	bool bFiring = false;
};
