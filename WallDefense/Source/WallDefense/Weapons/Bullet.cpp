#include "Bullet.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(CollisionRadius);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CollisionSphere->SetGenerateOverlapEvents(true);
	SetRootComponent(CollisionSphere);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(BulletMeshScale);

	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
	TrailComponent->SetupAttachment(CollisionSphere);
	TrailComponent->SetAutoActivate(false);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->SetSphereRadius(CollisionRadius);
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABullet::HandleOverlap);
	CollisionSphere->OnComponentHit.AddDynamic(this, &ABullet::HandleHit);

	if (Mesh)
	{
		Mesh->SetRelativeScale3D(BulletMeshScale);
	}

	if (TrailComponent)
	{
		TrailComponent->SetAsset(TrailEffect);
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

void ABullet::ActivateBullet(const FVector& StartLocation, const FVector& Direction, float Speed, float InDamage, float InMaxRange, int32 InMaxTargets, float InKnockback, AActor* InInstigatorActor)
{
	SpawnLocation = StartLocation;
	Damage = InDamage;
	MaxRange = FMath::Max(1.f, InMaxRange);
	RemainingTargets = FMath::Max(1, InMaxTargets);
	KnockbackPower = FMath::Max(0.f, InKnockback);
	TraveledDistance = 0.f;
	AlreadyHitActors.Reset();

	SetInstigator(Cast<APawn>(InInstigatorActor));
	SetOwner(InInstigatorActor);

	const FVector NormDir = Direction.GetSafeNormal();
	CurrentVelocity = NormDir * Speed;

	SetActorLocationAndRotation(StartLocation, NormDir.Rotation());
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	if (TrailComponent && TrailEffect)
	{
		TrailComponent->SetAsset(TrailEffect);
		TrailComponent->Activate(true);
	}

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

	if (TrailComponent)
	{
		TrailComponent->Deactivate();
	}

	if (bDestroyOnDeactivate)
	{
		Destroy();
	}
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
	ProcessImpact(OtherActor, Sweep);
}

void ABullet::HandleHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, FVector /*NormalImpulse*/, const FHitResult& Hit)
{
	ProcessImpact(OtherActor, Hit);
}

void ABullet::ProcessImpact(AActor* OtherActor, const FHitResult& Hit)
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
	ApplyKnockbackToActor(OtherActor);

	const FVector ImpactLoc = Hit.ImpactPoint.IsZero() ? GetActorLocation() : FVector(Hit.ImpactPoint);
	SpawnImpactFX(ImpactLoc);

	OnBulletHit.Broadcast(OtherActor, Hit);

	--RemainingTargets;
	if (RemainingTargets <= 0)
	{
		DeactivateBullet();
	}
}

void ABullet::ApplyKnockbackToActor(AActor* OtherActor)
{
	if (KnockbackPower <= 0.f || !OtherActor)
	{
		return;
	}

	const FVector ImpulseDir = CurrentVelocity.GetSafeNormal();

	if (ACharacter* CharOther = Cast<ACharacter>(OtherActor))
	{
		const FVector Launch = ImpulseDir * KnockbackPower;
		CharOther->LaunchCharacter(Launch, /*bXYOverride*/ true, /*bZOverride*/ false);
	}
}

void ABullet::SpawnImpactFX(const FVector& Location)
{
	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, Location, FRotator::ZeroRotator);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location, ImpactSoundVolume);
	}
}
