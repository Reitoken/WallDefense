#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDAuraHealComponent.generated.h"

/**
 * Support aura: pulses heals on nearby wounded monsters while advancing AND at the wall
 * (the healer's "attack" is a buff, GDD §6.1). Priority target by design.
 */
UCLASS(ClassGroup = (Monsters), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDAuraHealComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDAuraHealComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Configure(float InAmount, float InInterval, float InRadius, EWDElement InElement);

private:
	float Amount = 15.f;
	float Interval = 2.5f;
	float Radius = 500.f;
	float Cooldown = 1.f;
	EWDElement Element = EWDElement::Normal;
};
