#include "Core/WDSettingsSubsystem.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Internationalization/TextLocalizationResource.h"
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

	/**
	 * The game's UI strings (namespace WDUI), French source -> English. Applied as a LIVE
	 * localization resource: no compiled .locres needed, the editor language never changes,
	 * and every visible FText refreshes instantly. The Localization Dashboard pipeline
	 * (8 languages, .po exports) replaces this table at the content pass — same keys.
	 * RULE: French strings here must match the NSLOCTEXT sources EXACTLY (hash-checked).
	 */
	struct FWDUIString { const TCHAR* Key; const TCHAR* French; const TCHAR* English; };
	const FWDUIString GWDUIStrings[] =
	{
		{ TEXT("LoadingTitle"),     TEXT("CHARGEMENT…"),                                TEXT("LOADING…") },
		{ TEXT("PauseTitle"),       TEXT("PAUSE"),                                      TEXT("PAUSED") },
		{ TEXT("PauseResume"),      TEXT("Reprendre"),                                  TEXT("Resume") },
		{ TEXT("PauseAbandon"),     TEXT("Abandonner (défaite, loot conservé)"),        TEXT("Abandon (defeat, loot kept)") },
		{ TEXT("SummaryVictory"),   TEXT("VICTOIRE !"),                                 TEXT("VICTORY!") },
		{ TEXT("SummaryDefeat"),    TEXT("DÉFAITE — le loot est conservé"),             TEXT("DEFEAT — your loot is kept") },
		{ TEXT("SummaryMult"),      TEXT("Multiplicateur de récompenses : ×{0}"),       TEXT("Reward multiplier: ×{0}") },
		{ TEXT("SummaryGold"),      TEXT("Or  +{0}"),                                   TEXT("Gold  +{0}") },
		{ TEXT("SummaryGoldBonus"), TEXT("Or  +{0}  (dont bonus d'étoiles +{1})"),      TEXT("Gold  +{0}  (incl. star bonus +{1})") },
		{ TEXT("SummaryXP"),        TEXT("XP  +{0}  (niveau du personnage : {1})"),     TEXT("XP  +{0}  (character level: {1})") },
		{ TEXT("SummaryReplay"),    TEXT("Rejouer"),                                    TEXT("Replay") },
		{ TEXT("SummaryMenu"),      TEXT("Quartier général"),                           TEXT("Headquarters") },
		{ TEXT("HubWallet"),        TEXT("Or : {0}      Niveau : {1}  ({2} XP)"),       TEXT("Gold: {0}      Level: {1}  ({2} XP)") },
		{ TEXT("HubWall"),          TEXT("Mur — Niveau {0}/{1}"),                       TEXT("Wall — Level {0}/{1}") },
		{ TEXT("HubWallUp"),        TEXT("Améliorer — {0} or"),                         TEXT("Upgrade — {0} gold") },
		{ TEXT("HubWeapon"),        TEXT("{0} — Nv {1}"),                               TEXT("{0} — Lv {1}") },
		{ TEXT("HubWeaponUp"),      TEXT("Améliorer — {0} or + {1} {2}"),               TEXT("Upgrade — {0} gold + {1} {2}") },
		{ TEXT("HubMode"),          TEXT("Mode :"),                                     TEXT("Mode:") },
		{ TEXT("ModeNormal"),       TEXT("Normal"),                                     TEXT("Normal") },
		{ TEXT("ModeHard"),         TEXT("Hard"),                                       TEXT("Hard") },
		{ TEXT("ModeHell"),         TEXT("Enfer"),                                      TEXT("Hell") },
		{ TEXT("HubQuality"),       TEXT("Qualité :"),                                  TEXT("Quality:") },
		{ TEXT("QLow"),             TEXT("Basse"),                                      TEXT("Low") },
		{ TEXT("QMedium"),          TEXT("Moyenne"),                                    TEXT("Medium") },
		{ TEXT("QHigh"),            TEXT("Haute"),                                      TEXT("High") },
		{ TEXT("QEpic"),            TEXT("Épique"),                                     TEXT("Epic") },
		{ TEXT("HubLanguage"),      TEXT("Langue :"),                                   TEXT("Language:") },
		// Navigation screens
		{ TEXT("Back"),             TEXT("← Retour"),                                   TEXT("← Back") },
		{ TEXT("SplashTagline"),    TEXT("Défends le mur. Améliore. Reviens plus forte."), TEXT("Defend the wall. Upgrade. Come back stronger.") },
		{ TEXT("SplashContinue"),   TEXT("Appuyer pour commencer"),                     TEXT("Press to start") },
		{ TEXT("MenuNewGame"),      TEXT("Nouvelle partie"),                            TEXT("New game") },
		{ TEXT("MenuLoadGame"),     TEXT("Charger une partie"),                         TEXT("Load game") },
		{ TEXT("MenuOptions"),      TEXT("Options"),                                    TEXT("Options") },
		{ TEXT("MenuQuit"),         TEXT("Quitter"),                                    TEXT("Quit") },
		{ TEXT("SlotsNewTitle"),    TEXT("NOUVELLE PARTIE — CHOISIS UN EMPLACEMENT"),   TEXT("NEW GAME — PICK A SLOT") },
		{ TEXT("SlotsLoadTitle"),   TEXT("CHARGER UNE PARTIE"),                         TEXT("LOAD GAME") },
		{ TEXT("SlotSummary"),      TEXT("Slot {0} — Or {1} · Niveau {2} · {3} ★"),     TEXT("Slot {0} — Gold {1} · Level {2} · {3} ★") },
		{ TEXT("SlotEmpty"),        TEXT("Slot {0} — (vide)"),                          TEXT("Slot {0} — (empty)") },
		{ TEXT("LobbyTitle"),       TEXT("QUARTIER GÉNÉRAL"),                           TEXT("HEADQUARTERS") },
		{ TEXT("LobbyAdventure"),   TEXT("⚔  Partir à l'aventure"),                     TEXT("⚔  Set out on the adventure") },
		{ TEXT("LobbyArmory"),      TEXT("Armurerie & améliorations"),                  TEXT("Armory & upgrades") },
		{ TEXT("LobbyEncyclo"),     TEXT("Encyclopédie"),                               TEXT("Encyclopedia") },
		{ TEXT("LobbyMainMenu"),    TEXT("Menu principal"),                             TEXT("Main menu") },
		{ TEXT("AdvTitle"),         TEXT("AVENTURE — CHOISIS TON STAGE"),               TEXT("ADVENTURE — PICK YOUR STAGE") },
		{ TEXT("AdvZone"),          TEXT("Zone {0} :"),                                 TEXT("Zone {0}:") },
		{ TEXT("AdvStage"),         TEXT("Stage {0}"),                                  TEXT("Stage {0}") },
		{ TEXT("AdvComing"),        TEXT("Zone {0} — à venir"),                         TEXT("Zone {0} — coming soon") },
		{ TEXT("AdvHint"),          TEXT("Hard se débloque en finissant le stage en Normal ; Enfer en le finissant en Hard."), TEXT("Hard unlocks by finishing the stage on Normal; Hell by finishing it on Hard.") },
		{ TEXT("ArmoryTitle"),      TEXT("ARMURERIE & AMÉLIORATIONS"),                  TEXT("ARMORY & UPGRADES") },
		{ TEXT("EncycloTitle"),     TEXT("ENCYCLOPÉDIE — ZONE 1"),                      TEXT("ENCYCLOPEDIA — ZONE 1") },
		{ TEXT("EncycloUnknown"),   TEXT("?????  —  monstre jamais croisé"),            TEXT("?????  —  never encountered") },
		{ TEXT("EncycloWeak"),      TEXT("{0}  —  faiblesse : {1}"),                    TEXT("{0}  —  weakness: {1}") },
		{ TEXT("EncycloNoWeak"),    TEXT("{0}  —  faiblesse : ?"),                      TEXT("{0}  —  weakness: ?") },
		{ TEXT("EncycloHint"),      TEXT("Croise un monstre pour révéler sa page ; touche sa faiblesse pour la confirmer."), TEXT("Meet a monster to reveal its page; hit its weakness to confirm it.") },
		{ TEXT("OptionsTitle"),     TEXT("OPTIONS"),                                    TEXT("OPTIONS") },
		{ TEXT("OptVolume"),        TEXT("Volumes (audio à venir) :"),                  TEXT("Volumes (audio coming):") },
		{ TEXT("OptVolMaster"),     TEXT("Général"),                                    TEXT("Master") },
		{ TEXT("OptVolMusic"),      TEXT("Musique"),                                    TEXT("Music") },
		{ TEXT("OptVolSfx"),        TEXT("Effets"),                                     TEXT("SFX") },
	};
}

void UWDSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadFromConfig();

	if (!LanguageCulture.IsEmpty())
	{
		ApplyLanguage(LanguageCulture);
	}
}

void UWDSettingsSubsystem::ApplyLanguage(const FString& Culture)
{
	// Push the game's translations as a LIVE localization resource: visible immediately,
	// works with zero compiled .locres, and NEVER touches the editor language (the culture
	// of the process is left alone — only the WDUI namespace entries change).
	const bool bEnglish = Culture.StartsWith(TEXT("en"));

	FTextLocalizationResource Resource;
	for (const FWDUIString& Entry : GWDUIStrings)
	{
		Resource.AddEntry(FTextKey(TEXT("WDUI")), FTextKey(Entry.Key), Entry.French,
			bEnglish ? Entry.English : Entry.French, /*Priority=*/0);
	}
	FTextLocalizationManager::Get().UpdateFromLocalizationResource(Resource);
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
	ApplyLanguage(Culture);
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
