#include "GameCameraShake.h"

#include "Shakes/PerlinNoiseCameraShakePattern.h"

UGameCameraShake::UGameCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPerlinNoiseCameraShakePattern>(TEXT("RootShakePattern")))
{
}
