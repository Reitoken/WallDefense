#include "Core/WDSettingsSubsystem.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"
#include "Internationalization/Internationalization.h"
#include "Misc/App.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
	const TCHAR* ConfigSection = TEXT("WallDefense.Settings");

	const TCHAR* VolumeKeyFor(EWDVolumeChannel Channel)
	{
		switch (Channel)
		{
		case EWDVolumeChannel::Music: return TEXT("VolumeMusic");
		case EWDVolumeChannel::Sfx:   return TEXT("VolumeSfx");
		default:                      return TEXT("VolumeMaster");
		}
	}
}

void UWDSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadFromConfig();

	if (!LanguageCulture.IsEmpty())
	{
		FInternationalization::Get().SetCurrentCulture(LanguageCulture);
	}
}

void UWDSettingsSubsystem::LoadFromConfig()
{
	GConfig->GetBool(ConfigSection, TEXT("bConfigured"), bConfigured, GGameUserSettingsIni);
	GConfig->GetString(ConfigSection, TEXT("Language"), LanguageCulture, GGameUserSettingsIni);
	for (int32 i = 0; i < 3; ++i)
	{
		GConfig->GetFloat(ConfigSection, VolumeKeyFor(static_cast<EWDVolumeChannel>(i)), Volumes[i], GGameUserSettingsIni);
	}
}

void UWDSettingsSubsystem::SaveToConfig() const
{
	GConfig->SetBool(ConfigSection, TEXT("bConfigured"), bConfigured, GGameUserSettingsIni);
	GConfig->SetString(ConfigSection, TEXT("Language"), *LanguageCulture, GGameUserSettingsIni);
	for (int32 i = 0; i < 3; ++i)
	{
		GConfig->SetFloat(ConfigSection, VolumeKeyFor(static_cast<EWDVolumeChannel>(i)), Volumes[i], GGameUserSettingsIni);
	}
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UWDSettingsSubsystem::RunFirstLaunchSetup()
{
	if (bConfigured)
	{
		return;
	}
	// Hardware benchmark -> automatic graphics preset (skipped headless/unattended).
	if (GEngine && GEngine->GetGameUserSettings() && !FApp::IsUnattended())
	{
		UGameUserSettings* GraphicsSettings = GEngine->GetGameUserSettings();
		GraphicsSettings->RunHardwareBenchmark();
		GraphicsSettings->ApplyHardwareBenchmarkResults();
	}
	bConfigured = true;
	SaveToConfig();
}

void UWDSettingsSubsystem::SetLanguage(const FString& Culture)
{
	LanguageCulture = Culture;
	FInternationalization::Get().SetCurrentCulture(Culture);
	SaveToConfig();
	OnLanguageChanged.Broadcast(Culture);
}

int32 UWDSettingsSubsystem::GetOverallQuality() const
{
	const UGameUserSettings* GraphicsSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	return GraphicsSettings ? GraphicsSettings->GetOverallScalabilityLevel() : 2;
}

void UWDSettingsSubsystem::SetOverallQuality(int32 QualityLevel)
{
	if (UGameUserSettings* GraphicsSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		GraphicsSettings->SetOverallScalabilityLevel(FMath::Clamp(QualityLevel, 0, 3));
		GraphicsSettings->ApplySettings(false);
	}
	OnQualityChanged.Broadcast(QualityLevel);
}

float UWDSettingsSubsystem::GetVolume(EWDVolumeChannel Channel) const
{
	return Volumes[static_cast<int32>(Channel)];
}

void UWDSettingsSubsystem::SetVolume(EWDVolumeChannel Channel, float Volume)
{
	Volumes[static_cast<int32>(Channel)] = FMath::Clamp(Volume, 0.f, 1.f);
	SaveToConfig();
	OnVolumeChanged.Broadcast(Channel, Volumes[static_cast<int32>(Channel)]);
}
