#include "GameModes/WDStageGameMode.h"
#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroController.h"
#include "Wall/WDWall.h"
#include "Wall/WDWallSkillsComponent.h"
#include "Combat/WDHealthComponent.h"
#include "Stage/WDStageDirector.h"
#include "Stage/WDStageData.h"
#include "Stage/WDStageMath.h"
#include "Monsters/WDMonster.h"
#include "Core/WDPreloadSubsystem.h"
#include "Core/WDDebugSubsystem.h"
#include "UI/WDLoadingScreenWidget.h"
#include "Weapons/WDWeaponInventoryComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "TimerManager.h"

namespace
{
	const FVector WallLocation(-900.f, 0.f, 150.f);
	const FVector MonsterSpawnCenter(2200.f, 0.f, 0.f);
	constexpr float MonsterSpawnHalfWidth = 800.f;
}

AWDStageGameMode::AWDStageGameMode()
{
	DefaultPawnClass = AWDHeroCharacter::StaticClass();
	PlayerControllerClass = AWDHeroController::StaticClass();
}

void AWDStageGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// No PlayerStart in the map -> Unreal would spawn nobody. Create one above the origin.
	if (TActorIterator<APlayerStart>(GetWorld()).operator bool() == false)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APlayerStart>(FVector(0.f, 0.f, 200.f), FRotator::ZeroRotator, Params);
		UE_LOG(LogTemp, Warning, TEXT("[WDStage] No PlayerStart found — spawned one at (0,0,200)."));
	}
}

void AWDStageGameMode::StartPlay()
{
	Super::StartPlay();

	if (!Stage)
	{
		Stage = UWDStageData::MakeDebugStage1(this);
	}
	if (!WallData)
	{
		WallData = UWDWallData::MakeDebugWall(this);
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Wall = GetWorld()->SpawnActor<AWDWall>(WallLocation, FRotator::ZeroRotator, Params);
	Wall->InitFromData(WallData, WallLevel);

	Director = GetWorld()->SpawnActor<AWDStageDirector>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
	Director->Configure(Stage, Wall, MonsterSpawnCenter, MonsterSpawnHalfWidth);

	// THE wiring point (ArchitectureTechnique §6.6): victory, defeat, and the wall's repulsion push.
	Wall->Health->OnDied.AddDynamic(this, &AWDStageGameMode::HandleWallDestroyed);
	Wall->Skills->OnSkillTriggered.AddDynamic(this, &AWDStageGameMode::HandleWallSkill);
	Director->OnStageCompleted.AddDynamic(this, &AWDStageGameMode::HandleStageCompleted);

	// Loading screen + preload, then fade in and start the waves.
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		LoadingScreen = CreateWidget<UWDLoadingScreenWidget>(PlayerController, UWDLoadingScreenWidget::StaticClass());
		if (LoadingScreen)
		{
			LoadingScreen->AddToViewport(/*ZOrder=*/100);
		}
	}

	if (UWDPreloadSubsystem* Preload = GetGameInstance()->GetSubsystem<UWDPreloadSubsystem>())
	{
		Preload->OnPreloadFinished.AddDynamic(this, &AWDStageGameMode::HandlePreloadFinished);

		TArray<UObject*> ExtraRoots{ WallData.Get() };
		APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
		UWDWeaponInventoryComponent* Inventory = Pawn ? Pawn->FindComponentByClass<UWDWeaponInventoryComponent>() : nullptr;
		if (Inventory && Inventory->Weapons.Num() > 0)
		{
			ExtraRoots.Append(Inventory->Weapons);
		}
		Preload->PreloadStage(Stage, ExtraRoots, GetWorld());
	}
	else
	{
		HandlePreloadFinished();
	}
}

void AWDStageGameMode::HandlePreloadFinished()
{
	// Fade transition: the loading screen removes itself, the camera fades in.
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (PlayerController->PlayerCameraManager)
		{
			PlayerController->PlayerCameraManager->StartCameraFade(1.f, 0.f, 0.8f, FLinearColor::Black, false, false);
		}
	}

	GetWorldTimerManager().SetTimer(StartTimer, [this]()
	{
		if (IsValid(Wall) && IsValid(Director))
		{
			Wall->HandleStageStarted(); // starting shield fires at wave start
			Director->StartStage();
		}
	}, FMath::Max(StartDelay, 0.01f), false);
}

void AWDStageGameMode::HandleWallSkill(const FWDWallSkillDef& Skill)
{
	if (Skill.Type != EWDWallSkill::RepulsionWave || !IsValid(Wall))
	{
		return;
	}
	// The wall announced the wave; the assembler pushes the monsters (the wall knows none).
	for (TActorIterator<AWDMonster> It(GetWorld()); It; ++It)
	{
		AWDMonster* Monster = *It;
		if (Monster->IsDead())
		{
			continue;
		}
		FVector ClosestPoint;
		const float Distance = Wall->ActorGetDistanceToCollision(Monster->GetActorLocation(), ECC_WorldDynamic, ClosestPoint);
		if (Distance >= 0.f && Distance <= Skill.Radius)
		{
			Monster->PushBack(Skill.Value);
		}
	}
}

void AWDStageGameMode::HandleWallDestroyed(AActor* Killer)
{
	if (bStageOver)
	{
		return;
	}
	bStageOver = true;

	if (IsValid(Director))
	{
		Director->StopStage(/*bDespawnMonsters=*/true);
	}

	const FString Message = TEXT("DÉFAITE — LE MUR EST TOMBÉ. Le loot est conservé : améliore et retente !");
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, Message);
	}
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		Debug->DrawText(WallLocation + FVector(0, 0, 450.f), Message, EWDElement::Dark, 6.f);
	}

	OnStageFinished.Broadcast(false, 0);
	ScheduleRestart(6.f);
}

void AWDStageGameMode::HandleStageCompleted()
{
	if (bStageOver || !IsValid(Wall))
	{
		return;
	}
	bStageOver = true;

	const float HealthPercent = Wall->GetHealthPercent();
	const int32 Stars = WDStageMath::StarsFromWallHealth(HealthPercent);
	const float Multiplier = WDStageMath::RewardMultiplierForStars(Stars);

	const TCHAR* StarIcons[] = { TEXT("☆☆☆"), TEXT("★☆☆"), TEXT("★★☆"), TEXT("★★★") };
	const FString Message = FString::Printf(TEXT("VICTOIRE !  %s — Mur %.0f%% — Récompenses ×%.2f"),
		StarIcons[Stars], HealthPercent * 100.f, Multiplier);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, Message);
	}
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		Debug->DrawText(WallLocation + FVector(0, 0, 450.f), Message, EWDElement::Light, 8.f);
	}

	OnStageFinished.Broadcast(true, Stars);
	ScheduleRestart(8.f);
}

void AWDStageGameMode::ScheduleRestart(float Delay)
{
	GetWorldTimerManager().SetTimer(RestartTimer, this, &AWDStageGameMode::RestartStageLevel, Delay, false);
}

void AWDStageGameMode::RestartStageLevel()
{
	if (UWDPreloadSubsystem* Preload = GetGameInstance()->GetSubsystem<UWDPreloadSubsystem>())
	{
		Preload->OnPreloadFinished.RemoveDynamic(this, &AWDStageGameMode::HandlePreloadFinished);
		Preload->ReleaseBundle();
	}
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}
