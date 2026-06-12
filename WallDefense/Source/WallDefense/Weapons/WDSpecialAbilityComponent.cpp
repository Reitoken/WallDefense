#include "Weapons/WDSpecialAbilityComponent.h"
#include "Weapons/WDWeaponData.h"
#include "Weapons/WDWeaponMath.h"
#include "Weapons/WDProjectilePoolSubsystem.h"
#include "Monsters/WDMonster.h"
#include "Monsters/WDMonsterData.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDProgressionMath.h"
#include "Core/WDGameFeelSubsystem.h"
#include "Core/WDDebugSubsystem.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"

UWDSpecialAbilityComponent::UWDSpecialAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWDSpecialAbilityComponent::SetWeapon(UWDWeaponData* InWeapon)
{
	Weapon = InWeapon;
}

void UWDSpecialAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CooldownRemaining > 0.f)
	{
		CooldownRemaining = FMath::Max(0.f, CooldownRemaining - DeltaTime);
		OnSpecialCooldownChanged.Broadcast(CooldownRemaining, Cooldown);
	}
}

void UWDSpecialAbilityComponent::TryActivate()
{
	if (!Weapon || CooldownRemaining > 0.f)
	{
		return;
	}

	// Scaled by the weapon's meta level: the Special IS the level-100 effect, weaker early.
	int32 WeaponLevel = 1;
	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (const UWDProgressionSubsystem* Progression = GameInstance ? GameInstance->GetSubsystem<UWDProgressionSubsystem>() : nullptr)
	{
		WeaponLevel = Progression->GetWeaponLevel(Weapon->Element);
	}
	const float TotalDamage = Weapon->Damage * WDProgressionMath::SpecialDamageMultiplier(WeaponLevel);
	const FVector Origin = GetOwner()->GetActorLocation();

	// Nova: a ring of empowered piercing shots of the element (chains for lightning, push for wind).
	if (UWDProjectilePoolSubsystem* Pool = GetWorld()->GetSubsystem<UWDProjectilePoolSubsystem>())
	{
		constexpr int32 RingCount = 20;
		FWDShotParams Shot = Weapon->MakeShotParams(GetOwner());
		Shot.Damage = TotalDamage / 4.f; // per ray; piercing multi-hits make up the rest
		Shot.Speed = 1800.f;
		Shot.Range = 1300.f;
		Shot.Radius = FMath::Max(30.f, Shot.Radius);
		Shot.Pierce = 999;
		Shot.FragmentCount = 0;
		Shot.ChainCount = Weapon->Element == EWDElement::Lightning ? 2 : 0;
		Shot.PushBackDistance = Weapon->Element == EWDElement::Wind ? 300.f : 0.f;

		for (const FVector& Direction : WDWeaponMath::ComputeRingDirections(RingCount))
		{
			Pool->FireShot(Shot, Origin + FVector(0, 0, 50.f), Direction);
		}
	}

	// Wind Special also shoves everything already close — even the heavy ones, halved.
	if (Weapon->Element == EWDElement::Wind)
	{
		for (TActorIterator<AWDMonster> It(GetWorld()); It; ++It)
		{
			AWDMonster* Monster = *It;
			if (Monster->IsDead() || FVector::Dist2D(Origin, Monster->GetActorLocation()) > 900.f)
			{
				continue;
			}
			const UWDMonsterData* Data = Monster->GetData();
			const bool bHeavy = Data && (Data->Role == EWDMonsterRole::Tank || Data->Role == EWDMonsterRole::Boss);
			Monster->PushBack(bHeavy ? 250.f : 500.f);
		}
	}

	CooldownRemaining = Cooldown;
	OnSpecialCooldownChanged.Broadcast(CooldownRemaining, Cooldown);
	OnSpecialCast.Broadcast(Weapon); // future video hook (UISubsystem, with media assets)

	if (UWDGameFeelSubsystem* GameFeel = GetWorld()->GetSubsystem<UWDGameFeelSubsystem>())
	{
		GameFeel->Hitstop(0.08f, 0.05f);
		GameFeel->Rumble(0.8f, 0.25f);
		GameFeel->CameraShake(1.5f);
	}
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		Debug->DrawGroundCircle(Origin, 600.f, Weapon->Element, 1.2f);
		Debug->DrawText(Origin + FVector(0, 0, 220.f), FString::Printf(TEXT("SPÉCIALE — %s !"), *Weapon->DisplayName.ToString()), Weapon->Element, 2.f);
	}
}
