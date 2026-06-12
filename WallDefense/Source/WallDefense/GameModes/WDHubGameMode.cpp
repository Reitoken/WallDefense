#include "GameModes/WDHubGameMode.h"
#include "UI/WDHubWidget.h"
#include "Core/WDSettingsSubsystem.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

AWDHubGameMode::AWDHubGameMode()
{
	// Pure UI: nobody to possess, just a camera-less spectator and the mouse.
	DefaultPawnClass = ASpectatorPawn::StaticClass();
	bStartPlayersAsSpectators = true;
}

void AWDHubGameMode::StartPlay()
{
	Super::StartPlay();

	// First launch ever: hardware benchmark -> automatic graphics preset (GDD decision).
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>())
	{
		Settings->RunFirstLaunchSetup();
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(FInputModeUIOnly());

	HubWidget = CreateWidget<UWDHubWidget>(PlayerController, UWDHubWidget::StaticClass());
	if (HubWidget)
	{
		HubWidget->OnStartStageRequested.AddDynamic(this, &AWDHubGameMode::HandleStartStageRequested);
		HubWidget->AddToViewport();
	}
}

void AWDHubGameMode::HandleStartStageRequested(int32 StageNumber, EWDDifficulty Mode)
{
	// Same map, stage rules — the URL options carry the choice (stage + mode).
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)), true,
		FString::Printf(TEXT("game=/Script/WallDefense.WDStageGameMode?WDStage=%d?WDMode=%d"),
			StageNumber, static_cast<int32>(Mode)));
}
