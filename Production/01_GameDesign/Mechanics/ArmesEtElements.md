# Les 7 armes élémentaires

> v0.3 — 12 juin 2026. Référencé par le GDD §5 et §10.
> **Une arme par élément, un seul switch ✅ confirmé.** Niveaux **1 → 100** : un changement à chaque niveau, un **gros palier tous les 5 niveaux**.

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
| Ténèbres | **mauve** ✅ | laser, marque, drain |

- Les couleurs sont **la** signalétique du jeu : debug des projectiles, VFX, icônes, ressources, faiblesses affichées.
- **Pas de table élémentaire globale** ✅ : chaque monstre définit SA faiblesse et SES résistances.

## 2. Fiches d'armes ✅ types (v0.5 : laser → Ténèbres, tornade → Vent, frappe céleste → Lumière)

| Élément | Arme | Profil de tir | Effet niveau 1 |
|---|---|---|---|
| Normal | **Fusil automatique** | mono-cible, cadence soutenue, portée longue | aucun — stats brutes supérieures |
| Feu | **Lance-flammes** | cône court continu | **Brûlure** : DoT 3 s |
| Glace | **Canon givrant** | projectile moyen, cadence lente | **Ralentit** ~30 %, 2 s |
| Foudre | **Arc électrique** | tir précis, cadence moyenne | **Chaîne** sur 1 ennemi proche (50 % des dégâts) |
| Vent | **Canon à tornade** | projectile tornade lent, perçant | **Repousse** les ennemis légers sur son passage |
| Lumière | **Frappe céleste** | colonne de lumière sur la zone visée, cadence lente | **×2 dégâts sur les boucliers** |
| Ténèbres | **Rayon laser sombre** | faisceau continu | **Marque** : +15 % dégâts subis, 3 s |

## 3. Évolution 1 → 100 ✅ structure / 🔶 valeurs

**Règle générale (tous niveaux)** : chaque niveau donne **+3 % de dégâts** (cumulatif ≈ ×19 au nv 100) et une **micro-amélioration** de l'effet signature (durée, %, portée). **Tous les 5 niveaux : palier majeur** (tableaux ci-dessous, 20 paliers par arme). Coût : ressources élémentaires + or, courbe 🔶 (`Progression.md`).

Le **palier 100 est un « capstone »** : une capacité périodique spectaculaire, pensée pour les grosses vagues du **mode infini**.

### 3.1 Fusil automatique (Normal) 🔶

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
| 100 | **Pluie de plomb** : toutes les 5 s, une salve à 360° qui transperce tout |

### 3.2 Lance-flammes (Feu) 🔶

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
| 100 | **Mer de feu** : toutes les 30 s, un anneau de feu entoure le joueur pendant 5 s |

### 3.3 Canon givrant (Glace) 🔶

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
| 100 | **Ère glaciaire** : toutes les 30 s, gèle tous les ennemis à l'écran (2 s) |

### 3.4 Arc électrique (Foudre) 🔶

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
| 100 | **Orage** : toutes les 20 s, la foudre frappe chaque ennemi à l'écran |

### 3.5 Canon à tornade (Vent) 🔶

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
| 100 | **Ouragan** : toutes les 30 s, une tornade géante traverse le terrain |

### 3.6 Frappe céleste (Lumière) 🔶

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
| 100 | **Aube** : toutes les 30 s, une vague de lumière traverse le terrain et **brise tous les boucliers** |

### 3.7 Rayon laser sombre (Ténèbres) 🔶

| Niveau | Palier |
|---|---|
| 5 | Marque +20 % |
| 10 | Le faisceau **perce 2** ennemis alignés |
| 15 | Marque 5 s |
| 20 | La mort d'un marqué **propage la marque** (1 voisin) |
| 25 | Perce 3 |
| 30 | Marque +25 % |
| 35 | **Canalisation** : +5 % dégâts/s en maintenant le tir (max +50 %) |
| 40 | Propagation : 2 voisins |
| 45 | Perce 5 |
| 50 | **Drain** : les dégâts sur marqués **soignent le mur** (1 %) |
| 55 | Marque +30 % |
| 60 | Canalisation max +75 % |
| 65 | Perce 7 |
| 70 | Propagation : 3 voisins |
| 75 | Marque 8 s |
| 80 | Drain 2 % |
| 85 | +25 % dégâts |
| 90 | Perce tout l'alignement |
| 95 | Canalisation max +100 % |
| 100 | **Éclipse** : toutes les 30 s, marque TOUS les ennemis à l'écran |

## 4. Switch et déblocage ✅ confirmé

- **Un seul switch** ✅. 🔶 Contrôles : molette + touches 1–7 (clavier), roue radiale (manette).
- Toutes les armes possédées sont accessibles en combat (pas de loadout).
- **Ordre de déblocage confirmé** ✅ : départ Normal, puis par boss de zone : Feu → Glace → Foudre → Vent → Lumière → Ténèbres.

## 5. Approche technique ✅

### 5.1 Niagara et collisions ✅ confirmé
**Aucune détection d'impact dans Niagara** ✅ — ses collisions sont cosmétiques. Les dégâts sont déterministes, dans le gameplay.

### 5.2 Debug-first ✅ (décision v0.5)
**Tout le comportement des projectiles doit être visualisable en debug pur, sans mesh ni VFX** :
- trajectoires (lignes), projectiles (sphères/formes), impacts (points), zones (cercles), chaînes/propagations (segments entre cibles) — **colorés par élément** ;
- activable par console (ex. `wd.Debug.Bullets 1`) ;
- c'est le rendu de référence des **tests gameplay** : le comportement réel de l'arme (perçants, splits, spawns secondaires) se lit directement ;
- la **couche visuelle vient après, via DataAssets** : refs Niagara optionnelles dans `DA_Weapon` pour le projectile **principal** ET chaque **sous-effet** (fragments, pics, orbes…). Pas de ref = rendu debug.

### 5.3 Architecture : le gameplay détecte, Niagara décore

| Type de tir | Détection des dégâts | Visuel (après coup) |
|---|---|---|
| Continu (lance-flammes, laser sombre) | traces/overlaps répétés en C++ | faisceau/cône en boucle + impacts |
| Projectile (fusil, givrant, tornade, arc) | ⚙️ `Bullet` + `BulletPool`, collision par composant | trail + impact à l'événement de hit |
| Zone désignée (frappe céleste) | requête de zone au point visé (délai puis dégâts) | télégraphe au sol + colonne |

Les comportements d'évolution (perçant, fragments, spawns, propagation) vivent dans le **gameplay**, paramétrés par `DA_Weapon`.

### 5.4 Systèmes Niagara maîtres
- `NS_Muzzle`, `NS_Trail`, `NS_Impact` + **User Parameters** (couleur d'élément, intensité, échelle, module additionnel). Variante dédiée seulement si visuellement unique (chaîne d'éclairs, colonne céleste).

### 5.5 Une arme = des données
- `DA_Weapon` : élément, stats de base, **courbe 1–100** (+3 %/nv), **liste des paliers** (enum comportement + paramètres), refs visuelles/sonores **optionnelles** (debug-first), coûts.
- Le `WeaponComponent` ⚙️ lit tout ; l'**encyclopédie** (GDD §2.3) se génère depuis ces mêmes données.

### 5.6 SFX en couches
- **MetaSounds** : corps mécanique + couche élémentaire. 7 armes ≈ 1 base + 7 couches.
