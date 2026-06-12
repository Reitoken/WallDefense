#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WDGameFeelSubsystem.generated.h"

/**
 * Global impact feedback that cannot live on a monster (ArchitectureTechnique §5):
 * hitstop (brief time dilation) and gamepad rumble. Camera shake joins once content exists.
 * Respects accessibility settings later (intensity scale).
 */
UCLASS()
class WALLDEFENSE_API UWDGameFeelSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UWDGameFeelSubsystem, STATGROUP_Tickables); }

	/** Freezes the game for a few milliseconds — sells a heavy hit. */
	UFUNCTION(BlueprintCallable, Category = "WD|GameFeel")
	void Hitstop(float DurationSeconds, float TimeDilation = 0.05f);

	/** Gamepad vibration on player 0. */
	UFUNCTION(BlueprintCallable, Category = "WD|GameFeel")
	void Rumble(float Intensity, float DurationSeconds);

private:
	float HitstopRealTimeRemaining = 0.f;
	float ActiveDilation = 1.f;
};
