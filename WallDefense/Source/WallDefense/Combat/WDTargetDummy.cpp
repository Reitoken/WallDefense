#include "Combat/WDTargetDummy.h"
#include "Combat/WDHealthComponent.h"
#include "Core/WDDebugSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

AWDTargetDummy::AWDTargetDummy()
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
		Body->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.6f));
	}

	Health = CreateDefaultSubobject<UWDHealthComponent>(TEXT("Health"));
}

void AWDTargetDummy::BeginPlay()
{
	Super::BeginPlay();

	// Tinted with the WEAKNESS color: shoot me with this element.
	if (Body->GetMaterial(0))
	{
		UMaterialInstanceDynamic* Material = Body->CreateAndSetMaterialInstanceDynamic(0);
		const FLinearColor Color = Health->ElementalProfile.bHasWeakness
			? UWDTypeLibrary::GetElementColor(Health->ElementalProfile.Weakness)
			: UWDTypeLibrary::GetElementColor(EWDElement::Normal);
		Material->SetVectorParameterValue(TEXT("Color"), Color);
	}

	Health->OnDamaged.AddDynamic(this, &AWDTargetDummy::HandleDamaged);
	Health->OnDied.AddDynamic(this, &AWDTargetDummy::HandleDied);
}

void AWDTargetDummy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		const FVector Above = GetActorLocation() + FVector(0, 0, 120.f);
		if (Health->IsDead())
		{
			Debug->DrawText(Above, TEXT("K.O."), EWDElement::Dark, 0.f);
		}
		else
		{
			FString Status = FString::Printf(TEXT("PV %.0f/%.0f"), Health->GetHealth(), Health->GetMaxHealth());
			if (Health->HasShield())
			{
				Status += FString::Printf(TEXT("  [Bouclier %.0f]"), Health->GetShield());
			}
			Debug->DrawText(Above, Status, EWDElement::Normal, 0.f);
		}
	}
}

void AWDTargetDummy::HandleDamaged(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match)
{
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		FString Text = FString::Printf(TEXT("%.0f"), AppliedDamage);
		switch (Match)
		{
		case EWDElementalMatch::Weakness: Text += TEXT("  FAIBLESSE !"); break;
		case EWDElementalMatch::Resisted: Text += TEXT("  résisté v");   break;
		default: break;
		}
		const FVector Location = GetActorLocation() + FVector(FMath::FRandRange(-40.f, 40.f), FMath::FRandRange(-40.f, 40.f), 160.f);
		Debug->DrawText(Location, Text, DamageEvent.Element, 0.8f);
	}
}

void AWDTargetDummy::HandleDied(AActor* Killer)
{
	Body->SetVisibility(false);
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorldTimerManager().SetTimer(RespawnTimer, [this]()
	{
		Health->ResetToFull();
		if (Health->StartingShield > 0.f)
		{
			Health->AddShield(Health->StartingShield);
		}
		Body->SetVisibility(true);
		Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}, RespawnDelay, false);
}
