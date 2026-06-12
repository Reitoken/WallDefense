#include "Weapons/WDTargeting.h"
#include "Combat/WDHealthComponent.h"
#include "EngineUtils.h"

namespace WDTargeting
{
	UWDHealthComponent* GetLiveHealth(const AActor* Actor)
	{
		if (!IsValid(Actor))
		{
			return nullptr;
		}
		UWDHealthComponent* Health = Actor->FindComponentByClass<UWDHealthComponent>();
		return (Health && !Health->IsDead()) ? Health : nullptr;
	}

	AActor* FindNearestDamageable(UWorld* World, const FVector& Origin, float Radius,
		const TSet<TObjectPtr<AActor>>& Ignored, AActor* AlsoIgnore)
	{
		AActor* Best = nullptr;
		float BestDistSq = Radius * Radius;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor == AlsoIgnore || Ignored.Contains(Actor) || !GetLiveHealth(Actor))
			{
				continue;
			}
			const float DistSq = FVector::DistSquared2D(Actor->GetActorLocation(), Origin);
			if (DistSq <= BestDistSq)
			{
				BestDistSq = DistSq;
				Best = Actor;
			}
		}
		return Best;
	}

	TArray<AActor*> FindAllDamageable(UWorld* World, const FVector& Origin, float Radius, AActor* AlsoIgnore)
	{
		TArray<AActor*> Result;
		const float RadiusSq = Radius * Radius;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor != AlsoIgnore && GetLiveHealth(Actor) &&
				FVector::DistSquared2D(Actor->GetActorLocation(), Origin) <= RadiusSq)
			{
				Result.Add(Actor);
			}
		}
		return Result;
	}
}
