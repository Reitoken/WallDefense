#include "UI/WDMenuScreen.h"
#include "UI/WDWidgetKit.h"
#include "Core/WDUISubsystem.h"
#include "Core/WDProgressionSubsystem.h"
#include "TimerManager.h"

void UWDMenuScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);

	if (!WidgetTree || WidgetTree->RootWidget != nullptr)
	{
		return;
	}
	UBorder* Backdrop = WDWidgetKit::MakeBackdrop(WidgetTree, FLinearColor(0.012f, 0.015f, 0.04f));
	WidgetTree->RootWidget = Backdrop;
	ContentColumn = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	Backdrop->SetContent(ContentColumn);
}

void UWDMenuScreen::NativeConstruct()
{
	Super::NativeConstruct();
	Rebuild();
}

void UWDMenuScreen::Rebuild()
{
	if (!ContentColumn)
	{
		return;
	}
	ContentColumn->ClearChildren();
	FirstFocus = nullptr;
	RebuildContent();
	if (FirstFocus)
	{
		FirstFocus->SetKeyboardFocus(); // pads navigate from here (arrows / left stick)
	}
}

void UWDMenuScreen::ScheduleRebuild()
{
	// Never rebuild mid-click — the clicked button would die under the mouse.
	if (bRebuildScheduled || !GetWorld())
	{
		return;
	}
	bRebuildScheduled = true;
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		bRebuildScheduled = false;
		Rebuild();
	});
}

FReply UWDMenuScreen::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if ((Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right) && GetUI() && GetUI()->CanGoBack())
	{
		GetUI()->GoBack();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UWDMenuScreen::AddTitle(const FText& Title)
{
	WDWidgetKit::AddRow(ContentColumn, WDWidgetKit::MakeText(WidgetTree, Title, 30), 22.f);
}

void UWDMenuScreen::AddText(const FText& Text, int32 FontSize, const FLinearColor& Color, float PadBottom)
{
	WDWidgetKit::AddRow(ContentColumn, WDWidgetKit::MakeText(WidgetTree, Text, FontSize, Color), PadBottom);
}

UButton* UWDMenuScreen::AddButton(const FText& Label, const FName& HandlerName, int32 FontSize, float PadBottom)
{
	UButton* Button = WDWidgetKit::MakeTextButton(WidgetTree, Label, FontSize);
	WDWidgetKit::BindClick(Button, this, HandlerName);
	WDWidgetKit::AddRow(ContentColumn, Button, PadBottom);
	if (!FirstFocus)
	{
		FirstFocus = Button;
	}
	return Button;
}

void UWDMenuScreen::AddBackButton()
{
	AddButton(NSLOCTEXT("WDUI", "Back", "← Retour"), TEXT("HandleBack"), 15, 0.f);
}

void UWDMenuScreen::HandleBack()
{
	if (GetUI())
	{
		GetUI()->GoBack();
	}
}

UWDUISubsystem* UWDMenuScreen::GetUI() const
{
	return GetGameInstance() ? GetGameInstance()->GetSubsystem<UWDUISubsystem>() : nullptr;
}

UWDProgressionSubsystem* UWDMenuScreen::GetProgression() const
{
	return GetGameInstance() ? GetGameInstance()->GetSubsystem<UWDProgressionSubsystem>() : nullptr;
}
