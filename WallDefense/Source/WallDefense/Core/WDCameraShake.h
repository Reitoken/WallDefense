#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "WDCameraShake.generated.h"

/**
 * The impact camera shake (ArchitectureTechnique §6.4) — pure code, no asset:
 * a short Perlin jolt, scaled at play time (small on hits, big on boss deaths).
 * Single instance: rapid-fire weapons restart it instead of stacking nausea.
 */
UCLASS()
class WALLDEFENSE_API UWDCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UWDCameraShake(const FObjectInitializer& ObjectInitializer);
};
