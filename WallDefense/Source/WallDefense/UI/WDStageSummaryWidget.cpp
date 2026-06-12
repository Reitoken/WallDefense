#include "UI/WDStageSummaryWidget.h"
#include "UI/WDWidgetKit.h"

void UWDStageSummaryWidget::InitSummary(bool bVictory, int32 Stars, const FWDLootBundle& Granted, float Multiplier, int32 BonusGold, int32 CharacterLevel)
{
	if (!WidgetTree || WidgetTree->RootWidget != nullptr)
	{
		return;
	}

	UBorder* Backdrop = WDWidgetKit::MakeBackdrop(WidgetTree, FLinearColor(0.01f, 0.012f, 0.03f, 0.92f));
	WidgetTree->RootWidget = Backdrop;

	UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	Backdrop->SetContent(Column);

	// Verdict + stars.
	const FText Verdict = bVictory
		? NSLOCTEXT("WDUI", "SummaryVictory", "VICTOIRE !")
		: NSLOCTEXT("WDUI", "SummaryDefeat", "DÉFAITE — le loot est conservé");
	const FLinearColor VerdictColor = bVictory ? FLinearColor(1.f, 0.85f, 0.3f) : FLinearColor(0.9f, 0.25f, 0.2f);
	WDWidgetKit::AddRow(Column, WDWidgetKit::MakeText(WidgetTree, Verdict, 34, VerdictColor), 6.f);

	if (bVictory)
	{
		const TCHAR* StarIcons[] = { TEXT("☆ ☆ ☆"), TEXT("★ ☆ ☆"), TEXT("★ ★ ☆"), TEXT("★ ★ ★") };
		WDWidgetKit::AddRow(Column, WDWidgetKit::MakeText(WidgetTree, FText::FromString(StarIcons[FMath::Clamp(Stars, 0, 3)]), 30, FLinearColor(1.f, 0.85f, 0.3f)), 14.f);
	}

	// Loot lines (already multiplied — what the player actually banked).
	WDWidgetKit::AddRow(Column, WDWidgetKit::MakeText(WidgetTree,
		FText::Format(NSLOCTEXT("WDUI", "SummaryMult", "Multiplicateur de récompenses : ×{0}"), FText::AsNumber(Multiplier)), 16,
		FLinearColor(0.7f, 0.75f, 0.9f)), 12.f);

	FText GoldLine = FText::Format(NSLOCTEXT("WDUI", "SummaryGold", "Or  +{0}"), Granted.Gold + BonusGold);
	if (BonusGold > 0)
	{
		GoldLine = FText::Format(NSLOCTEXT("WDUI", "SummaryGoldBonus", "Or  +{0}  (dont bonus d'étoiles +{1})"), Granted.Gold + BonusGold, BonusGold);
	}
	WDWidgetKit::AddRow(Column, WDWidgetKit::MakeText(WidgetTree, GoldLine, 20, FLinearColor(1.f, 0.8f, 0.15f)), 4.f);

	WDWidgetKit::AddRow(Column, WDWidgetKit::MakeText(WidgetTree,
		FText::Format(NSLOCTEXT("WDUI", "SummaryXP", "XP  +{0}  (niveau du personnage : {1})"), Granted.XP, CharacterLevel), 20,
		FLinearColor(0.45f, 1.f, 0.4f)), 4.f);

	for (const FWDResourceStack& Stack : Granted.Resources)
	{
		const FText Line = FText::Format(NSLOCTEXT("WDUI", "SummaryResource", "{0} ({1})  +{2}"),
			UEnum::GetDisplayValueAsText(Stack.Tier), UEnum::GetDisplayValueAsText(Stack.Element), Stack.Amount);
		WDWidgetKit::AddRow(Column, WDWidgetKit::MakeText(WidgetTree, Line, 20, UWDTypeLibrary::GetElementColor(Stack.Element)), 4.f);
	}

	// Choices.
	UHorizontalBox* Buttons = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UButton* ReplayButton = WDWidgetKit::MakeTextButton(WidgetTree, NSLOCTEXT("WDUI", "SummaryReplay", "Rejouer"), 20);
	ReplayButton->OnClicked.AddDynamic(this, &UWDStageSummaryWidget::HandleReplayClicked);
	WDWidgetKit::AddCell(Buttons, ReplayButton, 16.f);

	UButton* MenuButton = WDWidgetKit::MakeTextButton(WidgetTree, NSLOCTEXT("WDUI", "SummaryMenu", "Quartier général"), 20);
	MenuButton->OnClicked.AddDynamic(this, &UWDStageSummaryWidget::HandleMenuClicked);
	WDWidgetKit::AddCell(Buttons, MenuButton, 0.f);

	UVerticalBoxSlot* ButtonsSlot = Column->AddChildToVerticalBox(Buttons);
	ButtonsSlot->SetPadding(FMargin(0.f, 18.f, 0.f, 0.f));
	ButtonsSlot->SetHorizontalAlignment(HAlign_Center);
}

void UWDStageSummaryWidget::HandleReplayClicked()
{
	OnReplayRequested.Broadcast();
}

void UWDStageSummaryWidget::HandleMenuClicked()
{
	OnMenuRequested.Broadcast();
}
