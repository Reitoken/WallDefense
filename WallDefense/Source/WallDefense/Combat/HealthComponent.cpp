#include "HealthComponent.h"

#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bDead = false;

	if (bBindOwnerTakeDamage)
	{
		if (AActor* Owner = GetOwner())
		{
			Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleOwnerTakeAnyDamage);
		}
	}
}

void UHealthComponent::ApplyDamage(float Damage, AActor* DamageInstigator)
{
	if (bDead || Damage <= 0.f)
	{
		return;
	}

	const float Mitigated = FMath::Max(0.f, Damage - Defense);
	if (Mitigated <= 0.f)
	{
		OnDamageTaken.Broadcast(Damage, 0.f, DamageInstigator);
		return;
	}

	CurrentHealth = FMath::Max(0.f, CurrentHealth - Mitigated);
	OnDamageTaken.Broadcast(Damage, Mitigated, DamageInstigator);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		bDead = true;
		OnDied.Broadcast(DamageInstigator);
	}
}

void UHealthComponent::Heal(float Amount)
{
	if (bDead || Amount <= 0.f)
	{
		return;
	}
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::Kill(AActor* Killer)
{
	if (bDead)
	{
		return;
	}
	CurrentHealth = 0.f;
	bDead = true;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDied.Broadcast(Killer);
}

void UHealthComponent::SetMaxHealth(float NewMax, bool bRefill)
{
	MaxHealth = FMath::Max(1.f, NewMax);
	if (bRefill)
	{
		CurrentHealth = MaxHealth;
		bDead = false;
	}
	else
	{
		CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);
	}
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::HandleOwnerTakeAnyDamage(AActor* /*DamagedActor*/, float Damage, const UDamageType* /*DamageType*/, AController* InstigatedBy, AActor* DamageCauser)
{
	AActor* Instigator = DamageCauser;
	if (!Instigator && InstigatedBy)
	{
		Instigator = InstigatedBy->GetPawn();
	}
	ApplyDamage(Damage, Instigator);
}
