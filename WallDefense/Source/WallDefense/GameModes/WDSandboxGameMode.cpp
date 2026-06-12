#include "GameModes/WDSandboxGameMode.h"
#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroController.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AWDSandboxGameMode::AWDSandboxGameMode()
{
	DefaultPawnClass = AWDHeroCharacter::StaticClass();
	PlayerControllerClass = AWDHeroController::StaticClass();
}

void AWDSandboxGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// No PlayerStart in the map -> Unreal would spawn nobody. Create one above the origin.
	if (TActorIterator<APlayerStart>(GetWorld()).operator bool() == false)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APlayerStart>(FVector(0.f, 0.f, 200.f), FRotator::ZeroRotator, Params);
		UE_LOG(LogTemp, Warning, TEXT("[WDSandbox] No PlayerStart found — spawned one at (0,0,200)."));
	}
}
