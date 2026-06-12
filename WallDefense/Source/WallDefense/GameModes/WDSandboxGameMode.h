#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WDSandboxGameMode.generated.h"

/**
 * Training-range game mode for the debug-first phases: the heroine, her twin-stick
 * controller and three elemental target dummies — weapons testing without any stakes.
 * For the full stage loop (wall, waves, victory/defeat), use AWDStageGameMode instead.
 * Use as World Settings > GameMode Override in any test map with a floor.
 * If the map has no PlayerStart, one is created automatically above the origin
 * (without it, Unreal silently spawns nothing — black screen, no input).
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDSandboxGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWDSandboxGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
