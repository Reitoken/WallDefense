#include "Core/WDCameraShake.h"
#include "Shakes/PerlinNoiseCameraShakePattern.h"

UWDCameraShake::UWDCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPerlinNoiseCameraShakePattern>(TEXT("RootShakePattern")))
{
	bSingleInstance = true;

	if (UPerlinNoiseCameraShakePattern* Pattern = Cast<UPerlinNoiseCameraShakePattern>(GetRootShakePattern()))
	{
		Pattern->Duration = 0.18f;
		Pattern->BlendInTime = 0.02f;
		Pattern->BlendOutTime = 0.1f;

		Pattern->X.Amplitude = 9.f;
		Pattern->X.Frequency = 28.f;
		Pattern->Y.Amplitude = 9.f;
		Pattern->Y.Frequency = 24.f;
		Pattern->Z.Amplitude = 5.f;
		Pattern->Z.Frequency = 20.f;

		Pattern->Pitch.Amplitude = 0.35f;
		Pattern->Pitch.Frequency = 18.f;
		Pattern->Yaw.Amplitude = 0.35f;
		Pattern->Yaw.Frequency = 16.f;
	}
}
