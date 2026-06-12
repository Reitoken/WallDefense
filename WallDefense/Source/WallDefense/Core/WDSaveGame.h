#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/WDProgressionSubsystem.h"
#include "WDSaveGame.generated.h"

/** One save slot on disk = one progression snapshot (ArchitectureTechnique §5). */
UCLASS()
class WALLDEFENSE_API UWDSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FWDProgressionState State;
};
