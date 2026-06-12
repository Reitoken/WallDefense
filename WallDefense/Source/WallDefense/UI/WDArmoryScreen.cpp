#include "UI/WDArmoryScreen.h"
#include "UI/WDWidgetKit.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDProgressionMath.h"
#include "Core/WDSaveSubsystem.h"
#include "Weapons/WDWeaponData.h"

void UWDArmoryScreen::NativeConstruct()
{
	Arsenal = UWDWeaponData::MakeDebugArsenal(this);

	if (UWDProgressionSubsystem* Progression = GetProgression())
	{
		Progression->OnGoldChanged.AddDynamic(this, &UWDArmoryScreen::HandleGoldChanged);
		Progression->OnResourceChanged.AddDynamic(this, &UWDArmoryScreen::HandleResourceChanged);
		Progression->OnWallUpgraded.AddDynamic(this, &UWDArmoryScreen::HandleWallUpgraded);
		Progression->OnWeaponLeveledUp.AddDynamic(this, &UWDArmoryScreen::HandleWeaponLeveledUp);
	}
	Super::NativeConstruct(); // builds the content
}

void UWDArmoryScreen::NativeDestruct()
{
	if (UWDProgressionSubsystem* Progression = GetProgression())
	{
		Progression->OnGoldChanged.RemoveDynamic(this, &UWDArmoryScreen::HandleGoldChanged);
		Progression->OnResourceChanged.RemoveDynamic(this, &UWDArmoryScreen::HandleResourceChanged);
		Progression->OnWallUpgraded.RemoveDynamic(this, &UWDArmoryScreen::HandleWallUpgraded);
		Progression->OnWeaponLeveledUp.RemoveDynamic(this, &UWDArmoryScreen::HandleWeaponLeveledUp);
	}
	Super::NativeDestruct();
}

void UWDArmoryScreen::RebuildContent()
{
	UWDProgressionSubsystem* Progression = GetProgression();
	if (!Progression)
	{
		return;
	}

	AddTitle(NSLOCTEXT("WDUI", "ArmoryTitle", "ARMURERIE & AMÉLIORATIONS"));

	// Wallet + materials.
	AddText(FText::Format(NSLOCTEXT("WDUI", "HubWallet", "Or : {0}      Niveau : {1}  ({2} XP)"),
		Progression->GetGold(), Progression->GetCharacterLevel(), Progression->GetXP()), 17, FLinearColor(1.f, 0.8f, 0.15f), 4.f);

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
		AddText(FText::FromString(ResourceLine), 13, FLinearColor(0.62f, 0.66f, 0.8f), 14.f);
	}

	// The wall.
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		const int32 WallLevel = Progression->GetWallLevel();
		const int32 MaxLevel = 4; // debug wall data
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(WidgetTree,
			FText::Format(NSLOCTEXT("WDUI", "HubWall", "Mur — Niveau {0}/{1}"), WallLevel, MaxLevel), 17), 14.f);
		if (WallLevel < MaxLevel)
		{
			const int32 Cost = WDProgressionMath::WallUpgradeGoldCost(WallLevel);
			UButton* UpgradeButton = WDWidgetKit::MakeTextButton(WidgetTree,
				FText::Format(NSLOCTEXT("WDUI", "HubWallUp", "Améliorer — {0} or"), Cost), 14);
			UpgradeButton->SetIsEnabled(Progression->GetGold() >= Cost);
			WDWidgetKit::BindClick(UpgradeButton, this, TEXT("HandleUpgradeWall"));
			WDWidgetKit::AddCell(Row, UpgradeButton, 0.f);
		}
		WDWidgetKit::AddRow(ContentColumn, Row, 10.f);
	}

	// The 7 weapons.
	const TMap<EWDElement, FName> Handlers = {
		{ EWDElement::Normal, TEXT("HandleUpgradeNormal") }, { EWDElement::Fire, TEXT("HandleUpgradeFire") },
		{ EWDElement::Ice, TEXT("HandleUpgradeIce") }, { EWDElement::Lightning, TEXT("HandleUpgradeLightning") },
		{ EWDElement::Wind, TEXT("HandleUpgradeWind") }, { EWDElement::Light, TEXT("HandleUpgradeLight") },
		{ EWDElement::Dark, TEXT("HandleUpgradeDark") } };

	for (const UWDWeaponData* Weapon : Arsenal)
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		const int32 Level = Progression->GetWeaponLevel(Weapon->Element);
		WDWidgetKit::AddCell(Row, WDWidgetKit::MakeText(WidgetTree,
			FText::Format(NSLOCTEXT("WDUI", "HubWeapon", "{0} — Nv {1}"), Weapon->DisplayName, Level), 15,
			UWDTypeLibrary::GetElementColor(Weapon->Element)), 14.f);

		const int32 GoldCost = WDProgressionMath::WeaponUpgradeGoldCost(Level);
		const int32 ResourceCost = WDProgressionMath::WeaponUpgradeResourceCost(Level);
		const EWDResourceTier Tier = WDProgressionMath::TierForWeaponLevel(Level + 1);
		UButton* UpgradeButton = WDWidgetKit::MakeTextButton(WidgetTree,
			FText::Format(NSLOCTEXT("WDUI", "HubWeaponUp", "Améliorer — {0} or + {1} {2}"),
				GoldCost, ResourceCost, UEnum::GetDisplayValueAsText(Tier)), 13);
		UpgradeButton->SetIsEnabled(Progression->GetGold() >= GoldCost && Progression->GetResourceAmount(Weapon->Element, Tier) >= ResourceCost);
		if (const FName* Handler = Handlers.Find(Weapon->Element))
		{
			WDWidgetKit::BindClick(UpgradeButton, this, *Handler);
		}
		WDWidgetKit::AddCell(Row, UpgradeButton, 0.f);
		WDWidgetKit::AddRow(ContentColumn, Row, 6.f);
	}

	AddBackButton();
}

void UWDArmoryScreen::HandleUpgradeWall()
{
	if (UWDProgressionSubsystem* Progression = GetProgression())
	{
		if (Progression->TryUpgradeWall(/*MaxLevel=*/4))
		{
			AutoSave();
		}
	}
}

void UWDArmoryScreen::TryUpgradeWeapon(EWDElement Element)
{
	if (UWDProgressionSubsystem* Progression = GetProgression())
	{
		if (Progression->TryLevelUpWeapon(Element))
		{
			AutoSave();
		}
	}
}

void UWDArmoryScreen::AutoSave()
{
	if (UWDSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UWDSaveSubsystem>())
	{
		Save->SaveActive();
	}
}
