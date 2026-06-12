#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WDSandboxGameMode.generated.h"

/**
 * Test game mode for the debug-first phases: spawns the heroine with her twin-stick controller.
 * Use it as World Settings > GameMode Override in any test map with a floor.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDSandboxGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWDSandboxGameMode();
};
