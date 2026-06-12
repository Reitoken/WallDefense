#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/WDTypes.h"
#include "WDMonsterData.generated.h"

UENUM(BlueprintType)
enum class EWDMonsterRole : uint8
{
	Standard UMETA(DisplayName = "Standard"),
	Fast     UMETA(DisplayName = "Rapide"),
	Tank     UMETA(DisplayName = "Tank"),
	Shooter  UMETA(DisplayName = "Tireur"),
	Healer   UMETA(DisplayName = "Soigneur"),
	Boss     UMETA(DisplayName = "Boss"),
};

UENUM(BlueprintType)
enum class EWDMovePattern : uint8
{
	Straight    UMETA(DisplayName = "Ligne droite"),
	Sinusoidal  UMETA(DisplayName = "Sinusoïdal"),
	Zigzag      UMETA(DisplayName = "Zigzag brusque"),
	ChargePause UMETA(DisplayName = "Charge-pause"),
	Flanker     UMETA(DisplayName = "Flanqueur"),
	Spiral      UMETA(DisplayName = "Spirale"),
	Hopper      UMETA(DisplayName = "Sauteur"),
	Burrower    UMETA(DisplayName = "Fouisseur"),
};

/**
 * One bestiary sheet = one DataAsset (Bestiaire.md). Multipliers apply on the stage base
 * (MonsterScaling.md). MakeZone1Bestiary() builds the 5+boss debug sheets when no assets exist.
 */
UCLASS(BlueprintType)
class WALLDEFENSE_API UWDMonsterData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
	EWDMonsterRole Role = EWDMonsterRole::Standard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
	EWDMovePattern Pattern = EWDMovePattern::Straight;

	/** uu/s — speed NEVER scales with stages (monster identity). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster", meta = (ClampMin = "10.0"))
	float Speed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Stats", meta = (ClampMin = "0.05"))
	float HealthMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Stats", meta = (ClampMin = "0.0"))
	float Defense = 0.f;

	/** Multiplier on the stage's base wall damage. 0 = does not hurt the wall (supports). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Stats", meta = (ClampMin = "0.0"))
	float WallDamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Stats")
	FWDElementalProfile ElementalProfile;

	/** Native shield layer (0 = none), as a fraction of max health. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Stats", meta = (ClampMin = "0.0"))
	float ShieldFraction = 0.f;

	// --- Attack on the wall/target ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Attack", meta = (ClampMin = "50.0"))
	float AttackRange = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Attack", meta = (ClampMin = "0.2"))
	float AttackInterval = 1.5f;

	/** Shooters attack from afar with a slow projectile instead of melee. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Attack")
	bool bRangedAttack = false;

	// --- Support (healer) ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Support", meta = (ClampMin = "0.0"))
	float HealPulseAmount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Support", meta = (ClampMin = "0.5"))
	float HealPulseInterval = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Support", meta = (ClampMin = "100.0"))
	float HealPulseRadius = 500.f;

	/** Body scale of the debug mesh (bosses are big and visible, GDD §2.3). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
	float BodyScale = 1.f;

	// --- Drops (GDD §7: gold, XP, the monster's elemental resource; rare items later) ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Drops", meta = (ClampMin = "0"))
	int32 GoldDropMin = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Drops", meta = (ClampMin = "0"))
	int32 GoldDropMax = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Drops", meta = (ClampMin = "0"))
	int32 XPDrop = 4;

	/** Chance to drop the monster's elemental material (its weakness element). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Drops", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ResourceDropChance = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Drops", meta = (ClampMin = "0"))
	int32 ResourceDropAmount = 1;

	/** Zone 1 debug bestiary: M101..M105 + B1 (Bestiaire.md). */
	static TArray<UWDMonsterData*> MakeZone1Bestiary(UObject* Outer);
};
