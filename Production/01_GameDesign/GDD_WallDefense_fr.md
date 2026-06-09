# Wall Defense — Game Design Document

> **Statut du document** : v0.2 (scope réduit MVP) — 9 juin 2026
> Les règles marquées ✅ sont déjà implémentées dans le projet. Celles marquées 🔶 sont à décider/valider.
> Ce document est la **référence unique** des règles du jeu : toute mécanique ajoutée au code doit être décrite ici.

---

## 1. Vision

### 1.1 Pitch ✅ *(décidé v0.2)*
Un **petit jeu nerveux** : tower defense à saveur RPG. Des monstres foncent sur un **mur** à travers des **lanes** (déplacement type *Mega Man Battle Network*) ; le joueur doit les tuer **le plus vite possible** en **switchant entre ses armes** pour maximiser les dégâts. Les monstres tués lâchent du **loot** qui sert, au menu, à **améliorer les armes et le personnage** — perdre n'est jamais une fin : on revient plus fort et on bat le stage.

**Scope volontairement réduit** : pas d'empilement de systèmes — une seule boucle, courte et généreuse, détaillée dans `Mechanics/Progression_fr.md`.

### 1.2 Piliers de design
🔶 *Proposition de départ, à valider :*
1. **Lisibilité** — on comprend d'un coup d'œil quelle lane est menacée et par quoi.
2. **Mobilité = survie** — bien se positionner compte autant que bien viser.
3. **Montée en tension** — chaque vague est plus dangereuse, le mur ne se répare pas tout seul.

### 1.3 Références
- *Mega Man Battle Network* — déplacement par grille/lanes.
- 🔶 Autres références à lister dans `Production/05_References/`.

---

## 2. Boucle de jeu (Core Loop) ✅ *(décidé v0.2)*

```
MENU (hub) : améliorer armes (matériaux) / personnage (or) / choisir un stage
   └─> STAGE : vagues de monstres qui foncent sur le mur
        ├─ Combat : switcher d'arme + se placer sur la bonne lane = tuer vite
        ├─ VICTOIRE (toutes les vagues nettoyées) → stage suivant + loot bonus
        └─ DÉFAITE (mur détruit) → retour menu, LOOT CONSERVÉ
             └─> améliorer → retenter le stage
```

- **Défaite** : le mur est détruit. **Victoire** : toutes les vagues du stage nettoyées.
- **Perdre fait progresser** : le loot des monstres tués est toujours conservé.
- Détail complet de la boucle et du scope : `Mechanics/Progression_fr.md`.

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

### 4.2 Armes et switch ✅ code de base / 🔶 design décidé v0.2
- Le joueur porte **3 armes** et **switche à la volée** — c'est le cœur du skill : la bonne arme contre le bon monstre (mitrailleuse/mono-cible, canon lourd/perce-armure, onde/zone).
- Raison de switcher (à valider en prototype 🔶) : surchauffe/rotation + rôles distincts. Détail : `Mechanics/Progression_fr.md` §2.
- Code existant : `Weapon` + `WeaponComponent`, projectiles `Bullet` avec **pool d'objets** (`BulletPool`). Le multi-armes et le switch restent à implémenter.
- Les armes s'améliorent au menu avec le **loot** (niveaux +N, paliers qualitatifs).

### 4.3 Santé du joueur ✅ *(décidé v0.2)*
- **Le joueur ne meurt pas (MVP)** : seul le mur compte. Au pire, knockback/ralentissement 🔶.
- Le `HealthComponent` existant servira au mur et aux monstres.

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

## 7. Progression, loot et stages ✅ *(décidé v0.2)* / 🔶 valeurs

Scope réduit assumé : **une seule boucle de progression** (armes + personnage léger), pas d'empilement de systèmes. L'esprit « il y a toujours quelque chose à améliorer » est conservé, mais concentré.

- **Loot** : chaque monstre tué lâche de l'or + des matériaux typés par monstre (farmer a du sens). Conservé même en défaite.
- **Améliorations** : armes (+10 %/niveau, paliers qualitatifs aux nv 5/10/15/20) et 4 stats de personnage. Détail : `Mechanics/Progression_fr.md`.
- **Stages fixes** : les stats des monstres sont figées par stage ; c'est le joueur qui monte. Courbes, TTK et coûts : `Balancing/MonsterScaling_fr.md`.
- Cibles MVP : 3 armes, 5 monstres, 10 stages, 1 arène.
- 🔶 Valeurs exactes à caler en playtest (rythme cible : un stage battu en 2–3 runs).

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

*Résolues en v0.2 : conditions de victoire/défaite (mur), mortalité du joueur (non), scope de progression (armes + perso léger).*

1. Fonctionnement détaillé du mur : PV global ou par segment de lane ? réparation ? (§3.2)
2. Mécanisme de switch : surchauffe seule, rôles seuls, ou les deux ? → à trancher au prototype (§4.2).
3. Stats exactes des 3 armes (→ DPS de référence, `Balancing/MonsterScaling_fr.md` §6).
4. Fiches des 5 monstres MVP : grunt, rapide, tank, tireur, boss (§5.5).
5. Angle de caméra définitif (§8).
6. « Armure » du tank : réduction % contre les mauvaises armes — valider la valeur (`Balancing/MonsterScaling_fr.md` §5).
