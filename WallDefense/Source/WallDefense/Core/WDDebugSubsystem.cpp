#include "Core/WDDebugSubsystem.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> CVarWDDebugDraw(
	TEXT("wd.Debug.Draw"),
	1,
	TEXT("Enable Wall Defense debug drawing (projectiles, impacts, areas). 0 = off, 1 = on."),
	ECVF_Cheat);

bool UWDDebugSubsystem::IsDebugDrawEnabled() const
{
	return CVarWDDebugDraw.GetValueOnGameThread() != 0;
}

void UWDDebugSubsystem::DrawLine(const FVector& Start, const FVector& End, EWDElement Element, float Duration, float Thickness)
{
	if (!IsDebugDrawEnabled())
	{
		return;
	}
	DrawDebugLine(GetWorld(), Start, End, UWDTypeLibrary::GetElementColor(Element).ToFColor(true), false, Duration, 0, Thickness);
}

void UWDDebugSubsystem::DrawSphere(const FVector& Center, float Radius, EWDElement Element, float Duration)
{
	if (!IsDebugDrawEnabled())
	{
		return;
	}
	DrawDebugSphere(GetWorld(), Center, Radius, 12, UWDTypeLibrary::GetElementColor(Element).ToFColor(true), false, Duration);
}

void UWDDebugSubsystem::DrawGroundCircle(const FVector& Center, float Radius, EWDElement Element, float Duration)
{
	if (!IsDebugDrawEnabled())
	{
		return;
	}
	DrawDebugCircle(GetWorld(), Center, Radius, 32, UWDTypeLibrary::GetElementColor(Element).ToFColor(true), false, Duration, 0, 2.f,
		FVector(1, 0, 0), FVector(0, 1, 0), false);
}

void UWDDebugSubsystem::DrawImpact(const FVector& Point, EWDElement Element, float Duration)
{
	if (!IsDebugDrawEnabled())
	{
		return;
	}
	DrawDebugPoint(GetWorld(), Point, 12.f, UWDTypeLibrary::GetElementColor(Element).ToFColor(true), false, Duration);
}

void UWDDebugSubsystem::DrawChain(const FVector& From, const FVector& To, EWDElement Element, float Duration)
{
	if (!IsDebugDrawEnabled())
	{
		return;
	}
	const FColor Color = UWDTypeLibrary::GetElementColor(Element).ToFColor(true);
	DrawDebugLine(GetWorld(), From, To, Color, false, Duration, 0, 3.f);
	DrawDebugPoint(GetWorld(), To, 10.f, Color, false, Duration);
}

void UWDDebugSubsystem::DrawText(const FVector& Location, const FString& Text, EWDElement Element, float Duration)
{
	if (!IsDebugDrawEnabled())
	{
		return;
	}
	DrawDebugString(GetWorld(), Location, Text, nullptr, UWDTypeLibrary::GetElementColor(Element).ToFColor(true), Duration, true);
}
