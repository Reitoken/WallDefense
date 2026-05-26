#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameInstanceComponent.generated.h"

class UGameInstance;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories)
class WALLDEFENSE_API UGameInstanceComponent : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameInstance* InOwner);
	virtual void Deinitialize();

	UFUNCTION(BlueprintPure, Category = "GameInstanceComponent")
	UGameInstance* GetOwningGameInstance() const { return OwningGameInstance.Get(); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInstanceComponent")
	bool bEnabled = true;

protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<UGameInstance> OwningGameInstance;
};
