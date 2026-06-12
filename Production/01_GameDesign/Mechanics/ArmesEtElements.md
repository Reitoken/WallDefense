# Les 7 armes élémentaires

> v0.2 — 12 juin 2026. Référencé par le GDD §5 et §10.
> **Une arme par élément, un seul switch.** Chaque arme évolue par niveaux : stats + nouveaux comportements, tout paramétrable par arme (data-driven).

---

## 1. Les 7 éléments ✅

| Élément | Couleur | Identité |
|---|---|---|
| Normal | gris / neutre | la valeur sûre : jamais résisté, stats brutes solides |
| Feu | rouge/orange | dégâts sur la durée (brûlure) |
| Glace | bleu clair | contrôle (ralentissement, gel) |
| Foudre | **jaune** ✅ | chaînes entre ennemis |
| Vent | **turquoise** ✅ | repoussement, zone large |
| Lumière | **blanche** ✅ | anti-bouclier, perçant |
| Ténèbres | **mauve** ✅ | affaiblissement (marque) |

- Les couleurs sont **la** signalétique du jeu : VFX, icônes, ressources de loot, faiblesse affichée sur les monstres — tout parle la même langue.
- **Pas de table élémentaire globale** ✅ : pas de cycle de forces. Chaque monstre définit SA faiblesse (un élément) et SES résistances (un ou plusieurs).

## 2. Fiches d'armes 🔶 proposition à valider

> Types choisis pour que chaque arme ait un *gameplay* distinct (cadence, portée, forme de tir), pas seulement une couleur.

| Élément | Arme | Profil de tir | Effet niveau 1 |
|---|---|---|---|
| Normal | **Fusil automatique** | mono-cible, cadence soutenue, portée longue | aucun — stats brutes supérieures |
| Feu | **Lance-flammes** | cône court continu | **Brûlure** : DoT 3 s |
| Glace | **Canon givrant** | projectile moyen, cadence lente | **Ralentit** ~30 % pendant 2 s |
| Foudre | **Arc électrique** | tir précis, cadence moyenne | **Chaîne** sur 1 ennemi proche (50 % des dégâts) |
| Vent | **Fusil à rafale** | cône large, courte portée | **Repousse** les ennemis légers |
| Lumière | **Rayon laser** | faisceau continu, précis | **×2 dégâts sur les boucliers** |
| Ténèbres | **Lance-orbes** | projectile lent, petite zone | **Marque** : +15 % dégâts subis, 3 s |

## 3. Évolution par niveaux ✅ principe / 🔶 grilles

- Améliorer une arme : **ressources élémentaires de son élément + or**.
- Chaque niveau augmente les **stats** ; les **paliers 5/10/15/20** 🔶 ajoutent un **comportement nouveau**.
- Comportements paramétrables par arme (données, pas de code dédié) : perçant, fragments à la mort, **spawn de projectiles secondaires à l'impact**, zones au sol, propagation d'effets…
- Effet recherché ✅ : les stages du début deviennent progressivement faciles (farm confortable), les stages de fin prennent leur sens.

### Grilles proposées 🔶

| Arme | Nv 5 | Nv 10 | Nv 15 | Nv 20 |
|---|---|---|---|---|
| Fusil (Normal) | transperce **2** ennemis | la cible tuée **éclate en fragments** | transperce 3, fragments ++ | tir double |
| Lance-flammes (Feu) | la brûlure **se propage** à 1 ennemi proche | **flaque enflammée** au sol à la mort | cône élargi | brûlures cumulables ×2 |
| Canon givrant (Glace) | traverse 1 ennemi | **nova de givre** à l'impact (zone ralentie) | 3 touches = **gel** (1 s) | pics de glace secondaires à l'impact |
| Arc (Foudre) | chaîne **2** | chaîne 3, dégâts de chaîne 75 % | la cible tuée **émet une décharge** | chaîne 5 |
| Rafale (Vent) | cône élargi | les projectiles **ricochent** 1 fois | repousse aussi les ennemis moyens | double rafale |
| Laser (Lumière) | perce 2 ennemis alignés | ×3 sur boucliers + **éclat** quand un bouclier casse | faisceau élargi | perce tout l'alignement |
| Lance-orbes (Ténèbres) | zone élargie | l'orbe **spawn 2 orbes mineurs** à l'impact | marque +25 % | la mort d'un marqué **propage la marque** |

## 4. Switch et déblocage ✅

- **Un seul switch** : changer d'arme = changer d'élément. 🔶 Contrôles : molette + touches 1–7 (clavier), gâchettes/roue radiale (manette).
- Pas de loadout : on possède jusqu'à 7 armes, toutes accessibles en combat.
- **Déblocage par les boss** : on commence avec Normal ; chaque boss de zone vaincu débloque l'arme suivante. 🔶 Ordre proposé : Feu → Glace → Foudre → Vent → Lumière → Ténèbres.

## 5. Approche technique : armes, VFX, SFX, impacts ✅

### 5.1 La question Niagara et les collisions
**Verdict : la détection des impacts ne doit PAS passer par Niagara.** Ses modules de collision servent au **visuel** (étincelles, débris) :
- en GPU : depth buffer / distance fields → approximatif, dépend de la caméra, pas d'événement gameplay fiable ;
- en CPU : ray-trace par particule coûteux ; remonter les hits au gameplay (Export Data/Data Channels) = complexe et fragile ;
- les dégâts doivent être déterministes.

### 5.2 Architecture : le gameplay détecte, Niagara décore

| Type de tir | Détection des dégâts | Visuel Niagara |
|---|---|---|
| Continu (lance-flammes, laser) | **traces** répétées / overlap de cône en C++ | faisceau/cône en boucle + impacts au point touché |
| Projectile (les autres) | ⚙️ acteurs `Bullet` + `BulletPool`, collision par composant | trail sur le projectile + impact à l'événement de hit |

Les comportements d'évolution (perçant, fragments, spawns secondaires) vivent dans le **projectile/trace gameplay**, pilotés par les données de l'arme — Niagara ne fait que les rendre visibles.

### 5.3 Pas de duplication : 3 systèmes « maîtres » paramétrés
- `NS_Muzzle`, `NS_Trail`, `NS_Impact`, exposant des **User Parameters** (couleur d'élément, intensité, échelle, mesh, module additionnel — ex. arcs pour Foudre).
- 7 armes = 3 systèmes + paramètres, pas 21 assets. Variante dédiée seulement si un comportement visuel est vraiment unique (chaîne d'éclairs).

### 5.4 Une arme = des données, pas du code
- `DA_Weapon` : élément, stats de base, **courbe de progression par niveau**, **liste des comportements par palier** (enum + paramètres : perçant N, fragments N, spawn secondaire type/quantité…), refs `NS_*`/sons/icône, coûts.
- Le `WeaponComponent` ⚙️ lit le DataAsset → créer/équilibrer une arme = éditer un asset.

### 5.5 SFX en couches
- **MetaSounds** : corps mécanique du tir + couche élémentaire (crépitement, cristal, statique…). 7 armes ≈ 1 base + 7 couches.
