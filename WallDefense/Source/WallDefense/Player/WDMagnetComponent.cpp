#include "Player/WDMagnetComponent.h"
#include "Loot/WDLootPickup.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDProgressionMath.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"

UWDMagnetComponent::UWDMagnetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

float UWDMagnetComponent::GetMagnetRadius() const
{
	// The attraction range is a character stat (GDD §4): grows with the level.
	const UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	const UWDProgressionSubsystem* Progression = GameInstance ? GameInstance->GetSubsystem<UWDProgressionSubsystem>() : nullptr;
	if (Progression)
	{
		return WDProgressionMath::MagnetRadiusForLevel(Progression->GetCharacterLevel()) + (BaseRadius - 350.f);
	}
	return BaseRadius;
}

void UWDMagnetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	const float Radius = GetMagnetRadius();

	for (TActorIterator<AWDLootPickup> It(GetWorld()); It; ++It)
	{
		AWDLootPickup* Pickup = *It;
		const float Distance = FVector::Dist2D(OwnerLocation, Pickup->GetActorLocation());

		if (Distance <= CollectDistance)
		{
			const EWDLootType Type = Pickup->GetLootType();
			const EWDElement Element = Pickup->GetElement();
			const EWDResourceTier Tier = Pickup->GetTier();
			const int32 Amount = Pickup->GetAmount();
			if (Pickup->TryCollect())
			{
				OnLootCollected.Broadcast(Type, Element, Tier, Amount);
			}
		}
		else if (Distance <= Radius)
		{
			// Pull on the ground plane; the pickup keeps its own bobbing on Z.
			const FVector ToOwner = (OwnerLocation - Pickup->GetActorLocation()).GetSafeNormal2D();
			Pickup->SetActorLocation(Pickup->GetActorLocation() + ToOwner * PullSpeed * DeltaTime);
		}
	}
}
