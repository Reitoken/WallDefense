# Évolution des monstres — Équilibrage par stage (MVP)

> v0.2 — 9 juin 2026. Document jumeau : `MonsterScaling_en.md`.
> Prérequis : `../Mechanics/Progression_fr.md` (boucle stage → loot → amélioration → retry).

---

## 1. Le modèle : des stages fixes, un joueur qui monte

Avec la boucle « retenter le stage après amélioration », l'équilibrage change de nature :
- **Les stats des monstres sont FIXES par stage** (le stage 4 est toujours le stage 4). C'est le joueur qui bouge.
- À l'intérieur d'un stage, les vagues successives augmentent la **quantité/densité**, pas les stats — la lisibilité reste constante pendant une run.
- Le « mur de difficulté » est naturel : le joueur échoue au stage N, farme (les stages précédents ou ses tentatives), améliore, repasse. **La dent de scie, c'est le rythme défaite → amélioration → victoire.**

La variable ressentie reste le **TTK** (time-to-kill) : `TTK = PV_monstre(stage) / DPS_joueur(améliorations)`.

## 2. Courbes par stage 🔶

```
PV_m(s)  = 100 × 1,32^(s−1)      (grunt, stage s)
ATK_m(s) = 10  × 1,22^(s−1)
Vitesse  = constante par type (la vitesse ne scale PAS — c'est un trait d'identité du monstre)
```

| Stage | PV grunt | ATK grunt | DPS joueur « attendu » | TTK (bonne arme) |
|---|---|---|---|---|
| 1 | 100 | 10 | 40 | 2,5 s |
| 3 | 174 | 15 | 62 | 2,8 s |
| 5 | 304 | 22 | 97 | 3,1 s |
| 7 | 529 | 33 | 152 | 3,5 s |
| 10 | 1 217 | 60 | 305 | 4,0 s |

- **Zone cible TTK** : 2–5 s pour un grunt avec la bonne arme (×1,5–×2 si mauvaise arme — c'est la punition/récompense du switch), 30–60 s pour un boss.
- Le DPS joueur « attendu » vient des améliorations (§3) : si le joueur arrive sous la courbe, le stage le repousse → farm → revient plus fort.

### Multiplicateurs de rôle (sur la base du stage)

| Rôle | PV | ATK | Vitesse | Note |
|---|---|---|---|---|
| Grunt | ×1 | ×1 | ×1 | référence |
| Rapide | ×0,5 | ×0,8 | ×1,8 | menace de percée |
| Tank | ×5 | ×0,8 | ×0,55 | armure 🔶 : résiste hors canon lourd |
| Tireur | ×0,8 | ×1,2 | ×0,9 | attaque à distance |
| Boss (fin de stage) | ×15 | ×2,5 | ×0,7 | matériaux garantis |

### Vagues dans un stage
- Un stage = **5 vagues + 1 vague de boss** 🔶.
- Quantité : `N(vague) = 4 + vague + ⌊stage/2⌋`, plafonnée par la capacité des lanes.
- La composition introduit les rôles progressivement (stage 1 : grunts seuls ; rapide dès le stage 2 ; tank au 3 ; tireur au 4).

## 3. La courbe du joueur (côté améliorations)

Sources de DPS au MVP (cf. Progression §2.2 et §4) :
- Niveau d'arme : **+10 % de dégâts de base par niveau** (~20 niveaux) → ×3 max par arme.
- Paliers d'arme (5/10/15/20) : bonus qualitatifs ≈ **×1,5 cumulé**.
- Dégâts globaux du personnage : +2 %/niveau × 20 → **×1,4**.
- **Bonne arme contre le bon monstre : ×1,5–×2 effectif** (gratuit — c'est le skill).

Total disponible ≈ ×6–9 sur la durée du MVP : cohérent avec la courbe des PV (×12 au stage 10) **à condition** que le joueur utilise le switch — c'est voulu : les stats seules ne suffisent pas tout à fait.

### Coûts d'amélioration (à caler avec le loot)
```
Coût_arme(niveau n) = base × 1,18^n   (matériaux + or)
```
Cible de rythme 🔶 : battre le stage N pour la première fois ≈ **2–3 runs** (1 défaite instructive + farm + victoire). Si les playtests montrent > 4 runs → baisser les coûts ou monter le loot, jamais l'inverse en douce.

## 4. L'évolution ne doit pas être que des stats

- Chaque **nouveau rôle** de monstre apparaît à un stage précis (§2) : la difficulté se renouvelle par la composition, pas seulement par les chiffres.
- Variations visuelles : recoloration des monstres tous les ~3 stages 🔶.

## 5. Pièges connus / contraintes du code actuel

1. ⚠️ **Défense en soustraction flat** (`dégâts réels = entrants − défense`, `HealthComponent`) : avec des ATK ×1,22 par stage, une défense flat du mur devient vite nulle ou bloque tout. Recommandation MVP : **mur sans défense, que des PV** (simple, lisible) ; l'« armure » du tank = réduction % contre les mauvaises armes. 🔶
2. La **vitesse ne scale jamais** : un rapide du stage 10 est aussi rapide qu'au stage 2 — c'est sa quantité et le contexte qui le rendent dangereux.
3. Tout en **DataTable** (`DT_StageScaling` : PV/ATK/quantité par stage ; `DT_MonsterRoles` : multiplicateurs) — l'équilibrage s'itère ici + dans la table, jamais en dur dans le code.

## 6. Prochaines étapes

1. Fixer les stats des 3 armes (→ DPS de référence `P_0 = 40` à valider) 🔶
2. Implémenter : spawner de vagues lisant `DT_StageScaling`, stats appliquées à l'apparition.
3. Loot : table de drop par rôle de monstre (or + matériau typé).
4. Playtest stages 1–3 : vérifier TTK 2–5 s et le rythme « 2–3 runs par stage ».
