#include "BulletPool.h"

#include "Bullet.h"
#include "Engine/World.h"

void UBulletPool::Initialize(UWorld* InWorld, TSubclassOf<ABullet> InBulletClass, AActor* InOwner, int32 Size)
{
	Shutdown();

	if (!InWorld || !InBulletClass || Size <= 0)
	{
		return;
	}

	Bullets.Reserve(Size);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InOwner;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i = 0; i < Size; ++i)
	{
		ABullet* Bullet = InWorld->SpawnActor<ABullet>(InBulletClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Bullet)
		{
			Bullet->DeactivateBullet();
			Bullets.Add(Bullet);
		}
	}

	NextIndex = 0;
}

void UBulletPool::Shutdown()
{
	for (ABullet* Bullet : Bullets)
	{
		if (Bullet)
		{
			Bullet->Destroy();
		}
	}
	Bullets.Reset();
	NextIndex = 0;
}

ABullet* UBulletPool::Acquire()
{
	const int32 Num = Bullets.Num();
	if (Num == 0)
	{
		return nullptr;
	}

	for (int32 Offset = 0; Offset < Num; ++Offset)
	{
		const int32 Idx = (NextIndex + Offset) % Num;
		ABullet* Bullet = Bullets[Idx];
		if (Bullet && !Bullet->IsActive())
		{
			NextIndex = (Idx + 1) % Num;
			return Bullet;
		}
	}

	ABullet* Recycled = Bullets[NextIndex];
	if (Recycled)
	{
		Recycled->DeactivateBullet();
	}
	NextIndex = (NextIndex + 1) % Num;
	return Recycled;
}

void UBulletPool::Release(ABullet* Bullet)
{
	if (Bullet)
	{
		Bullet->DeactivateBullet();
	}
}
