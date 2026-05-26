#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "GameInstanceComponent.h"
#include "AutoSaveComponent.generated.h"

class UWallDefenseSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutoSaveRequested, UWallDefenseSaveGame*, SaveGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutoSaveCompleted, bool, bSuccess);

UCLASS(Blueprintable)
class WALLDEFENSE_API UAutoSaveComponent : public UGameInstanceComponent
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameInstance* InOwner) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void TriggerSaveNow();

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void SetAutoSaveEnabled(bool bEnable);

	UFUNCTION(BlueprintPure, Category = "AutoSave")
	bool IsAutoSaveEnabled() const { return bAutoSaveEnabled; }

	UPROPERTY(BlueprintAssignable, Category = "AutoSave")
	FOnAutoSaveRequested OnAutoSaveRequested;

	UPROPERTY(BlueprintAssignable, Category = "AutoSave")
	FOnAutoSaveCompleted OnAutoSaveCompleted;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoSave", meta = (ClampMin = "1.0"))
	float AutoSaveIntervalSeconds = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoSave")
	bool bAutoSaveEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoSave")
	FString SaveSlotName = TEXT("AutoSave");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoSave")
	int32 UserIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoSave")
	TSubclassOf<UWallDefenseSaveGame> SaveGameClass;

private:
	void StartTimer();
	void StopTimer();
	bool HandleTick(float DeltaSeconds);

	FTSTicker::FDelegateHandle TickerHandle;
};
