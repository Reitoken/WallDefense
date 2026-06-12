#include "Monsters/WDAuraHealComponent.h"
#include "Monsters/WDMonster.h"
#include "Combat/WDHealthComponent.h"
#include "Core/WDDebugSubsystem.h"
#include "EngineUtils.h"

UWDAuraHealComponent::UWDAuraHealComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWDAuraHealComponent::Configure(float InAmount, float InInterval, float InRadius, EWDElement InElement)
{
	Amount = InAmount;
	Interval = InInterval;
	Radius = InRadius;
	Element = InElement;
}

void UWDAuraHealComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Cooldown -= DeltaTime;
	if (Cooldown > 0.f || Amount <= 0.f)
	{
		return;
	}
	Cooldown = Interval;

	UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>();
	const FVector Origin = GetOwner()->GetActorLocation();
	bool bHealedSomeone = false;

	for (TActorIterator<AWDMonster> It(GetWorld()); It; ++It)
	{
		AWDMonster* Ally = *It;
		if (Ally == GetOwner() || FVector::DistSquared2D(Ally->GetActorLocation(), Origin) > Radius * Radius)
		{
			continue;
		}
		UWDHealthComponent* Health = Ally->FindComponentByClass<UWDHealthComponent>();
		if (!Health || Health->IsDead() || Health->GetHealth() >= Health->GetMaxHealth())
		{
			continue;
		}
		Health->Heal(Amount);
		bHealedSomeone = true;
		if (Debug)
		{
			Debug->DrawChain(Origin, Ally->GetActorLocation(), Element, 0.4f);
			Debug->DrawText(Ally->GetActorLocation() + FVector(0, 0, 140.f), FString::Printf(TEXT("+%.0f"), Amount), Element, 0.6f);
		}
	}

	if (bHealedSomeone && Debug)
	{
		Debug->DrawGroundCircle(Origin, Radius, Element, 0.4f);
	}
}
