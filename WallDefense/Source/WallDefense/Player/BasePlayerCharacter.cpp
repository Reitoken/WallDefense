#include "BasePlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Weapons/WeaponComponent.h"

ABasePlayerCharacter::ABasePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.f, 720.f, 0.f);
		Movement->bConstrainToPlane = true;
		Movement->bSnapToPlaneAtStart = true;
	}

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetupAttachment(GetCapsuleComponent());
	WeaponComponent->SetRelativeLocation(FVector(50.f, 30.f, 20.f));
}

void ABasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABasePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input)
	{
		return;
	}

	if (MoveAction)
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::Move);
	}

	if (WeaponComponent)
	{
		WeaponComponent->BindInput(Input);
	}
}

void ABasePlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (Axis.IsNearlyZero())
	{
		return;
	}

	FVector Forward = FVector::ForwardVector;
	FVector Right = FVector::RightVector;

	if (bUseViewRelativeMovement && Controller)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FRotationMatrix RotMatrix(YawRotation);
		Forward = RotMatrix.GetUnitAxis(EAxis::X);
		Right = RotMatrix.GetUnitAxis(EAxis::Y);
	}

	AddMovementInput(Forward, Axis.Y);
	AddMovementInput(Right, Axis.X);
}
