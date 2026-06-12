#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WDHubGameMode.generated.h"

/**
 * The menu game mode (ArchitectureTechnique §6.6): boots the game-style navigation —
 * splash -> main menu -> new/load slots -> lobby -> adventure / armory / encyclopedia /
 * options (UWDUISubsystem screen stack). Coming back from a stage (?WDLobby=1) lands
 * straight in the lobby. Works in ANY map (no pawn, spectator + mouse).
 * Set it as GameMode Override to make a map boot on the menus.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDHubGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWDHubGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

private:
	bool bGoStraightToLobby = false;
};
