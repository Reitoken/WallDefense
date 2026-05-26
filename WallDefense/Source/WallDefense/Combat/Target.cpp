#include "Target.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "HealthComponent.h"
#include "UObject/ConstructorHelpers.h"

ATarget::ATarget()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	Health->MaxHealth = 1000.f;
	Health->Defense = 5.f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Mesh->SetStaticMesh(CubeFinder.Object);
		Mesh->SetRelativeScale3D(FVector(2.f, 2.f, 3.f));
	}
}

void ATarget::BeginPlay()
{
	Super::BeginPlay();

	if (Health)
	{
		Health->OnDied.AddDynamic(this, &ATarget::HandleDied);
	}
}

void ATarget::HandleDied(AActor* /*Killer*/)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Target destroyed."), *GetName());
}
