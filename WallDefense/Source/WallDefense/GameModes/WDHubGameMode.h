#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WDHubGameMode.generated.h"

class UWDHubWidget;

/**
 * The menu hub (ArchitectureTechnique §6.6): pure UI — upgrades, save slots, options,
 * stage launch. Works in ANY map (no pawn, spectator + mouse). Launching travels to
 * the SAME map with ?game=WDStageGameMode; the stage's "menu" button travels back.
 * Set it as GameMode Override to make a map boot on the menu.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDHubGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWDHubGameMode();

	virtual void StartPlay() override;

protected:
	UFUNCTION()
	void HandleStartStageRequested();

private:
	UPROPERTY(Transient)
	TObjectPtr<UWDHubWidget> HubWidget;
};
