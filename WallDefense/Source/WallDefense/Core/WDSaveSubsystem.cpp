#include "Core/WDSaveSubsystem.h"
#include "Core/WDSaveGame.h"
#include "Core/WDProgressionSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UWDSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency(UWDProgressionSubsystem::StaticClass());

	// Auto-load: slot 0 is the default game (slot selection lives in the hub).
	LoadSlot(0);
}

FString UWDSaveSubsystem::SlotNameFor(int32 SlotIndex)
{
	return FString::Printf(TEXT("WDSlot_%d"), SlotIndex);
}

bool UWDSaveSubsystem::DoesSlotExist(int32 SlotIndex) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotNameFor(SlotIndex), 0);
}

void UWDSaveSubsystem::LoadSlot(int32 SlotIndex)
{
	SlotIndex = FMath::Clamp(SlotIndex, 0, SlotCount - 1);
	UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>();
	if (!Progression)
	{
		return;
	}

	if (const UWDSaveGame* Loaded = Cast<UWDSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotNameFor(SlotIndex), 0)))
	{
		Progression->SetState(Loaded->State);
	}
	else
	{
		Progression->ResetState();
	}
	ActiveSlotIndex = SlotIndex;
	OnSlotLoaded.Broadcast(SlotIndex);
}

void UWDSaveSubsystem::SaveActive()
{
	const UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>();
	UWDSaveGame* Save = Cast<UWDSaveGame>(UGameplayStatics::CreateSaveGameObject(UWDSaveGame::StaticClass()));
	if (!Progression || !Save)
	{
		return;
	}
	Save->State = Progression->GetState();
	if (UGameplayStatics::SaveGameToSlot(Save, SlotNameFor(ActiveSlotIndex), 0))
	{
		OnSaved.Broadcast(ActiveSlotIndex);
	}
}

void UWDSaveSubsystem::NewGameInSlot(int32 SlotIndex)
{
	SlotIndex = FMath::Clamp(SlotIndex, 0, SlotCount - 1);
	if (UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>())
	{
		Progression->ResetState();
	}
	ActiveSlotIndex = SlotIndex;
	SaveActive();
	OnSlotLoaded.Broadcast(SlotIndex);
}
