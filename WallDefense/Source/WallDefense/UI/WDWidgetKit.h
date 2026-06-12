#pragma once

#include "CoreMinimal.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/CoreStyle.h"

/**
 * Tiny helpers to build the debug-first menu trees in C++ (no Blueprint design
 * needed, same philosophy as the loading screen). BP subclasses restyle later.
 */
namespace WDWidgetKit
{
	inline UTextBlock* MakeText(UWidgetTree* Tree, const FText& Text, int32 FontSize, const FLinearColor& Color = FLinearColor::White)
	{
		UTextBlock* Block = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Block->SetText(Text);
		Block->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", FontSize));
		Block->SetColorAndOpacity(FSlateColor(Color));
		return Block;
	}

	/** A button with a centered text label. */
	inline UButton* MakeTextButton(UWidgetTree* Tree, const FText& Label, int32 FontSize)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass());
		UTextBlock* Text = MakeText(Tree, Label, FontSize, FLinearColor(0.05f, 0.06f, 0.1f));
		Button->AddChild(Text);
		return Button;
	}

	/** Fullscreen colored backdrop with centered content. */
	inline UBorder* MakeBackdrop(UWidgetTree* Tree, const FLinearColor& Color)
	{
		UBorder* Border = Tree->ConstructWidget<UBorder>(UBorder::StaticClass());
		Border->SetBrushColor(Color);
		Border->SetHorizontalAlignment(HAlign_Center);
		Border->SetVerticalAlignment(VAlign_Center);
		return Border;
	}

	inline UVerticalBoxSlot* AddRow(UVerticalBox* Box, UWidget* Child, float PaddingBottom = 10.f)
	{
		UVerticalBoxSlot* RowSlot = Box->AddChildToVerticalBox(Child);
		RowSlot->SetPadding(FMargin(0.f, 0.f, 0.f, PaddingBottom));
		RowSlot->SetHorizontalAlignment(HAlign_Center);
		return RowSlot;
	}

	inline UHorizontalBoxSlot* AddCell(UHorizontalBox* Box, UWidget* Child, float PaddingRight = 10.f)
	{
		UHorizontalBoxSlot* CellSlot = Box->AddChildToHorizontalBox(Child);
		CellSlot->SetPadding(FMargin(0.f, 0.f, PaddingRight, 0.f));
		CellSlot->SetVerticalAlignment(VAlign_Center);
		return CellSlot;
	}
}
