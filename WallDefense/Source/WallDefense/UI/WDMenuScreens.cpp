#include "UI/WDMenuScreens.h"
#include "UI/WDWidgetKit.h"
#include "UI/WDAdventureScreen.h"
#include "UI/WDArmoryScreen.h"
#include "Core/WDUISubsystem.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDProgressionMath.h"
#include "Core/WDSaveSubsystem.h"
#include "Core/WDSettingsSubsystem.h"
#include "Monsters/WDMonsterData.h"
#include "Components/Slider.h"
#include "Kismet/KismetSystemLibrary.h"

// ---------------------------------------------------------------- Splash

void UWDSplashScreen::RebuildContent()
{
	AddText(FText::FromString(TEXT("🏰")), 60, FLinearColor::White, 4.f);
	AddTitle(FText::FromString(TEXT("WALL DEFENSE")));
	AddText(NSLOCTEXT("WDUI", "SplashTagline", "Défends le mur. Améliore. Reviens plus forte."), 16, FLinearColor(0.7f, 0.75f, 0.9f), 30.f);
	AddButton(NSLOCTEXT("WDUI", "SplashContinue", "Appuyer pour commencer"), TEXT("HandleContinue"), 22);
}

FReply UWDSplashScreen::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	HandleContinue(); // any key
	return FReply::Handled();
}

void UWDSplashScreen::HandleContinue()
{
	GetUI()->ShowScreenAndClearStack(UWDMainMenuScreen::StaticClass());
}

// ---------------------------------------------------------------- Main menu

void UWDMainMenuScreen::RebuildContent()
{
	AddTitle(FText::FromString(TEXT("WALL DEFENSE")));
	AddButton(NSLOCTEXT("WDUI", "MenuNewGame", "Nouvelle partie"), TEXT("HandleNewGame"));
	AddButton(NSLOCTEXT("WDUI", "MenuLoadGame", "Charger une partie"), TEXT("HandleLoadGame"));
	AddButton(NSLOCTEXT("WDUI", "MenuOptions", "Options"), TEXT("HandleOptions"));
	AddButton(NSLOCTEXT("WDUI", "MenuQuit", "Quitter"), TEXT("HandleQuit"));
}

void UWDMainMenuScreen::HandleNewGame()  { GetUI()->ShowScreen(UWDNewGameSlotsScreen::StaticClass()); }
void UWDMainMenuScreen::HandleLoadGame() { GetUI()->ShowScreen(UWDLoadGameSlotsScreen::StaticClass()); }
void UWDMainMenuScreen::HandleOptions()  { GetUI()->ShowScreen(UWDOptionsScreen::StaticClass()); }

void UWDMainMenuScreen::HandleQuit()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

// ---------------------------------------------------------------- Save slots

FText UWDSaveSlotsScreenBase::GetSlotsTitle() const
{
	return NSLOCTEXT("WDUI", "SlotsLoadTitle", "CHARGER UNE PARTIE");
}

FText UWDNewGameSlotsScreen::GetSlotsTitle() const
{
	return NSLOCTEXT("WDUI", "SlotsNewTitle", "NOUVELLE PARTIE — CHOISIS UN EMPLACEMENT");
}

void UWDSaveSlotsScreenBase::RebuildContent()
{
	AddTitle(GetSlotsTitle());

	UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>();
	for (int32 i = 0; i < UWDSaveSubsystem::SlotCount; ++i)
	{
		FWDProgressionState SlotState;
		const bool bExists = Save && Save->GetSlotState(i, SlotState);

		FText Label;
		if (bExists)
		{
			int32 TotalStars = 0;
			for (const FWDStageRecord& Record : SlotState.StageRecords)
			{
				TotalStars += Record.Stars;
			}
			Label = FText::Format(NSLOCTEXT("WDUI", "SlotSummary", "Slot {0} — Or {1} · Niveau {2} · {3} ★"),
				i + 1, SlotState.Gold, WDProgressionMath::LevelForXP(SlotState.XP), TotalStars);
		}
		else
		{
			Label = FText::Format(NSLOCTEXT("WDUI", "SlotEmpty", "Slot {0} — (vide)"), i + 1);
		}

		UButton* SlotButton = AddButton(Label, FName(*FString::Printf(TEXT("HandleSlot%d"), i)), 18, 8.f);
		SlotButton->SetIsEnabled(IsNewGameMode() || bExists); // loading needs an existing save
	}
	AddBackButton();
}

void UWDSaveSlotsScreenBase::ChooseSlot(int32 SlotIndex)
{
	UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>();
	if (!Save)
	{
		return;
	}
	if (IsNewGameMode())
	{
		Save->NewGameInSlot(SlotIndex);
	}
	else
	{
		Save->LoadSlot(SlotIndex);
	}
	GetUI()->ShowScreenAndClearStack(UWDLobbyScreen::StaticClass());
}

// ---------------------------------------------------------------- Lobby

void UWDLobbyScreen::RebuildContent()
{
	AddTitle(NSLOCTEXT("WDUI", "LobbyTitle", "QUARTIER GÉNÉRAL"));

	if (const UWDProgressionSubsystem* Progression = GetProgression())
	{
		AddText(FText::Format(NSLOCTEXT("WDUI", "HubWallet", "Or : {0}      Niveau : {1}  ({2} XP)"),
			Progression->GetGold(), Progression->GetCharacterLevel(), Progression->GetXP()), 17, FLinearColor(1.f, 0.8f, 0.15f), 20.f);
	}

	AddButton(NSLOCTEXT("WDUI", "LobbyAdventure", "⚔  Partir à l'aventure"), TEXT("HandleAdventure"), 24);
	AddButton(NSLOCTEXT("WDUI", "LobbyArmory", "Armurerie & améliorations"), TEXT("HandleArmory"));
	AddButton(NSLOCTEXT("WDUI", "LobbyEncyclo", "Encyclopédie"), TEXT("HandleEncyclopedia"));
	AddButton(NSLOCTEXT("WDUI", "MenuOptions", "Options"), TEXT("HandleOptions"));
	AddButton(NSLOCTEXT("WDUI", "LobbyMainMenu", "Menu principal"), TEXT("HandleMainMenu"), 15);
}

void UWDLobbyScreen::HandleAdventure()    { GetUI()->ShowScreen(UWDAdventureScreen::StaticClass()); }
void UWDLobbyScreen::HandleArmory()       { GetUI()->ShowScreen(UWDArmoryScreen::StaticClass()); }
void UWDLobbyScreen::HandleEncyclopedia() { GetUI()->ShowScreen(UWDEncyclopediaScreen::StaticClass()); }
void UWDLobbyScreen::HandleOptions()      { GetUI()->ShowScreen(UWDOptionsScreen::StaticClass()); }
void UWDLobbyScreen::HandleMainMenu()     { GetUI()->ShowScreenAndClearStack(UWDMainMenuScreen::StaticClass()); }

// ---------------------------------------------------------------- Options

void UWDOptionsScreen::RebuildContent()
{
	AddTitle(NSLOCTEXT("WDUI", "OptionsTitle", "OPTIONS"));

	// Quality.
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(WidgetTree, NSLOCTEXT("WDUI", "HubQuality", "Qualité :"), 15, FLinearColor(0.62f, 0.66f, 0.8f)), 10.f);
		const TPair<FText, FName> Choices[] = {
			{ NSLOCTEXT("WDUI", "QLow", "Basse"), TEXT("HandleQualityLow") },
			{ NSLOCTEXT("WDUI", "QMedium", "Moyenne"), TEXT("HandleQualityMedium") },
			{ NSLOCTEXT("WDUI", "QHigh", "Haute"), TEXT("HandleQualityHigh") },
			{ NSLOCTEXT("WDUI", "QEpic", "Épique"), TEXT("HandleQualityEpic") } };
		for (const auto& Choice : Choices)
		{
			UButton* Button = WDWidgetKit::MakeTextButton(WidgetTree, Choice.Key, 14);
			WDWidgetKit::BindClick(Button, this, Choice.Value);
			WDWidgetKit::AddCell(Row, Button, 6.f);
			if (!FirstFocus)
			{
				FirstFocus = Button;
			}
		}
		WDWidgetKit::AddRow(ContentColumn, Row, 14.f);
	}

	// Language.
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(WidgetTree, NSLOCTEXT("WDUI", "HubLanguage", "Langue :"), 15, FLinearColor(0.62f, 0.66f, 0.8f)), 10.f);
		UButton* FrenchButton = WDWidgetKit::MakeTextButton(WidgetTree, FText::FromString(TEXT("Français")), 14);
		WDWidgetKit::BindClick(FrenchButton, this, TEXT("HandleLanguageFrench"));
		WDWidgetKit::AddCell(Row, FrenchButton, 6.f);
		UButton* EnglishButton = WDWidgetKit::MakeTextButton(WidgetTree, FText::FromString(TEXT("English")), 14);
		WDWidgetKit::BindClick(EnglishButton, this, TEXT("HandleLanguageEnglish"));
		WDWidgetKit::AddCell(Row, EnglishButton, 0.f);
		WDWidgetKit::AddRow(ContentColumn, Row, 14.f);
	}

	// Volumes (stored now, routed into Sound Mixes when the audio pass lands).
	AddText(NSLOCTEXT("WDUI", "OptVolume", "Volumes (audio à venir) :"), 15, FLinearColor(0.62f, 0.66f, 0.8f), 6.f);
	AddVolumeRow(NSLOCTEXT("WDUI", "OptVolMaster", "Général"), EWDVolumeChannel::Master, TEXT("HandleVolumeMaster"));
	AddVolumeRow(NSLOCTEXT("WDUI", "OptVolMusic", "Musique"), EWDVolumeChannel::Music, TEXT("HandleVolumeMusic"));
	AddVolumeRow(NSLOCTEXT("WDUI", "OptVolSfx", "Effets"), EWDVolumeChannel::Sfx, TEXT("HandleVolumeSfx"));

	AddBackButton();
}

void UWDOptionsScreen::AddVolumeRow(const FText& Label, EWDVolumeChannel Channel, const FName& HandlerName)
{
	UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>();

	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(WidgetTree, Label, 14, FLinearColor::White), 10.f);

	USizeBox* SliderBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	SliderBox->SetWidthOverride(260.f);
	USlider* Slider = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass());
	Slider->SetValue(Settings ? Settings->GetVolume(Channel) : 1.f);
	FScriptDelegate ValueChanged;
	ValueChanged.BindUFunction(this, HandlerName);
	Slider->OnValueChanged.Add(ValueChanged);
	SliderBox->SetContent(Slider);
	WDWidgetKit::AddCell(Row, SliderBox, 0.f);

	WDWidgetKit::AddRow(ContentColumn, Row, 8.f);
}

void UWDOptionsScreen::HandleVolumeMaster(float Value)
{
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>()) { Settings->SetVolume(EWDVolumeChannel::Master, Value); }
}

void UWDOptionsScreen::HandleVolumeMusic(float Value)
{
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>()) { Settings->SetVolume(EWDVolumeChannel::Music, Value); }
}

void UWDOptionsScreen::HandleVolumeSfx(float Value)
{
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>()) { Settings->SetVolume(EWDVolumeChannel::Sfx, Value); }
}

void UWDOptionsScreen::SetQuality(int32 QualityLevel)
{
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>())
	{
		Settings->SetOverallQuality(QualityLevel);
	}
}

void UWDOptionsScreen::SetLanguage(const TCHAR* Culture)
{
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>())
	{
		Settings->SetLanguage(Culture);
	}
}

// ---------------------------------------------------------------- Encyclopedia

void UWDEncyclopediaScreen::RebuildContent()
{
	AddTitle(NSLOCTEXT("WDUI", "EncycloTitle", "ENCYCLOPÉDIE — ZONE 1"));

	if (Bestiary.IsEmpty())
	{
		Bestiary = UWDMonsterData::MakeZone1Bestiary(this);
	}
	const UWDProgressionSubsystem* Progression = GetProgression();
	for (const UWDMonsterData* Monster : Bestiary)
	{
		const FName MonsterName(*Monster->DisplayName.ToString());
		if (!Progression || !Progression->IsMonsterDiscovered(MonsterName))
		{
			AddText(NSLOCTEXT("WDUI", "EncycloUnknown", "?????  —  monstre jamais croisé"), 17, FLinearColor(0.45f, 0.48f, 0.6f), 8.f);
		}
		else if (Progression->IsWeaknessDiscovered(MonsterName) && Monster->ElementalProfile.bHasWeakness)
		{
			AddText(FText::Format(NSLOCTEXT("WDUI", "EncycloWeak", "{0}  —  faiblesse : {1}"),
				Monster->DisplayName, UEnum::GetDisplayValueAsText(Monster->ElementalProfile.Weakness)), 17,
				UWDTypeLibrary::GetElementColor(Monster->ElementalProfile.Weakness), 8.f);
		}
		else
		{
			AddText(FText::Format(NSLOCTEXT("WDUI", "EncycloNoWeak", "{0}  —  faiblesse : ?"), Monster->DisplayName), 17, FLinearColor::White, 8.f);
		}
	}

	AddText(NSLOCTEXT("WDUI", "EncycloHint", "Croise un monstre pour révéler sa page ; touche sa faiblesse pour la confirmer."), 13, FLinearColor(0.45f, 0.48f, 0.6f), 14.f);
	AddBackButton();
}
