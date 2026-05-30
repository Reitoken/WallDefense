#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseMonster.generated.h"

class UHealthComponent;
class UNiagaraSystem;
class USoundBase;
class UAnimMontage;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class ABullet;
class UStaticMesh;
class AMonsterMovementZone;
class ALaneGrid;

UENUM(BlueprintType)
enum class EMonsterMovementPattern : uint8
{
	Straight       UMETA(DisplayName = "Straight Line"),
	Sinusoidal     UMETA(DisplayName = "Sinusoidal / Zig-Zag"),
};

UENUM(BlueprintType)
enum class EMonsterAttackMode : uint8
{
	Melee    UMETA(DisplayName = "Melee"),
	Ranged   UMETA(DisplayName = "Ranged"),
};

UENUM(BlueprintType)
enum class EMonsterPathing : uint8
{
	DirectToTarget   UMETA(DisplayName = "Direct To Target"),
	LaneFromSpawn    UMETA(DisplayName = "Lane From Spawn (Auto Axis)"),
	LaneAlongX       UMETA(DisplayName = "Lane Along X (lock Y to spawn)"),
	LaneAlongY       UMETA(DisplayName = "Lane Along Y (lock X to spawn)"),
};

UCLASS(Abstract, Blueprintable)
class WALLDEFENSE_API ABaseMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMonster();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Monster")
	void SetTarget(AActor* InTarget);

	UFUNCTION(BlueprintPure, Category = "Monster")
	AActor* GetTarget() const { return CurrentTarget.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Monster")
	void SetMovementZone(AMonsterMovementZone* InZone);

	/** Bind this monster to a lane: it will advance along the lane toward the target. */
	UFUNCTION(BlueprintCallable, Category = "Monster|Lane")
	void SetLane(ALaneGrid* InLaneGrid, int32 InLaneIndex);

	UFUNCTION(BlueprintPure, Category = "Monster|Lane")
	int32 GetLaneIndex() const { return LaneIndex; }

	/** Current planar speed (cm/s). Use as float input in your AnimBP. */
	UFUNCTION(BlueprintPure, Category = "Monster|Animation")
	float GetSpeed() const;

	/** True if speed exceeds IdleSpeedThreshold. Use as bool transition in your AnimBP state machine. */
	UFUNCTION(BlueprintPure, Category = "Monster|Animation")
	bool IsMoving() const;

	UFUNCTION(BlueprintPure, Category = "Monster|Animation")
	bool IsDead() const { return bDead; }

	UFUNCTION(BlueprintPure, Category = "Monster|Animation")
	bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Monster|HitReact")
	void OnHitReact(float DamageDealt, AActor* DamageInstigator);
	virtual void OnHitReact_Implementation(float DamageDealt, AActor* DamageInstigator);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Components")
	TObjectPtr<UHealthComponent> Health;

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Movement")
	EMonsterMovementPattern MovementPattern = EMonsterMovementPattern::Straight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Movement", meta = (ClampMin = "0.0"))
	float MovementSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Movement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SinusoidalLateralStrength = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Movement", meta = (ClampMin = "0.01"))
	float SinusoidalFrequency = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Movement")
	EMonsterPathing Pathing = EMonsterPathing::LaneFromSpawn;

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat", meta = (ClampMin = "0.0"))
	float Strength = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat", meta = (ClampMin = "0.0"))
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat", meta = (ClampMin = "0.05"))
	float AttackCooldown = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat")
	EMonsterAttackMode AttackMode = EMonsterAttackMode::Melee;

	// Ranged-only
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat|Ranged")
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat|Ranged")
	TObjectPtr<UStaticMesh> BulletMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat|Ranged", meta = (ClampMin = "1.0"))
	float BulletSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat|Ranged", meta = (ClampMin = "1.0"))
	float BulletRange = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Combat|Ranged")
	FVector BulletSpawnOffset = FVector(50.f, 0.f, 50.f);

	// Hit React
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|HitReact", meta = (ClampMin = "0.0"))
	float HitBlinkDuration = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|HitReact")
	FLinearColor HitBlinkColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|HitReact", meta = (ClampMin = "0.0"))
	float HitShakeIntensity = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|HitReact", meta = (ClampMin = "0.0"))
	float HitShakeDuration = 0.2f;

	/** Optional overlay material applied to the mesh during the blink (rendered on top of the regular material). Easiest path: assign a simple unlit-white material here. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|HitReact")
	TObjectPtr<UMaterialInterface> HitFlashOverlayMaterial;

	// FX / SFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|FX")
	TObjectPtr<UNiagaraSystem> DeathEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|FX")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|FX")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|FX")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|FX", meta = (ClampMin = "0.0"))
	float DeathLifeSpan = 1.f;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Animation")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	/** Speed below which the monster is considered idle (cm/s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Animation", meta = (ClampMin = "0.0"))
	float IdleSpeedThreshold = 5.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDamageTaken(float IncomingDamage, float ActualDamage, AActor* DamageInstigator);

	UFUNCTION()
	void HandleDied(AActor* Killer);

	void UpdateMovement(float DeltaSeconds);
	void UpdateAttack(float DeltaSeconds);
	void UpdateHitReact(float DeltaSeconds);

	void DoMeleeAttack();
	void DoRangedAttack();

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> CurrentTarget;

	UPROPERTY(Transient)
	TWeakObjectPtr<AMonsterMovementZone> MovementZone;

	UPROPERTY(Transient)
	TWeakObjectPtr<ALaneGrid> LaneGrid;

	UPROPERTY(Transient)
	int32 LaneIndex = INDEX_NONE;

	void UpdateLaneMovement(float DeltaSeconds);

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> HitFlashMIDs;

	UPROPERTY(Transient)
	FVector MeshBaseRelativeLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	FVector InitialSpawnLocation = FVector::ZeroVector;

	float TimeSinceLastAttack = 0.f;
	float TimeAlive = 0.f;
	float BlinkTimer = 0.f;
	float ShakeTimer = 0.f;
	bool bDead = false;
	bool bIsAttacking = false;
};
