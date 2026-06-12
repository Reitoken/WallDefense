#include "GameModes/WDStageGameMode.h"
#include "Player/WDHeroCharacter.h"
#include "Player/WDHeroController.h"
#include "Player/WDMagnetComponent.h"
#include "Wall/WDWall.h"
#include "Wall/WDWallSkillsComponent.h"
#include "Combat/WDHealthComponent.h"
#include "Stage/WDStageDirector.h"
#include "Stage/WDStageData.h"
#include "Stage/WDStageMath.h"
#include "Monsters/WDMonster.h"
#include "Core/WDPreloadSubsystem.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDProgressionMath.h"
#include "Core/WDSaveSubsystem.h"
#include "Core/WDDebugSubsystem.h"
#include "UI/WDLoadingScreenWidget.h"
#include "UI/WDStageSummaryWidget.h"
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

	// The hub travels here with ?WDStage=N?WDMode=M (no options = stage 1 Normal).
	StageNumber = FMath::Clamp(UGameplayStatics::GetIntOption(Options, TEXT("WDStage"), 1), 1, 5);
	Mode = static_cast<EWDDifficulty>(FMath::Clamp(UGameplayStatics::GetIntOption(Options, TEXT("WDMode"), 0), 0, 2));

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
		Stage = UWDStageData::MakeZone1Stage(this, StageNumber);
	}
	if (!WallData)
	{
		WallData = UWDWallData::MakeDebugWall(this);
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// The wall fights at the level bought in the hub — upgrading it finally SHOWS in stage.
	int32 WallLevel = WallLevelOverride;
	if (WallLevel <= 0)
	{
		const UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>();
		WallLevel = Progression ? Progression->GetWallLevel() : 1;
	}

	Wall = GetWorld()->SpawnActor<AWDWall>(WallLocation, FRotator::ZeroRotator, Params);
	Wall->InitFromData(WallData, WallLevel);

	Director = GetWorld()->SpawnActor<AWDStageDirector>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
	Director->Configure(Stage, Wall, MonsterSpawnCenter, MonsterSpawnHalfWidth, Mode);

	// THE wiring point (ArchitectureTechnique §6.6): victory, defeat, repulsion, run loot.
	Wall->Health->OnDied.AddDynamic(this, &AWDStageGameMode::HandleWallDestroyed);
	Wall->Skills->OnSkillTriggered.AddDynamic(this, &AWDStageGameMode::HandleWallSkill);
	Director->OnStageCompleted.AddDynamic(this, &AWDStageGameMode::HandleStageCompleted);

	// Loading screen + preload, then fade in and start the waves.
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr)
	{
		if (UWDMagnetComponent* Magnet = Pawn->FindComponentByClass<UWDMagnetComponent>())
		{
			Magnet->OnLootCollected.AddDynamic(this, &AWDStageGameMode::HandleLootCollected);
		}
	}
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

void AWDStageGameMode::HandleLootCollected(EWDLootType Type, EWDElement Element, EWDResourceTier Tier, int32 Amount)
{
	RunLoot.Add(Type, Element, Tier, Amount);
}

void AWDStageGameMode::HandleWallDestroyed(AActor* Killer)
{
	FinishStage(/*bVictory=*/false);
}

void AWDStageGameMode::HandleStageCompleted()
{
	FinishStage(/*bVictory=*/true);
}

void AWDStageGameMode::AbandonStage()
{
	FinishStage(/*bVictory=*/false);
}

void AWDStageGameMode::FinishStage(bool bVictory)
{
	if (bStageOver || !IsValid(Wall))
	{
		return;
	}
	bStageOver = true;

	if (IsValid(Director))
	{
		Director->StopStage(/*bDespawnMonsters=*/!bVictory);
	}

	// Stars from the wall's remaining HP; the loot is ALWAYS kept — defeat just means ×1 (GDD §2.2/§7).
	const int32 Stars = bVictory ? WDStageMath::StarsFromWallHealth(Wall->GetHealthPercent()) : 0;
	const float Multiplier = bVictory ? WDStageMath::RewardMultiplierForStars(Stars) : 1.f;
	const int32 BonusGold = bVictory ? WDProgressionMath::StarBonusGold(Stars, StageNumber) : 0;

	FWDLootBundle Granted = RunLoot; // display fallback if no subsystem (tests/odd worlds)
	int32 CharacterLevel = 1;
	if (UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>())
	{
		Granted = Progression->ApplyRunRewards(RunLoot, Multiplier);
		if (BonusGold > 0)
		{
			Progression->AddGold(BonusGold);
		}
		if (bVictory)
		{
			Progression->RegisterStageResult(StageNumber, Mode, Stars);
		}
		CharacterLevel = Progression->GetCharacterLevel();
	}
	if (UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>())
	{
		Save->SaveActive(); // auto-save the banked run (ArchitectureTechnique §8)
	}

	OnStageFinished.Broadcast(bVictory, Stars);

	// The dopamine screen (GDD §11): verdict, stars, detailed loot, replay/menu.
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		SummaryWidget = CreateWidget<UWDStageSummaryWidget>(PlayerController, UWDStageSummaryWidget::StaticClass());
		if (SummaryWidget)
		{
			SummaryWidget->InitSummary(bVictory, Stars, Granted, Multiplier, BonusGold, CharacterLevel);
			SummaryWidget->OnReplayRequested.AddDynamic(this, &AWDStageGameMode::HandleSummaryReplay);
			SummaryWidget->OnMenuRequested.AddDynamic(this, &AWDStageGameMode::HandleSummaryMenu);
			SummaryWidget->AddToViewport(/*ZOrder=*/80);
		}
	}
}

void AWDStageGameMode::HandleSummaryReplay()
{
	// Same stage, same mode.
	TravelTo(*FString::Printf(TEXT("game=/Script/WallDefense.WDStageGameMode?WDStage=%d?WDMode=%d"),
		StageNumber, static_cast<int32>(Mode)));
}

void AWDStageGameMode::HandleSummaryMenu()
{
	// Back to the headquarters: land straight in the lobby, not on the splash screen.
	TravelTo(TEXT("game=/Script/WallDefense.WDHubGameMode?WDLobby=1"));
}

void AWDStageGameMode::TravelTo(const TCHAR* GameModeOption)
{
	if (UWDPreloadSubsystem* Preload = GetGameInstance()->GetSubsystem<UWDPreloadSubsystem>())
	{
		Preload->OnPreloadFinished.RemoveDynamic(this, &AWDStageGameMode::HandlePreloadFinished);
		Preload->ReleaseBundle();
	}
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)), true, GameModeOption);
}
