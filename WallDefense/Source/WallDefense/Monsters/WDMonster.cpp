#include "Monsters/WDMonster.h"
#include "Monsters/WDMonsterData.h"
#include "Monsters/WDMovePatternComponent.h"
#include "Monsters/WDWallAttackComponent.h"
#include "Monsters/WDAuraHealComponent.h"
#include "Loot/WDLootDropComponent.h"
#include "Combat/WDHealthComponent.h"
#include "Combat/WDHitResponseComponent.h"
#include "Core/WDDebugSubsystem.h"
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

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RootComponent = Body;
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

void AWDMonster::InitFromData(UWDMonsterData* Data, float StageBaseHealth, float StageBaseWallDamage, AActor* Target)
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

	// Stats: sheet multipliers × stage bases.
	Health->ElementalProfile = Data->ElementalProfile;
	Health->Defense = Data->Defense;
	Health->SetMaxHealth(StageBaseHealth * Data->HealthMultiplier, /*bRefill=*/true);
	if (Data->ShieldFraction > 0.f)
	{
		Health->AddShield(Health->GetMaxHealth() * Data->ShieldFraction);
	}

	// Wiring (the actor is the ONLY place that knows its components together).
	MovePattern->Configure(Data->Pattern, Data->Speed, Target, Data->bRangedAttack ? Data->AttackRange * 0.9f : Data->AttackRange * 0.8f);
	WallAttack->Configure(Target, StageBaseWallDamage * Data->WallDamageMultiplier, Data->AttackRange, Data->AttackInterval, Data->bRangedAttack, EWDElement::Normal);
	HealAura->Configure(Data->HealPulseAmount, Data->HealPulseInterval, Data->HealPulseRadius, EWDElement::Ice);
	HitResponse->Configure(Body, Tint, Data->Role == EWDMonsterRole::Boss);
	// Drops: the monster's elemental material = its weakness (the same color signage).
	// Tier follows the difficulty mode at step 7 — Normal drops Fragments for now (GDD §2.5).
	const EWDElement ResourceElement = Data->ElementalProfile.bHasWeakness ? Data->ElementalProfile.Weakness : EWDElement::Normal;
	LootDrop->Configure(Data->GoldDropMin, Data->GoldDropMax, Data->XPDrop, Data->ResourceDropChance,
		Data->ResourceDropAmount, ResourceElement, EWDResourceTier::Fragments);

	Health->OnDamaged.AddDynamic(this, &AWDMonster::HandleDamagedForward);
	Health->OnDied.AddDynamic(this, &AWDMonster::HandleDied);
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
