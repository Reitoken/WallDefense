#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponComponent.generated.h"

class AWeapon;
class UEnhancedInputComponent;
class UInputAction;
struct FInputActionValue;

UCLASS(ClassGroup = (Weapon), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Called by the owning pawn from SetupPlayerInputComponent. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void BindInput(UEnhancedInputComponent* InputComponent);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SpawnWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	AWeapon* GetWeapon() const { return Weapon; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName WeaponAttachSocket = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Input")
	TObjectPtr<UInputAction> FireAction;

	/** If true, holding the fire action fires continuously at the weapon's fire rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Input")
	bool bAutoFire = true;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AWeapon> Weapon;

	void HandleFireStarted(const FInputActionValue& Value);
	void HandleFireCompleted(const FInputActionValue& Value);
};
