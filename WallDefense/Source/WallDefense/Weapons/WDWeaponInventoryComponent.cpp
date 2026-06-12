#include "Weapons/WDWeaponInventoryComponent.h"
#include "Weapons/WDWeaponData.h"

void UWDWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Weapons.IsEmpty())
	{
		for (UWDWeaponData* Weapon : UWDWeaponData::MakeDebugArsenal(this))
		{
			Weapons.Add(Weapon);
		}
	}
	SelectWeapon(0);
}

void UWDWeaponInventoryComponent::NextWeapon()
{
	if (Weapons.Num() > 0)
	{
		SelectWeapon((ActiveIndex + 1) % Weapons.Num());
	}
}

void UWDWeaponInventoryComponent::SelectWeapon(int32 Index)
{
	if (!Weapons.IsValidIndex(Index))
	{
		return;
	}
	ActiveIndex = Index;
	OnWeaponSwitched.Broadcast(Weapons[ActiveIndex], ActiveIndex);
}

UWDWeaponData* UWDWeaponInventoryComponent::GetActiveWeapon() const
{
	return Weapons.IsValidIndex(ActiveIndex) ? Weapons[ActiveIndex] : nullptr;
}
