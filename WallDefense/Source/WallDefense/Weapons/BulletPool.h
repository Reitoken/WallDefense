#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BulletPool.generated.h"

class ABullet;

UCLASS(Blueprintable)
class WALLDEFENSE_API UBulletPool : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UWorld* InWorld, TSubclassOf<ABullet> InBulletClass, AActor* InOwner, int32 Size);
	void Shutdown();

	ABullet* Acquire();
	void Release(ABullet* Bullet);

	UFUNCTION(BlueprintPure, Category = "BulletPool")
	int32 GetPoolSize() const { return Bullets.Num(); }

protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ABullet>> Bullets;

	int32 NextIndex = 0;
};
