#include "UI/WDAdventureScreen.h"
#include "UI/WDWidgetKit.h"
#include "Core/WDProgressionSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UWDAdventureScreen::RebuildContent()
{
	AddTitle(NSLOCTEXT("WDUI", "AdvTitle", "AVENTURE — CHOISIS TON STAGE"));

	const UWDProgressionSubsystem* Progression = GetProgression();
	if (!Progression)
	{
		return;
	}

	// Difficulty row (modes structure the 1-100 climb, GDD §2.5).
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(WidgetTree, NSLOCTEXT("WDUI", "HubMode", "Mode :"), 15, FLinearColor(0.62f, 0.66f, 0.8f)), 10.f);
		const TTuple<FText, EWDDifficulty, FName> Choices[] = {
			{ NSLOCTEXT("WDUI", "ModeNormal", "Normal"), EWDDifficulty::Normal, FName(TEXT("HandleModeNormal")) },
			{ NSLOCTEXT("WDUI", "ModeHard", "Hard"), EWDDifficulty::Hard, FName(TEXT("HandleModeHard")) },
			{ NSLOCTEXT("WDUI", "ModeHell", "Enfer"), EWDDifficulty::Hell, FName(TEXT("HandleModeHell")) } };
		for (const auto& Choice : Choices)
		{
			FText Label = Choice.Get<0>();
			if (SelectedMode == Choice.Get<1>())
			{
				Label = FText::FromString(TEXT("[ ") + Label.ToString() + TEXT(" ]"));
			}
			UButton* ModeButton = WDWidgetKit::MakeTextButton(WidgetTree, Label, 15);
			ModeButton->SetIsEnabled(Progression->IsStageUnlocked(1, Choice.Get<1>()));
			WDWidgetKit::BindClick(ModeButton, this, Choice.Get<2>());
			WDWidgetKit::AddCell(Row, ModeButton, 6.f);
		}
		WDWidgetKit::AddRow(ContentColumn, Row, 18.f);
	}

	// Zone 1: the five playable stages with their stars in the selected mode.
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(WidgetTree,
			FText::Format(NSLOCTEXT("WDUI", "AdvZone", "Zone {0} :"), 1), 17, FLinearColor::White), 10.f);

		const TCHAR* StarIcons[] = { TEXT("☆☆☆"), TEXT("★☆☆"), TEXT("★★☆"), TEXT("★★★") };
		for (int32 StageIt = 1; StageIt <= 5; ++StageIt)
		{
			const bool bUnlocked = Progression->IsStageUnlocked(StageIt, SelectedMode);
			const int32 Best = Progression->GetBestStars(StageIt, SelectedMode);

			FString Suffix = TEXT("  —");
			if (!bUnlocked)
			{
				Suffix = TEXT("  🔒");
			}
			else if (Best >= 0)
			{
				Suffix = FString(TEXT("  ")) + StarIcons[FMath::Clamp(Best, 0, 3)];
			}
			const FText Label = FText::FromString(FString::Printf(TEXT("%s%s"),
				*FText::Format(NSLOCTEXT("WDUI", "AdvStage", "Stage {0}"), StageIt).ToString(), *Suffix));

			UButton* StageButton = WDWidgetKit::MakeTextButton(WidgetTree, Label, 15);
			StageButton->SetIsEnabled(bUnlocked);
			WDWidgetKit::BindClick(StageButton, this, FName(*FString::Printf(TEXT("HandleStage%d"), StageIt)));
			WDWidgetKit::AddCell(Row, StageButton, 6.f);
			if (bUnlocked && !FirstFocus)
			{
				FirstFocus = StageButton;
			}
		}
		WDWidgetKit::AddRow(ContentColumn, Row, 10.f);
	}

	// Zones 2-6: the grid shows the whole journey; their bestiaries arrive with the content pass.
	for (int32 Zone = 2; Zone <= 6; ++Zone)
	{
		AddText(FText::Format(NSLOCTEXT("WDUI", "AdvComing", "Zone {0} — à venir"), Zone), 15, FLinearColor(0.4f, 0.43f, 0.55f), 6.f);
	}

	AddText(NSLOCTEXT("WDUI", "AdvHint", "Hard se débloque en finissant le stage en Normal ; Enfer en le finissant en Hard."), 13, FLinearColor(0.45f, 0.48f, 0.6f), 14.f);
	AddBackButton();
}

void UWDAdventureScreen::SelectMode(EWDDifficulty Mode)
{
	SelectedMode = Mode;
	ScheduleRebuild();
}

void UWDAdventureScreen::LaunchStage(int32 StageNumber)
{
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)), true,
		FString::Printf(TEXT("game=/Script/WallDefense.WDStageGameMode?WDStage=%d?WDMode=%d"),
			StageNumber, static_cast<int32>(SelectedMode)));
}
