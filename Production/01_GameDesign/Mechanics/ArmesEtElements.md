# Les 7 armes élémentaires

> v0.4 — 12 juin 2026. Référencé par le GDD §5 et §10.
> **Une arme par élément, un seul switch ✅.** Niveaux **1 → 100** : un changement à chaque niveau, un **gros palier tous les 5 niveaux**. Chaque arme a une **Spéciale active** (✅ v0.6) avec cooldown et **vidéo de déclenchement**.

---

## 1. Les 7 éléments ✅

| Élément | Couleur | Identité |
|---|---|---|
| Normal | gris / neutre | la valeur sûre : jamais résisté, stats brutes solides |
| Feu | rouge/orange | dégâts sur la durée (brûlure) |
| Glace | bleu clair | contrôle (ralentissement, gel) |
| Foudre | **jaune** ✅ | chaînes entre ennemis |
| Vent | **turquoise** ✅ | tornades, repoussement, regroupement |
| Lumière | **blanche** ✅ | frappes de zone célestes, anti-bouclier |
| Ténèbres | **mauve** ✅ | laser perforant à rebonds, multi-hit |

- Les couleurs sont **la** signalétique du jeu : debug des projectiles, VFX, icônes, ressources, faiblesses affichées (et yeux lumineux des monstres, `Bestiaire.md` §0).
- **Pas de table élémentaire globale** ✅ : chaque monstre définit SA faiblesse et SES résistances.

## 2. Fiches d'armes ✅ types

| Élément | Arme | Profil de tir | Effet niveau 1 |
|---|---|---|---|
| Normal | **Fusil automatique** | mono-cible, cadence soutenue, portée longue | aucun — stats brutes supérieures |
| Feu | **Lance-flammes** | cône court continu | **Brûlure** : DoT 3 s |
| Glace | **Canon givrant** | projectile moyen, cadence lente | **Ralentit** ~30 %, 2 s |
| Foudre | **Arc électrique** | tir précis, cadence moyenne | **Chaîne** sur 1 ennemi proche (50 % des dégâts) |
| Vent | **Canon à tornade** | projectile tornade lent, perçant | **Repousse** les ennemis légers sur son passage |
| Lumière | **Frappe céleste** | colonne de lumière sur la zone visée, cadence lente | **×2 dégâts sur les boucliers** |
| Ténèbres | **Laser sombre** ✅ refonte v0.6 | rayon rapide qui **transperce toute la ligne** et **rebondit sur les bords** | dégâts faibles par cible mais multi-hit ; **cooldown de tir élevé** → switcher pendant le cooldown |

### Le Laser sombre en détail ✅ (refonte v0.6)
- Le rayon **transperce tous les ennemis sur sa ligne** (dégâts plus faibles par cible).
- Il **rebondit sur les bords du terrain** à vitesse élevée → frappe plusieurs fois les groupes.
- **Cadence lente** (cooldown de tir nettement plus élevé que les autres armes) : on tire, on **switche** sur une autre arme pendant le cooldown, on revient — l'arme « ponctuation » du kit.
- Améliorations : nombre de rebonds, **épaisseur** et force du rayon, **mini-lasers autoguidés** qui se dirigent seuls vers les monstres.

## 3. La Spéciale ✅ (v0.6)

- **Chaque arme possède une Spéciale active** : déclenchée par le joueur, **cooldown long** (🔶 ~60–90 s).
- **Son effet = celui du palier 100**, mais **plus faible à bas niveau** : la Spéciale **scale avec le niveau de l'arme** et atteint sa pleine puissance au nv 100 (qui réduit aussi son cooldown).
- **Vidéo de Spéciale** ✅ : au déclenchement, une **courte cinématique** est jouée (à la *Epic Seven*), skippable 🔶 — la **référence vidéo vit dans le DataAsset de l'arme** (`DA_Weapon`).

| Arme | Spéciale (pleine puissance au nv 100) | À bas niveau |
|---|---|---|
| Fusil | **Pluie de plomb** : salve à 360° qui transperce tout | salve partielle (90°), perçant limité |
| Lance-flammes | **Mer de feu** : anneau de feu autour du joueur (5 s) | demi-anneau, 2 s |
| Canon givrant | **Ère glaciaire** : gèle tous les ennemis à l'écran (2 s) | gèle les ennemis proches (1 s) |
| Arc électrique | **Orage** : la foudre frappe chaque ennemi à l'écran | frappe 1 ennemi (puis 2, 4, 8… avec le niveau) |
| Canon à tornade | **Ouragan** : tornade géante qui traverse le terrain | tornade moyenne, demi-terrain |
| Frappe céleste | **Aube** : vague de lumière qui traverse tout et **brise tous les boucliers** | vague courte, endommage les boucliers |
| Laser sombre | **Éclipse** : laser colossal, 10 rebonds + nuée de mini-lasers | laser épais, 3 rebonds, sans nuée |

## 4. Évolution 1 → 100 ✅ structure / 🔶 valeurs

**Règle générale** : chaque niveau donne **+3 % de dégâts** et une micro-amélioration de l'effet signature ; **tous les 5 niveaux : palier majeur**. La **Spéciale** scale en continu avec le niveau. Coûts : ressources élémentaires **par tiers de mode** (Normal/Hard/Enfer, voir `Progression.md`) + or.

### 4.1 Fusil automatique (Normal) 🔶

| Niveau | Palier |
|---|---|
| 5 | Transperce 2 ennemis |
| 10 | La cible tuée éclate en 2 **fragments** (25 % des dégâts) |
| 15 | Transperce 3 |
| 20 | **Tir double** |
| 25 | Fragments ×3, les fragments transpercent 1 ennemi |
| 30 | +25 % cadence |
| 35 | Transperce 4 |
| 40 | **Tir triple** |
| 45 | Les fragments explosent (mini-zone) |
| 50 | **Surcadence** : chaque kill +10 % cadence (3 s, cumul ×5) |
| 55 | Transperce 5 |
| 60 | Fragments ×4 |
| 65 | +10 % dégâts par ennemi transpercé |
| 70 | **Tir quadruple** |
| 75 | Fragments à chaque transpercement (plus seulement à la mort) |
| 80 | +25 % cadence |
| 85 | Transperce 7 |
| 90 | Fragments ×6 |
| 95 | Surcadence cumul ×10 |
| 100 | **Spéciale max — Pluie de plomb** (360°, transperce tout) + cooldown de Spéciale réduit |

### 4.2 Lance-flammes (Feu) 🔶

| Niveau | Palier |
|---|---|
| 5 | La brûlure **se propage** à 1 ennemi proche |
| 10 | **Flaque enflammée** au sol à la mort (3 s) |
| 15 | Cône +25 % |
| 20 | Brûlures **cumulables ×2** |
| 25 | Propagation : 2 ennemis |
| 30 | Flaques +50 % de durée |
| 35 | Portée du cône +30 % |
| 40 | Cumuls ×3 |
| 45 | Les flaques propagent la brûlure |
| 50 | **Embrasement** : un ennemi à 3 cumuls explose à sa mort (zone de feu) |
| 55 | Cône +25 % |
| 60 | Dégâts de brûlure +50 % |
| 65 | Propagation : 3 ennemis |
| 70 | Cumuls ×4 |
| 75 | Les explosions d'Embrasement laissent une flaque |
| 80 | Portée +30 % |
| 85 | Dégâts de brûlure +50 % |
| 90 | Cumuls ×5 |
| 95 | Flaques durée ×2 |
| 100 | **Spéciale max — Mer de feu** (anneau complet, 5 s) + cooldown réduit |

### 4.3 Canon givrant (Glace) 🔶

| Niveau | Palier |
|---|---|
| 5 | Traverse 1 ennemi |
| 10 | **Nova de givre** à l'impact (zone ralentie) |
| 15 | Ralentissement 45 % |
| 20 | 3 touches = **gel** (1 s) |
| 25 | 2 **pics de glace** secondaires à l'impact |
| 30 | Nova élargie |
| 35 | Traverse 2 |
| 40 | Gel 1,5 s |
| 45 | Les ennemis gelés prennent +25 % de dégâts (toutes sources) |
| 50 | **Hiver** : tuer un ennemi gelé déclenche une nova de gel |
| 55 | Pics ×3 |
| 60 | Ralentissement 60 % |
| 65 | Traverse 3 |
| 70 | Gel 2 s |
| 75 | Les pics ralentissent aussi |
| 80 | Nova élargie ×2 |
| 85 | +25 % dégâts sur cibles ralenties |
| 90 | 2 touches = gel |
| 95 | Pics ×5 |
| 100 | **Spéciale max — Ère glaciaire** (gel global, 2 s) + cooldown réduit |

### 4.4 Arc électrique (Foudre) 🔶

| Niveau | Palier |
|---|---|
| 5 | Chaîne 2 |
| 10 | Chaîne 3, dégâts de chaîne 75 % |
| 15 | Portée de chaîne +30 % |
| 20 | La cible tuée émet une **décharge** |
| 25 | Chaîne 4 |
| 30 | Dégâts de chaîne 90 % |
| 35 | Les décharges chaînent 1 fois |
| 40 | Chaîne 5 |
| 45 | +25 % cadence |
| 50 | **Surtension** : chaque rebond AUGMENTE les dégâts de 10 % (au lieu de les réduire) |
| 55 | Chaîne 6 |
| 60 | Portée +30 % |
| 65 | Les décharges chaînent 2 fois |
| 70 | Chaîne 7 |
| 75 | **Paralysie** brève (0,3 s) à chaque rebond |
| 80 | +25 % cadence |
| 85 | Chaîne 9 |
| 90 | Surtension +15 %/rebond |
| 95 | Paralysie 0,5 s |
| 100 | **Spéciale max — Orage** (frappe chaque ennemi à l'écran) + cooldown réduit |

### 4.5 Canon à tornade (Vent) 🔶

| Niveau | Palier |
|---|---|
| 5 | Tornade +25 % de largeur |
| 10 | La tornade **persiste 1 s** en fin de course |
| 15 | Repousse aussi les ennemis moyens |
| 20 | **2 tornades** en éventail |
| 25 | La tornade **aspire** légèrement les ennemis (les regroupe) |
| 30 | Vitesse des tornades +30 % |
| 35 | Persiste 2 s |
| 40 | Les ennemis aspirés prennent +15 % de dégâts (toutes sources) |
| 45 | **3 tornades** |
| 50 | **Cyclone** : les tornades fusionnent au contact (taille et dégâts cumulés) |
| 55 | Largeur +25 % |
| 60 | Persiste 3 s |
| 65 | Repousse les lourds (sauf boss) |
| 70 | Aspiration +50 % |
| 75 | **4 tornades** |
| 80 | Dégâts d'aspiration +30 % |
| 85 | Persiste 4 s |
| 90 | Les tornades **dévient les projectiles ennemis** |
| 95 | **5 tornades** |
| 100 | **Spéciale max — Ouragan** (tornade géante sur tout le terrain) + cooldown réduit |

### 4.6 Frappe céleste (Lumière) 🔶

| Niveau | Palier |
|---|---|
| 5 | Zone +25 % |
| 10 | ×3 sur les boucliers |
| 15 | Délai de frappe réduit |
| 20 | **Éclat** de zone quand un bouclier casse |
| 25 | **2 colonnes** (la 2e sur l'ennemi le plus proche) |
| 30 | Zone +25 % |
| 35 | +25 % cadence |
| 40 | ×4 sur les boucliers |
| 45 | **Aveugle** 0,5 s (les touchés cessent d'attaquer) |
| 50 | **Jugement** : les ennemis SANS bouclier prennent +25 % |
| 55 | **3 colonnes** |
| 60 | Zone +25 % |
| 65 | Délai quasi instantané |
| 70 | Aveuglement 1 s |
| 75 | **4 colonnes** |
| 80 | ×5 sur les boucliers |
| 85 | +25 % cadence |
| 90 | Jugement +40 % |
| 95 | **5 colonnes** |
| 100 | **Spéciale max — Aube** (vague totale, brise tous les boucliers) + cooldown réduit |

### 4.7 Laser sombre (Ténèbres) 🔶 — refonte v0.6

| Niveau | Palier |
|---|---|
| 5 | **+1 rebond** (2 au total) |
| 10 | Épaisseur du rayon +30 % |
| 15 | **Montée en puissance** : +20 % de dégâts à chaque rebond effectué |
| 20 | **2 mini-lasers autoguidés** tirés avec le rayon (30 % des dégâts) |
| 25 | +1 rebond (3) |
| 30 | Cooldown de tir −15 % |
| 35 | Épaisseur +30 % |
| 40 | Mini-lasers ×3 |
| 45 | +1 rebond (4) |
| 50 | **Résonance** : chaque ennemi traversé augmente les dégâts du prochain rayon de 5 % |
| 55 | Mini-lasers ×4 |
| 60 | Cooldown −15 % |
| 65 | +1 rebond (5) |
| 70 | Épaisseur +40 % (rayon massif) |
| 75 | Les mini-lasers transpercent 2 ennemis |
| 80 | +1 rebond (6) |
| 85 | Dégâts +25 % |
| 90 | Mini-lasers ×6 |
| 95 | Cooldown −20 % |
| 100 | **Spéciale max — Éclipse** (laser colossal, 10 rebonds + nuée de mini-lasers) + cooldown réduit |

## 5. Switch et déblocage ✅ confirmé

- **Un seul switch** ✅. Contrôles ✅ (GDD §4) : souris = visée + bouton tir + bouton arme suivante ; clavier = touches proches de ZQSD/WASD pour changer d'arme ; **manette = cible privilégiée** (twin-stick, roue/gâchettes).
- Toutes les armes possédées sont accessibles en combat (pas de loadout).
- **Ordre de déblocage confirmé** ✅ : départ Normal, puis par boss de zone : Feu → Glace → Foudre → Vent → Lumière → Ténèbres.

## 6. Approche technique ✅

### 6.1 Niagara et collisions ✅ confirmé
**Aucune détection d'impact dans Niagara** ✅ — ses collisions sont cosmétiques. Les dégâts sont déterministes, dans le gameplay.

### 6.2 Debug-first ✅
**Tout le comportement des projectiles est visualisable en debug pur, sans mesh ni VFX** :
- trajectoires (lignes), projectiles (sphères/formes), impacts (points), zones (cercles), chaînes/rebonds (segments) — **colorés par élément** ;
- activable par console (ex. `wd.Debug.Bullets 1`) ;
- c'est le rendu de référence des **tests gameplay** ;
- la **couche visuelle vient après, via DataAssets** ✅ : pour le projectile **principal** ET pour **chaque sous-effet** (fragments, pics, orbes, mini-lasers…), le DataAsset prévoit une **ref Niagara optionnelle ET une ref sonore optionnelle** ✅ v0.6. Pas de ref = rendu debug.

### 6.3 Architecture : le gameplay détecte, Niagara décore

| Type de tir | Détection des dégâts | Visuel (après coup) |
|---|---|---|
| Continu (lance-flammes) | traces/overlaps répétés en C++ | cône en boucle + impacts |
| Projectile (fusil, givrant, tornade, arc, mini-lasers) | ⚙️ `Bullet` + `BulletPool`, collision par composant | trail + impact à l'événement de hit |
| Rayon à rebonds (laser sombre) | line traces successives (segments réfléchis sur les bords) | beam segmenté suivant les traces |
| Zone désignée (frappe céleste) | requête de zone au point visé (délai puis dégâts) | télégraphe au sol + colonne |

### 6.4 Systèmes Niagara maîtres
- `NS_Muzzle`, `NS_Trail`, `NS_Impact` + **User Parameters** (couleur d'élément, intensité, échelle, module additionnel). Variante dédiée seulement si visuellement unique (chaîne d'éclairs, colonne céleste, beam à rebonds).

### 6.5 Une arme = des données
- `DA_Weapon` : élément, stats de base, **courbe 1–100**, **liste des paliers**, **Spéciale** (effet, scaling, cooldown, **vidéo** ✅), refs visuelles/sonores **optionnelles par effet et sous-effet**, tenue associée, coûts par tier.
- Le `WeaponComponent` ⚙️ lit tout ; l'**encyclopédie** (GDD §2.3) se génère depuis ces mêmes données (avec dévoilement progressif ✅).

### 6.6 SFX en couches
- **MetaSounds** : corps mécanique + couche élémentaire ; chaque sous-effet peut avoir son propre son ✅.
