#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDSaveSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnSlotLoaded, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnSaved, int32, SlotIndex);

/**
 * Sole master of the disk (ArchitectureTechnique §5): 5 slots, auto-load of the
 * last-used slot at boot, auto-save after every stage. Serializes the snapshot
 * the ProgressionSubsystem hands over — owns NO game rules itself.
 */
UCLASS()
class WALLDEFENSE_API UWDSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static constexpr int32 SlotCount = 5;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintPure, Category = "WD|Save")
	bool DoesSlotExist(int32 SlotIndex) const;

	/** Loads a slot into the Progression (fresh state if the slot is empty) and makes it active. */
	UFUNCTION(BlueprintCallable, Category = "WD|Save")
	void LoadSlot(int32 SlotIndex);

	/** Snapshots the Progression into the active slot. */
	UFUNCTION(BlueprintCallable, Category = "WD|Save")
	void SaveActive();

	/** Wipes the slot to a fresh state and makes it active. */
	UFUNCTION(BlueprintCallable, Category = "WD|Save")
	void NewGameInSlot(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "WD|Save")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UPROPERTY(BlueprintAssignable, Category = "WD|Save")
	FWDOnSlotLoaded OnSlotLoaded;

	UPROPERTY(BlueprintAssignable, Category = "WD|Save")
	FWDOnSaved OnSaved;

private:
	static FString SlotNameFor(int32 SlotIndex);

	int32 ActiveSlotIndex = 0;
};
