# Wall Defense — Game Design Document

> **Statut** : v0.4 — 12 juin 2026 — intégration des décisions de Pierre (commentaires BAK).
> ✅ = décidé · 🔶 = à approfondir/valider · ⚙️ = existe déjà dans le code.
> Ce document est la **référence unique** des règles du jeu. Documents détaillés : `Mechanics/` et `Balancing/`.

---

## 1. Vision

### 1.1 Pitch ✅
Un **tower defense incarné**, vue **top-down (Zelda-like)** : le joueur se déplace librement sur le terrain et défend son **mur** contre des vagues de monstres. Il possède **7 armes — une par élément** (Normal, Feu, Glace, Foudre, Vent, Lumière, Ténèbres) et **switche** entre elles : chaque monstre est **faible à un élément** et **résistant à un ou plusieurs autres** — connaître son bestiaire et switcher au bon moment, c'est tout le skill.

Chaque arme **évolue avec ses niveaux** : elle gagne des stats ET des comportements nouveaux (ex. l'arme Normal transperce 2 ennemis au niveau 5, fait éclater des fragments à la mort d'un ennemi au niveau 10). Progression par le loot ; **30 stages** (6 zones de 5) + un **stage infini**.

**Un simple jeu premium à la vente** : pas de classement en ligne, pas de battle pass — aucune feature online. ✅

### 1.2 Piliers ✅
1. **La bonne arme au bon moment** — le switch élémentaire est LE cœur du gameplay.
2. **Connaître son ennemi** — mémoriser les faiblesses des monstres récompense l'expérience.
3. **Perdre fait progresser** — le loot est toujours conservé ; on revient plus fort.
4. **Les armes grandissent avec le joueur** — chaque niveau d'arme change sa façon de jouer, pas seulement ses chiffres.

### 1.3 Format ✅
- Solo, PC (Steam), **hors ligne**. Jeu premium (achat unique).
- Contenu : **30 stages** organisés en **6 zones de 5 stages** (4 normaux + 1 boss) + **stage 31 infini**.
- **Sauvegarde : 5 slots** de partie indépendants, sauvegarde et chargement **automatiques**, possibilité de recommencer à zéro (nouvelle partie). ⚙️ base existante : `WallDefenseSaveGame` + `AutoSaveComponent` (à étendre aux slots).

---

## 2. Boucle de jeu ✅

```
MENU (hub)
 ├─ AMÉLIORER : armes (ressources élémentaires + or), personnage (XP/or), MUR (PV/défense)
 ├─ DÉBLOQUER : skins et équipements (objets rares + or) — les armes, elles, se gagnent sur les boss
 └─ JOUER : choisir un stage déjà atteint (progression linéaire : un stage se choisit
    seulement s'il a été terminé — le prochain stage non terminé est le « front »)
      └─> STAGE : vagues de monstres → ils attaquent le mur
           ├─ VICTOIRE → étoiles selon PV du mur → récompenses × étoiles → stage suivant
           │             (boss de zone vaincu → NOUVELLE ARME débloquée)
           └─ DÉFAITE (mur détruit) → retour menu, LOOT CONSERVÉ → améliorer → retenter
```

### 2.1 Structure des zones ✅
- **1 zone = 5 stages** : 4 stages normaux + 1 **stage de boss**.
- **Battre le boss d'une zone débloque une nouvelle arme élémentaire** (§5.2) et ouvre la zone suivante.
- 6 zones × 5 stages = 30, puis le **stage 31 (infini)** se débloque après la zone 6.
- Chaque zone a une identité (thème/monde) ; le bestiaire y est propre (§6.3).

### 2.2 Étoiles de stage ✅ / 🔶 valeurs
Chaque victoire rapporte **0 à 3 étoiles** selon les PV restants du mur :

| PV du mur | Étoiles |
|---|---|
| 100 % | ⭐⭐⭐ |
| ≥ 75 % | ⭐⭐ |
| ≥ 50 % | ⭐ |
| < 50 % | 0 (victoire quand même) |

**Rôle des étoiles** ✅ :
- **Récompense fixe de fin de stage** selon le nombre d'étoiles.
- Les étoiles **multiplient les drops obtenus** pendant le stage et donnent des **chances de drop rare** accrues 🔶 valeurs (proposition : ×1 / ×1,25 / ×1,5 / ×2).
- **3 étoiles = bonus supplémentaire** (objet rare garanti ? 🔶).

> 🔶 Reste à confirmer : le seuil « 25 % » évoqué initialement (conséquence sous 25 % ?) — pour l'instant 0 étoile dès < 50 %.

---

## 3. L'arène et le mur

### 3.1 Champ de bataille ✅
- **Le joueur est libre sur le terrain. Caméra vue top-down, comme un Zelda-like.** ✅
- Les monstres arrivent en vagues depuis le fond et convergent vers le mur, chacun avec sa vitesse et son **pattern de déplacement** (§6.2).
- ⚙️ Le code actuel (`LaneGrid`, `LanePlayerCharacter`) est construit sur les lanes → à remplacer par un déplacement libre top-down + nouvelle caméra.

### 3.2 Le mur ✅
- Le mur a des **PV et une défense** ; les monstres l'attaquent, chacun avec **ses propres dégâts au mur**.
- **Le mur est améliorable** ✅ : augmenter ses **PV** et sa **défense** au menu — indispensable pour ne pas se faire one-shot par des monstres de plus en plus forts.
- Défaite = mur détruit. PV restants = étoiles (§2.2).
- 🔶 PV global unique ou par segments ? Réparation en cours de stage ?

---

## 4. Le personnage

- **Tire** sur les vagues avec ses armes (§5). Ne meurt pas — seul le mur compte ✅.
- **Skinnable par construction** ✅ (§9.2).
- **Architecture par DataAssets** ✅ : tous les éléments graphiques du personnage (meshes, matériaux, VFX, sons, animations, icônes) sont regroupés dans des **DataAssets dédiés**, consommés directement par le code et les features. Plusieurs DataAssets si besoin (ex. `DA_CharacterVisuals`, `DA_CharacterAudio`…) pour faciliter l'implémentation et la maintenance — tout est rassemblé, rien d'éparpillé dans les Blueprints.
- 🔶 Stats du personnage montées via XP (vitesse ? rayon de ramassage ? bonus globaux ?) — à détailler.

---

## 5. Les 7 armes élémentaires ✅ concept

> Document détaillé : `Mechanics/ArmesEtElements.md` (fiches d'armes, évolutions, approche technique VFX/SFX).

### 5.1 Principes ✅
- **Une arme par élément, 7 armes au total.** Un seul **switch** : changer d'arme = changer d'élément (le double switch est abandonné).
- Chaque arme **évolue avec ses niveaux** : stats ET nouveaux comportements. Exemples : transpercer 2 ennemis (nv 5), éclater en fragments à la mort de la cible (nv 10), **spawner des projectiles secondaires à l'impact** — tout cela **paramétrable par arme** (data-driven).
- Conséquence assumée : avec la montée en puissance, **les premiers stages deviennent faciles** (farm confortable) et **les stages de fin prennent leur sens**.

### 5.2 Les armes 🔶 proposition (types et effets de niveau 1 à valider)

| Élément | Arme | Profil | Effet niveau 1 |
|---|---|---|---|
| **Normal** ⚪ | Fusil automatique | mono-cible, cadence soutenue | aucun effet — stats brutes solides, jamais résisté |
| **Feu** 🔴 | Lance-flammes | cône court continu | **Brûlure** : dégâts sur 3 s |
| **Glace** 🔵 | Canon givrant | projectile moyen | **Ralentit** la cible (~30 %, 2 s) |
| **Foudre** 🟡 | Arc électrique | tir précis | **Chaîne** sur 1 ennemi proche |
| **Vent** 🟦 | Fusil à rafale | cône large, courte portée | **Repousse** les ennemis légers |
| **Lumière** ⚪ | Rayon laser | faisceau continu précis | **×2 dégâts sur les boucliers** |
| **Ténèbres** 🟣 | Lance-orbes | projectile lent en zone | **Marque** : la cible subit +15 % de dégâts (3 s) |

### 5.3 Déblocage ✅ mécanique / 🔶 ordre
- On commence avec l'arme **Normal**. **Chaque boss de zone vaincu débloque l'arme suivante** : 6 boss = 6 armes élémentaires.
- 🔶 Ordre proposé : Feu (zone 1) → Glace → Foudre → Vent → Lumière → Ténèbres (zone 6).

### 5.4 Améliorations ✅
- Coût : **ressources élémentaires de l'élément de l'arme** + or.
- Chaque niveau : **+stats** ; les **paliers** (5/10/15/20 🔶) ajoutent un **comportement nouveau** propre à l'arme (grille par arme dans `ArmesEtElements.md`).

---

## 6. Les monstres ✅ modèle

### 6.1 Stats ✅
| Attribut | Rôle |
|---|---|
| PV | survie |
| Défense | réduction des dégâts reçus |
| **Faiblesse élémentaire** | UN élément qui lui fait très mal — le cœur du jeu |
| **Résistances élémentaires** | un ou **plusieurs** éléments qu'il encaisse bien |
| Bouclier (optionnel) | couche de PV à percer d'abord (Lumière efficace) |
| Vitesse de marche | propre à chaque type |
| Pattern de déplacement | propre à chaque type (§6.2) |
| Dégâts au mur | propres à chaque type |

**Pas de table élémentaire globale** ✅ : pas de cycle de forces (Feu > Glace…). Chaque monstre définit simplement SA faiblesse et SES résistances.

**Monstres support** ✅ : certains monstres **buffent les autres** → cibles prioritaires sous peine d'être envahi :
- ils peuvent **soigner** ou **appliquer des boucliers** aux alliés **pendant leur avancée** (aura/pulsation) ;
- arrivés « au contact », leur **attaque** peut aussi être un soin ou un bouclier (au lieu de frapper le mur).

### 6.2 Patterns de déplacement ✅ principe / 🔶 liste à prototyper
Chaque monstre a son pattern ; plusieurs patterns différents font la variété des vagues. Propositions :
1. **Ligne droite** ⚙️ (existant)
2. **Sinusoïdal** ⚙️ (existant)
3. **Zigzag brusque** — changements de direction aléatoires
4. **Charge-pause** — sprinte puis s'arrête, en rythme
5. **Flanqueur** — longe les bords du terrain puis converge vers le mur
6. **Spirale** — s'approche en orbitant
7. **Sauteur** — bonds successifs (intouchable en l'air 🔶)
8. **Fouisseur** — s'enterre, réapparaît plus loin

### 6.3 Bestiaire abstrait ✅
- Le bestiaire reste **abstrait** pour l'instant : `Monstre_01`, `Monstre_02`… avec stats, pattern, attaques/skills. Les designs et les noms seront remplacés plus tard ; fonctionnellement tout peut être construit dès maintenant, cohérent avec le thème de chaque zone.
- **Structure par zone (5 stages)** ✅ : **5 monstres + 1 boss** par zone, introduits progressivement :

| Stage de la zone | Monstres présents |
|---|---|
| 1 | 2 types |
| 2 | 3 types (1 nouveau) |
| 3 | 4 types (1 nouveau) |
| 4 | 5 types (1 nouveau) |
| 5 (boss) | les 5 types + le **boss** |

- 6 zones → **30 monstres + 6 boss** à terme (avec variantes/recolorations possibles entre zones pour réduire le coût).
- ⚙️ Base code : `BaseMonster`, `HealthComponent` (défense plate → à faire évoluer : multiplicateurs élémentaires, bouclier).

---

## 7. Loot et progression ✅ structure

> Document détaillé : `Mechanics/Progression.md`.

Un monstre vaincu droppe :
1. **De l'argent (or)** — monnaie universelle in-game.
2. **De l'expérience** — niveaux du personnage.
3. **Des ressources élémentaires** — améliorer l'arme de l'élément correspondant.
4. **Des objets rares** (selon le monstre) — débloquer équipements et skins gagnables.

- Les **étoiles du stage multiplient les drops** et les chances de rare (§2.2).
- Tout est conservé même en défaite.

---

## 8. Stage infini (31) ✅

- Se débloque après la zone 6. Difficulté en **croissance continue par vague**, sans plafond (`Balancing/MonsterScaling.md`).
- C'est la **rejouabilité long terme** et la **zone de farm** de fin de jeu : on y mesure sa build et on bat son **record personnel** (meilleure vague atteinte, stocké dans la sauvegarde).
- ~~Leaderboard Steam~~ ✅ **supprimé** (v0.4) : aucune feature online.

---

## 9. Cosmétiques ✅ cadre réduit

### 9.1 Périmètre
- ~~Battle pass~~ ✅ **supprimé** (v0.4). ~~Leaderboard~~ supprimé. **Jeu premium simple.**
- Les **skins se gagnent en jouant** (objets rares, §7).
- 🔶 Une boutique de skins payante reste envisageable **post-launch** — décision reportée, n'impacte pas le développement du jeu (le perso est skinnable de toute façon).

### 9.2 Contrainte technique : personnage skinnable
Le personnage est **construit pour recevoir des skins** dès le départ :
- mesh modulaire et/ou skins par mesh complet sur le même squelette ; matériaux paramétrés ; animations partagées ;
- les visuels du personnage vivent dans des **DataAssets** (§4) — un skin = un DataAsset de visuels ;
- 🔶 pipeline exact à prototyper AVANT de produire le personnage final.

---

## 10. Direction technique armes / VFX / SFX ✅ approche

> Détail et justification : `Mechanics/ArmesEtElements.md` §5.

- **La détection des impacts reste dans le gameplay** (traces/projectiles C++ — ⚙️ `Bullet` + `BulletPool`), **jamais dans Niagara** (ses collisions sont cosmétiques).
- **Niagara = visuel pur** : 3 systèmes « maîtres » paramétrés (tir, traînée, impact) × paramètres d'élément (couleur, intensité) — 7 armes ≠ 21 systèmes.
- **Une arme = un DataAsset** (`DA_Weapon` : stats, élément, courbe d'évolution par niveau, comportements de projectile — perçant, fragments, spawns secondaires —, refs VFX/SFX) : créer/équilibrer une arme ne demande aucun code.
- **SFX en couches** (MetaSounds) : corps mécanique + couche élémentaire.

---

## 11. Interface (UI) 🔶

- **HUD** : PV du mur, arme active + barre de switch (7 emplacements), progression de vague, loot ramassé.
- **Menu/hub** : améliorations (armes/perso/mur), déblocages, sélection de zone/stage avec étoiles, record du stage infini.
- **Gestion des sauvegardes** : 5 slots, nouvelle partie, suppression.
- Maquettes à faire dans `02_Art/UI/`.

---

## 12. Conventions de production

- Nommage des assets : `WallDefense/ReadeMe.txt` (BP_, SM_, M_, T_, WBP_, DA_, DT_…).
- Tout l'équilibrage et les définitions (armes, monstres, stages) en **DataTables/DataAssets**, jamais en dur.
- Toute nouvelle règle de gameplay → d'abord notée ici, puis implémentée.
- Documentation en français uniquement.

---

## 13. Points à approfondir

*Résolus en v0.4 : déplacement (libre, top-down Zelda-like), un seul switch, 7 armes (1/élément), pas de table élémentaire, déblocage d'armes par boss, structure 6 zones × 5 stages, rôle des étoiles, mur améliorable, 5 slots de sauvegarde, pas de leaderboard ni battle pass.*

### Gameplay
1. **Fiches des 7 armes** — valider types/effets nv 1 (§5.2) et concevoir la **grille d'évolution par niveau de chaque arme** (paliers 5/10/15/20).
2. **Le mur** — PV global ou segments ; réparation en cours de stage ou non ; coûts d'amélioration PV/défense.
3. **Monstres support** — portée/cadence des heals et boucliers, en marche et « au contact » ; contre-jeu (Lumière anti-bouclier suffit-il ?).
4. **Patterns** — prototyper la liste §6.2, décider lesquels sont lisibles en top-down.
5. **Stats du personnage via XP** — lesquelles (vitesse, ramassage, bonus globaux ?).
6. **Étoiles** — valeurs des multiplicateurs de drops et du bonus 3★ ; trancher le seuil « 25 % ».

### Contenu et équilibrage
7. **Courbes des 30 stages** (par zone) + courbe du stage infini — recaler `Balancing/MonsterScaling.md` sur la structure 6×5.
8. **Économie** — taux de drop, coûts d'amélioration (armes/perso/mur), récompenses fixes par étoiles.
9. **Bestiaire abstrait zone 1** — les 5 premiers monstres + boss (stats, faiblesses, patterns) pour le prototype.

### Technique
10. **Refonte top-down** — personnage libre (remplace `LanePlayerCharacter`), caméra Zelda-like, visée (vers la souris ? auto-aim ? 🔶).
11. **Système de sauvegarde 5 slots** — étendre `WallDefenseSaveGame`/`AutoSaveComponent` (slots, nouvelle partie, auto-load).
12. **`HealthComponent`** — multiplicateurs élémentaires (faiblesse/résistances multiples), bouclier en couche.
13. **`DA_Weapon` évolutif** — schéma de données des comportements par niveau (perçant, fragments, spawns à l'impact) sans code par arme.
14. **DataAssets du personnage** — découpage (visuels/audio/animations) et lien avec le système de skins.
15. **Prototype VFX paramétré** — valider les 3 systèmes Niagara maîtres sur 2 armes avant de produire les 7.
