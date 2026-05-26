#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UHealthComponent;

UCLASS(Blueprintable)
class WALLDEFENSE_API ATarget : public AActor
{
	GENERATED_BODY()

public:
	ATarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target|Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target|Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target|Components")
	TObjectPtr<UHealthComponent> Health;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDied(AActor* Killer);
};
