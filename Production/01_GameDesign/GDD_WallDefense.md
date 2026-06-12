# Wall Defense — Game Design Document

> **Statut** : v0.7 — 12 juin 2026 — ajout : écran de chargement, options, internationalisation.
> ✅ = décidé · 🔶 = à approfondir/valider · ⚙️ = existe déjà dans le code.
> Ce document est la **référence unique** des règles du jeu. Documents détaillés : `Mechanics/` et `Balancing/`.

---

## 1. Vision

### 1.1 Pitch ✅
Un **tower defense incarné**, vue **top-down (Zelda-like)** : des **monstres d'une autre dimension envahissent la Terre**, et une **jeune héroïne** les arrête en cherchant des armes pour les exterminer ✅. Elle se déplace librement sur le terrain et défend son **mur** contre les vagues. Elle possède **7 armes — une par élément** (Normal, Feu, Glace, Foudre, Vent, Lumière, Ténèbres) et **switche** entre elles : chaque monstre est **faible à un élément** et **résistant à un ou plusieurs autres** — connaître son bestiaire et switcher au bon moment, c'est tout le skill.

Chaque arme évolue jusqu'au **niveau 100** (gros palier tous les 5 niveaux) et possède une **Spéciale active** avec cinématique. Progression par le loot ; **30 stages** (6 zones de 5) rejouables en **Normal / Hard / Enfer** + un **stage infini**. Le mantra : **try and become stronger** — essayer, farmer, évoluer, revenir.

**Un simple jeu premium à la vente** : pas de classement en ligne, pas de battle pass — aucune feature online. ✅

### 1.2 Piliers ✅
1. **La bonne arme au bon moment** — le switch élémentaire est LE cœur du gameplay.
2. **Connaître son ennemi** — mémoriser les faiblesses des monstres récompense l'expérience.
3. **Perdre fait progresser** — le loot est toujours conservé ; on revient plus fort.
4. **Les armes grandissent avec le joueur** — 100 niveaux par arme, chaque niveau change quelque chose.

### 1.3 Format ✅
- Solo, PC (Steam), **hors ligne**. Jeu premium (achat unique).
- Contenu : **30 stages** en **6 zones de 5 stages** (4 normaux + 1 boss) + **stage 31 infini**.
- **Sauvegarde : 5 slots** indépendants, sauvegarde/chargement **automatiques**, nouvelle partie possible. ⚙️ base : `WallDefenseSaveGame` + `AutoSaveComponent` (à étendre aux slots).
- **Multilingue dès la conception** ✅ (v0.7) : choix de la langue au **premier démarrage** (sauvegardé, modifiable dans les options) ; textes écrits dans toutes les langues cibles (FR source, EN, ES, DE, PT-BR, JA, ZH, KO 🔶) ; **dubs** prévus techniquement (langue des voix ≠ langue des textes possible).

---

## 2. Boucle de jeu ✅

```
MENU (hub)
 ├─ AMÉLIORER : armes (ressources élémentaires + or), personnage (XP/or), MUR (PV/défense/skills)
 ├─ DÉBLOQUER : skins et équipements (objets rares + or) — les armes, elles, se gagnent sur les boss
 ├─ ENCYCLOPÉDIE : consulter armes, effets, coûts d'évolution, monstres rencontrés
 └─ JOUER : choisir un stage déjà atteint (progression linéaire) + son MODE (Normal/Hard/Enfer)
      └─> ÉCRAN DE CHARGEMENT ✅ : précharge TOUT ce que le stage utilisera (effets, sons,
      │   monstres, tenues, vidéos) — zéro accroc en partie — puis transition en fondu
      └─> STAGE : vagues de monstres → ils attaquent le mur
           ├─ VICTOIRE → étoiles selon PV du mur → récompenses × étoiles → stage suivant
           │             (boss de zone vaincu → NOUVELLE ARME débloquée)
           └─ DÉFAITE (mur détruit) → retour menu, LOOT CONSERVÉ → améliorer → retenter
```

### 2.1 Structure des zones ✅
- **1 zone = 5 stages** : 4 stages normaux + 1 **stage de boss**. ✅ confirmé.
- **Battre le boss d'une zone débloque une nouvelle arme élémentaire** (§5.3) et ouvre la zone suivante.
- 6 zones × 5 stages = 30, puis le **stage 31 (infini)** se débloque après la zone 6.
- Chaque zone a une identité (thème/monde) ; son bestiaire est propre (`Mechanics/Bestiaire.md`).

### 2.2 Étoiles de stage ✅
**Répartition équitable** des seuils (100 % = 3 étoiles, le reste divisé en tiers) ✅ :

| PV du mur à la victoire | Étoiles |
|---|---|
| 100 % | ⭐⭐⭐ |
| ≥ 66 % | ⭐⭐ |
| ≥ 33 % | ⭐ |
| < 33 % | 0 (victoire quand même) |

**Rôle des étoiles** ✅ :
- **Récompense fixe de fin de stage** selon le nombre d'étoiles.
- **Multiplicateur de drops** du stage ✅ : 0★ ×1 · 1★ ×1,25 · 2★ ×1,5 · 3★ ×2, + chances de drop rare accrues.
- **3 étoiles = bonus supplémentaire** (🔶 objet rare garanti ?).

### 2.3 Histoire, guide et encyclopédie ✅
- **La trame** ✅ : *des monstres d'une autre dimension envahissent la Terre ; une jeune héroïne tente de les arrêter en cherchant des armes pour les exterminer.* Racontée en « arcade » : écrans courts entre les stages clés (intro de zone, victoire de boss), quelques lignes + illustrations. 🔶 Détail de l'écriture défini plus tard.
- **Guide de début de partie** : un tutoriel basique qui explique le cœur (tirer, switcher, améliorer une arme, le mur). Court, contextuel, skippable.
- **Encyclopédie** consultable au menu, avec **dévoilement progressif** ✅ : elle garde une **part de mystère** — les entrées non découvertes affichent des `?????` pour montrer qu'une progression est possible (paliers d'armes pas encore atteints, monstres pas encore croisés, faiblesses pas encore testées).

### 2.4 Modes de difficulté : Normal / Hard / Enfer ✅ (v0.6)
Les 30 stages sont **rejouables en 3 modes** :
- **Hard** d'un stage : débloqué en le finissant en Normal ; **Enfer** : en le finissant en Hard 🔶.
- Les monstres y sont **plus résistants** (stats accrues + **résistances élémentaires supplémentaires**) et les **drops meilleurs**.
- **Chaque mode droppe son tier de matériaux** ✅ : les ressources élémentaires existent en 3 tiers — **Fragments** (Normal), **Cristaux** (Hard), **Noyaux** (Enfer) — nécessaires respectivement aux niveaux d'armes ~1–40, ~41–70, ~71–100. **Les modes structurent donc la progression 1–100.**
- Étoiles comptées **par mode** 🔶 (3 × 30 × 3 = 270 étoiles potentielles).
- Valeurs des multiplicateurs : `Balancing/MonsterScaling.md` §Modes.

---

## 3. L'arène et le mur

### 3.1 Champ de bataille ✅
- **Le joueur est libre sur le terrain. Caméra top-down, comme un Zelda-like.** ✅
- Les monstres arrivent en vagues depuis le fond et convergent vers le mur, chacun avec sa vitesse et son **pattern** (§6.2).
- ⚙️ Le code lanes (`LaneGrid`, `LanePlayerCharacter`) est à remplacer par un déplacement libre top-down + nouvelle caméra.

### 3.2 Le mur ✅
- Le mur a des **PV et une défense** ; chaque monstre a **ses propres dégâts au mur**.
- **Améliorable** ✅ : PV et défense au menu — indispensable contre des monstres de plus en plus forts.
- **Skills du mur** ✅ : en progressant, le mur débloque des **capacités à déclenchement unique et conditionnel** qui marquent sa montée en puissance 🔶 liste et valeurs :
  - **Bouclier de départ** (à partir d'un certain niveau du mur) : une couche de PV bonus en début de partie ;
  - **Onde de répulsion** : repousse une fois les ennemis au contact du mur (temporise) ;
  - 🔶 autres idées : auto-réparation sous 25 % (une fois), riposte (dégâts de contact), herse (ralentit la zone proche du mur).
- Défaite = mur détruit. PV restants = étoiles (§2.2).
- **PV global unique** ✅ (décision v0.6, mandat BAK) : une seule barre, lisible d'un coup d'œil en top-down, cohérente avec les seuils d'étoiles. Les segments ajouteraient de la gestion sans enrichir le choix du joueur (le positionnement compte déjà via les patterns).
- **Pas de réparation en cours de stage** ✅ : chaque PV perdu compte (tension des étoiles) ; la « réparation » existe sous forme de **skills conditionnels** (auto-réparation une fois sous 25 %) et le mur revient à 100 % à chaque début de stage.

---

## 4. Le personnage

- **Une jeune héroïne** ✅ (cf. la trame, §2.3) — à concevoir skinnable dès le départ.
- **Tire** sur les vagues avec ses armes (§5). Ne meurt pas — seul le mur compte ✅.
- **Contrôles ✅ (v0.6), deux schémas, manette privilégiée** :
  - **Manette (cible principale)** : twin-stick — stick gauche déplace, stick droit oriente/vise, gâchette tire, boutons/roue pour changer d'arme.
  - **Clavier + souris** : la souris oriente le personnage et vise ; **un bouton pour tirer, un bouton pour passer à l'arme suivante** ; touches proches de ZQSD/WASD pour le changement d'arme direct.
- **Skinnable par construction** ✅ — et v0.5 : **une tenue spéciale par arme** (§9.2).
- **Architecture par DataAssets** ✅ : tous les éléments graphiques (meshes, matériaux, VFX, sons, animations, icônes) regroupés dans des **DataAssets dédiés** (`DA_CharacterVisuals`, `DA_CharacterAudio`…), consommés par le code. Rien d'éparpillé dans les Blueprints.
- **Ramassage du loot** ✅ : les drops s'attirent automatiquement vers le joueur (**aimant/auto-aim**) ; la **portée d'attraction augmente avec le niveau du personnage**. Les **drops disparaissent avec le temps** → vraie décision moment-à-moment : ramasser ou continuer à tuer. 🔶 durées (proposition : 10 s de vie + clignotement les 3 dernières).
- 🔶 Autres stats via XP : vitesse de déplacement, bonus globaux ?

---

## 5. Les 7 armes élémentaires ✅

> Document détaillé : `Mechanics/ArmesEtElements.md` (fiches, **tableaux d'évolution complets par arme**, approche technique).

### 5.1 Principes ✅
- **Une arme par élément, 7 armes, un seul switch** ✅ confirmé.
- **Niveau 1 → 100 par arme** ✅ : **chaque niveau apporte un changement** (stats + micro-bonus), et **tous les 5 niveaux un GROS palier** ajoute/upgrade un comportement. 20 paliers majeurs par arme — durée de vie, curiosité d'amélioration, et la puissance nécessaire aux grosses vagues du mode infini.
- **La Spéciale** ✅ (v0.6) : chaque arme a une **capacité active à cooldown** dont l'effet est celui du palier 100, **plus faible à bas niveau** (elle scale avec le niveau de l'arme). Son déclenchement joue une **courte cinématique vidéo** (à la *Epic Seven*), référencée dans le DataAsset de l'arme. Détail : `Mechanics/ArmesEtElements.md` §3.
- Comportements paramétrables par arme (data-driven) : perçant, fragments, **spawn de projectiles secondaires à l'impact**, zones au sol, propagation, rebonds, autoguidage…
- Conséquence assumée : les premiers stages deviennent faciles (farm), les derniers prennent leur sens.

### 5.2 Les armes ✅ types / 🔶 valeurs

| Élément | Arme | Profil | Effet niveau 1 |
|---|---|---|---|
| **Normal** ⚪ | Fusil automatique | mono-cible, cadence soutenue | aucun effet — stats brutes solides, jamais résisté |
| **Feu** 🔴 | Lance-flammes | cône court continu | **Brûlure** : dégâts sur 3 s |
| **Glace** 🔵 | Canon givrant | projectile moyen | **Ralentit** ~30 %, 2 s |
| **Foudre** 🟡 | Arc électrique | tir précis | **Chaîne** sur 1 ennemi proche |
| **Vent** 🟦 | **Canon à tornade** ✅ v0.5 | projectile tornade lent et perçant | **Repousse** les ennemis légers sur son passage |
| **Lumière** ⚪ | **Frappe céleste** ✅ v0.5 | colonne de lumière qui s'abat sur la zone visée | **×2 dégâts sur les boucliers** |
| **Ténèbres** 🟣 | **Laser sombre** ✅ refonte v0.6 | rayon rapide qui **transperce toute la ligne** et **rebondit sur les bords** du terrain | dégâts faibles par cible, multi-hit ; **cooldown de tir élevé** → on switche pendant le cooldown ; évolutions : rebonds, épaisseur, **mini-lasers autoguidés** |

### 5.3 Déblocage ✅
- Départ : **Normal**. Chaque boss de zone débloque l'arme suivante. **Ordre confirmé** ✅ : Feu → Glace → Foudre → Vent → Lumière → Ténèbres.
- Ténèbres, débloquée au boss final, est l'arme « prestige » du stage infini.

### 5.4 Améliorations ✅
- Coût : **ressources élémentaires de l'élément** + or ; courbe de coûts 🔶 (`Progression.md`).
- Chaque niveau : +stats ; **paliers tous les 5 niveaux** : comportement nouveau ou upgrade majeur (tableaux complets dans `ArmesEtElements.md` §3).

---

## 6. Les monstres ✅ modèle

### 6.1 Stats ✅
| Attribut | Rôle |
|---|---|
| PV | survie |
| Défense | réduction des dégâts reçus |
| **Faiblesse élémentaire** | UN élément qui lui fait très mal |
| **Résistances élémentaires** | un ou **plusieurs** éléments encaissés |
| Bouclier (optionnel) | couche de PV à percer d'abord (Lumière efficace) |
| Vitesse / Pattern | propres à chaque type (§6.2) |
| Dégâts au mur | propres à chaque type |

**Pas de table élémentaire globale** ✅ : chaque monstre définit SA faiblesse et SES résistances.

**Monstres support** ✅ élargi v0.5 — des buffs variés, y compris **farfelus**, pour le fun et la priorisation :
- **Soigneur** — soigne en avançant ; au contact, soigne au lieu de frapper ;
- **Bouclier-porteur** — applique des boucliers en avançant et au contact ;
- **Accélérateur** ✅ — booste la vitesse de déplacement des alliés proches ;
- propositions farfelues 🔶 (voir `Bestiaire.md`) : **Ressusciteur** (ranime un mort), **Porte-étendard** (+dégâts au mur autour de lui), **Voleur** (ramasse les drops au sol et les emporte !), **Kamikaze** (explose sur le mur), **Diviseur** (se scinde en deux à la mort), **Brumeux** (voile les alliés : intouchables 1 s).

### 6.2 Patterns de déplacement ✅ principe / 🔶 prototyper
1. **Ligne droite** ⚙️ · 2. **Sinusoïdal** ⚙️ · 3. **Zigzag brusque** · 4. **Charge-pause** · 5. **Flanqueur** (longe les bords) · 6. **Spirale** · 7. **Sauteur** (bonds, intouchable en l'air 🔶) · 8. **Fouisseur** (s'enterre, réapparaît).

### 6.3 Bestiaire ✅
> **Document : `Mechanics/Bestiaire.md`** — le bestiaire **complet des 6 zones** (30 monstres + 6 boss), abstrait en gameplay (stats, faiblesses, patterns, skills définitifs ; noms = placeholders).

- **Direction artistique ✅ (v0.6)** : des créatures **fantastiques venues de l'autre dimension**, dans l'esprit des **Sans-cœur de *Kingdom Hearts*** — designs fun et particuliers, silhouettes fortes, **pas d'animaux réalistes**. Signature proposée : yeux/marquages lumineux de la couleur de l'élément.

- Structure par zone ✅ : 2 types au stage 1, +1 nouveau par stage, les 5 + boss au stage 5.
- **Objectif prototype ✅ (décision v0.5) : le jeu complet en gameplay** — tout le contenu (armes, monstres, stages, économie) fonctionnel ; seule la couche visuelle des levels manquera.
- ⚙️ Base code : `BaseMonster`, `HealthComponent` (à faire évoluer : multiplicateurs élémentaires, bouclier).

---

## 7. Loot et progression ✅

> Document détaillé : `Mechanics/Progression.md`.

Un monstre vaincu droppe : **or**, **XP**, **ressources élémentaires** (de son élément), **objets rares** (selon le monstre).
- **Étoiles ×1/×1,25/×1,5/×2** sur les drops ✅.
- Les drops **expirent au sol** (§4) — l'aimant de ramassage devient une stat qui compte.
- Tout est conservé même en défaite.

---

## 8. Stage infini (31) ✅

- Débloqué après la zone 6. Croissance continue par vague, sans plafond (`Balancing/MonsterScaling.md`).
- Zone de farm de fin de jeu + **record personnel** (meilleure vague, stocké par slot).
- Les hauts niveaux d'armes (50–100) et leurs capstones sont pensés pour ce mode.

---

## 9. Cosmétiques ✅

### 9.1 Périmètre
- Pas de battle pass, pas de boutique au lancement ✅ ; les **skins se gagnent en jouant** (objets rares).
- 🔶 Boutique payante envisageable post-launch (décision reportée).

### 9.2 Tenues liées aux armes ✅ v0.5
- **Chaque arme a sa tenue** : switcher d'arme change aussi la tenue du personnage — 7 looks élémentaires assortis, visuellement forts.
- Les **skins** sont donc des **variantes de tenues par arme** (un set de skins = 7 tenues 🔶 ou à l'unité ?).
- Conséquence technique : le pipeline tenue↔arme doit être data-driven dès le départ (`DA_CharacterVisuals` par arme/skin, même squelette, animations partagées).
- 🔶 pipeline à prototyper AVANT le personnage final.

---

## 10. Direction technique armes / VFX / SFX ✅

> Détail : `Mechanics/ArmesEtElements.md` §5.

- **Le gameplay détecte, Niagara décore** ✅ confirmé : traces/projectiles C++ (⚙️ `Bullet` + `BulletPool`), aucune détection dans Niagara.
- **Debug-first** ✅ confirmé : tous les comportements de projectiles (trajectoires, perçants, splits, rebonds, spawns secondaires) sont **visualisables en debug pur** (lignes/formes colorées par élément, sans mesh ni VFX). La couche visuelle est ajoutée **ensuite**, via DataAssets — **chaque sous-effet peut recevoir sa ref Niagara ET sa ref sonore** ✅ v0.6.
- **3 systèmes Niagara maîtres** (tir, traînée, impact) paramétrés par élément.
- **Une arme = un DataAsset** (`DA_Weapon` : stats, courbe 1–100, comportements par palier, **Spéciale + sa vidéo** ✅, refs VFX/SFX optionnelles par effet et sous-effet — absentes = rendu debug).
- **Un stage = un DataAsset** ✅ v0.6 : `DA_Stage` — liste des vagues, **liste des monstres par vague** + quantités + timing, mode, récompenses.
- **SFX en couches** (MetaSounds).
- **Architecture complète** : `Production/04_Documents/Technical/ArchitectureTechnique.md` (classes, composants, liens, UI).

---

## 11. Interface (UI) 🔶 maquettes / ✅ liste des écrans

- **HUD** : PV du mur, arme active + barre de switch (7), cooldown de Spéciale, vague, loot (avec expiration visible).
- **Menu/hub** : améliorations (armes/perso/mur), déblocages, **encyclopédie** (§2.3), sélection zone/stage/**mode** avec étoiles, record du stage infini.
- **Écran de chargement** ✅ (v0.7) : avant chaque stage — progression du préchargement, astuce 🔶, **transition en fondu** vers le jeu.
- **Options** ✅ (v0.7) : **Jeu** (langue des textes, langue des voix/dub), **Audio** (volume général / musique / SFX), **Graphismes** (qualité Low→Epic, résolution, fenêtré, VSync — la scalabilité standard d'Unreal).
- **Premier démarrage** ✅ : écran de **choix de langue** (une seule fois, modifiable ensuite dans les options).
- **Sauvegardes** : 5 slots, nouvelle partie, suppression.
- Maquettes dans `02_Art/UI/`.

---

## 12. Conventions de production

- Nommage : `WallDefense/ReadeMe.txt` (BP_, SM_, M_, T_, WBP_, DA_, DT_…).
- Équilibrage et définitions (armes, monstres, stages) en **DataTables/DataAssets**, jamais en dur.
- Toute nouvelle règle → d'abord ici, puis implémentée.
- Documentation en français uniquement.

---

## 13. Points à approfondir

*Résolus en v0.6 : skills du mur (liste validée), mur à PV global sans réparation en stage, drops à durée de vie + aimant évolutif, patterns validés, trame de l'histoire (héroïne vs invasion dimensionnelle), bestiaire validé (mandat équilibrage), modes Normal/Hard/Enfer + matériaux par tier, contrôles (souris/manette, manette privilégiée), Spéciale active + vidéo, laser Ténèbres refondu, sous-effets avec VFX+SFX optionnels, `DA_Stage` pour les vagues, encyclopédie à dévoilement progressif (`?????`), direction artistique fantastique (type Sans-cœur).*

### Mandats confiés (BAK : « je te laisse calculer/choisir »)
1. **Équilibrage du bestiaire** — caler stats/faiblesses pour un jeu évolutif et challenging, farm et *try and evolve* en avant.
2. **Économie complète** — coûts d'armes 1–100 (par tiers Fragments/Cristaux/Noyaux), coûts mur/perso, taux de drops, récompenses par étoiles — pour un *try and become stronger* agréable.
→ Les deux seront chiffrés dans `Balancing/` une fois le prototype jouable (les valeurs papier seront fausses ; on calibrera sur le ressenti, avec les garde-fous TTK et « 2–3 tentatives par stage »).

### Gameplay
3. Valider les **tableaux 1–100** des 7 armes (`ArmesEtElements.md` §4) — surtout le laser sombre refondu.
4. Skills du mur : **valeurs** (PV du bouclier de départ, force de l'onde, seuils).
5. Spéciale : cooldown exact, durée des vidéos, skippable.
6. Modes : déblocage par stage ou par zone ; valeurs des multiplicateurs Hard/Enfer.

### Technique
7. **Architecture technique complète** : `04_Documents/Technical/ArchitectureTechnique.md` — classes, composants, DataAssets, UI et bindings, **préchargement, options, i18n** (v0.7). C'est le document de référence AVANT de coder.
8. Refonte top-down (personnage, caméra, visée) selon les contrôles ✅ §4.
9. Sauvegarde 5 slots ; `HealthComponent` élémentaire + bouclier ; `DA_Weapon` 1–100 + Spéciale/vidéo ; `DA_Stage` vagues ; pipeline tenues/skins ; encyclopédie générée des DataAssets.
10. Liste finale des **langues cibles** et périmètre des **dubs** (textes seuls au lancement ? voix sur l'histoire ?) 🔶.
