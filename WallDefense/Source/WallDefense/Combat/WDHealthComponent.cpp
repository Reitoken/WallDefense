#include "Combat/WDHealthComponent.h"

UWDHealthComponent::UWDHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
	CurrentShield = StartingShield;
}

void UWDHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	ResetToFull();
	if (StartingShield > 0.f)
	{
		AddShield(StartingShield);
	}
}

float UWDHealthComponent::ApplyWDDamage(const FWDDamageEvent& DamageEvent)
{
	if (bDead || DamageEvent.Amount <= 0.f)
	{
		return 0.f;
	}

	const EWDElementalMatch Match = ElementalProfile.Match(DamageEvent.Element);

	float Multiplier = 1.f;
	switch (Match)
	{
	case EWDElementalMatch::Weakness: Multiplier = WeaknessMultiplier;   break;
	case EWDElementalMatch::Resisted: Multiplier = ResistanceMultiplier; break;
	default: break;
	}

	const float Mitigation = 100.f / (100.f + FMath::Max(0.f, Defense));
	float Remaining = DamageEvent.Amount * Multiplier * Mitigation;
	float Applied = 0.f;

	// Shield layer absorbs first.
	if (CurrentShield > 0.f)
	{
		const float Absorbed = FMath::Min(CurrentShield, Remaining);
		CurrentShield -= Absorbed;
		Remaining -= Absorbed;
		Applied += Absorbed;
		OnShieldChanged.Broadcast(CurrentShield, MaxShieldSeen);
		if (CurrentShield <= 0.f)
		{
			OnShieldBroken.Broadcast();
		}
	}

	if (Remaining > 0.f)
	{
		const float Dealt = FMath::Min(CurrentHealth, Remaining);
		CurrentHealth -= Dealt;
		Applied += Dealt;
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}

	OnDamaged.Broadcast(DamageEvent, Applied, Match);

	if (CurrentHealth <= 0.f)
	{
		Die(DamageEvent.Instigator);
	}

	return Applied;
}

void UWDHealthComponent::Heal(float Amount)
{
	if (bDead || Amount <= 0.f)
	{
		return;
	}
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UWDHealthComponent::AddShield(float Amount)
{
	if (bDead || Amount <= 0.f)
	{
		return;
	}
	CurrentShield += Amount;
	MaxShieldSeen = FMath::Max(MaxShieldSeen, CurrentShield);
	OnShieldChanged.Broadcast(CurrentShield, MaxShieldSeen);
}

void UWDHealthComponent::Kill(AActor* Killer)
{
	if (bDead)
	{
		return;
	}
	CurrentShield = 0.f;
	CurrentHealth = 0.f;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	Die(Killer);
}

void UWDHealthComponent::ResetToFull()
{
	bDead = false;
	CurrentHealth = MaxHealth;
	CurrentShield = 0.f;
	MaxShieldSeen = 0.f;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnShieldChanged.Broadcast(CurrentShield, MaxShieldSeen);
}

void UWDHealthComponent::SetMaxHealth(float NewMax, bool bRefill)
{
	MaxHealth = FMath::Max(1.f, NewMax);
	CurrentHealth = bRefill ? MaxHealth : FMath::Min(CurrentHealth, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UWDHealthComponent::Die(AActor* Killer)
{
	if (bDead)
	{
		return;
	}
	bDead = true;
	OnDied.Broadcast(Killer);
}
