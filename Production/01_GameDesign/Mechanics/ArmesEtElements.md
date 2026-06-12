# Armes et éléments

> v0.1 — 12 juin 2026. Référencé par le GDD §5 et §10.

---

## 1. Les 7 éléments ✅

| Élément | Couleur | Effet de statut proposé 🔶 |
|---|---|---|
| Normal | gris / neutre | aucun — mais stats brutes supérieures (la valeur sûre) |
| Feu | rouge/orange | **Brûlure** : dégâts sur la durée |
| Glace | bleu clair | **Gel/Ralentissement** : réduit la vitesse de marche |
| Foudre | **jaune** ✅ | **Chaîne** : l'éclair rebondit sur les monstres proches |
| Vent | **turquoise** ✅ | **Projection** : repousse/dévie les monstres légers |
| Lumière | **blanche** ✅ | 🔶 (proposition : bonus contre boucliers, révèle les invisibles ?) |
| Ténèbres | **mauve** ✅ | 🔶 (proposition : affaiblissement — la cible prend plus de dégâts) |

- Les couleurs sont **la** signalétique du jeu : VFX, icônes, barres de vie des monstres (faiblesse affichée), ressources de loot — tout parle la même langue de couleurs.
- 🔶 Table des forces/faiblesses à construire (cycle Feu>Glace>… ? Lumière⇄Ténèbres mutuellement faibles ?) — voir GDD §13.2.

## 2. Catégories d'armes 🔶

Proposition de départ (à trancher) :

| Catégorie | Profil | Rôle |
|---|---|---|
| Fusil | mono-cible, cadence moyenne, précis | le couteau suisse |
| Shotgun | courte portée, dégâts étalés | proximité du mur / groupes |
| 🔶 3e catégorie ? | sniper (burst lent perçant) ou lance-grenades (zone) | anti-tank ou anti-essaim |

**Une « arme » = catégorie × élément** (ex. shotgun Feu). La matrice complète fait N×7 — le déblocage progressif est obligatoire (GDD §13.7).

## 3. Le double switch ✅ concept / 🔶 contrôles

Deux axes de switch en combat, sur le loadout emporté :

```
                 Fusil Feu  ←─ switch basique ─→  Shotgun Feu
                     ↕ switch élémentaire              ↕
                 Fusil Glace ←─ switch basique ─→ Shotgun Glace
```

- **Switch basique** : changer de **catégorie** en gardant l'élément.
- **Switch élémentaire** : changer d'**élément** en gardant la catégorie.
- 🔶 Contrôles à prototyper (ex. molette = catégorie ; touches/croix directionnelle = élément ; roue radiale en maintien).
- 🔶 Taille du loadout : combien de catégories × combien d'éléments emportés par stage ? (Trop = pas de choix ; trop peu = frustration. Proposition de départ : 2 catégories × 3 éléments.)
- Le **choix du loadout AVANT le stage** est une décision stratégique : il faut connaître (ou découvrir) les monstres du stage — synergie avec le pilier « connaître son ennemi ».

## 4. Améliorations d'armes 🔶

- Chaque arme a des **niveaux** (coût : ressources élémentaires de son élément + or).
- Monter une arme donne des **avantages** : dégâts, et paliers qualitatifs propres à la catégorie (cadence, largeur de cône, perçant…) et à l'élément (durée de brûlure, nombre de rebonds de chaîne…).
- 🔶 Grille exacte des paliers par arme à concevoir avec l'économie (`Progression.md`).

## 5. Approche technique : armes, VFX, SFX, impacts ✅ recommandation

### 5.1 La question Niagara et les collisions
**Verdict : la détection des impacts ne doit PAS passer par Niagara.** Les modules de collision Niagara servent au **visuel** (étincelles qui rebondissent, débris) :
- en GPU, la collision utilise le depth buffer ou les distance fields → approximative, dépend de ce que la caméra voit, aucun événement gameplay fiable ;
- en CPU, le ray-trace par particule est coûteux et remonter les hits vers le gameplay (Export Data/Data Channels) est complexe et fragile ;
- les dégâts, eux, doivent être déterministes.

### 5.2 Architecture recommandée
**Le gameplay détecte, Niagara décore.**

| Type d'arme | Détection des dégâts | Visuel Niagara |
|---|---|---|
| Instantanée (fusil) | **Line trace** C++ au tir → dégâts immédiats | muzzle flash + tracer (beam/ribbon) + impact spawné au point touché |
| Projectile (shotgun, grenades) | ⚙️ acteurs `Bullet` + `BulletPool` existants, collision par composant | mesh/trail sur le projectile + impact à l'événement de hit |

### 5.3 Pas de duplication d'effets : des systèmes « maîtres » paramétrés
Ne PAS créer 1 effet par arme (3 catégories × 7 éléments × 3 effets = 63 assets ingérables). À la place :
- **3 systèmes Niagara maîtres** : `NS_Muzzle`, `NS_Trail`, `NS_Impact` ;
- chacun exposé avec des **User Parameters** : couleur de l'élément, intensité, échelle, mesh optionnel, module additionnel (ex. arcs électriques pour Foudre) ;
- le gameplay pousse les paramètres au spawn (`SetVariableLinearColor`…).
- Si un élément a besoin d'un comportement vraiment unique (chaîne de foudre), on fait une **variante** de ce seul système, pas une duplication de tout.

### 5.4 Une arme = des données, pas du code
- `DA_Weapon` (DataAsset) : catégorie, élément, dégâts, cadence, portée, refs `NS_*`, refs sons, icône, coûts d'amélioration.
- Le `WeaponComponent` ⚙️ lit le DataAsset → créer/équilibrer une arme = créer un asset, zéro code.
- L'élément est une **donnée** (`E_Element`) consommée par : calcul de dégâts (faiblesses), VFX (couleur), SFX (couche), UI (icône).

### 5.5 SFX en couches
- **MetaSounds** : son mécanique par **catégorie** (le « corps » du tir) + **couche élémentaire** (crépitement feu, cristal glace, statique foudre…).
- 3 catégories + 7 couches = 10 assets sonores de base au lieu de 21.
