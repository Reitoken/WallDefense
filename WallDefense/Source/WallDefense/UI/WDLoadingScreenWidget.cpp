#include "UI/WDLoadingScreenWidget.h"
#include "Core/WDPreloadSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Styling/CoreStyle.h"

void UWDLoadingScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// No Blueprint design assigned -> build the debug-first fallback tree.
	if (WidgetTree && WidgetTree->RootWidget == nullptr)
	{
		BuildDebugTree();
	}
}

void UWDLoadingScreenWidget::BuildDebugTree()
{
	UBorder* Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Background"));
	Background->SetBrushColor(FLinearColor(0.01f, 0.012f, 0.03f));
	Background->SetHorizontalAlignment(HAlign_Center);
	Background->SetVerticalAlignment(VAlign_Center);
	WidgetTree->RootWidget = Background;

	UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Column"));
	Background->SetContent(Column);

	DebugTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
	DebugTitle->SetText(NSLOCTEXT("WDUI", "LoadingTitle", "CHARGEMENT…"));
	DebugTitle->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 30));
	DebugTitle->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	DebugTitle->SetJustification(ETextJustify::Center);
	if (UVerticalBoxSlot* TitleSlot = Column->AddChildToVerticalBox(DebugTitle))
	{
		TitleSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 24.f));
		TitleSlot->SetHorizontalAlignment(HAlign_Center);
	}

	USizeBox* BarBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BarBox"));
	BarBox->SetWidthOverride(420.f);
	BarBox->SetHeightOverride(20.f);
	if (UVerticalBoxSlot* BarSlot = Column->AddChildToVerticalBox(BarBox))
	{
		BarSlot->SetHorizontalAlignment(HAlign_Center);
	}

	DebugProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("Bar"));
	DebugProgressBar->SetPercent(0.f);
	DebugProgressBar->SetFillColorAndOpacity(FLinearColor(0.6f, 0.8f, 1.f));
	BarBox->SetContent(DebugProgressBar);
}

void UWDLoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWDPreloadSubsystem* Preload = GetGameInstance() ? GetGameInstance()->GetSubsystem<UWDPreloadSubsystem>() : nullptr)
	{
		Preload->OnPreloadProgress.AddDynamic(this, &UWDLoadingScreenWidget::HandlePreloadProgress);
		Preload->OnPreloadFinished.AddDynamic(this, &UWDLoadingScreenWidget::HandlePreloadFinished);
	}
}

void UWDLoadingScreenWidget::NativeDestruct()
{
	if (UWDPreloadSubsystem* Preload = GetGameInstance() ? GetGameInstance()->GetSubsystem<UWDPreloadSubsystem>() : nullptr)
	{
		Preload->OnPreloadProgress.RemoveDynamic(this, &UWDLoadingScreenWidget::HandlePreloadProgress);
		Preload->OnPreloadFinished.RemoveDynamic(this, &UWDLoadingScreenWidget::HandlePreloadFinished);
	}
	Super::NativeDestruct();
}

void UWDLoadingScreenWidget::HandlePreloadProgress(float Progress)
{
	if (DebugProgressBar)
	{
		DebugProgressBar->SetPercent(Progress);
	}
	OnLoadingProgress(Progress);
}

void UWDLoadingScreenWidget::HandlePreloadFinished()
{
	OnLoadingFinished();
	RemoveFromParent();
}
