#include "UI/WDPauseWidget.h"
#include "UI/WDWidgetKit.h"

void UWDPauseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!WidgetTree || WidgetTree->RootWidget != nullptr)
	{
		return;
	}

	UBorder* Backdrop = WDWidgetKit::MakeBackdrop(WidgetTree, FLinearColor(0.f, 0.f, 0.02f, 0.78f));
	WidgetTree->RootWidget = Backdrop;

	UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	Backdrop->SetContent(Column);

	WDWidgetKit::AddRow(Column, WDWidgetKit::MakeText(WidgetTree, NSLOCTEXT("WDUI", "PauseTitle", "PAUSE"), 36), 28.f);

	UButton* ResumeButton = WDWidgetKit::MakeTextButton(WidgetTree, NSLOCTEXT("WDUI", "PauseResume", "Reprendre"), 20);
	ResumeButton->OnClicked.AddDynamic(this, &UWDPauseWidget::HandleResumeClicked);
	WDWidgetKit::AddRow(Column, ResumeButton, 12.f);

	UButton* AbandonButton = WDWidgetKit::MakeTextButton(WidgetTree, NSLOCTEXT("WDUI", "PauseAbandon", "Abandonner (défaite, loot conservé)"), 20);
	AbandonButton->OnClicked.AddDynamic(this, &UWDPauseWidget::HandleAbandonClicked);
	WDWidgetKit::AddRow(Column, AbandonButton, 0.f);
}

void UWDPauseWidget::HandleResumeClicked()
{
	OnResumeRequested.Broadcast();
}

void UWDPauseWidget::HandleAbandonClicked()
{
	OnAbandonRequested.Broadcast();
}
