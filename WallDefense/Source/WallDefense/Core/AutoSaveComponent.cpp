#include "AutoSaveComponent.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "WallDefenseSaveGame.h"

void UAutoSaveComponent::Initialize(UGameInstance* InOwner)
{
	Super::Initialize(InOwner);

	if (!SaveGameClass)
	{
		SaveGameClass = UWallDefenseSaveGame::StaticClass();
	}

	if (bAutoSaveEnabled)
	{
		StartTimer();
	}
}

void UAutoSaveComponent::Deinitialize()
{
	StopTimer();
	Super::Deinitialize();
}

void UAutoSaveComponent::SetAutoSaveEnabled(bool bEnable)
{
	if (bAutoSaveEnabled == bEnable)
	{
		return;
	}

	bAutoSaveEnabled = bEnable;

	if (bAutoSaveEnabled)
	{
		StartTimer();
	}
	else
	{
		StopTimer();
	}
}

void UAutoSaveComponent::TriggerSaveNow()
{
	UGameInstance* GI = GetOwningGameInstance();
	if (!GI)
	{
		OnAutoSaveCompleted.Broadcast(false);
		return;
	}

	TSubclassOf<UWallDefenseSaveGame> ClassToUse = SaveGameClass;
	if (!ClassToUse)
	{
		ClassToUse = UWallDefenseSaveGame::StaticClass();
	}
	UWallDefenseSaveGame* SaveData = Cast<UWallDefenseSaveGame>(UGameplayStatics::CreateSaveGameObject(ClassToUse));
	if (!SaveData)
	{
		OnAutoSaveCompleted.Broadcast(false);
		return;
	}

	SaveData->SavedAt = FDateTime::Now();
	if (UWorld* World = GI->GetWorld())
	{
		SaveData->LevelName = World->GetMapName();
	}

	OnAutoSaveRequested.Broadcast(SaveData);

	const bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, UserIndex);
	OnAutoSaveCompleted.Broadcast(bSuccess);
}

void UAutoSaveComponent::StartTimer()
{
	StopTimer();

	TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &UAutoSaveComponent::HandleTick),
		AutoSaveIntervalSeconds);
}

void UAutoSaveComponent::StopTimer()
{
	if (TickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
		TickerHandle.Reset();
	}
}

bool UAutoSaveComponent::HandleTick(float /*DeltaSeconds*/)
{
	TriggerSaveNow();
	return true;
}
