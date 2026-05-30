#include "LanePlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Lanes/LaneGrid.h"

ALanePlayerCharacter::ALanePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bOrientRotationToMovement = false;
		Move->bConstrainToPlane = false;
		Move->GravityScale = 0.f;
	}
}

void ALanePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->SetMovementMode(MOVE_Flying);
	}

	FixedZ = GetActorLocation().Z;

	ResolveLaneGrid();
	if (LaneGrid)
	{
		CurrentLaneIndex = LaneGrid->GetNearestLaneIndex(GetActorLocation());
		DesiredYaw = LaneGrid->GetLaneForwardVector().Rotation().Yaw;

		const FVector LaneCenter = LaneGrid->GetLaneCenterWorld(CurrentLaneIndex);
		const float Along = FVector::DotProduct(GetActorLocation() - LaneCenter, LaneGrid->GetLaneForwardVector());
		const FVector Snap = LaneGrid->GetLanePointWorld(CurrentLaneIndex, Along);

		SetActorLocation(FVector(Snap.X, Snap.Y, FixedZ));
		SetActorRotation(FRotator(0.f, DesiredYaw, 0.f));
	}
}

ALaneGrid* ALanePlayerCharacter::ResolveLaneGrid()
{
	if (LaneGrid)
	{
		return LaneGrid;
	}

	for (TActorIterator<ALaneGrid> It(GetWorld()); It; ++It)
	{
		LaneGrid = *It;
		break;
	}

	if (!LaneGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No ALaneGrid found in level — lane movement disabled."), *GetName());
	}
	return LaneGrid;
}

void ALanePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			Input->BindAction(MoveAction, ETriggerEvent::Completed, this, &ALanePlayerCharacter::MoveCompleted);
			Input->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ALanePlayerCharacter::MoveCompleted);
		}
	}
}

void ALanePlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AlongInput = Axis.Y;

	const float Horizontal = bInvertLaneDirection ? -Axis.X : Axis.X;
	if (FMath::Abs(Horizontal) >= LaneInputThreshold)
	{
		if (bLaneChangeArmed)
		{
			ChangeLane(Horizontal > 0.f ? +1 : -1);
			bLaneChangeArmed = false;
		}
	}
	else
	{
		bLaneChangeArmed = true;
	}
}

void ALanePlayerCharacter::MoveCompleted(const FInputActionValue& /*Value*/)
{
	AlongInput = 0.f;
	bLaneChangeArmed = true;
}

void ALanePlayerCharacter::ChangeLane(int32 Direction)
{
	if (!LaneGrid)
	{
		return;
	}
	CurrentLaneIndex = FMath::Clamp(CurrentLaneIndex + Direction, 0, LaneGrid->GetNumLanes() - 1);
}

void ALanePlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!LaneGrid)
	{
		return;
	}

	const FVector Forward = LaneGrid->GetLaneForwardVector();
	const FVector Right = LaneGrid->GetLaneRightVector();
	const FVector LaneCenter = LaneGrid->GetLaneCenterWorld(CurrentLaneIndex);

	const FVector Rel = GetActorLocation() - LaneCenter;
	float Along = FVector::DotProduct(Rel, Forward);
	float Lateral = FVector::DotProduct(Rel, Right);

	Along = LaneGrid->ClampAlong(Along + AlongInput * AlongLaneSpeed * DeltaSeconds);
	Lateral = FMath::FInterpTo(Lateral, 0.f, DeltaSeconds, LaneChangeInterpSpeed);

	const FVector NewLoc = LaneCenter + Forward * Along + Right * Lateral;
	SetActorLocation(FVector(NewLoc.X, NewLoc.Y, FixedZ), false);

	if (FMath::Abs(AlongInput) > KINDA_SMALL_NUMBER)
	{
		const float ForwardYaw = Forward.Rotation().Yaw;
		DesiredYaw = (AlongInput >= 0.f) ? ForwardYaw : ForwardYaw + 180.f;
	}

	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), FRotator(0.f, DesiredYaw, 0.f), DeltaSeconds, FacingInterpSpeed);
	SetActorRotation(NewRot);
}
