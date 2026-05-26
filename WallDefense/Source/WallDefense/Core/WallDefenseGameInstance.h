#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WallDefenseGameInstance.generated.h"

class UGameInstanceComponent;

UCLASS(Blueprintable)
class WALLDEFENSE_API UWallDefenseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintPure, Category = "WallDefense|GameInstance", meta = (DeterminesOutputType = "ComponentClass"))
	UGameInstanceComponent* FindComponentByClass(TSubclassOf<UGameInstanceComponent> ComponentClass) const;

	template <typename T>
	T* FindComponent() const
	{
		return Cast<T>(FindComponentByClass(T::StaticClass()));
	}

	UFUNCTION(BlueprintPure, Category = "WallDefense|GameInstance")
	const TArray<UGameInstanceComponent*>& GetComponents() const { return Components; }

protected:
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "WallDefense|Components")
	TArray<TObjectPtr<UGameInstanceComponent>> Components;
};
