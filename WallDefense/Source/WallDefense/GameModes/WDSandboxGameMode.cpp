#include "GameModes/WDSandboxGameMode.h"
#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroController.h"

AWDSandboxGameMode::AWDSandboxGameMode()
{
	DefaultPawnClass = AWDHeroCharacter::StaticClass();
	PlayerControllerClass = AWDHeroController::StaticClass();
}
