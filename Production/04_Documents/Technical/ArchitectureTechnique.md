# Architecture technique — Wall Defense

> v1.2 — 12 juin 2026 (ajouts : préchargement/écran de chargement, options, internationalisation, **hit response/game feel**). Document de référence AVANT d'écrire le code.
> Tout est **C++** ; les Blueprints/assets ne font que du visuel et de la donnée. Préfixe de classes : `WD`.

---

## 1. Les 5 principes

1. **Composants indépendants** — chaque composant fait UNE chose, ne connaît **aucun autre composant**, et peut être modifié/supprimé/remplacé sans rien casser. Le câblage entre composants se fait **au niveau de l'acteur qui les possède** (ou du GameMode), jamais entre eux.
2. **Communication par délégués** — un composant **expose des événements** (delegates multicast `BlueprintAssignable`) et **ne sait pas qui l'écoute**. Aucune référence « montante » (un composant ne connaît jamais son GameMode, un widget n'est jamais connu du gameplay).
3. **DataAssets = le pont des données** — le code ne référence **jamais** un asset en dur. Stats, courbes, comportements, VFX, SFX, vidéos, tenues : tout passe par des DataAssets (`TSoftObjectPtr`). Créer/équilibrer du contenu = éditer un asset, zéro code.
4. **UI event-driven** — les widgets s'abonnent aux délégués et reçoivent les données poussées par le C++ (zéro polling, zéro Tick d'UI). Le Blueprint du widget ne fait QUE le visuel.
5. **Debug-first** — chaque feature gameplay est visualisable/jouable sans assets (formes de debug colorées par élément). Les refs visuelles des DataAssets sont **optionnelles** : absentes = rendu debug.

## 2. Vue d'ensemble

```
                          ┌─────────────── DONNÉES (DataAssets / DataTables) ───────────────┐
                          │  DA_Weapon · DA_Monster · DA_Stage · DA_Wall · DA_Character     │
                          │  DA_Outfit · DT_DifficultyModes                                 │
                          └──────────────┬──────────────────────────────┬───────────────────┘
                                lues par │                              │ lues par
                                         ▼                              ▼
┌─ PERSISTANT (GameInstance) ────────────────────┐   ┌─ UNE PARTIE (monde du stage) ─────────────────┐
│ UWDSaveSubsystem      (5 slots, auto-save)     │   │ AWDStageGameMode  (assemble et arbitre)       │
│ UWDProgressionSubsystem (or, ressources, XP,   │◄──┤ UWDStageDirector  (vagues depuis DA_Stage)    │
│   niveaux d'armes, étoiles, découvertes…)      │   │ AWDHeroCharacter  + composants                │
│ UWDUISubsystem  (écrans, vidéos, transitions)  │   │ AWDWall           + composants                │
│ UWDSettingsSubsystem (langue, audio, qualité)  │   │ AWDMonster ×N     + composants                │
│ UWDPreloadSubsystem  (chargement avant stage)  │   │ AWDProjectile (pool) · AWDLootPickup ×N       │
│ UWDDebugSubsystem     (draw debug, CVars)      │   └───────────────┬───────────────────────────────┘
└───────────────┬────────────────────────────────┘                   │ délégués (événements)
                ▼                                                    │ délégués (événements)
┌─ UI (widgets C++ → visuel en BP) ──────────────────────────────────▼───────────────────────────────┐
│ HUD (mur, armes, vague, loot) · Menus (hub, améliorations, encyclopédie, stages, slots) · Histoire │
└────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

## 3. Types partagés (aucune dépendance)

| Type | Contenu | Utilisé par |
|---|---|---|
| `EWDElement` | Normal, Feu, Glace, Foudre, Vent, Lumière, Ténèbres (+ couleur de debug associée) | tout le jeu |
| `EWDDifficulty` | Normal, Hard, Enfer | stages, drops, monstres |
| `FWDDamageEvent` | montant, élément, instigateur, position d'impact | santé, UI, FX |
| `FWDElementalProfile` | faiblesse (1), résistances (N) — + résistances bonus Hard/Enfer | monstres, calcul de dégâts |
| `FWDProjectileBehavior` | enum (perçant, rebond, fragment, autoguidé, zone, propagation…) + paramètres | armes, projectiles |
| `FWDEffectCue` | `TSoftObjectPtr<UNiagaraSystem>` + `TSoftObjectPtr<USoundBase>` — **les deux optionnels** | tous les effets et sous-effets |

## 4. DataAssets — les ponts de données

| Asset | Description simple | Consommé par |
|---|---|---|
| **`UWDWeaponData`** (DA_Weapon_×7) | Une arme : élément, stats de base, courbe 1–100, liste des **paliers** (niveau → comportements `FWDProjectileBehavior`), **Spéciale** (effet, scaling, cooldown, **vidéo** `TSoftObjectPtr<UMediaSource>`), `FWDEffectCue` par effet ET sous-effet, tenue associée (`UWDOutfitData`), coûts par tier | WeaponComponent, SpecialComponent, Progression (coûts), Encyclopédie, Outfit |
| **`UWDMonsterData`** (DA_Monster_×36) | Une fiche du bestiaire : multiplicateurs (PV/déf/dégâts mur), vitesse, pattern, `FWDElementalProfile`, bouclier, skill support (type d'aura + params), table de drops | Monster (à l'apparition), StageDirector, Encyclopédie |
| **`UWDStageData`** (DA_Stage_×31) | Un stage : zone, **liste des vagues** (monstres + quantités + timing), récompenses fixes par étoiles, configuration par mode | StageDirector, GameMode, sélection de stage |
| **`UWDWallData`** | Niveaux du mur : PV/défense par niveau, **skills** (condition + effet + niveau de déblocage), coûts | Wall, Progression, menu d'amélioration |
| **`UWDCharacterData`** | Stats du perso par niveau d'XP (vitesse, **portée d'aimant**…), courbe d'XP | HeroCharacter, MagnetComponent, Progression |
| **`UWDOutfitData`** (DA_Outfit_×N) | Une tenue : meshes/matériaux/anims (même squelette). 1 par arme + variantes (skins) | OutfitComponent |
| **`DT_DifficultyModes`** | Multiplicateurs Normal/Hard/Enfer (stats, drops, tiers) | StageDirector, Monster, LootDrop |
| **`UWDHitResponseData`** (DA_HitResponse_×N) | Une « recette de ressenti » à l'impact, par **gabarit** (léger/moyen/lourd/boss — chaque fiche monstre en référence un, avec overrides possibles) : **flash** du mesh (couleur/durée), **secousse** du mesh, **hitstop** (micro-gel en ms), **knockback** (force selon le poids), **camera shake** (classe + intensité), **vibration manette**, `FWDEffectCue` d'impact, **chiffres de dégâts flottants** (style par élément + style spécial **FAIBLESSE!/résisté** — c'est ce feedback qui enseigne le bestiaire), réponse à la **mort** (pop, FX, ralenti bref sur les boss) | HitResponseComponent, GameFeelSubsystem, Monster |

## 5. Subsystems persistants (GameInstance)

| Classe | Description simple | Événements exposés |
|---|---|---|
| **`UWDSaveSubsystem`** | Seul maître du disque : 5 slots, auto-save/auto-load, nouvelle partie. Sérialise l'état que lui donne la Progression. | `OnSlotLoaded`, `OnSaved` |
| **`UWDProgressionSubsystem`** | L'état méta du joueur : or, ressources (élément × tier), XP/niveau, armes débloquées + niveaux, étoiles par stage×mode, niveaux du mur, tenues/skins, découvertes d'encyclopédie, record du stage infini. API : `CanAfford/Spend/Add`, `LevelUpWeapon`, `RegisterDiscovery`… Lit les DataAssets pour les règles ; notifie le SaveSubsystem. | `OnGoldChanged`, `OnResourceChanged`, `OnXPChanged`, `OnWeaponUnlocked`, `OnWeaponLeveledUp`, `OnWallUpgraded`, `OnDiscovery` |
| **`UWDUISubsystem`** | Pile d'écrans (push/pop), **transitions fondu** (écran de chargement ↔ jeu), lecture des **vidéos de Spéciale** (MediaPlayer plein écran skippable). Les widgets ne s'empilent jamais eux-mêmes. | `OnScreenChanged`, `OnVideoFinished`, `OnFadeFinished` |
| **`UWDSettingsSubsystem`** | Les **paramètres globaux** (indépendants des 5 slots) : **langue** (+ dub), **volumes** (général/musique/SFX), **qualité graphique**. Enveloppe `UGameUserSettings` (scalabilité UE standard : Low/Medium/High/Epic + résolution/fenêtré/VSync) + Sound Classes/Mix pour l'audio + `FInternationalization::SetCurrentCulture` pour la langue. Persisté en config utilisateur (pas dans les slots). Applique tout au démarrage. | `OnLanguageChanged`, `OnVolumeChanged`, `OnQualityChanged` |
| **`UWDPreloadSubsystem`** | Le **chargement avant stage** : collecte toutes les `TSoftObjectPtr` nécessaires à la partie (depuis le `DA_Stage` choisi : fiches monstres → leurs FX/sons ; armes possédées → leurs FX/sons/vidéos ; tenues ; mur) et les charge en **asynchrone** (`FStreamableManager::RequestAsyncLoad`), **réchauffe les pools** (projectiles pré-spawnés) et précompile les shaders (PSO precaching UE5). Garde un handle sur le bundle pendant la partie, le libère au retour menu. | `OnPreloadProgress(0–1)`, `OnPreloadFinished` |
| **`UWDGameFeelSubsystem`** *(WorldSubsystem — vit avec la partie)* | Les retours **globaux** d'impact, qui ne peuvent pas vivre sur un monstre : **hitstop** (dilation du temps quelques ms), **camera shake** (via la caméra du joueur), **vibration manette**, brefs **ralentis** (mort de boss). API : `Play(HitResponseData, contexte)`. Respecte les options d'accessibilité (intensité de shake réglable/désactivable). | — |
| **`UWDDebugSubsystem`** | Rendu debug centralisé (lignes, formes, couleurs par élément) + CVars (`wd.Debug.Bullets`…). Tous les composants dessinent à travers lui. | — |

## 6. Acteurs et composants gameplay

### 6.1 L'héroïne
| Classe | Description simple | Composants utilisés |
|---|---|---|
| **`AWDHeroCharacter`** | Le pawn top-down : se déplace librement, ne meurt pas. Ne fait QUE bouger — tout le reste est dans ses composants. | WeaponInventory, Special, Magnet, Outfit |
| **`AWDHeroController`** | Traduit les inputs (Enhanced Input, 2 schémas : souris+clavier / **manette twin-stick privilégiée**) en appels : déplacer, viser, tirer, switcher, Spéciale. | — |
| **`UWDWeaponInventoryComponent`** | Les 7 emplacements d'armes, l'arme active, le **switch** (suivant/direct). | → `OnWeaponSwitched(ancienne, nouvelle)` |
| **`UWDWeaponComponent`** | Tire l'arme active : lit son `DA_Weapon` (cadence, type de tir, comportements du niveau actuel), demande les projectiles au pool, applique le cooldown (laser sombre). | → `OnFired`, `OnCooldownChanged` |
| **`UWDSpecialAbilityComponent`** | La Spéciale : cooldown, scaling par niveau, déclenche l'effet (mêmes comportements projectiles) et demande la vidéo à l'UISubsystem via événement. | → `OnSpecialCast`, `OnSpecialCooldownChanged` |
| **`UWDMagnetComponent`** | Attire les `AWDLootPickup` dans son rayon (stat du niveau du perso). | → `OnLootCollected(type, quantité)` |
| **`UWDOutfitComponent`** | Applique la tenue (`DA_Outfit`) liée à l'arme active ; écoute `OnWeaponSwitched` (câblé par le Character). | — |

### 6.2 Le mur
| Classe | Description simple | Composants utilisés |
|---|---|---|
| **`AWDWall`** | L'objectif à défendre : PV global unique, défense %. | Health, WallSkills |
| **`UWDHealthComponent`** ⚙️ refonte | **LE composant de vie universel** (mur, monstres, tout) : PV, défense en %, **bouclier en couche**, **profil élémentaire** (faiblesse/résistances) ; calcule les dégâts depuis `FWDDamageEvent`. | → `OnDamaged`, `OnHealed`, `OnShieldBroken`, `OnDied` |
| **`UWDWallSkillsComponent`** | Les skills conditionnels à usage unique (bouclier de départ, onde de répulsion, auto-réparation…) selon `DA_Wall` et le niveau. | → `OnSkillTriggered(skill)` |

### 6.3 Les monstres
| Classe | Description simple | Composants utilisés |
|---|---|---|
| **`AWDMonster`** ⚙️ refonte de `BaseMonster` | Un monstre : initialisé à l'apparition depuis `DA_Monster` × multiplicateurs (stage, mode). | Health, MovePattern, WallAttack, Aura (opt.), LootDrop |
| **`UWDMovePatternComponent`** | Avance vers le mur selon le pattern de la fiche (droite, sinus, zigzag, charge-pause, flanqueur, spirale, sauteur, fouisseur) — stratégies data-driven, une seule classe. | → `OnReachedWall` |
| **`UWDWallAttackComponent`** | L'« attaque » au contact/à distance : dégâts au mur OU buff allié (pour les supports). | → `OnAttacked` |
| **`UWDAuraComponent`** (+ variantes Heal/Shield/Speed/Banner) | Buff de zone en pulsation pendant l'avancée. Indépendant : ajouté seulement aux fiches support. | → `OnPulsed` |
| **`UWDLootDropComponent`** | À la mort (écoute `OnDied`, câblé par le Monster) : spawn les `AWDLootPickup` selon la table de drops × mode × étoiles en cours. | — |
| **`UWDHitResponseComponent`** ⚙️ refonte du hit-react de `BaseMonster` | Le **ressenti local** d'un impact : écoute `OnDamaged`/`OnShieldBroken`/`OnDied` (câblé par le Monster) et exécute la recette du `DA_HitResponse` — flash, secousse du mesh, knockback, FX/SFX d'impact, **chiffre de dégâts flottant** (style élément/faiblesse) — et délègue le global (hitstop, shake, vibration) au `GameFeelSubsystem`. Supprimable sans toucher au gameplay : les dégâts restent identiques, seul le ressenti disparaît. | — |

### 6.4 Projectiles et loot
| Classe | Description simple |
|---|---|
| **`AWDProjectile`** ⚙️ refonte de `Bullet` | Exécute une **liste de `FWDProjectileBehavior`** (perçant, rebond sur les bords, fragments, autoguidé, zone…) — c'est LE moteur générique des 7 armes et de leurs sous-effets. Collision gameplay par composant ; rendu debug par défaut, `FWDEffectCue` si fourni. |
| **`AWDProjectilePool`** ⚙️ | Le pool existant, étendu aux sous-projectiles (fragments, mini-lasers). |
| **`AWDLootPickup`** | Un drop au sol : type/quantité, **durée de vie + clignotement**, attiré par le Magnet. → `OnExpired`, `OnCollected` |
| **`AWDDamageNumber`** *(poolé)* | Un chiffre de dégâts flottant : valeur, style (élément, **FAIBLESSE!**, résisté, critique 🔶), petite anim, retour au pool. Désactivable dans les options. |

### 6.5 La partie (stage)
| Classe | Description simple |
|---|---|
| **`AWDStageGameMode`** | **L'assembleur** : spawn héroïne + mur, crée le StageDirector avec le `DA_Stage` + mode choisis, écoute `OnDied` du mur (défaite) et `OnStageCompleted` (victoire → calcul d'étoiles depuis les PV du mur → récompenses × multiplicateur → `Progression` → auto-save). C'est ICI que les composants sont câblés entre eux. |
| **`UWDStageDirector`** | Déroule les **vagues** du `DA_Stage` : spawn les monstres (init fiche × stage × mode), compte les vivants. Mode infini : génère les vagues par la courbe au lieu de la liste. → `OnWaveStarted(n)`, `OnWaveCleared(n)`, `OnStageCompleted`, `OnMonsterKilled` |
| **`AWDMenuGameMode`** ⚙️ | Le hub (améliorations, encyclopédie, sélection) — surtout de l'UI. |

## 7. UI — widgets et binding

**Le pattern de binding (pour TOUT widget)** :
1. Classe C++ `UWD…Widget : UUserWidget`, dans `NativeConstruct` elle **s'abonne** aux délégués (subsystems via GameInstance, ou composants passés par le GameMode/HUD à la création).
2. Chaque donnée reçue est **poussée** vers un `BlueprintImplementableEvent` (`OnWallHealthChanged(Percent)`, `OnWeaponSwitched(Icon, Element)`, …).
3. Le **Blueprint du widget ne fait que le visuel** (anims, layout) à partir de ces événements. Zéro logique en BP, zéro Tick.
4. Se désabonne dans `NativeDestruct`. Un widget supprimé/remplacé ne casse rien (les délégués perdent juste un abonné).

| Widget | Écoute | Affiche |
|---|---|---|
| `UWDHUDWidget` | Wall.Health, WeaponInventory, Special, StageDirector, Magnet | barre du mur, barre d'armes (7), cooldowns, n° de vague, loot ramassé |
| `UWDSpecialVideoWidget` | UISubsystem | la vidéo de Spéciale (skippable) |
| `UWDMenuHubWidget` | Progression | or/ressources/XP, accès aux sous-menus |
| `UWDUpgradeWidget` | Progression (+ DA_Weapon/DA_Wall/DA_Character) | niveaux, coûts, prochain palier |
| `UWDEncyclopediaWidget` | Progression.OnDiscovery (+ DataAssets) | pages armes/monstres avec **`?????` non découverts** |
| `UWDStageSelectWidget` | Progression | zones, stages, étoiles par mode, modes débloqués |
| `UWDSaveSlotsWidget` | SaveSubsystem | 5 slots, nouvelle partie |
| `UWDStoryWidget` | UISubsystem | écrans d'histoire arcade |
| `UWDLoadingScreenWidget` | PreloadSubsystem | barre/anim de progression, astuce de gameplay 🔶 ; reste affiché jusqu'à `OnPreloadFinished` + fondu |
| `UWDOptionsWidget` | SettingsSubsystem | onglets : **Jeu** (langue, dub), **Audio** (général/musique/SFX), **Graphismes** (qualité, résolution, fenêtré, VSync) |
| `UWDLanguageSelectWidget` | SettingsSubsystem | **premier démarrage uniquement** : choix de la langue (drapeaux/noms natifs), enregistré aussitôt |

## 8. Flux types (comment tout se parle)

**Tir** : Input → `HeroController` → `WeaponComponent.Fire()` (lit DA_Weapon) → `ProjectilePool` → `AWDProjectile` (behaviors) → hit → `HealthComponent.ApplyDamage(FWDDamageEvent)` → `OnDamaged`/`OnDied` → le Monster réagit (hit-react) ; à la mort `LootDropComponent` spawn les pickups ; le `StageDirector` décompte. *Personne dans cette chaîne ne connaît l'UI — elle écoute.*

**Switch d'arme** : Input → `WeaponInventory.SwitchNext()` → `OnWeaponSwitched` → trois abonnés indépendants réagissent : `WeaponComponent` (recharge les données), `OutfitComponent` (change la tenue), `HUDWidget` (met à jour la barre). Supprimer l'Outfit ne touche ni au tir ni au HUD.

**Spéciale** : Input → `SpecialAbilityComponent` (cooldown ok ?) → `OnSpecialCast` → `UISubsystem` joue la vidéo → à la fin (ou skip), l'effet gameplay s'exécute (behaviors projectiles à l'échelle du niveau).

**Victoire** : `StageDirector.OnStageCompleted` → `StageGameMode` lit les PV du mur → étoiles → `Progression.ApplyRunRewards(loot run × multiplicateur)` → `OnGoldChanged`/`OnResourceChanged` (l'UI du résumé s'anime) → `SaveSubsystem.AutoSave()`.

**Ressenti d'un impact (hit response)** : `Projectile` touche → `HealthComponent.ApplyDamage` → `OnDamaged(FWDDamageEvent)` → deux abonnés indépendants : (1) `HitResponseComponent` du monstre joue la recette locale du `DA_HitResponse` — flash, secousse, knockback, FX/SFX, chiffre flottant (style **FAIBLESSE!** si l'élément correspond — c'est le feedback qui enseigne le bestiaire) — et (2) le `GameFeelSubsystem` joue le global — hitstop de quelques ms, camera shake, vibration. *Le gameplay (dégâts) est déjà résolu avant tout ça : couper le ressenti ne change rien aux chiffres.*

**Lancement d'un stage (écran de chargement)** : sélection du stage + mode → `UISubsystem` fondu vers le `LoadingScreenWidget` → `PreloadSubsystem.PreloadStage(DA_Stage, armes possédées)` charge tout en asynchrone (FX, sons, monstres, tenues, vidéos) + réchauffe les pools → `OnPreloadProgress` anime la barre → `OnPreloadFinished` → ouverture du niveau → premier tick rendu → **fondu de transition** vers le jeu. *Garantie : aucun hitch d'effet/son en partie — tout ce qu'un stage peut faire apparaître est déjà en mémoire (les DataAssets listent tout, c'est leur 2e raison d'être).* Au retour menu, le bundle est libéré.

**Premier démarrage** : pas de config trouvée → `LanguageSelectWidget` (choix de langue) → `SettingsSubsystem` applique et sauvegarde → menu. Ensuite la langue se change dans les Options à tout moment.

## 9. Internationalisation (i18n) — prévue dès le départ ✅

Le jeu est **localisé dès la conception** (les textes seront écrits/traduits par Claude dans toutes les langues cibles) :

- **Règle absolue dès la première ligne de code : tout texte visible = `FText`**, jamais de `FString` ni de littéral dans l'UI. Localiser après coup coûte 10× plus cher que cette discipline gratuite au jour 1.
- **String Tables** (`ST_UI`, `ST_Weapons`, `ST_Monsters`, `ST_Story`…) : les textes vivent dans des tables, les DataAssets et widgets y font référence par clé — armes, monstres, paliers, histoire, encyclopédie, options.
- **Localization Dashboard** d'Unreal : collecte, export/import `.po`, compilation des cultures. Langues cibles 🔶 : **FR (source), EN, ES, DE, PT-BR, JA, ZH-Hans, KO**.
- **Choix au premier démarrage** ✅ : écran de sélection de langue, sauvegardé par le `SettingsSubsystem`, modifiable dans les Options.
- **Dubs (audio localisé)** ✅ prévu techniquement : les assets audio « parlés » passent par le système de localisation d'assets d'UE (`Content/L10N/<culture>/…`) — le bon asset est chargé selon la culture audio, qui peut différer de la langue des textes (option « Voix »). 🔶 Le contenu voix lui-même (quelles lignes, quelles langues) sera décidé plus tard — l'architecture le permet sans refonte.
- Les **nombres/formats** passent par `FText::AsNumber/AsPercent` (séparateurs corrects par culture).

## 10. Règles de modularité (le contrat)

- Un composant n'inclut JAMAIS le header d'un autre composant ; il ne connaît que les **types partagés** (§3) et **son** DataAsset.
- Toute communication sortante = **délégué**. Toute configuration entrante = **DataAsset ou paramètres à l'init**.
- Le câblage (qui écoute qui) vit dans **l'acteur propriétaire** ou le **GameMode** — un seul endroit à lire pour comprendre les liens, un seul à modifier pour remplacer un composant.
- Chaque composant doit fonctionner **dans une map de test vide** avec le DebugSubsystem (critère d'indépendance).
- Tout asset = `TSoftObjectPtr` dans un DataAsset. Référence absente → rendu debug, jamais de crash.

## 11. Ordre d'implémentation proposé

1. **Fondations** : types partagés (§3), `UWDHealthComponent` (élémentaire + bouclier), `UWDDebugSubsystem`. **Discipline `FText` + String Tables dès ce jour 1** (§9).
2. **Héroïne top-down** : Character + Controller (2 schémas d'input) + caméra — en capsule de debug.
3. **Armes debug-first** : `DA_Weapon` + `WeaponComponent` + `AWDProjectile` à behaviors + pool — le fusil d'abord, puis 1 palier de chaque type de behavior (perçant, rebond, fragment, autoguidé, zone).
4. **Monstres** : `AWDMonster` + patterns + `DA_Monster` (zone 1) + `UWDStageDirector` + `DA_Stage` (stages 1–5) + **hit response** (`DA_HitResponse`, `HitResponseComponent`, `GameFeelSubsystem`, chiffres flottants) — le ressenti se règle dès que des monstres meurent, pas en fin de projet.
5. **Le mur + la boucle** : `AWDWall`, victoire/défaite, étoiles, `AWDStageGameMode`, **écran de chargement + `PreloadSubsystem` + transitions** (dès qu'on charge un vrai stage).
6. **Méta** : pickups + magnet, `Progression`, `Save` (5 slots), menu hub minimal, **`SettingsSubsystem` + Options + premier démarrage (langue)**.
7. **Le reste** : Spéciale + vidéos, modes Hard/Enfer, encyclopédie, tenues, skills du mur, histoire, dubs.

→ Fin de l'étape 5 = **boucle de jeu complète jouable en debug**. C'est le premier jalon.
