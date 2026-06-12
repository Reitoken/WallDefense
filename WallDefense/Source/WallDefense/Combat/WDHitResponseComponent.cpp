#include "Combat/WDHitResponseComponent.h"
#include "Core/WDDebugSubsystem.h"
#include "Core/WDGameFeelSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

void UWDHitResponseComponent::Configure(UStaticMeshComponent* InMesh, const FLinearColor& InBaseColor, bool bInBigDeath)
{
	Mesh = InMesh;
	BaseColor = InBaseColor;
	bBigDeath = bInBigDeath;
}

void UWDHitResponseComponent::HandleDamaged(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match)
{
	// Flash white, restore shortly after.
	if (Mesh.IsValid())
	{
		if (UMaterialInstanceDynamic* Material = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(0)))
		{
			Material->SetVectorParameterValue(TEXT("Color"), FLinearColor::White);
			GetWorld()->GetTimerManager().SetTimer(FlashTimer, this, &UWDHitResponseComponent::RestoreColor, FlashDuration, false);
		}
	}

	// Knockback: nudged away from the impact.
	if (KnockbackDistance > 0.f)
	{
		const FVector Away = (GetOwner()->GetActorLocation() - DamageEvent.ImpactPoint).GetSafeNormal2D();
		if (!Away.IsNearlyZero())
		{
			GetOwner()->AddActorWorldOffset(Away * KnockbackDistance, /*bSweep=*/false);
		}
	}

	// Damage number with the elemental verdict (big lesson of the game).
	if (bShowDamageNumbers)
	{
		if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
		{
			FString Text = FString::Printf(TEXT("%.0f"), AppliedDamage);
			switch (Match)
			{
			case EWDElementalMatch::Weakness: Text += TEXT("  FAIBLESSE !"); break;
			case EWDElementalMatch::Resisted: Text += TEXT("  résisté v");   break;
			default: break;
			}
			const FVector Location = GetOwner()->GetActorLocation()
				+ FVector(FMath::FRandRange(-40.f, 40.f), FMath::FRandRange(-40.f, 40.f), 170.f);
			Debug->DrawText(Location, Text, DamageEvent.Element, 0.7f);
		}
	}

	// Global feedback: switching to the right element FEELS right.
	if (UWDGameFeelSubsystem* GameFeel = GetWorld()->GetSubsystem<UWDGameFeelSubsystem>())
	{
		if (Match == EWDElementalMatch::Weakness && WeaknessHitstop > 0.f)
		{
			GameFeel->Hitstop(WeaknessHitstop);
			GameFeel->Rumble(0.35f, 0.1f);
		}
	}
}

void UWDHitResponseComponent::HandleDied(AActor* Killer)
{
	if (UWDGameFeelSubsystem* GameFeel = GetWorld()->GetSubsystem<UWDGameFeelSubsystem>())
	{
		GameFeel->Hitstop(bBigDeath ? 0.18f : 0.04f, bBigDeath ? 0.02f : 0.05f);
		GameFeel->Rumble(bBigDeath ? 0.9f : 0.4f, bBigDeath ? 0.35f : 0.12f);
	}
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		Debug->DrawGroundCircle(GetOwner()->GetActorLocation(), bBigDeath ? 320.f : 130.f, EWDElement::Normal, 0.5f);
	}
}

void UWDHitResponseComponent::RestoreColor()
{
	if (Mesh.IsValid())
	{
		if (UMaterialInstanceDynamic* Material = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(0)))
		{
			Material->SetVectorParameterValue(TEXT("Color"), BaseColor);
		}
	}
}
