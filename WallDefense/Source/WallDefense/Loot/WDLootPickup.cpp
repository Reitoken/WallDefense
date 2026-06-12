#include "Loot/WDLootPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AWDLootPickup::AWDLootPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	// Scene root + mesh CHILD: the bob animates the mesh locally. With the mesh as root,
	// a relative offset IS a world location — every drop would teleport to the origin.
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(RootComponent);
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Body->SetStaticMesh(SphereMesh.Object);
		Body->SetRelativeScale3D(FVector(0.3f));
	}
}

void AWDLootPickup::Init(EWDLootType InType, EWDElement InElement, EWDResourceTier InTier, int32 InAmount)
{
	LootType = InType;
	Element = InElement;
	Tier = InTier;
	Amount = InAmount;

	// Color signage: gold = gold, XP = green, resource = its element color.
	FLinearColor Color;
	switch (LootType)
	{
	case EWDLootType::Gold:     Color = FLinearColor(1.f, 0.8f, 0.15f); break;
	case EWDLootType::XP:       Color = FLinearColor(0.45f, 1.f, 0.4f); break;
	default:                    Color = UWDTypeLibrary::GetElementColor(Element); break;
	}
	if (Body->GetMaterial(0))
	{
		Body->CreateAndSetMaterialInstanceDynamic(0)->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void AWDLootPickup::StartBounceTo(const FVector& LandingSpot)
{
	BounceStart = GetActorLocation();
	BounceLand = LandingSpot;
	BounceTime = 0.f;
	bBouncing = true;
}

bool AWDLootPickup::TryCollect()
{
	if (!IsCollectable())
	{
		return false;
	}
	bCollected = true;
	Destroy();
	return true;
}

void AWDLootPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Age += DeltaSeconds;
	if (Age >= LifeTime)
	{
		Destroy(); // expired — the moment-to-moment decision was made
		return;
	}

	// Always spin: drops read as "pick me up" even as debug spheres.
	AddActorWorldRotation(FRotator(0.f, 120.f * DeltaSeconds, 0.f));

	if (bBouncing)
	{
		// Spawn pop: a decaying double hop from the corpse to the landing spot.
		BounceTime += DeltaSeconds;
		const float T = FMath::Min(BounceTime / BounceDuration, 1.f);
		FVector NewLocation = FMath::Lerp(BounceStart, BounceLand, T);
		NewLocation.Z = BounceLand.Z + BounceHeight * (1.f - T) * FMath::Abs(FMath::Sin(T * PI * 2.2f));
		SetActorLocation(NewLocation);
		if (T >= 1.f)
		{
			SetActorLocation(BounceLand);
			bBouncing = false;
		}
		return;
	}

	// Landed: local bob on the mesh only.
	Body->SetRelativeLocation(FVector(0.f, 0.f, 12.f * FMath::Sin(Age * 4.f)));

	if (LifeTime - Age <= BlinkTime)
	{
		Body->SetVisibility(FMath::Fmod(Age, 0.25f) < 0.15f);
	}
}
