#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDWallAttackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWDOnWallAttack);

/**
 * Attacks the target (the wall) once in range: melee hit or slow ranged projectile.
 * Knows nothing about patterns or health — pure attacker.
 */
UCLASS(ClassGroup = (Monsters), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDWallAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDWallAttackComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Configure(AActor* InTarget, float InDamage, float InRange, float InInterval, bool bInRanged, EWDElement InElement);

	UFUNCTION(BlueprintPure, Category = "WD|Monster")
	bool IsAttacking() const { return bAttacking; }

	UPROPERTY(BlueprintAssignable, Category = "WD|Monster")
	FWDOnWallAttack OnAttack;

private:
	void DoAttack();

	TWeakObjectPtr<AActor> Target;
	FVector ImpactPoint = FVector::ZeroVector; // closest point on the target's collision (the façade)
	float Damage = 10.f;
	float Range = 180.f;
	float Interval = 1.5f;
	float Cooldown = 0.f;
	bool bRanged = false;
	bool bAttacking = false;
	EWDElement Element = EWDElement::Normal;
};
