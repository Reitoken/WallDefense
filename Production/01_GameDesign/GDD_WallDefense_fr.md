# Wall Defense — Game Design Document

> **Statut du document** : brouillon v0.1 — 9 juin 2026
> Les règles marquées ✅ sont déjà implémentées dans le projet. Celles marquées 🔶 sont à décider/valider.
> Ce document est la **référence unique** des règles du jeu : toute mécanique ajoutée au code doit être décrite ici.

---

## 1. Vision

### 1.1 Pitch
🔶 *À affiner — proposition de départ :*
Un jeu de défense où le joueur, seul sur un champ de bataille divisé en **lanes**, protège un **mur** contre des vagues de monstres. Le joueur se déplace de lane en lane à la manière de *Mega Man Battle Network* et abat les assaillants avant qu'ils n'atteignent (ou ne détruisent) le mur.

### 1.2 Piliers de design
🔶 *Proposition de départ, à valider :*
1. **Lisibilité** — on comprend d'un coup d'œil quelle lane est menacée et par quoi.
2. **Mobilité = survie** — bien se positionner compte autant que bien viser.
3. **Montée en tension** — chaque vague est plus dangereuse, le mur ne se répare pas tout seul.

### 1.3 Références
- *Mega Man Battle Network* — déplacement par grille/lanes.
- 🔶 Autres références à lister dans `Production/05_References/`.

---

## 2. Boucle de jeu (Core Loop)

🔶 *À valider :*

```
Menu principal
   └─> Partie
        └─> Vague de monstres annoncée
             └─> Combat : se déplacer entre les lanes / tirer / esquiver
                  └─> Vague nettoyée → récompense / préparation   🔶
                       └─> Vague suivante (plus difficile)
                            └─> Défaite (mur ou joueur détruit) ou Victoire 🔶
```

- 🔶 **Condition de défaite** : mur détruit ? joueur mort ? les deux ?
- 🔶 **Condition de victoire** : survivre à N vagues ? mode infini avec score ?
- 🔶 **Entre les vagues** : pause de préparation, boutique, réparation du mur ?

---

## 3. L'arène

### 3.1 La grille de lanes ✅
Le terrain de jeu est une grille de **lanes parallèles** (implémentée par `LaneGrid`) :

| Paramètre | Valeur par défaut | Note |
|---|---|---|
| Nombre de lanes | **3** | configurable par niveau |
| Espacement entre lanes | 250 uu | |
| Longueur d'une lane | 2000 uu | |
| Colonnes par lane | **8** | subdivisions le long de la lane (prévu pour des sauts de colonne futurs 🔶) |

- Les monstres avancent **le long** des lanes vers le mur.
- Le joueur saute **entre** les lanes (latéral) et se déplace **le long** de sa lane.
- La grille peut être orientée librement dans le niveau ; toutes les règles suivent son orientation.

### 3.2 Le mur 🔶
*Cœur du concept, pas encore implémenté :*
- Le mur occupe l'extrémité des lanes, côté joueur.
- Il a des points de vie ; les monstres qui l'atteignent l'attaquent.
- 🔶 PV par segment de lane ou PV global unique ?
- 🔶 Réparation possible ? (coût, entre les vagues ?)
- 🔶 Améliorations du mur (armure, pièges, tourelles ?)

### 3.3 Zones ✅
- **Zone de spawn des monstres** (`MonsterSpawnZone`) — extrémité opposée au mur.
- **Zone de déplacement des monstres** (`MonsterMovementZone`) — délimite où les monstres peuvent évoluer.

---

## 4. Le joueur

### 4.1 Déplacement (style MMBN) ✅
Implémenté par `LanePlayerCharacter` :
- **Gauche / Droite** : saute d'une lane à l'autre (1 lane par pression, seuil d'input 0.5).
- **Haut / Bas** : avance/recule le long de la lane courante (600 uu/s), le personnage fait face à sa direction.
- Le joueur reste **aimanté au centre** de sa lane (interpolation rapide, réglable).
- Lane de départ : index 1 (lane centrale sur une grille de 3).

### 4.2 Tir ✅
- Le joueur possède une arme (`Weapon` + `WeaponComponent`) qui tire des projectiles (`Bullet`).
- Les projectiles utilisent un **pool d'objets** (`BulletPool`) pour la performance.
- 🔶 Cadence, dégâts, portée de l'arme de base : valeurs à fixer dans `Balancing/`.
- 🔶 Plusieurs armes ? Système d'amélioration ? Munitions limitées ?

### 4.3 Santé du joueur ✅ / 🔶
- Le joueur a des PV via `HealthComponent` (défaut : 100 PV, défense 0).
- 🔶 Le joueur peut-il mourir, ou seul le mur compte ? Respawn ?

---

## 5. Les monstres

### 5.1 Comportement de base ✅
Tous les monstres héritent de `BaseMonster` :

| Caractéristique | Défaut | Description |
|---|---|---|
| PV | 100 | via `HealthComponent` |
| Vitesse | 200 uu/s | |
| Force (dégâts) | 10 | |
| Portée d'attaque | 200 uu | |
| Temps de recharge d'attaque | 1,5 s | |

### 5.2 Patterns de déplacement ✅
- **Ligne droite** ou **sinusoïdal / zig-zag** (amplitude et fréquence réglables).
- Pathing : direct vers la cible, ou **verrouillé sur une lane** depuis le spawn (mode par défaut).

### 5.3 Modes d'attaque ✅
- **Mêlée** : frappe au contact (montage d'animation d'attaque).
- **Distance** : tire des projectiles (vitesse 800 uu/s, portée 1200 uu par défaut).

### 5.4 Feedback de combat ✅
- Flash blanc + secousse du mesh à l'impact, effets Niagara et sons (impact, mort), montages hit-react et mort.

### 5.5 Bestiaire 🔶
*À concevoir — chaque monstre = une fiche dans `Mechanics/` :*

| Monstre | Rôle | Statut |
|---|---|---|
| Monstre de test (import ancien jeu) | valider le pipeline | ✅ en jeu |
| 🔶 Grunt de base | chair à canon | à définir |
| 🔶 Rapide | force le repositionnement | à définir |
| 🔶 Tank | éponge à dégâts, menace le mur | à définir |
| 🔶 Tireur | attaque le joueur à distance | à définir |
| 🔶 Boss | fin de séquence de vagues | à définir |

---

## 6. Combat — règles de dégâts

- Les dégâts passent par `HealthComponent.ApplyDamage` : **dégâts réels = dégâts entrants − défense** (la défense est une stat du composant). ✅
- Soins possibles via `Heal`. ✅
- 🔶 Types de dégâts / faiblesses élémentaires ?
- 🔶 Coups critiques ?

---

## 7. Vagues et progression 🔶

*Système non implémenté — à concevoir :*
- Composition des vagues (quoi, combien, sur quelles lanes, à quel rythme).
- Courbe de difficulté (PV/vitesse croissants ? nouveaux types ?).
- Récompenses de fin de vague (monnaie ? améliorations ?).
- Tableaux d'équilibrage à tenir dans `01_GameDesign/Balancing/`.

---

## 8. Caméra et ressenti ✅ / 🔶

- Caméra de jeu dédiée (`GameCamera`) avec **camera shake** (`GameCameraShake`). ✅
- 🔶 Angle final de la caméra : vue de côté ? trois-quarts arrière (comme MMBN) ? fixe ou suit le joueur ?

---

## 9. Interface (UI) 🔶

*À concevoir — maquettes dans `Production/02_Art/UI/` :*
- Barre de vie du mur (et/ou par segment de lane).
- Barre de vie du joueur.
- Numéro / progression de la vague.
- Menu principal (un `MenuGameMode` existe déjà ✅), pause, écran de défaite/victoire.

---

## 10. Méta et sauvegarde ✅ / 🔶

- Système de sauvegarde en place : `WallDefenseSaveGame` + `AutoSaveComponent` (sauvegarde auto). ✅
- 🔶 Que sauvegarde-t-on exactement ? (progression des niveaux, améliorations, options, meilleurs scores)

---

## 11. Conventions de production

- Nommage des assets : voir `WallDefense/ReadeMe.txt` (BP_, SM_, M_, T_, WBP_, etc.).
- Code C++ par modules : `Core`, `Player`, `Lanes`, `Monsters`, `Weapons`, `Combat`, `GameModes`, `Camera`, `Arena`.
- Toute nouvelle règle de gameplay → d'abord notée ici, puis implémentée.

---

## 12. Questions ouvertes (prochaines décisions)

1. Condition de victoire/défaite exacte (§2).
2. Fonctionnement détaillé du mur : PV, segments, réparation (§3.2).
3. Le joueur peut-il mourir ? (§4.3)
4. Premier bestiaire : 3–4 monstres pour un prototype jouable (§5.5).
5. Système de vagues minimal pour boucler une première partie complète (§7).
6. Angle de caméra définitif (§8).
