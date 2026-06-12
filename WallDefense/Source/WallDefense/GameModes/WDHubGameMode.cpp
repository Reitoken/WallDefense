#include "GameModes/WDHubGameMode.h"
#include "UI/WDMenuScreens.h"
#include "Core/WDUISubsystem.h"
#include "Core/WDSettingsSubsystem.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

AWDHubGameMode::AWDHubGameMode()
{
	// Pure UI: nobody to possess, just a spectator and the mouse.
	DefaultPawnClass = ASpectatorPawn::StaticClass();
	bStartPlayersAsSpectators = true;
}

void AWDHubGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	// A stage's "headquarters" button travels back with ?WDLobby=1 — skip the splash chain.
	bGoStraightToLobby = UGameplayStatics::HasOption(Options, TEXT("WDLobby"));
}

void AWDHubGameMode::StartPlay()
{
	Super::StartPlay();

	// First launch ever: hardware benchmark -> automatic graphics preset (GDD decision).
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>())
	{
		Settings->RunFirstLaunchSetup();
	}

	if (UWDUISubsystem* UI = GetGameInstance()->GetSubsystem<UWDUISubsystem>())
	{
		UI->ShowScreenAndClearStack(bGoStraightToLobby
			? TSubclassOf<UWDMenuScreen>(UWDLobbyScreen::StaticClass())
			: TSubclassOf<UWDMenuScreen>(UWDSplashScreen::StaticClass()));
	}
}
