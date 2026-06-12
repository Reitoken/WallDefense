#include "Core/WDGameFeelSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UWDGameFeelSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HitstopRealTimeRemaining > 0.f)
	{
		// DeltaTime arrives dilated — convert back to real time to count down correctly.
		const float RealDelta = DeltaTime / FMath::Max(ActiveDilation, KINDA_SMALL_NUMBER);
		HitstopRealTimeRemaining -= RealDelta;
		if (HitstopRealTimeRemaining <= 0.f)
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
			ActiveDilation = 1.f;
		}
	}
}

void UWDGameFeelSubsystem::Hitstop(float DurationSeconds, float TimeDilation)
{
	if (DurationSeconds <= 0.f)
	{
		return;
	}
	ActiveDilation = FMath::Clamp(TimeDilation, 0.01f, 1.f);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), ActiveDilation);
	HitstopRealTimeRemaining = FMath::Max(HitstopRealTimeRemaining, DurationSeconds);
}

void UWDGameFeelSubsystem::Rumble(float Intensity, float DurationSeconds)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->PlayDynamicForceFeedback(FMath::Clamp(Intensity, 0.f, 1.f), DurationSeconds,
			true, true, true, true);
	}
}
