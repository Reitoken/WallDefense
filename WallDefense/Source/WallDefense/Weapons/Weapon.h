#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UStaticMesh;
class ABullet;
class UBulletPool;

USTRUCT(BlueprintType)
struct FAmmoTypeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	FName Name = TEXT("Default");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	TObjectPtr<UStaticMesh> BulletMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	FLinearColor Tint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = "0.0"))
	float DamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = "0.0"))
	float SpeedMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = "0.0"))
	float RangeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 PierceBonus = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = "0.0"))
	float KnockbackForceBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = "0.0"))
	float AreaOfEffectBonus = 0.f;

	/** Optional limited stock for this ammo type. -1 = infinite. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 ReserveAmount = -1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponLevelChanged, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoTypeChanged, int32, NewIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFired);

UCLASS(Blueprintable)
class WALLDEFENSE_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Level")
	void SetLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Level")
	void AddLevel(int32 Delta = 1);

	UFUNCTION(BlueprintPure, Category = "Weapon|Level")
	int32 GetWeaponLevel() const { return Level; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void SetAmmoTypeIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void CycleAmmoType(int32 Delta = 1);

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int32 GetAmmoTypeIndex() const { return CurrentAmmoTypeIndex; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetDamage() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetBulletSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetFireRate() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetMaxRange() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	int32 GetMaxTargets() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetKnockback() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetAreaOfEffect() const;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponLevelChanged OnLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponAmmoTypeChanged OnAmmoTypeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponFired OnFired;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	TObjectPtr<USceneComponent> Muzzle;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Level", meta = (ClampMin = "1"))
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Level", meta = (ClampMin = "1"))
	int32 MaxLevel = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Base Stats", meta = (ClampMin = "0.0"))
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Base Stats", meta = (ClampMin = "0.0"))
	float BaseBulletSpeed = 2000.f;

	/** Shots per second. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Base Stats", meta = (ClampMin = "0.01"))
	float BaseFireRate = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Base Stats", meta = (ClampMin = "0.0"))
	float BaseMaxRange = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Base Stats", meta = (ClampMin = "1"))
	int32 BaseMaxTargets = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Base Stats", meta = (ClampMin = "0.0"))
	float BaseKnockback = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Base Stats", meta = (ClampMin = "0.0"))
	float BaseAreaOfEffect = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Per Level")
	float DamagePerLevel = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Per Level")
	float BulletSpeedPerLevel = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Per Level")
	float FireRatePerLevel = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Per Level")
	float MaxRangePerLevel = 100.f;

	/** Each full level adds N targets pierced (fractional ok: 0.5 = +1 every 2 levels). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Per Level")
	float MaxTargetsPerLevel = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Per Level")
	float KnockbackPerLevel = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Per Level")
	float AreaOfEffectPerLevel = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Extras", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CritChance = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Extras", meta = (ClampMin = "1.0"))
	float CritMultiplier = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Extras", meta = (ClampMin = "1"))
	int32 BulletsPerShot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Extras", meta = (ClampMin = "0.0"))
	float SpreadAngleDegrees = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Extras", meta = (ClampMin = "0.0"))
	float RecoilImpulse = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	TArray<FAmmoTypeData> AmmoTypes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmoTypeIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pool", meta = (ClampMin = "1"))
	int32 PoolSize = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pool")
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pool")
	TObjectPtr<UStaticMesh> DefaultBulletMesh;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Weapon|Pool")
	TObjectPtr<UBulletPool> BulletPool;

	void EnsureBulletPool();

private:
	FTimerHandle FireTimerHandle;
};
