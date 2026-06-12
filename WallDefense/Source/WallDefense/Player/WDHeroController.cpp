#include "Player/WDHeroController.h"
#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroMath.h"
#include "Core/WDDebugSubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

AWDHeroController::AWDHeroController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AWDHeroController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, /*Priority=*/0);
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AWDHeroController::SetupInputComponent()
{
	Super::SetupInputComponent();

	BuildInputObjects();

	UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(InputComponent);
	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWDHeroController::OnMove);
	Input->BindAction(AimAction, ETriggerEvent::Triggered, this, &AWDHeroController::OnAimStick);
	Input->BindAction(AimAction, ETriggerEvent::Completed, this, &AWDHeroController::OnAimStick);
	Input->BindAction(FireAction, ETriggerEvent::Started, this, &AWDHeroController::OnFire);
	Input->BindAction(NextWeaponAction, ETriggerEvent::Started, this, &AWDHeroController::OnNextWeapon);
}

namespace
{
	/** Maps a key to a 2D axis component. Axis: 0 = X (right), 1 = Y (up). */
	void MapAxis2DKey(UInputMappingContext* Context, UInputAction* Action, const FKey& Key, int32 Axis, bool bNegate, UObject* Outer)
	{
		FEnhancedActionKeyMapping& Mapping = Context->MapKey(Action, Key);
		if (Axis == 1)
		{
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(Outer);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			Mapping.Modifiers.Add(Swizzle);
		}
		if (bNegate)
		{
			Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(Outer));
		}
	}
}

void AWDHeroController::BuildInputObjects()
{
	MappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Hero"));

	// --- Move (Axis2D: X = right, Y = up) ---
	MoveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
	MoveAction->ValueType = EInputActionValueType::Axis2D;
	MapAxis2DKey(MappingContext, MoveAction, EKeys::D, 0, false, this);
	MapAxis2DKey(MappingContext, MoveAction, EKeys::A, 0, true, this);  // QWERTY
	MapAxis2DKey(MappingContext, MoveAction, EKeys::Q, 0, true, this);  // AZERTY
	MapAxis2DKey(MappingContext, MoveAction, EKeys::W, 1, false, this); // QWERTY
	MapAxis2DKey(MappingContext, MoveAction, EKeys::Z, 1, false, this); // AZERTY
	MapAxis2DKey(MappingContext, MoveAction, EKeys::S, 1, true, this);
	MappingContext->MapKey(MoveAction, EKeys::Gamepad_Left2D);

	// --- Aim (gamepad right stick; the mouse path lives in PlayerTick) ---
	AimAction = NewObject<UInputAction>(this, TEXT("IA_Aim"));
	AimAction->ValueType = EInputActionValueType::Axis2D;
	MappingContext->MapKey(AimAction, EKeys::Gamepad_Right2D);

	// --- Fire ---
	FireAction = NewObject<UInputAction>(this, TEXT("IA_Fire"));
	FireAction->ValueType = EInputActionValueType::Boolean;
	MappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
	MappingContext->MapKey(FireAction, EKeys::Gamepad_RightTrigger);

	// --- Next weapon (placeholder until step 3) ---
	NextWeaponAction = NewObject<UInputAction>(this, TEXT("IA_NextWeapon"));
	NextWeaponAction->ValueType = EInputActionValueType::Boolean;
	MappingContext->MapKey(NextWeaponAction, EKeys::MouseScrollUp);
	MappingContext->MapKey(NextWeaponAction, EKeys::MouseScrollDown);
	MappingContext->MapKey(NextWeaponAction, EKeys::Tab);
	MappingContext->MapKey(NextWeaponAction, EKeys::Gamepad_RightShoulder);
}

void AWDHeroController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	AWDHeroCharacter* Hero = GetHero();
	if (!Hero)
	{
		return;
	}

	if (!StickAim.IsNearlyZero(0.2f))
	{
		// Gamepad aim wins whenever the right stick is pushed.
		Hero->SetAimDirection(WDHeroMath::InputToWorldDirection(StickAim));
	}
	else
	{
		// Mouse aim: intersect the cursor ray with the hero's ground plane (no floor collision needed).
		FVector Origin, Direction;
		if (DeprojectMousePositionToWorld(Origin, Direction) && !FMath::IsNearlyZero(Direction.Z))
		{
			const FVector HeroLocation = Hero->GetActorLocation();
			const float T = (HeroLocation.Z - Origin.Z) / Direction.Z;
			if (T > 0.f)
			{
				const FVector Target = Origin + Direction * T;
				Hero->SetAimDirection(Target - HeroLocation);
			}
		}
	}
}

void AWDHeroController::OnMove(const FInputActionValue& Value)
{
	if (AWDHeroCharacter* Hero = GetHero())
	{
		Hero->Move(Value.Get<FVector2D>());
	}
}

void AWDHeroController::OnAimStick(const FInputActionValue& Value)
{
	StickAim = Value.Get<FVector2D>();
}

void AWDHeroController::OnFire(const FInputActionValue& Value)
{
	// Step 2 placeholder: a debug shot in the current preview element.
	// Validates aiming end-to-end; real weapons arrive at step 3.
	AWDHeroCharacter* Hero = GetHero();
	if (!Hero)
	{
		return;
	}
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		const EWDElement Element = static_cast<EWDElement>(PreviewElementIndex);
		const FVector Start = Hero->GetActorLocation();
		const FVector End = Start + Hero->GetActorForwardVector() * 1200.f;
		Debug->DrawLine(Start, End, Element, 0.5f, 3.f);
		Debug->DrawImpact(End, Element, 0.5f);
	}
}

void AWDHeroController::OnNextWeapon(const FInputActionValue& Value)
{
	PreviewElementIndex = (PreviewElementIndex + 1) % 7;
	if (AWDHeroCharacter* Hero = GetHero())
	{
		if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
		{
			const EWDElement Element = static_cast<EWDElement>(PreviewElementIndex);
			Debug->DrawText(Hero->GetActorLocation() + FVector(0, 0, 120.f),
				StaticEnum<EWDElement>()->GetNameStringByValue(PreviewElementIndex), Element, 1.f);
			Debug->DrawGroundCircle(Hero->GetActorLocation(), 150.f, Element, 1.f);
		}
	}
}

AWDHeroCharacter* AWDHeroController::GetHero() const
{
	return Cast<AWDHeroCharacter>(GetPawn());
}
