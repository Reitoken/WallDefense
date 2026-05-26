#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WallDefenseSaveGame.generated.h"

UCLASS(Blueprintable, BlueprintType)
class WALLDEFENSE_API UWallDefenseSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	FDateTime SavedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	int32 SaveVersion = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	FString LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	TMap<FName, FString> Payload;
};
