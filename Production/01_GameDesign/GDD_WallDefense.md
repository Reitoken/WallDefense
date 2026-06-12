# Wall Defense — Game Design Document

> **Statut** : v0.3 — 12 juin 2026 — refonte (développement solo).
> ✅ = décidé · 🔶 = à approfondir/valider · ⚙️ = existe déjà dans le code.
> Ce document est la **référence unique** des règles du jeu. Documents détaillés : `Mechanics/` et `Balancing/`.

---

## 1. Vision

### 1.1 Pitch ✅
Un **tower defense incarné** : le joueur contrôle un personnage qui défend son **mur** contre des vagues de monstres, sur un champ de bataille ouvert (**plus de lanes**). Pour survivre, il doit tuer vite — en jonglant entre ses armes sur **deux niveaux de switch** : changer de **type d'arme** (fusil ↔ shotgun) et changer d'**élément** (Normal, Feu, Glace, Foudre, Vent, Lumière, Ténèbres). Chaque monstre a ses **faiblesses élémentaires** : connaître son bestiaire et switcher au bon moment, c'est tout le skill.

Un **petit jeu complet** : 30 stages + 1 stage infini avec **classement Steam**. Progression par le loot (argent, XP, ressources élémentaires, objets rares) ; monétisation **100 % cosmétique** (skins + battle pass).

### 1.2 Piliers ✅
1. **Le bon élément au bon moment** — le switch d'arme à deux niveaux est LE cœur du gameplay.
2. **Connaître son ennemi** — mémoriser les faiblesses des monstres récompense l'expérience.
3. **Perdre fait progresser** — le loot est toujours conservé ; on revient plus fort.
4. **Cosmétique seulement** — la puissance ne s'achète jamais avec de l'argent réel.

### 1.3 Format ✅
- Solo, PC (Steam). Seule feature en ligne : le **leaderboard du stage infini**.
- Contenu : **30 stages** à difficulté croissante + **stage 31 infini**.

---

## 2. Boucle de jeu ✅

```
MENU (hub)
 ├─ S'ÉQUIPER : composer son loadout d'armes (types × éléments) — obligatoire avant de partir
 ├─ AMÉLIORER : armes (ressources élémentaires + or), personnage (XP/or)
 ├─ DÉBLOQUER : nouvelles armes, équipements, skins (objets rares + or)
 ├─ BOUTIQUE / BATTLE PASS : cosmétiques uniquement
 └─ JOUER : choisir un stage (1–30 débloqués progressivement, 31 = infini)
      └─> STAGE : vagues de monstres → ils attaquent le mur
           ├─ VICTOIRE → étoiles selon les PV restants du mur + loot bonus → stage suivant
           └─ DÉFAITE (mur détruit) → retour menu, LOOT CONSERVÉ → améliorer → retenter
```

### 2.1 Étoiles de stage ✅ / 🔶 seuils
Chaque stage rapporte **0 à 3 étoiles** selon les PV restants du mur à la victoire :

| PV du mur | Étoiles |
|---|---|
| 100 % | ⭐⭐⭐ |
| ≥ 75 % | ⭐⭐ |
| ≥ 50 % | ⭐ |
| < 50 % | 0 (victoire quand même) |

> 🔶 Tu as mentionné « 25 % = 0 » : à confirmer si < 25 % doit avoir une conséquence supplémentaire, ou si 0 étoile commence dès < 50 %.
- 🔶 À quoi servent les étoiles ? (proposition : déblocages bonus, exigence d'accès à certains stages, récompenses du battle pass gratuit.)

---

## 3. L'arène et le mur

### 3.1 Champ de bataille ✅ décidé / 🔶 modèle exact
- **Plus de lanes** (décision v0.3). Les monstres arrivent en vagues depuis le fond du terrain et convergent vers le mur, chacun avec sa vitesse et son **pattern de déplacement** propre.
- ⚙️ Le code actuel (`LaneGrid`, `LanePlayerCharacter`) est construit sur les lanes → à remplacer.
- 🔶 **Modèle de déplacement du joueur à définir** (question n°1 de la §13) : libre sur le terrain ? confiné à une bande devant le mur ? sur le mur ?

### 3.2 Le mur ✅ rôle / 🔶 détails
- Le mur a des **PV** ; les monstres au contact (ou à distance) l'attaquent, chacun avec **ses propres dégâts au mur**.
- Défaite = mur détruit. Les PV restants déterminent les étoiles (§2.1).
- 🔶 PV global unique ou par segments ? Réparation entre les stages ? Mur améliorable ?

---

## 4. Le personnage

- **Tire** sur les vagues avec son loadout d'armes (§5). Ne meurt pas — seul le mur compte ✅.
- **Skinnable par construction** ✅ : le personnage est conçu dès le départ pour recevoir des skins (voir §9.3 — contrainte technique forte, à poser avant de produire le perso définitif).
- 🔶 Mobilité exacte, esquive éventuelle, stats du personnage (montées via XP) — à détailler.

---

## 5. Armes et éléments ✅ concept

> Document détaillé : `Mechanics/ArmesEtElements.md` (catégories, effets élémentaires, double switch, améliorations, approche technique VFX/SFX).

- **7 éléments** : Normal, Feu 🔴, Glace 🔵, Foudre 🟡 (jaune), Vent 🟦 (turquoise), Lumière ⚪ (blanche), Ténèbres 🟣 (mauve).
- **Catégories d'armes** (fusil, shotgun, … 🔶) × éléments = la matrice d'armes à débloquer.
- **Double switch** ✅ :
  - **Switch basique** : changer de catégorie (fusil Feu → shotgun Feu).
  - **Switch élémentaire** : changer d'élément dans la catégorie (shotgun Feu → shotgun Glace).
- **Loadout** ✅ : on s'équipe au menu avant de partir — le stage se joue avec ce qu'on a emporté.
- **Améliorer une arme donne des avantages** (dégâts, et paliers qualitatifs propres à l'arme) 🔶 détails par arme.
- Chaque arme a ses **VFX** (Niagara) et **SFX** colorés par élément.

---

## 6. Les monstres ✅ modèle de stats

Chaque monstre est défini par :

| Attribut | Rôle |
|---|---|
| PV | survie |
| Défense | réduction des dégâts reçus |
| **Résistance élémentaire** | élément contre lequel il encaisse moins (ex. ×0,5) |
| **Faiblesse élémentaire** | élément contre lequel il prend plus (ex. ×1,75) — le cœur du jeu |
| Bouclier (optionnel) | couche de PV à percer d'abord 🔶 mécanique exacte |
| Soin (certains monstres) | des **healers** soignent les autres → cibles prioritaires |
| Vitesse de marche | propre à chaque type |
| Pattern de déplacement | ⚙️ droit/sinusoïdal existants ; à enrichir sans lanes 🔶 |
| Dégâts au mur | propres à chaque type |

- ⚙️ Base saine dans le code : `BaseMonster` (patterns, mêlée/distance, feedback d'impact), `HealthComponent` (PV, défense plate 🔶 à faire évoluer pour les multiplicateurs élémentaires).
- Le bestiaire complet (fiches par monstre : stats, élément, rôle, stage d'apparition) est à construire dans `Mechanics/` 🔶.

---

## 7. Loot et progression ✅ structure

> Document détaillé : `Mechanics/Progression.md`.

Un monstre vaincu droppe :
1. **De l'argent** (or) — monnaie universelle in-game.
2. **De l'expérience** — niveaux du personnage.
3. **Des ressources élémentaires** (par élément) — améliorer les armes de cet élément.
4. **Des objets rares** (selon le monstre) — débloquer équipements, armes, skins gagnables.

Tout sert à : **débloquer** (armes, équipements, skins in-game) et **power up** (personnage, armes).

---

## 8. Stage infini (31) et leaderboard Steam ✅

- Difficulté en **croissance continue** (par vague, sans plafond) : courbe dans `Balancing/MonsterScaling.md`.
- Le joueur s'améliore en jouant (loot) pour repousser son record — c'est la rejouabilité long terme.
- **Score** publié sur un **leaderboard Steam** (Steamworks). Seule feature online du jeu.
- 🔶 Formule de score (vague atteinte ? kills ? PV du mur ?), règles anti-triche minimales.

---

## 9. Monétisation ✅ cadre

### 9.1 Règle absolue
**Cosmétique uniquement.** Aucun gameplay, aucune puissance, aucun raccourci de progression contre de l'argent réel.

### 9.2 Produits
- **Boutique de skins** (argent réel) : skins de personnage, et 🔶 périmètre à définir (armes ? mur ? VFX ?).
- **Battle pass** : récompenses **exclusivement cosmétiques**. 🔶 Modèle (saisons ? gratuit/payant ? progression par défis ?).
- À distinguer des **skins gagnables in-game** (objets rares, §7) — les deux familles coexistent.

### 9.3 Contrainte technique : personnage skinnable
Le personnage doit être **construit pour recevoir des skins** dès le départ :
- mesh modulaire (slots : corps, tête, accessoires 🔶) et/ou skins par mesh complet sur le même squelette ;
- matériaux paramétrés ; les animations partagées par tous les skins ;
- 🔶 pipeline exact à définir AVANT de produire le personnage final (refaire un perso non modulaire coûte très cher).

---

## 10. Direction technique armes / VFX / SFX ✅ approche

> Détail et justification : `Mechanics/ArmesEtElements.md` §5.

Résumé de la recommandation :
- **La détection des impacts reste dans le gameplay** (traces/projectiles C++ — ⚙️ `Bullet` + `BulletPool` existants), **jamais dans Niagara**. Les modules de collision Niagara sont faits pour le visuel (débris qui rebondissent), pas pour infliger des dégâts.
- **Niagara = visuel pur**, piloté par le gameplay : 3 systèmes « maîtres » paramétrés (tir, trainée, impact) × paramètres d'élément (couleur, intensité) — au lieu de dupliquer 3 effets × 7 éléments × N catégories.
- **Une arme = un DataAsset** (stats, élément, refs VFX/SFX) : créer une nouvelle arme ne demande aucun code.
- **SFX en couches** (MetaSounds) : son mécanique de la catégorie + couche élémentaire.

---

## 11. Interface (UI) 🔶

- HUD : PV du mur, arme/élément actifs + roue/barre de switch, progression de vague, loot ramassé.
- Menu/hub : loadout, améliorations, déblocages, boutique, battle pass, sélection de stage (carte ? liste ?), étoiles par stage.
- Leaderboard du stage 31.
- Maquettes à faire dans `02_Art/UI/`.

---

## 12. Conventions de production

- Nommage des assets : `WallDefense/ReadeMe.txt` (BP_, SM_, M_, T_, WBP_, DA_, DT_…).
- Tout l'équilibrage en **DataTables/DataAssets**, jamais en dur.
- Toute nouvelle règle de gameplay → d'abord notée ici, puis implémentée.
- Documentation en français uniquement.

---

## 13. Points à approfondir (la liste des prochaines décisions)

### Gameplay
1. **Déplacement du joueur sans lanes** — libre ? bande devant le mur ? Impact direct sur le code à refondre (`LaneGrid`/`LanePlayerCharacter`) et sur la caméra. *La décision la plus structurante.*
2. **Table élémentaire complète** — qui est fort/faible contre quoi (cycle Feu/Glace/…, opposition Lumière/Ténèbres ?) et **effet de statut par élément** (brûlure, gel, chaîne d'éclairs, projection…).
3. **Catégories d'armes** — combien et lesquelles (fusil, shotgun, + sniper ? lance-grenades ?) ; taille du loadout (combien d'armes emportées ?) ; contrôles du double switch (molette + touches ?).
4. **Le mur** — PV global ou segments, réparation, améliorable ou non.
5. **Boucliers et healers** — perce-bouclier par quel élément ? portée/ciblage du heal ?
6. **Patterns de déplacement** sans lanes — lesquels (zigzag, vol, fouisseur, sauteur ?).

### Contenu et équilibrage
7. **Matrice de contenu armes × éléments** — N catégories × 7 éléments explose vite (3 cat. = 21 armes à équilibrer + VFX/SFX). Ordre de déblocage et périmètre MVP à trancher.
8. **Courbe des 30 stages** + courbe infinie du 31 (`Balancing/MonsterScaling.md` à recaler).
9. **Économie à 4 monnaies** (or, XP, ressources élémentaires ×7, objets rares) — taux de drop, coûts, risque de farm obligatoire.
10. **Bestiaire** — combien de monstres pour couvrir 30 stages sans lasser (rôles × éléments × variantes).
11. **Seuils d'étoiles** — clarifier le « 25 % » (§2.1) et l'usage des étoiles.

### Méta et business
12. **Score du stage infini** — formule, et stratégie anti-triche (un leaderboard Steam se fait polluer facilement : validation serveur impossible sans serveur → choisir son niveau d'ambition).
13. **Battle pass** — saisons et durée, gratuit/payant, source des défis ; charge de production de cosmétiques en solo (c'est un pipeline de contenu permanent).
14. **Modèle de prix** — jeu payant + cosmétiques, ou free-to-play + cosmétiques ? (Impacte tout le funnel Steam.)
15. **Pipeline de skins** — architecture modulaire du personnage (§9.3) à prototyper tôt.
16. **Steamworks** — leaderboard, succès, cloud save : intégration à planifier.

### Technique
17. **Refonte du déplacement** (post-décision n°1) et de la caméra.
18. **Évolution de `HealthComponent`** — défense plate → multiplicateurs élémentaires (résistance/faiblesse), bouclier.
19. **Prototype VFX paramétré par élément** — valider l'approche « 3 systèmes maîtres » sur une arme avant de produire les 7 éléments.
