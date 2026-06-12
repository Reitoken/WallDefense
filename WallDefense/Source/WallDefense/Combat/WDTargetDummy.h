#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/WDTypes.h"
#include "WDTargetDummy.generated.h"

class UStaticMeshComponent;
class UWDHealthComponent;

/**
 * Training target for the sandbox: a cube tinted with its WEAKNESS color
 * (shoot the matching element!). Shows HP and damage numbers as debug text,
 * dies, then resets after a short delay — endless testing.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDTargetDummy : public AActor
{
	GENERATED_BODY()

public:
	AWDTargetDummy();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Dummy")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Dummy")
	TObjectPtr<UWDHealthComponent> Health;

	/** Seconds before the dummy comes back after dying. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Dummy", meta = (ClampMin = "0.5"))
	float RespawnDelay = 2.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDamaged(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match);

	UFUNCTION()
	void HandleDied(AActor* Killer);

private:
	FTimerHandle RespawnTimer;
};
