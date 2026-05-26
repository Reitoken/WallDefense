#include "Weapon.h"

#include "Bullet.h"
#include "BulletPool.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(WeaponMesh);
	Muzzle->SetRelativeLocation(FVector(100.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		WeaponMesh->SetStaticMesh(CubeFinder.Object);
		WeaponMesh->SetRelativeScale3D(FVector(1.f, 0.2f, 0.2f));
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereFinder.Succeeded())
	{
		DefaultBulletMesh = SphereFinder.Object;
	}

	BulletClass = ABullet::StaticClass();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	EnsureBulletPool();

	if (!BulletClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] AWeapon has no BulletClass set — Fire() will do nothing."), *GetName());
	}
	if (WeaponMesh && !WeaponMesh->GetStaticMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] AWeapon has no WeaponMesh assigned — weapon will be invisible."), *GetName());
	}
	if (!DefaultBulletMesh && AmmoTypes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] AWeapon has no DefaultBulletMesh and no AmmoTypes — bullets will be invisible."), *GetName());
	}
}

void AWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopFire();

	if (BulletPool)
	{
		BulletPool->Shutdown();
		BulletPool = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AWeapon::EnsureBulletPool()
{
	if (BulletPool || !BulletClass)
	{
		return;
	}

	BulletPool = NewObject<UBulletPool>(this);
	BulletPool->Initialize(GetWorld(), BulletClass, this, PoolSize);
}

void AWeapon::SetLevel(int32 NewLevel)
{
	const int32 Clamped = FMath::Clamp(NewLevel, 1, MaxLevel);
	if (Clamped == Level)
	{
		return;
	}
	Level = Clamped;
	OnLevelChanged.Broadcast(Level);

	if (UWorld* World = GetWorld())
	{
		if (World->GetTimerManager().IsTimerActive(FireTimerHandle))
		{
			StartFire();
		}
	}
}

void AWeapon::AddLevel(int32 Delta)
{
	SetLevel(Level + Delta);
}

void AWeapon::SetAmmoTypeIndex(int32 Index)
{
	if (AmmoTypes.Num() == 0)
	{
		CurrentAmmoTypeIndex = 0;
		return;
	}
	const int32 Clamped = FMath::Clamp(Index, 0, AmmoTypes.Num() - 1);
	if (Clamped == CurrentAmmoTypeIndex)
	{
		return;
	}
	CurrentAmmoTypeIndex = Clamped;
	OnAmmoTypeChanged.Broadcast(CurrentAmmoTypeIndex);
}

void AWeapon::CycleAmmoType(int32 Delta)
{
	if (AmmoTypes.Num() == 0)
	{
		return;
	}
	const int32 N = AmmoTypes.Num();
	const int32 NewIndex = ((CurrentAmmoTypeIndex + Delta) % N + N) % N;
	SetAmmoTypeIndex(NewIndex);
}

float AWeapon::GetDamage() const
{
	return BaseDamage + DamagePerLevel * (Level - 1);
}

float AWeapon::GetBulletSpeed() const
{
	return BaseBulletSpeed + BulletSpeedPerLevel * (Level - 1);
}

float AWeapon::GetFireRate() const
{
	return BaseFireRate + FireRatePerLevel * (Level - 1);
}

float AWeapon::GetMaxRange() const
{
	return BaseMaxRange + MaxRangePerLevel * (Level - 1);
}

int32 AWeapon::GetMaxTargets() const
{
	return BaseMaxTargets + FMath::FloorToInt(MaxTargetsPerLevel * (Level - 1));
}

float AWeapon::GetKnockback() const
{
	return BaseKnockback + KnockbackPerLevel * (Level - 1);
}

float AWeapon::GetAreaOfEffect() const
{
	return BaseAreaOfEffect + AreaOfEffectPerLevel * (Level - 1);
}

void AWeapon::StartFire()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	Fire();

	const float Interval = 1.f / FMath::Max(0.01f, GetFireRate());
	World->GetTimerManager().SetTimer(FireTimerHandle, this, &AWeapon::Fire, Interval, true);
}

void AWeapon::StopFire()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

void AWeapon::Fire()
{
	EnsureBulletPool();
	if (!BulletPool || !Muzzle)
	{
		return;
	}

	const FAmmoTypeData* Ammo = AmmoTypes.IsValidIndex(CurrentAmmoTypeIndex) ? &AmmoTypes[CurrentAmmoTypeIndex] : nullptr;

	const float DamageOut = GetDamage() * (Ammo ? Ammo->DamageMultiplier : 1.f);
	const float SpeedOut = GetBulletSpeed() * (Ammo ? Ammo->SpeedMultiplier : 1.f);
	const float RangeOut = GetMaxRange() * (Ammo ? Ammo->RangeMultiplier : 1.f);
	const int32 PierceOut = GetMaxTargets() + (Ammo ? Ammo->PierceBonus : 0);
	const float KnockbackOut = GetKnockback() + (Ammo ? Ammo->KnockbackForceBonus : 0.f);

	UStaticMesh* BulletMeshOut = (Ammo && Ammo->BulletMesh) ? Ammo->BulletMesh.Get() : DefaultBulletMesh.Get();
	const FLinearColor Tint = Ammo ? Ammo->Tint : FLinearColor::White;

	const FVector Origin = Muzzle->GetComponentLocation();
	const FVector BaseDir = Muzzle->GetForwardVector();

	AActor* InstigatorActor = GetOwner();
	const int32 Shots = FMath::Max(1, BulletsPerShot);

	if (FireSound)
	{
		const float Pitch = 1.f + FMath::FRandRange(-FirePitchVariance, FirePitchVariance);
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Origin, FireSoundVolume, Pitch);
	}

	for (int32 i = 0; i < Shots; ++i)
	{
		FVector Direction = BaseDir;
		if (SpreadAngleDegrees > KINDA_SMALL_NUMBER)
		{
			const float HalfSpread = SpreadAngleDegrees * 0.5f;
			float Angle = 0.f;
			if (Shots > 1)
			{
				Angle = FMath::Lerp(-HalfSpread, HalfSpread, static_cast<float>(i) / static_cast<float>(Shots - 1));
			}
			else
			{
				Angle = FMath::FRandRange(-HalfSpread, HalfSpread);
			}
			Direction = BaseDir.RotateAngleAxis(Angle, FVector::UpVector);
		}

		float ActualDamage = DamageOut;
		if (CritChance > 0.f && FMath::FRand() < CritChance)
		{
			ActualDamage *= CritMultiplier;
		}

		if (ABullet* Bullet = BulletPool->Acquire())
		{
			Bullet->SetBulletMesh(BulletMeshOut);
			Bullet->SetBulletTint(Tint);
			Bullet->ActivateBullet(Origin, Direction, SpeedOut, ActualDamage, RangeOut, PierceOut, KnockbackOut, InstigatorActor);
		}
	}

	OnFired.Broadcast();
}
