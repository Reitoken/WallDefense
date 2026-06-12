#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDHitResponseComponent.generated.h"

class UStaticMeshComponent;
class UWDHealthComponent;

/**
 * The FEEL of an impact, local side (ArchitectureTechnique §6.4): flash, knockback,
 * damage numbers (with the elemental verdict — that's what teaches the bestiary),
 * plus global feedback (hitstop, rumble) delegated to the GameFeel subsystem.
 * Removing this component changes nothing about the damage itself.
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDHitResponseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Owner wires this once (mesh to flash + its base tint; bBig = boss-grade feedback). */
	void Configure(UStaticMeshComponent* InMesh, const FLinearColor& InBaseColor, bool bInBigDeath);

	/** Owner forwards its health events here. */
	UFUNCTION()
	void HandleDamaged(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match);

	UFUNCTION()
	void HandleDied(AActor* Killer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse", meta = (ClampMin = "0.0"))
	float FlashDuration = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse", meta = (ClampMin = "0.0"))
	float KnockbackDistance = 25.f;

	/** Hitstop only on weakness hits — the reward for switching right. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse", meta = (ClampMin = "0.0"))
	float WeaknessHitstop = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse")
	bool bShowDamageNumbers = true;

private:
	void RestoreColor();

	TWeakObjectPtr<UStaticMeshComponent> Mesh;
	FLinearColor BaseColor = FLinearColor::White;
	bool bBigDeath = false;
	FTimerHandle FlashTimer;
};
