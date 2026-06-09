# Évolution des monstres — Équilibrage face à la progression du joueur

> Brouillon v0.1 — 9 juin 2026. Document jumeau : `MonsterScaling_en.md`.
> Prérequis : `../Mechanics/Progression_fr.md` (les sources de puissance du joueur).

---

## 1. Le problème à résoudre

Le joueur empile des multiplicateurs de puissance en permanence. Si les monstres n'évoluent pas au bon rythme :
- **trop lents** → le jeu devient trivial, la progression ne se *sent* plus (tout meurt instantanément quoi qu'on fasse) ;
- **trop rapides** → mur de frustration, la progression semble inutile.

L'objectif n'est PAS que les monstres suivent exactement le joueur : c'est de créer une **dent de scie** maîtrisée.

## 2. Le modèle en dent de scie

On définit :
- `P(w)` : puissance offensive attendue du joueur « à jour » à la vague `w` (DPS effectif).
- `HP_m(w)` : points de vie d'un monstre de base à la vague `w`.
- **TTK** (time-to-kill) = `HP_m(w) / P(w)` : temps pour tuer un monstre. C'est LA variable ressentie.

**Règle d'or : les monstres croissent légèrement plus vite que la progression « passive » du joueur.**

```
HP_m(w)  = HP_0  × g_hp^w          (croissance géométrique par vague)
ATK_m(w) = ATK_0 × g_atk^w         avec g_atk < g_hp
P(w)     = P_0   × g_p^w  × M(w)   M(w) = multiplicateurs « par paliers » (étoiles, rareté, éveil)
```

- Le ratio `r = g_hp / g_p > 1` fait monter lentement le TTK → pression croissante.
- Quand le TTK sort de la zone de confort, le joueur investit dans un **palier** (étoile, nouvelle rareté, éveil) → `M(w)` saute (×1,5 à ×2) → le TTK retombe → **sensation de puissance**. C'est la dent de scie.
- `g_atk < g_hp` : la difficulté doit se manifester par l'**attrition** (monstres plus longs à tuer, le mur encaisse), pas par le one-shot du joueur.

### Cadence des murs de difficulté

Le TTK double tous les `N = ln(2) / ln(r)` vagues si le joueur n'investit pas.

| `r` choisi | TTK double toutes les… | Ressenti |
|---|---|---|
| 1,02 | ~35 vagues | très doux, jeu chill |
| **1,035** | **~20 vagues** | **recommandé pour le prototype** |
| 1,05 | ~14 vagues | exigeant, jeu nerveux |

## 3. Valeurs proposées pour le prototype 🔶

Calées sur les défauts du code actuel (`BaseMonster` : 100 PV, force 10 ; `HealthComponent` : 100 PV joueur) :

| Paramètre | Valeur | Note |
|---|---|---|
| `HP_0` (grunt, vague 1) | 100 | défaut actuel du code |
| `ATK_0` | 10 | défaut actuel du code |
| `P_0` (DPS joueur, vague 1) | 40 | 🔶 ex. 10 dégâts × 4 tirs/s — à fixer avec l'arme |
| `g_hp` | 1,10 / vague | |
| `g_atk` | 1,07 / vague | attrition > burst |
| `g_p` (joueur à jour) | 1,065 / vague | → `r ≈ 1,033`, mur ~toutes les 21 vagues |
| Vitesse monstre | +0,5 % / vague, **plafonnée à +50 %** | la vitesse ne doit jamais rendre la lane injouable |
| Nombre par vague | `6 + ⌊w/2⌋`, plafonné selon les lanes | la densité participe à la difficulté |

### Multiplicateurs de rôle (sur la base de la vague)

| Rôle | PV | ATK | Apparition |
|---|---|---|---|
| Grunt | ×1 | ×1 | toutes les vagues |
| Rapide | ×0,6 | ×0,8 (vitesse ×1,8) | dès la vague 3 |
| Tank | ×4 | ×0,7 (vitesse ×0,6) | toutes les 5 vagues |
| Élite | ×4 | ×2 | toutes les 5 vagues |
| Boss | ×15 | ×3 | toutes les 10 vagues |

### Table de contrôle (grunt, joueur « à jour », sans palier investi)

| Vague | PV monstre | DPS joueur | TTK |
|---|---|---|---|
| 1 | 110 | 43 | 2,6 s |
| 5 | 161 | 55 | 2,9 s |
| 10 | 259 | 75 | 3,5 s |
| 15 | 418 | 103 | 4,1 s |
| 20 | 673 | 141 | 4,8 s |
| 30 | 1 745 | 265 | 6,6 s |
| 40 | 4 526 | 497 | 9,1 s |
| 50 | 11 739 | 932 | 12,6 s |

**Lecture** : sans investissement de palier, le TTK dérive de 2,6 s vers 12,6 s. Chaque palier acheté (×1,5–×2) le ramène dans la **zone cible : 2 à 6 s pour un grunt** (boss : 30–60 s). Si le joueur sur-investit, il « roule » sur 10–15 vagues — c'est voulu, c'est la récompense.

## 4. L'évolution ne doit pas être que des stats 🔶

Les chiffres seuls deviennent invisibles. Paliers **qualitatifs** :
- **Toutes les ~10 vagues** : nouveau comportement (pattern sinusoïdal plus agressif, tireurs, sauts de lane 🔶…).
- **Variantes visuelles** : recolorations entre les paliers, nouveau monstre aux paliers majeurs.
- Le bestiaire (GDD §5.5) doit être pensé comme une **grille rôle × palier**.

## 5. Pièges connus / contraintes du code actuel

1. ⚠️ **Défense en soustraction flat** (`dégâts réels = entrants − défense`, `HealthComponent`) : avec des ATK qui croissent géométriquement, une défense flat devient vite nulle — ou invincible en sens inverse. Si on garde la formule, la défense du joueur/mur doit croître sur la **même courbe géométrique** que `g_atk` ; sinon passer à une réduction en % (`dégâts × 100/(100+DEF)`). 🔶 décision à prendre avant d'implémenter l'équipement défensif.
2. La **vitesse** ne doit jamais scaler sans plafond (lane intraversable = frustration, pas difficulté).
3. Tout doit être **piloté par données** : DataTable `DT_MonsterScaling` (courbes PV/ATK/vitesse/quantité par vague) + multiplicateurs de rôle, jamais de constantes en dur — l'équilibrage se fera par itérations dans ce fichier et la DataTable.

## 6. Prochaines étapes

1. Fixer les stats de l'arme de base (→ `P_0` réel) 🔶
2. Implémenter le scaling à l'apparition : `Stats = Base × Courbe(vague) × MultiplicateurRôle` (DataTable + spawner de vagues).
3. Playtest : vérifier la zone TTK 2–6 s sur les 10 premières vagues, ajuster `g_hp`.
