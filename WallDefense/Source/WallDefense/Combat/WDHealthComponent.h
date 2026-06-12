#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnShieldChanged, float, NewShield, float, MaxShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWDOnDamaged, const FWDDamageEvent&, DamageEvent, float, AppliedDamage, EWDElementalMatch, Match);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWDOnShieldBroken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnDied, AActor*, Killer);

/**
 * THE universal health component (wall, monsters, anything destructible).
 * - Percent defense:        damage × 100 / (100 + Defense)
 * - Elemental multipliers:  weakness ×1.75, resistance ×0.5 (Normal never resisted)
 * - Optional shield layer:  absorbed before health
 * Pure logic + events; knows nothing about who listens (ArchitectureTechnique §6.2).
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDHealthComponent();

	/** Applies a damage event. Returns the damage actually dealt (shield + health). */
	UFUNCTION(BlueprintCallable, Category = "WD|Health")
	float ApplyWDDamage(const FWDDamageEvent& DamageEvent);

	/** Convenience: damages any actor carrying a WDHealthComponent. Returns damage dealt (0 if none/dead). */
	UFUNCTION(BlueprintCallable, Category = "WD|Health")
	static float DamageActor(AActor* Target, const FWDDamageEvent& DamageEvent);

	UFUNCTION(BlueprintCallable, Category = "WD|Health")
	void Heal(float Amount);

	/** Adds (or refreshes) a shield layer on top of health. */
	UFUNCTION(BlueprintCallable, Category = "WD|Health")
	void AddShield(float Amount);

	UFUNCTION(BlueprintCallable, Category = "WD|Health")
	void Kill(AActor* Killer);

	/** Refills health (and clears shield) — used at stage start. */
	UFUNCTION(BlueprintCallable, Category = "WD|Health")
	void ResetToFull();

	UFUNCTION(BlueprintPure, Category = "WD|Health")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "WD|Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "WD|Health")
	float GetHealthPercent() const { return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f; }

	UFUNCTION(BlueprintPure, Category = "WD|Health")
	float GetShield() const { return CurrentShield; }

	UFUNCTION(BlueprintPure, Category = "WD|Health")
	bool HasShield() const { return CurrentShield > 0.f; }

	UFUNCTION(BlueprintPure, Category = "WD|Health")
	bool IsDead() const { return bDead; }

	/** How this target relates to an element — for UI/encyclopedia previews. */
	UFUNCTION(BlueprintPure, Category = "WD|Health")
	EWDElementalMatch GetElementalMatch(EWDElement Element) const { return ElementalProfile.Match(Element); }

	UFUNCTION(BlueprintCallable, Category = "WD|Health")
	void SetMaxHealth(float NewMax, bool bRefill = false);

	// --- Configuration (per instance; monsters get it from their data asset later) ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.f;

	/** Percent-based mitigation: damage × 100 / (100 + Defense). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Health", meta = (ClampMin = "0.0"))
	float Defense = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Health")
	FWDElementalProfile ElementalProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Health", meta = (ClampMin = "1.0"))
	float WeaknessMultiplier = 1.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Health", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ResistanceMultiplier = 0.5f;

	/** Shield the actor starts with (0 = none). Support monsters can add more at runtime. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Health", meta = (ClampMin = "0.0"))
	float StartingShield = 0.f;

	// --- Events (anyone may listen; this component knows nobody) ---

	UPROPERTY(BlueprintAssignable, Category = "WD|Health")
	FWDOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "WD|Health")
	FWDOnShieldChanged OnShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "WD|Health")
	FWDOnDamaged OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "WD|Health")
	FWDOnShieldBroken OnShieldBroken;

	UPROPERTY(BlueprintAssignable, Category = "WD|Health")
	FWDOnDied OnDied;

protected:
	virtual void BeginPlay() override;

	void Die(AActor* Killer);

	float CurrentHealth = 0.f;
	float CurrentShield = 0.f;
	float MaxShieldSeen = 0.f;
	bool bDead = false;
};
