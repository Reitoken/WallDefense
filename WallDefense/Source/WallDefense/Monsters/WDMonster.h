#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/WDTypes.h"
#include "WDMonster.generated.h"

class UStaticMeshComponent;
class UWDHealthComponent;
class UWDMovePatternComponent;
class UWDWallAttackComponent;
class UWDAuraHealComponent;
class UWDHitResponseComponent;
class UWDMonsterData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnMonsterKilled, AWDMonster*, Monster);

/**
 * A monster = a bestiary sheet brought to life. The actor only ASSEMBLES and wires
 * its independent components (ArchitectureTechnique §10): health, pattern movement,
 * wall attack, heal aura (supports), hit response. Debug body: engine shape per role,
 * tinted with the WEAKNESS color (the game's signage).
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDMonster : public AActor
{
	GENERATED_BODY()

public:
	AWDMonster();

	virtual void Tick(float DeltaSeconds) override;

	/** Brings a sheet to life: stats = sheet multipliers × stage bases. Target = the wall. */
	void InitFromData(UWDMonsterData* Data, float StageBaseHealth, float StageBaseWallDamage, AActor* Target);

	UFUNCTION(BlueprintPure, Category = "WD|Monster")
	UWDMonsterData* GetData() const { return MonsterData; }

	UFUNCTION(BlueprintPure, Category = "WD|Monster")
	UWDHealthComponent* GetHealth() const { return Health; }

	// --- Animation state (read by UWDMonsterAnimInstance) ---
	UFUNCTION(BlueprintPure, Category = "WD|Monster|Animation")
	float GetSpeed() const;

	UFUNCTION(BlueprintPure, Category = "WD|Monster|Animation")
	bool IsMoving() const { return GetSpeed() > 5.f; }

	UFUNCTION(BlueprintPure, Category = "WD|Monster|Animation")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category = "WD|Monster|Animation")
	bool IsAttacking() const;

	UPROPERTY(BlueprintAssignable, Category = "WD|Monster")
	FWDOnMonsterKilled OnKilled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Monster")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Monster")
	TObjectPtr<UWDHealthComponent> Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Monster")
	TObjectPtr<UWDMovePatternComponent> MovePattern;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Monster")
	TObjectPtr<UWDWallAttackComponent> WallAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Monster")
	TObjectPtr<UWDAuraHealComponent> HealAura;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Monster")
	TObjectPtr<UWDHitResponseComponent> HitResponse;

protected:
	UFUNCTION()
	void HandleDamagedForward(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match);

	UFUNCTION()
	void HandleDied(AActor* Killer);

private:
	UPROPERTY(Transient)
	TObjectPtr<UWDMonsterData> MonsterData;

	bool bWasBurrowed = false;
};
