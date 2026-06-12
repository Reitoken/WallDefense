#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDHitResponseComponent.generated.h"

class UStaticMeshComponent;
class UWDHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnKnockbackRequested, float, Distance);

/**
 * The FEEL of an impact, local side (ArchitectureTechnique §6.4): flash, mesh shake,
 * damage numbers (with the elemental verdict — that's what teaches the bestiary),
 * plus global feedback (hitstop, rumble, camera shake) delegated to the GameFeel
 * subsystem. Knockback is only ANNOUNCED (OnKnockbackRequested): the owner decides
 * how to recoil — the monster routes it through its move pattern, so a hit can only
 * ever push it AWAY from the wall, never through it.
 * Removing this component changes nothing about the damage itself.
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDHitResponseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDHitResponseComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Owner wires this once (mesh to flash + its base tint; bBig = boss-grade feedback). */
	void Configure(UStaticMeshComponent* InMesh, const FLinearColor& InBaseColor, bool bInBigDeath);

	/** Owner forwards its health events here. */
	UFUNCTION()
	void HandleDamaged(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match);

	UFUNCTION()
	void HandleDied(AActor* Killer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse", meta = (ClampMin = "0.0"))
	float FlashDuration = 0.1f;

	/** Recoil announced to the owner on hit. 0 by default: only the WIND weapon pushes (GDD §5.2). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse", meta = (ClampMin = "0.0"))
	float KnockbackDistance = 0.f;

	/** Local mesh jitter on impact — needs the mesh to be a CHILD of the actor root. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse", meta = (ClampMin = "0.0"))
	float ShakeAmplitude = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse", meta = (ClampMin = "0.0"))
	float ShakeDuration = 0.12f;

	/** Hitstop only on weakness hits — the reward for switching right. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse", meta = (ClampMin = "0.0"))
	float WeaknessHitstop = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|HitResponse")
	bool bShowDamageNumbers = true;

	UPROPERTY(BlueprintAssignable, Category = "WD|HitResponse")
	FWDOnKnockbackRequested OnKnockbackRequested;

private:
	void RestoreColor();

	TWeakObjectPtr<UStaticMeshComponent> Mesh;
	FLinearColor BaseColor = FLinearColor::White;
	FVector MeshBaseOffset = FVector::ZeroVector;
	float ShakeTimeRemaining = 0.f;
	bool bBigDeath = false;
	FTimerHandle FlashTimer;
};
