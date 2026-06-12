#include "Weapons/WDProjectilePoolSubsystem.h"
#include "Weapons/WDProjectile.h"

AWDProjectile* UWDProjectilePoolSubsystem::FireShot(const FWDShotParams& Params, const FVector& Start, const FVector& Direction)
{
	AWDProjectile* Projectile = nullptr;
	while (FreeProjectiles.Num() > 0 && !Projectile)
	{
		Projectile = FreeProjectiles.Pop();
		if (!IsValid(Projectile))
		{
			Projectile = nullptr;
		}
	}
	if (!Projectile)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Projectile = GetWorld()->SpawnActor<AWDProjectile>(Start, FRotator::ZeroRotator, SpawnParams);
	}
	if (Projectile)
	{
		Projectile->Launch(Params, Start, Direction);
	}
	return Projectile;
}

void UWDProjectilePoolSubsystem::Release(AWDProjectile* Projectile)
{
	if (IsValid(Projectile))
	{
		FreeProjectiles.Add(Projectile);
	}
}

void UWDProjectilePoolSubsystem::Warmup(int32 Count)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int32 i = 0; i < Count; ++i)
	{
		if (AWDProjectile* Projectile = GetWorld()->SpawnActor<AWDProjectile>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
		{
			Projectile->SetActorTickEnabled(false);
			FreeProjectiles.Add(Projectile);
		}
	}
}
