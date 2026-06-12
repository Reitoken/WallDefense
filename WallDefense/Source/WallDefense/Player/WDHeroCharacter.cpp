#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroMath.h"
#include "Core/WDDebugSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AWDHeroCharacter::AWDHeroCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// She faces the aim, never the movement (twin-stick).
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	// Fixed top-down camera: ignores pawn/controller rotation entirely.
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = CameraDistance;
	CameraArm->SetRelativeRotation(FRotator(CameraPitch, 0.f, 0.f));
	CameraArm->bUsePawnControlRotation = false;
	CameraArm->bInheritPitch = false;
	CameraArm->bInheritYaw = false;
	CameraArm->bInheritRoll = false;
	CameraArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm);
}

void AWDHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	DesiredYaw = GetActorRotation().Yaw;
	CameraArm->TargetArmLength = CameraDistance;
	CameraArm->SetRelativeRotation(FRotator(CameraPitch, 0.f, 0.f));
}

void AWDHeroCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Smoothly turn toward the aim.
	FRotator Rotation = GetActorRotation();
	Rotation.Yaw = FMath::FInterpTo(Rotation.Yaw, DesiredYaw, DeltaSeconds, FacingInterpSpeed);
	SetActorRotation(Rotation);

	// Debug-first: show the aim while there is no mesh (capsule stage).
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		const FVector Start = GetActorLocation();
		Debug->DrawLine(Start, Start + GetActorForwardVector() * 200.f, EWDElement::Normal, 0.f, 3.f);
	}
}

void AWDHeroCharacter::Move(const FVector2D& Input)
{
	const FVector Direction = WDHeroMath::InputToWorldDirection(Input);
	if (!Direction.IsNearlyZero())
	{
		AddMovementInput(Direction, FMath::Min(Input.Size(), 1.f));
	}
}

void AWDHeroCharacter::SetAimDirection(const FVector& WorldDirection)
{
	const FVector Flat = WorldDirection.GetSafeNormal2D();
	if (!Flat.IsNearlyZero())
	{
		DesiredYaw = WDHeroMath::DirectionToYaw(Flat);
	}
}

float AWDHeroCharacter::GetSpeed() const
{
	return GetVelocity().Size2D();
}

bool AWDHeroCharacter::IsMoving() const
{
	return GetSpeed() > IdleSpeedThreshold;
}

float AWDHeroCharacter::GetMoveDirectionAngle() const
{
	return WDHeroMath::ComputeMoveDirectionAngle(GetVelocity(), GetActorForwardVector());
}
