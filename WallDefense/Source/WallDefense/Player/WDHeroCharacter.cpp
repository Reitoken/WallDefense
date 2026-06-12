#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroMath.h"
#include "Core/WDDebugSubsystem.h"
#include "Weapons/WDWeaponInventoryComponent.h"
#include "Weapons/WDWeaponComponent.h"
#include "Weapons/WDWeaponData.h"
#include "Player/WDMagnetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "UObject/ConstructorHelpers.h"

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

	// Visible debug body — the collision capsule never renders in game.
	// Engine basic shapes: nothing to create on the content side (debug-first).
	// Note: UE 5.7 has no Capsule basic shape — Cylinder it is.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));

	DebugBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugBody"));
	DebugBody->SetupAttachment(RootComponent);
	DebugBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CylinderMesh.Succeeded())
	{
		DebugBody->SetStaticMesh(CylinderMesh.Object);
		// Engine cylinder: radius 50, height 100 -> match the collision capsule (34 x 176).
		const float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
		const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		DebugBody->SetRelativeScale3D(FVector(Radius / 50.f, Radius / 50.f, HalfHeight / 50.f));
	}

	WeaponInventory = CreateDefaultSubobject<UWDWeaponInventoryComponent>(TEXT("WeaponInventory"));
	WeaponComponent = CreateDefaultSubobject<UWDWeaponComponent>(TEXT("WeaponComponent"));
	Magnet = CreateDefaultSubobject<UWDMagnetComponent>(TEXT("Magnet"));

	DebugNose = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugNose"));
	DebugNose->SetupAttachment(RootComponent);
	DebugNose->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ConeMesh.Succeeded())
	{
		DebugNose->SetStaticMesh(ConeMesh.Object);
		// A small cone lying forward at chest height: the facing reads instantly from above.
		DebugNose->SetRelativeLocation(FVector(45.f, 0.f, 20.f));
		DebugNose->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
		DebugNose->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.6f));
	}
}

void AWDHeroCharacter::BeginPlay()
{
	// Wire BEFORE Super: the inventory broadcasts its first weapon during its BeginPlay.
	WeaponInventory->OnWeaponSwitched.AddDynamic(this, &AWDHeroCharacter::HandleWeaponSwitched);

	Super::BeginPlay();
	DesiredYaw = GetActorRotation().Yaw;
	CameraArm->TargetArmLength = CameraDistance;
	CameraArm->SetRelativeRotation(FRotator(CameraPitch, 0.f, 0.f));
}

void AWDHeroCharacter::HandleWeaponSwitched(UWDWeaponData* Weapon, int32 Index)
{
	WeaponComponent->SetWeapon(Weapon);
}

bool AWDHeroCharacter::IsFiring() const
{
	return WeaponComponent && WeaponComponent->IsFiring();
}

void AWDHeroCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Smoothly turn toward the aim.
	FRotator Rotation = GetActorRotation();
	Rotation.Yaw = FMath::FInterpTo(Rotation.Yaw, DesiredYaw, DeltaSeconds, FacingInterpSpeed);
	SetActorRotation(Rotation);

	// Debug-first: aim line in the ACTIVE ELEMENT color (reads the switch at a glance).
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		const UWDWeaponData* Active = WeaponInventory->GetActiveWeapon();
		const EWDElement Element = Active ? Active->Element : EWDElement::Normal;
		const FVector Start = GetActorLocation();
		Debug->DrawLine(Start, Start + GetActorForwardVector() * 200.f, Element, 0.f, 3.f);
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
