# Wall Defense — Game Design Document

> **Statut** : v0.5 — 12 juin 2026 — intégration des commentaires BAK (2e passe).
> ✅ = décidé · 🔶 = à approfondir/valider · ⚙️ = existe déjà dans le code.
> Ce document est la **référence unique** des règles du jeu. Documents détaillés : `Mechanics/` et `Balancing/`.

---

## 1. Vision

### 1.1 Pitch ✅
Un **tower defense incarné**, vue **top-down (Zelda-like)** : le joueur se déplace librement sur le terrain et défend son **mur** contre des vagues de monstres. Il possède **7 armes — une par élément** (Normal, Feu, Glace, Foudre, Vent, Lumière, Ténèbres) et **switche** entre elles : chaque monstre est **faible à un élément** et **résistant à un ou plusieurs autres** — connaître son bestiaire et switcher au bon moment, c'est tout le skill.

Chaque arme évolue jusqu'au **niveau 100** : un changement à chaque niveau, un **gros palier de comportement tous les 5 niveaux**. Progression par le loot ; **30 stages** (6 zones de 5) + un **stage infini**.

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

---

## 2. Boucle de jeu ✅

```
MENU (hub)
 ├─ AMÉLIORER : armes (ressources élémentaires + or), personnage (XP/or), MUR (PV/défense/skills)
 ├─ DÉBLOQUER : skins et équipements (objets rares + or) — les armes, elles, se gagnent sur les boss
 ├─ ENCYCLOPÉDIE : consulter armes, effets, coûts d'évolution, monstres rencontrés
 └─ JOUER : choisir un stage déjà atteint (progression linéaire)
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

### 2.3 Histoire, guide et encyclopédie ✅ principe / 🔶 contenu
- **Petite histoire « arcade »** : un contexte léger raconté en avançant dans les zones (écrans courts entre les stages clés — intro de zone, victoire de boss). Pas de cinématiques lourdes ; quelques lignes + illustrations 🔶 ton et trame à écrire.
- **Guide de début de partie** : un tutoriel basique qui explique le cœur (tirer, switcher, améliorer une arme, le mur). Court, contextuel, skippable.
- **Encyclopédie** consultable au menu : pages des **armes** (effets actuels, prochain palier, **objets nécessaires** pour évoluer) et 🔶 pages des **monstres** rencontrés (faiblesse/résistances découvertes).

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
- 🔶 PV global unique ou par segments ? Réparation en cours de stage ?

---

## 4. Le personnage

- **Tire** sur les vagues avec ses armes (§5). Ne meurt pas — seul le mur compte ✅.
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
- Comportements paramétrables par arme (data-driven) : perçant, fragments, **spawn de projectiles secondaires à l'impact**, zones au sol, propagation…
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
| **Ténèbres** 🟣 | **Rayon laser sombre** ✅ v0.5 | faisceau continu | **Marque** : +15 % dégâts subis, 3 s |

*(v0.5 : le laser passe aux Ténèbres ✅ ; le Vent reçoit la tornade — plus lisible et plus « vent » que le fusil à rafale ; la Lumière frappe depuis le ciel.)*

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
> **Document : `Mechanics/Bestiaire.md`** — le bestiaire **complet des 6 zones** (30 monstres + 6 boss), abstrait (`Monstre_01`… : stats, faiblesses, patterns, skills ; designs et noms remplacés plus tard).

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
- **Debug-first** ✅ v0.5 : tous les comportements de projectiles (trajectoires, perçants, splits, spawns secondaires) sont **visualisables en debug pur** (lignes/formes colorées par élément, sans mesh ni VFX) pour tester le gameplay réel. La couche visuelle est ajoutée **ensuite**, via DataAssets — pour le projectile principal ET les sous-effets (splits, fragments).
- **3 systèmes Niagara maîtres** (tir, traînée, impact) paramétrés par élément.
- **Une arme = un DataAsset** (`DA_Weapon` : stats, courbe 1–100, comportements par palier, refs VFX/SFX optionnelles — absentes = rendu debug).
- **SFX en couches** (MetaSounds).

---

## 11. Interface (UI) 🔶

- **HUD** : PV du mur, arme active + barre de switch (7), vague, loot (avec expiration visible).
- **Menu/hub** : améliorations (armes/perso/mur), déblocages, **encyclopédie** (§2.3), sélection zone/stage avec étoiles, record du stage infini.
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

*Résolus en v0.5 : seuils d'étoiles (tiers), multiplicateurs de drops, laser → Ténèbres, tornade → Vent, frappe céleste → Lumière, niveaux 1–100 (palier majeur /5), ordre de déblocage, skills du mur (principe), tenues par arme, auto-aim de ramassage + drops qui expirent, monstres support élargis, debug-first, prototype = jeu complet gameplay.*

### Gameplay
1. **Valider les tableaux d'évolution 1–100** des 7 armes (`ArmesEtElements.md` §3) — types et valeurs.
2. **Skills du mur** — liste finale, conditions de déclenchement, niveaux de déblocage, valeurs.
3. **Le mur** — PV global ou segments ; réparation en cours de stage ?
4. **Drops qui expirent** — durée de vie, courbe de portée de l'aimant par niveau.
5. **Patterns** — prototyper la liste, valider la lisibilité top-down.
6. **Histoire arcade** — trame, ton, quantité de texte par zone ; contenu du tutoriel.

### Contenu et équilibrage
7. **Valider le bestiaire** (`Bestiaire.md`) — 30 monstres + 6 boss : stats, faiblesses, patterns.
8. **Courbes des 30 stages + infini** — recaler `Balancing/MonsterScaling.md` (zones, nv d'armes 1–100).
9. **Économie** — coûts 1–100 des armes, coûts mur/perso, taux et durée de vie des drops, récompenses par étoiles.

### Technique
10. **Refonte top-down** — personnage libre, caméra, visée (souris ? auto-aim ? 🔶).
11. **Sauvegarde 5 slots** — étendre `WallDefenseSaveGame`/`AutoSaveComponent`.
12. **`HealthComponent`** — multiplicateurs élémentaires, bouclier en couche.
13. **`DA_Weapon` 1–100** — schéma de données des paliers ; rendu **debug-first** des projectiles.
14. **DataAssets personnage + tenues par arme** — pipeline skins.
15. **Encyclopédie** — données auto-générées depuis les DataAssets (armes : effets actuels/prochains, coûts).
