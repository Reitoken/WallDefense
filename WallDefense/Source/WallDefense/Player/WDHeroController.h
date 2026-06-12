#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/WDTypes.h"
#include "WDHeroController.generated.h"

class AWDHeroCharacter;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * Twin-stick top-down controller, two schemes at once (GDD §4):
 *  - Gamepad (primary target): left stick moves, right stick aims, RT fires, RB switches.
 *  - Keyboard + mouse: WASD/ZQSD moves, mouse aims, left click fires, wheel/Tab switches.
 * Input objects are built in C++ at runtime (debug-first, no assets needed).
 * Remapping migrates this to user-mappable settings at step 6.
 */
UCLASS()
class WALLDEFENSE_API AWDHeroController : public APlayerController
{
	GENERATED_BODY()

public:
	AWDHeroController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	void OnMove(const FInputActionValue& Value);
	void OnAimStick(const FInputActionValue& Value);
	void OnFireStarted(const FInputActionValue& Value);
	void OnFireCompleted(const FInputActionValue& Value);
	void OnNextWeapon(const FInputActionValue& Value);

	AWDHeroCharacter* GetHero() const;

	/** Builds the mapping context + actions in code. */
	void BuildInputObjects();

	UPROPERTY(Transient) TObjectPtr<UInputMappingContext> MappingContext;
	UPROPERTY(Transient) TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(Transient) TObjectPtr<UInputAction> AimAction;
	UPROPERTY(Transient) TObjectPtr<UInputAction> FireAction;
	UPROPERTY(Transient) TObjectPtr<UInputAction> NextWeaponAction;

private:
	/** Last right-stick aim this frame; zero means "use the mouse". */
	FVector2D StickAim = FVector2D::ZeroVector;
};
