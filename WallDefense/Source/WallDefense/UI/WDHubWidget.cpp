#include "UI/WDHubWidget.h"
#include "UI/WDWidgetKit.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDProgressionMath.h"
#include "Core/WDSaveSubsystem.h"
#include "Core/WDSettingsSubsystem.h"
#include "Wall/WDWallData.h"
#include "Weapons/WDWeaponData.h"
#include "TimerManager.h"

namespace
{
	const FLinearColor MutedText(0.62f, 0.66f, 0.8f);

	/** AddDynamic is a name-stringifying macro — runtime-chosen handlers bind by FName instead. */
	void BindClick(UButton* Button, UObject* Target, const FName& FunctionName)
	{
		FScriptDelegate Delegate;
		Delegate.BindUFunction(Target, FunctionName);
		Button->OnClicked.Add(Delegate);
	}
}

void UWDHubWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!WidgetTree || WidgetTree->RootWidget != nullptr)
	{
		return;
	}
	UBorder* Backdrop = WDWidgetKit::MakeBackdrop(WidgetTree, FLinearColor(0.012f, 0.015f, 0.04f));
	WidgetTree->RootWidget = Backdrop;
	ContentColumn = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	Backdrop->SetContent(ContentColumn);
}

void UWDHubWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Arsenal = UWDWeaponData::MakeDebugArsenal(this);

	if (UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>())
	{
		Progression->OnGoldChanged.AddDynamic(this, &UWDHubWidget::HandleGoldChanged);
		Progression->OnXPChanged.AddDynamic(this, &UWDHubWidget::HandleXPChanged);
		Progression->OnResourceChanged.AddDynamic(this, &UWDHubWidget::HandleResourceChanged);
		Progression->OnWallUpgraded.AddDynamic(this, &UWDHubWidget::HandleWallUpgraded);
		Progression->OnWeaponLeveledUp.AddDynamic(this, &UWDHubWidget::HandleWeaponLeveledUp);
	}
	if (UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>())
	{
		Save->OnSlotLoaded.AddDynamic(this, &UWDHubWidget::HandleSlotLoaded);
	}
	RebuildContent();
}

void UWDHubWidget::NativeDestruct()
{
	if (UWDProgressionSubsystem* Progression = GetGameInstance() ? GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>() : nullptr)
	{
		Progression->OnGoldChanged.RemoveDynamic(this, &UWDHubWidget::HandleGoldChanged);
		Progression->OnXPChanged.RemoveDynamic(this, &UWDHubWidget::HandleXPChanged);
		Progression->OnResourceChanged.RemoveDynamic(this, &UWDHubWidget::HandleResourceChanged);
		Progression->OnWallUpgraded.RemoveDynamic(this, &UWDHubWidget::HandleWallUpgraded);
		Progression->OnWeaponLeveledUp.RemoveDynamic(this, &UWDHubWidget::HandleWeaponLeveledUp);
	}
	if (UWDSaveSubsystem* Save = GetGameInstance() ? GetGameInstance()->GetSubsystem<UWDSaveSubsystem>() : nullptr)
	{
		Save->OnSlotLoaded.RemoveDynamic(this, &UWDHubWidget::HandleSlotLoaded);
	}
	Super::NativeDestruct();
}

void UWDHubWidget::ScheduleRefresh()
{
	// Never rebuild mid-click (the clicked button would die under the mouse): next tick.
	if (bRefreshScheduled || !GetWorld())
	{
		return;
	}
	bRefreshScheduled = true;
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		bRefreshScheduled = false;
		RebuildContent();
	});
}

void UWDHubWidget::RebuildContent()
{
	UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>();
	UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>();
	UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>();
	if (!ContentColumn || !Progression)
	{
		return;
	}
	ContentColumn->ClearChildren();
	UWidgetTree* Tree = WidgetTree;

	WDWidgetKit::AddRow(ContentColumn, WDWidgetKit::MakeText(Tree, NSLOCTEXT("WDUI", "HubTitle", "WALL DEFENSE — QUARTIER GÉNÉRAL"), 30), 16.f);

	// --- Wallet ---
	const FText Wallet = FText::Format(NSLOCTEXT("WDUI", "HubWallet", "Or : {0}      Niveau : {1}  ({2} XP)"),
		Progression->GetGold(), Progression->GetCharacterLevel(), Progression->GetXP());
	WDWidgetKit::AddRow(ContentColumn, WDWidgetKit::MakeText(Tree, Wallet, 20, FLinearColor(1.f, 0.8f, 0.15f)), 4.f);

	FString ResourceLine;
	for (const FWDResourceStack& Stack : Progression->GetState().Resources)
	{
		if (Stack.Amount > 0)
		{
			ResourceLine += FString::Printf(TEXT("%s %s : %d    "),
				*UEnum::GetDisplayValueAsText(Stack.Tier).ToString(), *UEnum::GetDisplayValueAsText(Stack.Element).ToString(), Stack.Amount);
		}
	}
	if (!ResourceLine.IsEmpty())
	{
		WDWidgetKit::AddRow(ContentColumn, WDWidgetKit::MakeText(Tree, FText::FromString(ResourceLine), 14, MutedText), 4.f);
	}

	const int32 BestStars = Progression->GetBestStars(1, EWDDifficulty::Normal);
	if (BestStars >= 0)
	{
		const TCHAR* StarIcons[] = { TEXT("☆☆☆"), TEXT("★☆☆"), TEXT("★★☆"), TEXT("★★★") };
		WDWidgetKit::AddRow(ContentColumn, WDWidgetKit::MakeText(Tree,
			FText::Format(NSLOCTEXT("WDUI", "HubBest", "Meilleur score — Stage 1 : {0}"), FText::FromString(StarIcons[FMath::Clamp(BestStars, 0, 3)])), 14, MutedText), 10.f);
	}

	// --- Wall upgrade ---
	{
		UHorizontalBox* Row = Tree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		const int32 WallLevel = Progression->GetWallLevel();
		const int32 MaxLevel = 4; // debug wall data has 4 levels
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(Tree,
			FText::Format(NSLOCTEXT("WDUI", "HubWall", "Mur — Niveau {0}/{1}"), WallLevel, MaxLevel), 18), 14.f);
		if (WallLevel < MaxLevel)
		{
			UButton* UpgradeButton = WDWidgetKit::MakeTextButton(Tree,
				FText::Format(NSLOCTEXT("WDUI", "HubWallUp", "Améliorer — {0} or"), WDProgressionMath::WallUpgradeGoldCost(WallLevel)), 14);
			UpgradeButton->SetIsEnabled(Progression->GetGold() >= WDProgressionMath::WallUpgradeGoldCost(WallLevel));
			UpgradeButton->OnClicked.AddDynamic(this, &UWDHubWidget::HandleUpgradeWall);
			WDWidgetKit::AddCell(Row, UpgradeButton, 0.f);
		}
		WDWidgetKit::AddRow(ContentColumn, Row, 10.f);
	}

	// --- Weapon upgrades (one row per element, GDD §5.4: gold + the element's material) ---
	const TMap<EWDElement, FName> Handlers = {
		{ EWDElement::Normal, TEXT("HandleUpgradeNormal") }, { EWDElement::Fire, TEXT("HandleUpgradeFire") },
		{ EWDElement::Ice, TEXT("HandleUpgradeIce") }, { EWDElement::Lightning, TEXT("HandleUpgradeLightning") },
		{ EWDElement::Wind, TEXT("HandleUpgradeWind") }, { EWDElement::Light, TEXT("HandleUpgradeLight") },
		{ EWDElement::Dark, TEXT("HandleUpgradeDark") } };

	for (const UWDWeaponData* Weapon : Arsenal)
	{
		UHorizontalBox* Row = Tree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		const int32 Level = Progression->GetWeaponLevel(Weapon->Element);
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(Tree,
			FText::Format(NSLOCTEXT("WDUI", "HubWeapon", "{0} — Nv {1}"), Weapon->DisplayName, Level), 16,
			UWDTypeLibrary::GetElementColor(Weapon->Element)), 14.f);

		const int32 GoldCost = WDProgressionMath::WeaponUpgradeGoldCost(Level);
		const int32 ResourceCost = WDProgressionMath::WeaponUpgradeResourceCost(Level);
		const EWDResourceTier Tier = WDProgressionMath::TierForWeaponLevel(Level + 1);
		UButton* UpgradeButton = WDWidgetKit::MakeTextButton(Tree,
			FText::Format(NSLOCTEXT("WDUI", "HubWeaponUp", "Améliorer — {0} or + {1} {2}"),
				GoldCost, ResourceCost, UEnum::GetDisplayValueAsText(Tier)), 13);
		UpgradeButton->SetIsEnabled(Progression->GetGold() >= GoldCost && Progression->GetResourceAmount(Weapon->Element, Tier) >= ResourceCost);
		if (const FName* Handler = Handlers.Find(Weapon->Element))
		{
			BindClick(UpgradeButton, this, *Handler);
		}
		WDWidgetKit::AddCell(Row, UpgradeButton, 0.f);
		WDWidgetKit::AddRow(ContentColumn, Row, 6.f);
	}

	// --- Save slots ---
	if (Save)
	{
		UHorizontalBox* Row = Tree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(Tree, NSLOCTEXT("WDUI", "HubSlots", "Sauvegarde :"), 14, MutedText), 10.f);
		for (int32 i = 0; i < UWDSaveSubsystem::SlotCount; ++i)
		{
			FString Label = FString::Printf(TEXT("Slot %d"), i + 1);
			if (Save->GetActiveSlotIndex() == i)
			{
				Label = TEXT("● ") + Label;
			}
			else if (!Save->DoesSlotExist(i))
			{
				Label += TEXT(" (vide)");
			}
			UButton* SlotButton = WDWidgetKit::MakeTextButton(Tree, FText::FromString(Label), 13);
			BindClick(SlotButton, this, FName(*FString::Printf(TEXT("HandleSlot%d"), i)));
			WDWidgetKit::AddCell(Row, SlotButton, 6.f);
		}
		WDWidgetKit::AddRow(ContentColumn, Row, 10.f);
	}

	// --- Options: quality + language ---
	if (Settings)
	{
		UHorizontalBox* Row = Tree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(Tree, NSLOCTEXT("WDUI", "HubQuality", "Qualité :"), 14, MutedText), 10.f);
		const TPair<FText, FName> QualityChoices[] = {
			{ NSLOCTEXT("WDUI", "QLow", "Basse"), TEXT("HandleQualityLow") },
			{ NSLOCTEXT("WDUI", "QMedium", "Moyenne"), TEXT("HandleQualityMedium") },
			{ NSLOCTEXT("WDUI", "QHigh", "Haute"), TEXT("HandleQualityHigh") },
			{ NSLOCTEXT("WDUI", "QEpic", "Épique"), TEXT("HandleQualityEpic") } };
		for (const auto& Choice : QualityChoices)
		{
			UButton* QualityButton = WDWidgetKit::MakeTextButton(Tree, Choice.Key, 13);
			BindClick(QualityButton, this, Choice.Value);
			WDWidgetKit::AddCell(Row, QualityButton, 6.f);
		}

		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(Tree, NSLOCTEXT("WDUI", "HubLanguage", "Langue :"), 14, MutedText), 10.f);
		UButton* FrenchButton = WDWidgetKit::MakeTextButton(Tree, FText::FromString(TEXT("FR")), 13);
		FrenchButton->OnClicked.AddDynamic(this, &UWDHubWidget::HandleLanguageFrench);
		WDWidgetKit::AddCell(Row, FrenchButton, 6.f);
		UButton* EnglishButton = WDWidgetKit::MakeTextButton(Tree, FText::FromString(TEXT("EN")), 13);
		EnglishButton->OnClicked.AddDynamic(this, &UWDHubWidget::HandleLanguageEnglish);
		WDWidgetKit::AddCell(Row, EnglishButton, 0.f);
		WDWidgetKit::AddRow(ContentColumn, Row, 18.f);
	}

	// --- Launch ---
	UButton* StartButton = WDWidgetKit::MakeTextButton(Tree, NSLOCTEXT("WDUI", "HubStart", "⚔  LANCER LE STAGE 1"), 24);
	StartButton->OnClicked.AddDynamic(this, &UWDHubWidget::HandleStartStage);
	WDWidgetKit::AddRow(ContentColumn, StartButton, 0.f);
}

void UWDHubWidget::HandleStartStage()
{
	OnStartStageRequested.Broadcast();
}

void UWDHubWidget::HandleUpgradeWall()
{
	if (UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>())
	{
		if (Progression->TryUpgradeWall(/*MaxLevel=*/4))
		{
			if (UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>())
			{
				Save->SaveActive();
			}
		}
	}
}

void UWDHubWidget::TryUpgradeWeapon(EWDElement Element)
{
	if (UWDProgressionSubsystem* Progression = GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>())
	{
		if (Progression->TryLevelUpWeapon(Element))
		{
			if (UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>())
			{
				Save->SaveActive();
			}
		}
	}
}

void UWDHubWidget::SelectSlot(int32 SlotIndex)
{
	if (UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>())
	{
		Save->LoadSlot(SlotIndex);
	}
}

void UWDHubWidget::SetQuality(int32 QualityLevel)
{
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>())
	{
		Settings->SetOverallQuality(QualityLevel);
	}
}

void UWDHubWidget::SetLanguage(const TCHAR* Culture)
{
	if (UWDSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UWDSettingsSubsystem>())
	{
		Settings->SetLanguage(Culture);
	}
}
