#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasePlayerCharacter.generated.h"

class UInputAction;
class UInputComponent;
class UWeaponComponent;
struct FInputActionValue;

UCLASS(Abstract, Blueprintable)
class WALLDEFENSE_API ABasePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABasePlayerCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = "Player")
	UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Components")
	TObjectPtr<UWeaponComponent> WeaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	TObjectPtr<UInputAction> MoveAction;

	/** If true, movement is taken relative to the controller's view yaw. Else world-space. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	bool bUseViewRelativeMovement = false;
};
