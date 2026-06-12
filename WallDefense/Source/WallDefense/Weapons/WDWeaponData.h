#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/WDTypes.h"
#include "WDWeaponData.generated.h"

UENUM(BlueprintType)
enum class EWDFireMode : uint8
{
	Projectile     UMETA(DisplayName = "Projectile"),
	ContinuousCone UMETA(DisplayName = "Cône continu (lance-flammes)"),
	BouncingRay    UMETA(DisplayName = "Rayon à rebonds (laser)"),
	TargetedStrike UMETA(DisplayName = "Frappe de zone (céleste)"),
};

/** Everything a single shot needs — derived from weapon data, reused for fragments/sub-shots. */
USTRUCT(BlueprintType)
struct FWDShotParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") EWDElement Element = EWDElement::Normal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") float Damage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") float Speed = 2500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") float Range = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") float Radius = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") int32 Pierce = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") int32 Bounce = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") int32 FragmentCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") float FragmentDamageFraction = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") int32 ChainCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") float ChainRange = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") float ChainDamageFraction = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") bool bHoming = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") float HomingTurnSpeed = 180.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") TObjectPtr<AActor> Instigator = nullptr;
	/** Friendly-fire filter: actors of this class are never hit (monster shots ignore monsters). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot") TSubclassOf<AActor> IgnoredClass;
};

/**
 * One weapon = one DataAsset (ArchitectureTechnique §6.6). Level-1 stats for now;
 * the 1-100 curve and milestone behaviors plug in here later.
 * Visual refs are intentionally absent: debug rendering is the reference (step 3).
 */
UCLASS(BlueprintType)
class WALLDEFENSE_API UWDWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWDElement Element = EWDElement::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWDFireMode FireMode = EWDFireMode::Projectile;

	/** Damage per shot (or per tick for continuous modes). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0.0"))
	float Damage = 10.f;

	/** Seconds between shots/ticks. The dark laser uses a long interval — switch while it cools. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0.02"))
	float FireInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "50.0"))
	float Range = 1500.f;

	// --- Projectile mode ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = "100.0"))
	float ProjectileSpeed = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = "1.0"))
	float ProjectileRadius = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = "0"))
	int32 Pierce = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = "0"))
	int32 Bounce = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = "0"))
	int32 FragmentCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = "0"))
	int32 ChainCount = 0;

	// --- Continuous cone mode ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Cone", meta = (ClampMin = "1.0", ClampMax = "89.0"))
	float ConeHalfAngleDeg = 25.f;

	// --- Bouncing ray mode ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ray", meta = (ClampMin = "1.0"))
	float RayThickness = 40.f;

	// --- Targeted strike mode ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Strike", meta = (ClampMin = "0.0"))
	float StrikeDelay = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Strike", meta = (ClampMin = "50.0"))
	float StrikeRadius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Strike", meta = (ClampMin = "100.0"))
	float StrikeMaxDistance = 900.f;

	/** Fallback muzzle when no weapon mesh/socket exists yet (socket convention §11). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FVector MuzzleOffset = FVector(60.f, 0.f, 50.f);

	/** Shot params derived from this weapon's stats. */
	FWDShotParams MakeShotParams(AActor* InInstigator) const;

	/** The 7 level-1 debug weapons (GDD §5.2) — used whenever no assets are assigned. */
	static TArray<UWDWeaponData*> MakeDebugArsenal(UObject* Outer);
};
