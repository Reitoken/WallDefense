#include "Core/WDUISubsystem.h"
#include "UI/WDMenuScreen.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"

UWDMenuScreen* UWDUISubsystem::ShowScreen(TSubclassOf<UWDMenuScreen> ScreenClass, bool bRememberCurrent)
{
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (!ScreenClass || !PlayerController)
	{
		return nullptr;
	}

	if (IsValid(CurrentScreen))
	{
		if (bRememberCurrent)
		{
			BackStack.Push(CurrentScreen->GetClass());
		}
		CurrentScreen->RemoveFromParent();
	}

	// Menus own the input: cursor + UI focus (gamepad navigates via widget focus).
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(FInputModeUIOnly());

	CurrentScreen = CreateWidget<UWDMenuScreen>(PlayerController, ScreenClass);
	if (CurrentScreen)
	{
		CurrentScreen->AddToViewport(/*ZOrder=*/60);
		OnScreenChanged.Broadcast(CurrentScreen);
	}
	return CurrentScreen;
}

UWDMenuScreen* UWDUISubsystem::ShowScreenAndClearStack(TSubclassOf<UWDMenuScreen> ScreenClass)
{
	BackStack.Reset();
	return ShowScreen(ScreenClass, /*bRememberCurrent=*/false);
}

void UWDUISubsystem::GoBack()
{
	if (BackStack.Num() == 0)
	{
		return;
	}
	const TSubclassOf<UWDMenuScreen> Previous = BackStack.Pop();
	ShowScreen(Previous, /*bRememberCurrent=*/false);
}
