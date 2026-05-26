#include "WallDefenseGameInstance.h"

#include "GameInstanceComponent.h"

void UWallDefenseGameInstance::Init()
{
	Super::Init();

	for (UGameInstanceComponent* Component : Components)
	{
		if (Component && Component->bEnabled)
		{
			Component->Initialize(this);
		}
	}
}

void UWallDefenseGameInstance::Shutdown()
{
	for (UGameInstanceComponent* Component : Components)
	{
		if (Component && Component->bEnabled)
		{
			Component->Deinitialize();
		}
	}

	Super::Shutdown();
}

UGameInstanceComponent* UWallDefenseGameInstance::FindComponentByClass(TSubclassOf<UGameInstanceComponent> ComponentClass) const
{
	if (!ComponentClass)
	{
		return nullptr;
	}

	for (UGameInstanceComponent* Component : Components)
	{
		if (Component && Component->IsA(ComponentClass))
		{
			return Component;
		}
	}

	return nullptr;
}
