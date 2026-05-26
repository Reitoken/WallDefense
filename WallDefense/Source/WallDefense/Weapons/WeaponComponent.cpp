#include "WeaponComponent.h"

#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "InputAction.h"
#include "Weapon.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] UWeaponComponent has no WeaponClass set — no weapon will spawn. Assign one in the Blueprint defaults."), *GetOwner()->GetName());
	}
	if (!FireAction)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] UWeaponComponent has no FireAction set — fire input will not be bound."), *GetOwner()->GetName());
	}

	SpawnWeapon();
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopFire();
	if (Weapon)
	{
		Weapon->Destroy();
		Weapon = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::SpawnWeapon()
{
	if (Weapon || !WeaponClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Weapon = World->SpawnActor<AWeapon>(WeaponClass, GetComponentTransform(), SpawnParams);
	if (Weapon)
	{
		Weapon->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocket);
	}
}

void UWeaponComponent::BindInput(UEnhancedInputComponent* InputComponent)
{
	if (!InputComponent || !FireAction)
	{
		return;
	}

	InputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UWeaponComponent::HandleFireStarted);
	InputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UWeaponComponent::HandleFireCompleted);
	InputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &UWeaponComponent::HandleFireCompleted);
}

void UWeaponComponent::HandleFireStarted(const FInputActionValue& /*Value*/)
{
	if (bAutoFire)
	{
		StartFire();
	}
	else if (Weapon)
	{
		Weapon->Fire();
	}
}

void UWeaponComponent::HandleFireCompleted(const FInputActionValue& /*Value*/)
{
	if (bAutoFire)
	{
		StopFire();
	}
}

void UWeaponComponent::StartFire()
{
	if (Weapon)
	{
		Weapon->StartFire();
	}
}

void UWeaponComponent::StopFire()
{
	if (Weapon)
	{
		Weapon->StopFire();
	}
}
