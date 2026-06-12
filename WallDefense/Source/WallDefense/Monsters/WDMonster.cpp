#include "Monsters/WDMonster.h"
#include "Monsters/WDMonsterData.h"
#include "Monsters/WDMovePatternComponent.h"
#include "Monsters/WDWallAttackComponent.h"
#include "Monsters/WDAuraHealComponent.h"
#include "Loot/WDLootDropComponent.h"
#include "Combat/WDHealthComponent.h"
#include "Combat/WDHitResponseComponent.h"
#include "Stage/WDDifficultyMath.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDDebugSubsystem.h"
#include "Engine/GameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	const TCHAR* MeshPathForRole(EWDMonsterRole Role)
	{
		switch (Role)
		{
		case EWDMonsterRole::Fast:    return TEXT("/Engine/BasicShapes/Cone.Cone");
		case EWDMonsterRole::Shooter: return TEXT("/Engine/BasicShapes/Cylinder.Cylinder");
		case EWDMonsterRole::Healer:  return TEXT("/Engine/BasicShapes/Sphere.Sphere");
		default:                      return TEXT("/Engine/BasicShapes/Cube.Cube");
		}
	}
}

AWDMonster::AWDMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	// Scene root + mesh CHILD: the hit-response shake jitters the body LOCALLY while
	// the move pattern keeps driving the actor — the two never fight.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(RootComponent);
	Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Body->SetCollisionObjectType(ECC_WorldDynamic);
	Body->SetCollisionResponseToAllChannels(ECR_Block);

	Health = CreateDefaultSubobject<UWDHealthComponent>(TEXT("Health"));
	MovePattern = CreateDefaultSubobject<UWDMovePatternComponent>(TEXT("MovePattern"));
	WallAttack = CreateDefaultSubobject<UWDWallAttackComponent>(TEXT("WallAttack"));
	HealAura = CreateDefaultSubobject<UWDAuraHealComponent>(TEXT("HealAura"));
	HitResponse = CreateDefaultSubobject<UWDHitResponseComponent>(TEXT("HitResponse"));
	LootDrop = CreateDefaultSubobject<UWDLootDropComponent>(TEXT("LootDrop"));
}

void AWDMonster::InitFromData(UWDMonsterData* Data, float StageBaseHealth, float StageBaseWallDamage, AActor* Target,
	EWDDifficulty Difficulty)
{
	if (!Data)
	{
		return;
	}
	MonsterData = Data;

	// Body: engine shape per role, tinted with the weakness color (signage).
	if (UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, MeshPathForRole(Data->Role)))
	{
		Body->SetStaticMesh(Mesh);
	}
	Body->SetWorldScale3D(FVector(Data->BodyScale, Data->BodyScale, Data->BodyScale * 1.2f));

	FLinearColor Tint = UWDTypeLibrary::GetElementColor(EWDElement::Normal);
	if (Data->ElementalProfile.bHasWeakness)
	{
		Tint = UWDTypeLibrary::GetElementColor(Data->ElementalProfile.Weakness);
	}
	if (Body->GetMaterial(0))
	{
		UMaterialInstanceDynamic* Material = Body->CreateAndSetMaterialInstanceDynamic(0);
		Material->SetVectorParameterValue(TEXT("Color"), Tint);
	}

	// Stats: sheet multipliers × stage bases × difficulty mode (GDD §2.5).
	Health->ElementalProfile = Data->ElementalProfile;
	Health->Defense = Data->Defense;
	Health->SetMaxHealth(StageBaseHealth * Data->HealthMultiplier * WDDifficultyMath::HealthMultiplier(Difficulty), /*bRefill=*/true);
	if (Data->ShieldFraction > 0.f)
	{
		Health->AddShield(Health->GetMaxHealth() * Data->ShieldFraction);
	}

	// Harder modes pile EXTRA resistances on the sheet (never on the weakness).
	int32 ExtraLeft = WDDifficultyMath::ExtraResistances(Difficulty);
	for (const EWDElement Candidate : { EWDElement::Fire, EWDElement::Ice, EWDElement::Lightning, EWDElement::Wind, EWDElement::Light, EWDElement::Dark })
	{
		if (ExtraLeft <= 0)
		{
			break;
		}
		const bool bIsWeakness = Health->ElementalProfile.bHasWeakness && Health->ElementalProfile.Weakness == Candidate;
		if (!bIsWeakness && !Health->ElementalProfile.Resistances.Contains(Candidate))
		{
			Health->ElementalProfile.Resistances.Add(Candidate);
			--ExtraLeft;
		}
	}

	// Wiring (the actor is the ONLY place that knows its components together).
	MovePattern->Configure(Data->Pattern, Data->Speed, Target, Data->bRangedAttack ? Data->AttackRange * 0.9f : Data->AttackRange * 0.8f);
	HealAura->Configure(Data->HealPulseAmount, Data->HealPulseInterval, Data->HealPulseRadius, EWDElement::Ice);
	HitResponse->Configure(Body, Tint, Data->Role == EWDMonsterRole::Boss);
	// Drops: the monster's elemental material = its weakness (the same color signage).
	// Each mode drops its own tier and pays better (GDD §2.5).
	const EWDElement ResourceElement = Data->ElementalProfile.bHasWeakness ? Data->ElementalProfile.Weakness : EWDElement::Normal;
	const float DropMult = WDDifficultyMath::DropAmountMultiplier(Difficulty);
	LootDrop->Configure(
		FMath::CeilToInt32(Data->GoldDropMin * DropMult), FMath::CeilToInt32(Data->GoldDropMax * DropMult),
		FMath::CeilToInt32(Data->XPDrop * DropMult), Data->ResourceDropChance,
		FMath::CeilToInt32(Data->ResourceDropAmount * DropMult), ResourceElement, WDDifficultyMath::DropTier(Difficulty));

	WallAttack->Configure(Target, StageBaseWallDamage * Data->WallDamageMultiplier * WDDifficultyMath::WallDamageMultiplier(Difficulty),
		Data->AttackRange, Data->AttackInterval, Data->bRangedAttack, EWDElement::Normal);

	Health->OnDamaged.AddDynamic(this, &AWDMonster::HandleDamagedForward);
	Health->OnDied.AddDynamic(this, &AWDMonster::HandleDied);
	// Knockback recoils along the monster's OWN advance axis: away from the wall, always.
	HitResponse->OnKnockbackRequested.AddDynamic(this, &AWDMonster::HandleKnockback);

	// Encyclopedia: meeting a monster reveals its page (GDD §2.4).
	if (UWDProgressionSubsystem* Progression = GetGameInstance() ? GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>() : nullptr)
	{
		Progression->RegisterMonsterDiscovered(FName(*Data->DisplayName.ToString()));
	}
}

void AWDMonster::HandleKnockback(float Distance)
{
	PushBack(Distance);
}

void AWDMonster::PushBack(float Distance)
{
	if (!IsDead())
	{
		MovePattern->PushBack(Distance);
	}
}

void AWDMonster::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!MonsterData || IsDead())
	{
		return;
	}

	// Burrower: untouchable and invisible underground.
	const bool bBurrowed = MovePattern->IsBurrowed();
	if (bBurrowed != bWasBurrowed)
	{
		bWasBurrowed = bBurrowed;
		Body->SetVisibility(!bBurrowed);
		Body->SetCollisionEnabled(bBurrowed ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
	}

	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		const float HeadHeight = 90.f * MonsterData->BodyScale + 40.f;
		FString Status = FString::Printf(TEXT("%s  %.0f/%.0f"), *MonsterData->DisplayName.ToString(), Health->GetHealth(), Health->GetMaxHealth());
		if (Health->HasShield())
		{
			Status += FString::Printf(TEXT(" [B%.0f]"), Health->GetShield());
		}
		Debug->DrawText(GetActorLocation() + FVector(0, 0, HeadHeight), Status, EWDElement::Normal, 0.f);
	}
}

void AWDMonster::HandleDamagedForward(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match)
{
	HitResponse->HandleDamaged(DamageEvent, AppliedDamage, Match);

	// Encyclopedia: landing a weakness hit confirms it on the monster's page (GDD §2.4).
	if (Match == EWDElementalMatch::Weakness && MonsterData)
	{
		if (UWDProgressionSubsystem* Progression = GetGameInstance() ? GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>() : nullptr)
		{
			Progression->RegisterWeaknessDiscovered(FName(*MonsterData->DisplayName.ToString()));
		}
	}
}

void AWDMonster::HandleDied(AActor* Killer)
{
	HitResponse->HandleDied(Killer);
	LootDrop->SpawnDrops(); // gold/XP/material scatter where it fell (GDD §7)
	OnKilled.Broadcast(this);

	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Body->SetVisibility(false);
	MovePattern->SetComponentTickEnabled(false);
	WallAttack->SetComponentTickEnabled(false);
	HealAura->SetComponentTickEnabled(false);
	SetLifeSpan(1.f);
}

float AWDMonster::GetSpeed() const
{
	return MovePattern ? MovePattern->GetCurrentSpeed() : 0.f;
}

bool AWDMonster::IsDead() const
{
	return Health && Health->IsDead();
}

bool AWDMonster::IsAttacking() const
{
	return WallAttack && WallAttack->IsAttacking();
}
