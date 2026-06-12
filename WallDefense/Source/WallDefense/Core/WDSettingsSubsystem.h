#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDSettingsSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWDVolumeChannel : uint8
{
	Master UMETA(DisplayName = "Général"),
	Music  UMETA(DisplayName = "Musique"),
	Sfx    UMETA(DisplayName = "Effets"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnLanguageChanged, const FString&, Culture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnVolumeChanged, EWDVolumeChannel, Channel, float, Volume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnQualityChanged, int32, QualityLevel);

/**
 * Global settings, independent from the 5 save slots (ArchitectureTechnique §5):
 * language, volumes, graphics quality. Persisted in the user config file.
 * First launch: UE hardware benchmark -> automatic graphics preset (GDD decision).
 * Volumes are stored and broadcast now; they route into Sound Mixes when audio lands.
 */
UCLASS()
class WALLDEFENSE_API UWDSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** True until the first-launch setup ran once (drives the language prompt). */
	UFUNCTION(BlueprintPure, Category = "WD|Settings")
	bool IsFirstLaunch() const { return !bConfigured; }

	/** Benchmark + auto graphics preset, then marks the install configured. Safe to call again (no-op). */
	UFUNCTION(BlueprintCallable, Category = "WD|Settings")
	void RunFirstLaunchSetup();

	UFUNCTION(BlueprintPure, Category = "WD|Settings")
	FString GetLanguage() const { return LanguageCulture; }

	UFUNCTION(BlueprintCallable, Category = "WD|Settings")
	void SetLanguage(const FString& Culture);

	/** Scalability 0..3 (Low/Medium/High/Epic); -1 = mixed/custom. */
	UFUNCTION(BlueprintPure, Category = "WD|Settings")
	int32 GetOverallQuality() const;

	UFUNCTION(BlueprintCallable, Category = "WD|Settings")
	void SetOverallQuality(int32 QualityLevel);

	UFUNCTION(BlueprintPure, Category = "WD|Settings")
	float GetVolume(EWDVolumeChannel Channel) const;

	UFUNCTION(BlueprintCallable, Category = "WD|Settings")
	void SetVolume(EWDVolumeChannel Channel, float Volume);

	UPROPERTY(BlueprintAssignable, Category = "WD|Settings")
	FWDOnLanguageChanged OnLanguageChanged;

	UPROPERTY(BlueprintAssignable, Category = "WD|Settings")
	FWDOnVolumeChanged OnVolumeChanged;

	UPROPERTY(BlueprintAssignable, Category = "WD|Settings")
	FWDOnQualityChanged OnQualityChanged;

	/** Applies the game-side translations LIVE (table-driven; the editor language NEVER changes). */
	static void ApplyLanguage(const FString& Culture);

private:
	void LoadFromConfig();
	void SaveToConfig() const;

	FString LanguageCulture; // empty = system default
	float Volumes[3] = { 1.f, 1.f, 1.f };
	bool bConfigured = false;
};
