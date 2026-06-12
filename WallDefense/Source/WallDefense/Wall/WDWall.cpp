#include "Wall/WDWall.h"
#include "Wall/WDWallSkillsComponent.h"
#include "Combat/WDHealthComponent.h"
#include "Core/WDDebugSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AWDWall::AWDWall()
{
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RootComponent = Body;
	Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Body->SetCollisionObjectType(ECC_WorldDynamic);
	Body->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Body->SetStaticMesh(CubeMesh.Object);
		// 120 deep × 2800 wide × 300 tall: a façade the monsters spread along.
		Body->SetRelativeScale3D(FVector(1.2f, 28.f, 3.f));
	}

	Health = CreateDefaultSubobject<UWDHealthComponent>(TEXT("Health"));
	Skills = CreateDefaultSubobject<UWDWallSkillsComponent>(TEXT("Skills"));
}

void AWDWall::InitFromData(UWDWallData* Data, int32 Level)
{
	WallData = Data;
	WallLevel = Level;

	if (!Data)
	{
		return;
	}

	const FWDWallLevelDef LevelDef = Data->GetLevelDef(Level);
	Health->Defense = LevelDef.Defense;
	Health->SetMaxHealth(LevelDef.MaxHealth, /*bRefill=*/true);
	Skills->Configure(Data->GetUnlockedSkills(Level));

	// Optional visual ref (preloaded before the stage); absent = debug block.
	if (UStaticMesh* Mesh = Data->WallMesh.LoadSynchronous())
	{
		Body->SetStaticMesh(Mesh);
	}
}

void AWDWall::BeginPlay()
{
	Super::BeginPlay();

	// The actor is the ONLY place wiring its components together (§10).
	Health->OnDamaged.AddDynamic(this, &AWDWall::HandleDamaged);
	Health->OnDied.AddDynamic(this, &AWDWall::HandleDied);
	Skills->OnSkillTriggered.AddDynamic(this, &AWDWall::HandleSkillTriggered);

	RefreshTint();
}

void AWDWall::HandleStageStarted()
{
	Skills->NotifyStageStarted();
}

float AWDWall::GetHealthPercent() const
{
	return Health->GetHealthPercent();
}

void AWDWall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		const FVector Above = GetActorLocation() + FVector(0, 0, 220.f);
		if (Health->IsDead())
		{
			Debug->DrawText(Above, TEXT("MUR DÉTRUIT"), EWDElement::Dark, 0.f);
			return;
		}
		FString Status = FString::Printf(TEXT("MUR  %.0f/%.0f (%.0f%%)"),
			Health->GetHealth(), Health->GetMaxHealth(), Health->GetHealthPercent() * 100.f);
		if (Health->HasShield())
		{
			Status += FString::Printf(TEXT("  [Bouclier %.0f]"), Health->GetShield());
		}
		if (Skills->GetArmedCount() > 0)
		{
			Status += FString::Printf(TEXT("  — %d skill(s) armé(s)"), Skills->GetArmedCount());
		}
		Debug->DrawText(Above, Status, EWDElement::Normal, 0.f);
	}
}

void AWDWall::HandleDamaged(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match)
{
	// Contact = how far the attacker stands from our collision (ranged shooters stay far away).
	float AttackerDistance = TNumericLimits<float>::Max();
	if (const AActor* Attacker = DamageEvent.Instigator.Get())
	{
		FVector ClosestPoint;
		const float Distance = ActorGetDistanceToCollision(Attacker->GetActorLocation(), ECC_WorldDynamic, ClosestPoint);
		if (Distance >= 0.f)
		{
			AttackerDistance = Distance;
		}
	}

	Skills->NotifyWallDamaged(Health->GetHealthPercent(), AttackerDistance);
	RefreshTint();
}

void AWDWall::HandleSkillTriggered(const FWDWallSkillDef& Skill)
{
	FString Announce;
	switch (Skill.Type)
	{
	case EWDWallSkill::StartingShield:
		Health->AddShield(Skill.Value * Health->GetMaxHealth());
		Announce = TEXT("SKILL DU MUR : Bouclier de départ !");
		break;
	case EWDWallSkill::AutoRepair:
		Health->Heal(Skill.Value * Health->GetMaxHealth());
		RefreshTint();
		Announce = TEXT("SKILL DU MUR : Auto-réparation !");
		break;
	case EWDWallSkill::RepulsionWave:
		// The push itself is applied by the GameMode (the wall knows no monster).
		Announce = TEXT("SKILL DU MUR : Onde de répulsion !");
		break;
	}

	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		Debug->DrawText(GetActorLocation() + FVector(0, 0, 380.f), Announce, EWDElement::Light, 3.f);
		if (Skill.Type == EWDWallSkill::RepulsionWave)
		{
			Debug->DrawGroundCircle(GetActorLocation(), Skill.Radius, EWDElement::Wind, 1.5f);
		}
	}
}

void AWDWall::HandleDied(AActor* Killer)
{
	RefreshTint();
}

void AWDWall::RefreshTint()
{
	if (!Body->GetMaterial(0))
	{
		return;
	}
	UMaterialInstanceDynamic* Material = Body->CreateAndSetMaterialInstanceDynamic(0);
	if (Health->IsDead())
	{
		Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.05f, 0.05f, 0.05f));
		return;
	}
	// Grey when healthy, reddens as it suffers — readable from the top-down camera.
	const FLinearColor Healthy(0.45f, 0.45f, 0.5f);
	const FLinearColor Hurt(0.7f, 0.08f, 0.05f);
	Material->SetVectorParameterValue(TEXT("Color"), FMath::Lerp(Hurt, Healthy, Health->GetHealthPercent()));
}
