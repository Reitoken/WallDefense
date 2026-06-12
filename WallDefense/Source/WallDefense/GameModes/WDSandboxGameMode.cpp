#include "GameModes/WDSandboxGameMode.h"
#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroController.h"
#include "Combat/WDTargetDummy.h"
#include "Combat/WDHealthComponent.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AWDSandboxGameMode::AWDSandboxGameMode()
{
	DefaultPawnClass = AWDHeroCharacter::StaticClass();
	PlayerControllerClass = AWDHeroController::StaticClass();
}

void AWDSandboxGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// No PlayerStart in the map -> Unreal would spawn nobody. Create one above the origin.
	if (TActorIterator<APlayerStart>(GetWorld()).operator bool() == false)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APlayerStart>(FVector(0.f, 0.f, 200.f), FRotator::ZeroRotator, Params);
		UE_LOG(LogTemp, Warning, TEXT("[WDSandbox] No PlayerStart found — spawned one at (0,0,200)."));
	}

	// No training targets in the map -> spawn three with distinct elemental profiles
	// (their tint = their weakness: shoot the matching element).
	if (TActorIterator<AWDTargetDummy>(GetWorld()).operator bool() == false)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Weak to Fire, plain.
		if (AWDTargetDummy* Dummy = GetWorld()->SpawnActor<AWDTargetDummy>(FVector(700.f, -350.f, 130.f), FRotator::ZeroRotator, Params))
		{
			Dummy->Health->ElementalProfile.bHasWeakness = true;
			Dummy->Health->ElementalProfile.Weakness = EWDElement::Fire;
		}
		// Weak to Ice, resists Fire.
		if (AWDTargetDummy* Dummy = GetWorld()->SpawnActor<AWDTargetDummy>(FVector(700.f, 0.f, 130.f), FRotator::ZeroRotator, Params))
		{
			Dummy->Health->ElementalProfile.bHasWeakness = true;
			Dummy->Health->ElementalProfile.Weakness = EWDElement::Ice;
			Dummy->Health->ElementalProfile.Resistances.Add(EWDElement::Fire);
		}
		// Shielded, weak to Light (the anti-shield weapon).
		if (AWDTargetDummy* Dummy = GetWorld()->SpawnActor<AWDTargetDummy>(FVector(700.f, 350.f, 130.f), FRotator::ZeroRotator, Params))
		{
			Dummy->Health->ElementalProfile.bHasWeakness = true;
			Dummy->Health->ElementalProfile.Weakness = EWDElement::Light;
			Dummy->Health->StartingShield = 50.f;
		}
	}
}
