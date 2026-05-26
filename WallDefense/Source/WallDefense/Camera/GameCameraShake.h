#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "GameCameraShake.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class WALLDEFENSE_API UGameCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UGameCameraShake(const FObjectInitializer& ObjectInitializer);
};
