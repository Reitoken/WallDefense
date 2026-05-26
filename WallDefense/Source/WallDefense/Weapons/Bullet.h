#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UStaticMesh;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBulletHit, AActor*, HitActor, const FHitResult&, Hit);

UCLASS(Blueprintable)
class WALLDEFENSE_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	ABullet();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Bullet")
	void ActivateBullet(const FVector& StartLocation, const FVector& Direction, float Speed, float InDamage, float InMaxRange, int32 InMaxTargets, float InKnockback, AActor* InInstigatorActor);

	UFUNCTION(BlueprintCallable, Category = "Bullet")
	void DeactivateBullet();

	UFUNCTION(BlueprintCallable, Category = "Bullet")
	void SetBulletMesh(UStaticMesh* InMesh);

	UFUNCTION(BlueprintCallable, Category = "Bullet")
	void SetBulletTint(const FLinearColor& Tint);

	UFUNCTION(BlueprintPure, Category = "Bullet")
	bool IsActive() const { return bIsActive; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|Components")
	TObjectPtr<UNiagaraComponent> TrailComponent;

	UPROPERTY(BlueprintAssignable, Category = "Bullet")
	FOnBulletHit OnBulletHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|Setup")
	FName CollisionProfile = TEXT("OverlapAllDynamic");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|Setup", meta = (ClampMin = "0.1"))
	float CollisionRadius = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|Setup")
	FVector BulletMeshScale = FVector(0.3f);

	/** If true, the bullet destroys itself on Deactivate. Set true for monster bullets (no pool). Pool-managed bullets keep this false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|Setup")
	bool bDestroyOnDeactivate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|FX")
	TObjectPtr<UNiagaraSystem> TrailEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|FX")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|FX")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|FX", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ImpactSoundVolume = 1.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ProcessImpact(AActor* OtherActor, const FHitResult& Hit);
	void ApplyKnockbackToActor(AActor* OtherActor);
	void SpawnImpactFX(const FVector& Location);

	UPROPERTY(Transient)
	FVector SpawnLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	FVector CurrentVelocity = FVector::ZeroVector;

	UPROPERTY(Transient)
	float TraveledDistance = 0.f;

	UPROPERTY(Transient)
	float Damage = 0.f;

	UPROPERTY(Transient)
	float MaxRange = 0.f;

	UPROPERTY(Transient)
	float KnockbackPower = 0.f;

	UPROPERTY(Transient)
	int32 RemainingTargets = 1;

	UPROPERTY(Transient)
	bool bIsActive = false;

	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<AActor>> AlreadyHitActors;
};
