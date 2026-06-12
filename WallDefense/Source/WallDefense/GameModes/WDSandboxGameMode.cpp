#include "GameModes/WDSandboxGameMode.h"
#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroController.h"
#include "Combat/WDTargetDummy.h"
#include "Combat/WDHealthComponent.h"
#include "Stage/WDStageDirector.h"
#include "Stage/WDStageData.h"
#include "Core/WDDebugSubsystem.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "TimerManager.h"

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

void AWDSandboxGameMode::StartPlay()
{
	Super::StartPlay();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Wall stand-in (step 5 brings the real AWDWall): a wide, tough grey block the monsters attack.
	AWDTargetDummy* WallStandIn = GetWorld()->SpawnActor<AWDTargetDummy>(FVector(-900.f, 0.f, 130.f), FRotator::ZeroRotator, Params);
	if (WallStandIn)
	{
		WallStandIn->SetActorScale3D(FVector(1.f, 14.f, 2.6f));
		WallStandIn->Health->SetMaxHealth(2000.f, /*bRefill=*/true);
		WallStandIn->RespawnDelay = 6.f;
	}

	// Stage director with the zone 1 debug stage; monsters arrive from +X after a short delay.
	AWDStageDirector* Director = GetWorld()->SpawnActor<AWDStageDirector>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
	if (Director && WallStandIn)
	{
		Director->Configure(UWDStageData::MakeDebugStage1(Director), WallStandIn, FVector(2200.f, 0.f, 0.f), 800.f);
		GetWorldTimerManager().SetTimer(StageStartTimer, [Director]()
		{
			if (IsValid(Director))
			{
				Director->StartStage();
			}
		}, 5.f, false);
	}
}
