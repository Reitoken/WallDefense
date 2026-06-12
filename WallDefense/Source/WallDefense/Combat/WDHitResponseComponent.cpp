#include "Combat/WDHitResponseComponent.h"
#include "Core/WDDebugSubsystem.h"
#include "Core/WDGameFeelSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

UWDHitResponseComponent::UWDHitResponseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false; // only ticks while shaking
}

void UWDHitResponseComponent::Configure(UStaticMeshComponent* InMesh, const FLinearColor& InBaseColor, bool bInBigDeath)
{
	Mesh = InMesh;
	BaseColor = InBaseColor;
	bBigDeath = bInBigDeath;
	MeshBaseOffset = InMesh ? InMesh->GetRelativeLocation() : FVector::ZeroVector;
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

	// Local mesh jitter — the punch you SEE on the body itself.
	if (ShakeDuration > 0.f && ShakeAmplitude > 0.f && Mesh.IsValid())
	{
		ShakeTimeRemaining = ShakeDuration;
		SetComponentTickEnabled(true);
	}

	// Knockback: announced only. The owner routes it (monsters recoil along their own
	// advance axis -> always AWAY from the wall, never shoved through it).
	// Boss-grade bodies barely budge — their mass is part of the readability.
	if (KnockbackDistance > 0.f)
	{
		OnKnockbackRequested.Broadcast(bBigDeath ? KnockbackDistance * 0.25f : KnockbackDistance);
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

	// Global feedback: every hit jolts a little; the right element FEELS right.
	if (UWDGameFeelSubsystem* GameFeel = GetWorld()->GetSubsystem<UWDGameFeelSubsystem>())
	{
		if (Match == EWDElementalMatch::Weakness)
		{
			GameFeel->CameraShake(0.8f);
			if (WeaknessHitstop > 0.f)
			{
				GameFeel->Hitstop(WeaknessHitstop);
				GameFeel->Rumble(0.35f, 0.1f);
			}
		}
		else
		{
			GameFeel->CameraShake(0.3f);
		}
	}
}

void UWDHitResponseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ShakeTimeRemaining -= DeltaTime;
	if (!Mesh.IsValid() || ShakeTimeRemaining <= 0.f)
	{
		if (Mesh.IsValid())
		{
			Mesh->SetRelativeLocation(MeshBaseOffset);
		}
		SetComponentTickEnabled(false);
		return;
	}

	// Decaying random jitter around the rest pose.
	const float Falloff = ShakeTimeRemaining / FMath::Max(ShakeDuration, KINDA_SMALL_NUMBER);
	const FVector Jitter(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-0.4f, 0.4f));
	Mesh->SetRelativeLocation(MeshBaseOffset + Jitter * ShakeAmplitude * Falloff);
}

void UWDHitResponseComponent::HandleDied(AActor* Killer)
{
	if (UWDGameFeelSubsystem* GameFeel = GetWorld()->GetSubsystem<UWDGameFeelSubsystem>())
	{
		GameFeel->Hitstop(bBigDeath ? 0.18f : 0.04f, bBigDeath ? 0.02f : 0.05f);
		GameFeel->Rumble(bBigDeath ? 0.9f : 0.4f, bBigDeath ? 0.35f : 0.12f);
		GameFeel->CameraShake(bBigDeath ? 2.2f : 0.8f);
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
