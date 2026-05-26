#include "GameInstanceComponent.h"

void UGameInstanceComponent::Initialize(UGameInstance* InOwner)
{
	OwningGameInstance = InOwner;
}

void UGameInstanceComponent::Deinitialize()
{
	OwningGameInstance.Reset();
}
