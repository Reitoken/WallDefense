#include "Bullet.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(CollisionRadius);
	CollisionSphere->SetCollisionProfileName(CollisionProfile);
	CollisionSphere->SetGenerateOverlapEvents(true);
	SetRootComponent(CollisionSphere);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(BulletMeshScale);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->SetSphereRadius(CollisionRadius);
	CollisionSphere->SetCollisionProfileName(CollisionProfile);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABullet::HandleOverlap);

	if (Mesh)
	{
		Mesh->SetRelativeScale3D(BulletMeshScale);
	}
}

void ABullet::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsActive)
	{
		return;
	}

	const FVector Delta = CurrentVelocity * DeltaSeconds;
	const float StepSize = Delta.Size();

	AddActorWorldOffset(Delta, /*bSweep*/ true);

	TraveledDistance += StepSize;
	if (TraveledDistance >= MaxRange)
	{
		DeactivateBullet();
	}
}

void ABullet::ActivateBullet(const FVector& StartLocation, const FVector& Direction, float Speed, float InDamage, float InMaxRange, int32 InMaxTargets, AActor* InInstigatorActor)
{
	SpawnLocation = StartLocation;
	Damage = InDamage;
	MaxRange = FMath::Max(1.f, InMaxRange);
	RemainingTargets = FMath::Max(1, InMaxTargets);
	TraveledDistance = 0.f;
	AlreadyHitActors.Reset();

	SetInstigator(Cast<APawn>(InInstigatorActor));
	SetOwner(InInstigatorActor);

	const FVector NormDir = Direction.GetSafeNormal();
	CurrentVelocity = NormDir * Speed;

	SetActorLocationAndRotation(StartLocation, NormDir.Rotation());
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	bIsActive = true;
	SetActorTickEnabled(true);
}

void ABullet::DeactivateBullet()
{
	bIsActive = false;
	CurrentVelocity = FVector::ZeroVector;
	TraveledDistance = 0.f;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	AlreadyHitActors.Reset();
}

void ABullet::SetBulletMesh(UStaticMesh* InMesh)
{
	if (Mesh)
	{
		Mesh->SetStaticMesh(InMesh);
	}
}

void ABullet::SetBulletTint(const FLinearColor& Tint)
{
	if (!Mesh || !Mesh->GetStaticMesh())
	{
		return;
	}

	UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (MID)
	{
		MID->SetVectorParameterValue(TEXT("Color"), Tint);
		MID->SetVectorParameterValue(TEXT("Tint"), Tint);
		MID->SetVectorParameterValue(TEXT("BaseColor"), Tint);
	}
}

void ABullet::HandleOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& Sweep)
{
	if (!bIsActive || !OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	const TWeakObjectPtr<AActor> WeakOther(OtherActor);
	if (AlreadyHitActors.Contains(WeakOther))
	{
		return;
	}
	AlreadyHitActors.Add(WeakOther);

	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);
	OnBulletHit.Broadcast(OtherActor, Sweep);

	--RemainingTargets;
	if (RemainingTargets <= 0)
	{
		DeactivateBullet();
	}
}
