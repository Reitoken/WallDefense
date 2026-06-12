#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WDTypes.generated.h"

/**
 * Shared types for Wall Defense. Zero dependencies on gameplay classes —
 * everything in the game speaks these types (see ArchitectureTechnique §3).
 */

UENUM(BlueprintType)
enum class EWDElement : uint8
{
	Normal    UMETA(DisplayName = "Normal"),
	Fire      UMETA(DisplayName = "Feu"),
	Ice       UMETA(DisplayName = "Glace"),
	Lightning UMETA(DisplayName = "Foudre"),
	Wind      UMETA(DisplayName = "Vent"),
	Light     UMETA(DisplayName = "Lumière"),
	Dark      UMETA(DisplayName = "Ténèbres"),
};

UENUM(BlueprintType)
enum class EWDDifficulty : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Hard   UMETA(DisplayName = "Hard"),
	Hell   UMETA(DisplayName = "Enfer"),
};

/** Material tiers — each difficulty mode drops its own (GDD §2.5): weapon levels ~1-40 / 41-70 / 71-100. */
UENUM(BlueprintType)
enum class EWDResourceTier : uint8
{
	Fragments UMETA(DisplayName = "Fragments"),
	Crystals  UMETA(DisplayName = "Cristaux"),
	Cores     UMETA(DisplayName = "Noyaux"),
};

/** What a defeated monster drops (GDD §7). Rare items arrive with the encyclopedia step. */
UENUM(BlueprintType)
enum class EWDLootType : uint8
{
	Gold     UMETA(DisplayName = "Or"),
	XP       UMETA(DisplayName = "XP"),
	Resource UMETA(DisplayName = "Ressource élémentaire"),
};

/** A pile of one elemental material: element × tier × amount. */
USTRUCT(BlueprintType)
struct FWDResourceStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	EWDElement Element = EWDElement::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	EWDResourceTier Tier = EWDResourceTier::Fragments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	int32 Amount = 0;
};

/** Everything earned during one run — tallied by the GameMode, applied to the Progression at the end. */
USTRUCT(BlueprintType)
struct FWDLootBundle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	int32 Gold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	int32 XP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	TArray<FWDResourceStack> Resources;

	void AddResource(EWDElement Element, EWDResourceTier Tier, int32 Amount)
	{
		if (Amount <= 0)
		{
			return;
		}
		for (FWDResourceStack& Stack : Resources)
		{
			if (Stack.Element == Element && Stack.Tier == Tier)
			{
				Stack.Amount += Amount;
				return;
			}
		}
		Resources.Add({ Element, Tier, Amount });
	}

	void Add(EWDLootType Type, EWDElement Element, EWDResourceTier Tier, int32 Amount)
	{
		switch (Type)
		{
		case EWDLootType::Gold:     Gold += Amount; break;
		case EWDLootType::XP:       XP += Amount;   break;
		case EWDLootType::Resource: AddResource(Element, Tier, Amount); break;
		}
	}

	bool IsEmpty() const { return Gold == 0 && XP == 0 && Resources.IsEmpty(); }
};

/** How a target relates to an incoming element (drives damage feedback: big icon / small icon + down arrow). */
UENUM(BlueprintType)
enum class EWDElementalMatch : uint8
{
	Neutral   UMETA(DisplayName = "Neutre"),
	Weakness  UMETA(DisplayName = "Faiblesse"),
	Resisted  UMETA(DisplayName = "Résisté"),
};

/** A damage event traveling from a weapon/projectile to a HealthComponent. */
USTRUCT(BlueprintType)
struct FWDDamageEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float Amount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	EWDElement Element = EWDElement::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	FVector ImpactPoint = FVector::ZeroVector;
};

/**
 * Elemental identity of a monster: ONE weakness, any number of resistances.
 * No global element table by design (GDD §6.1) — each monster defines its own.
 */
USTRUCT(BlueprintType)
struct FWDElementalProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	bool bHasWeakness = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element", meta = (EditCondition = "bHasWeakness"))
	EWDElement Weakness = EWDElement::Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	TSet<EWDElement> Resistances;

	/** Normal is never resisted (GDD rule). Weakness wins over resistance if misconfigured. */
	EWDElementalMatch Match(EWDElement Element) const
	{
		if (bHasWeakness && Element == Weakness)
		{
			return EWDElementalMatch::Weakness;
		}
		if (Element != EWDElement::Normal && Resistances.Contains(Element))
		{
			return EWDElementalMatch::Resisted;
		}
		return EWDElementalMatch::Neutral;
	}
};

/** Pure helpers shared by gameplay, debug drawing, VFX and UI. */
UCLASS()
class WALLDEFENSE_API UWDTypeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** The ONE color signage of the game (GDD §5.2) — debug, VFX, icons and UI all use this. */
	UFUNCTION(BlueprintPure, Category = "WD|Element")
	static FLinearColor GetElementColor(EWDElement Element)
	{
		switch (Element)
		{
		case EWDElement::Fire:      return FLinearColor(1.f, 0.25f, 0.05f);
		case EWDElement::Ice:       return FLinearColor(0.4f, 0.75f, 1.f);
		case EWDElement::Lightning: return FLinearColor(1.f, 0.9f, 0.1f);
		case EWDElement::Wind:      return FLinearColor(0.1f, 0.9f, 0.8f);
		case EWDElement::Light:     return FLinearColor(1.f, 1.f, 1.f);
		case EWDElement::Dark:      return FLinearColor(0.55f, 0.2f, 0.85f);
		case EWDElement::Normal:
		default:                    return FLinearColor(0.6f, 0.6f, 0.6f);
		}
	}
};
