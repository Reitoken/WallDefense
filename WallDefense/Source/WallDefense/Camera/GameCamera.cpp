#include "GameCamera.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SceneComponent.h"
#include "GameCameraShake.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AGameCamera::AGameCamera()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
}

void AGameCamera::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyCameraSettings();
}

void AGameCamera::BeginPlay()
{
	Super::BeginPlay();
	ApplyCameraSettings();

	if (bAutoActivateOnBeginPlay)
	{
		ActivateAsViewTarget(0.f);
	}
}

void AGameCamera::ApplyCameraSettings()
{
	if (SpringArm)
	{
		SpringArm->TargetArmLength = CameraDistance;
		SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.f));
	}

	if (Camera)
	{
		Camera->SetProjectionMode(bUseOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective);
		Camera->SetFieldOfView(FieldOfView);
		Camera->SetOrthoWidth(OrthoWidth);
	}
}

void AGameCamera::ActivateAsViewTarget(float BlendTime)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetViewTargetWithBlend(this, BlendTime);
	}
}

void AGameCamera::PlayCameraShake(TSubclassOf<UGameCameraShake> ShakeClass, float Scale)
{
	if (!ShakeClass)
	{
		return;
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (APlayerCameraManager* CM = PC->PlayerCameraManager)
		{
			CM->StartCameraShake(ShakeClass, Scale);
		}
	}
}

void AGameCamera::StopAllCameraShakes(bool bImmediately)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (APlayerCameraManager* CM = PC->PlayerCameraManager)
		{
			CM->StopAllCameraShakes(bImmediately);
		}
	}
}
