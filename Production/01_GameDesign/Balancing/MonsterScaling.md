# Évolution des monstres — 6 zones × 5 stages + stage infini

> v0.4 — 12 juin 2026. Référencé par le GDD §6 et §8. Prérequis : `../Mechanics/Progression.md`.

---

## 1. Le modèle ✅

- **Stages 1–30 : stats FIXES par stage** (progression linéaire : on débloque le stage suivant en terminant le précédent). C'est le joueur qui monte (loot → améliorations armes/perso/mur).
- **Structure ✅ : 6 zones de 5 stages** (4 normaux + 1 boss). Le boss de zone débloque l'arme élémentaire suivante.
- **Stage 31 (infini)** : croissance continue par vague, sans plafond ; record personnel (pas de online).
- La variable ressentie : le **TTK** (temps pour tuer), modulé par la **faiblesse élémentaire** du monstre.

## 2. Dégâts : la formule élémentaire 🔶 à valider

```
Dégâts = DégâtsArme × MultiplicateurÉlémentaire × (100 / (100 + Défense))
```

| Situation | Multiplicateur proposé |
|---|---|
| **Faiblesse** du monstre (1 élément) | ×1,75 |
| Neutre | ×1,0 |
| **Résistance** du monstre (1 ou plusieurs éléments) | ×0,5 |
| Élément Normal | ×1,0 partout (jamais résisté) — la valeur sûre |

- **Pas de table élémentaire globale** ✅ : chaque monstre définit SA faiblesse et SES résistances dans sa fiche.
- ⚠️ La défense passe en **réduction %** (`100/(100+DEF)`) — la soustraction flat du `HealthComponent` actuel ne survit pas à 30 stages de croissance. Le mur utilise la même formule (sa défense est améliorable ✅).
- **Bouclier** : couche de PV au-dessus des PV, percée d'abord ; **Lumière ×2 contre les boucliers** (arme dédiée).
- Bien joué (faiblesse) vs mal joué (résistance) = écart **×3,5** : le switch n'est pas optionnel.

## 3. Courbes des stages 1–30 🔶 valeurs de départ

```
PV_m(s)    = 100 × 1,17^(s−1)     → ×1 (st.1) … ×96 (st.30)
ATK_mur(s) = 10  × 1,12^(s−1)     → ×1 … ×27
Vitesse    : ne scale JAMAIS (identité du monstre)
Stage boss (5e de zone) : densité accrue + boss (×15 PV)
```

- La progression joueur (niveaux d'armes + paliers de comportements + nouvelles armes + mur) doit suivre ≈ ×60–100 de DPS sur les 30 stages — à caler quand les grilles d'armes seront chiffrées.
- **Zone cible TTK** (monstre standard, bonne arme) : 2–5 s ; boss : 30–60 s.
- Rythme cible : un stage battu en **2–3 tentatives** quand on est à jour.
- Effet assumé ✅ : en montant, les premiers stages deviennent faciles (re-clear rapide pour farmer les étoiles ×2), les derniers prennent leur sens.

## 4. Stage 31 : la courbe infinie ✅ modèle / 🔶 valeurs

```
PV_m(v)   = PV_base(≈ stage 26) × 1,05^v
ATK_mur(v)= ATK_base × 1,035^v
Quantité  : plafonnée à partir d'un seuil (perf + lisibilité top-down)
Composition : rotation des familles de monstres par tranches de vagues → toutes les armes servent
```

- Croissance douce mais sans fin : la chute du mur est inévitable ; la build détermine QUAND.
- **Record personnel** = meilleure vague atteinte, stocké dans la sauvegarde du slot.

## 5. Rôles de monstres (multiplicateurs sur la base du stage) 🔶

| Rôle | PV | Dégâts au mur | Vitesse | Particularité |
|---|---|---|---|---|
| Standard | ×1 | ×1 | moyenne | — |
| Rapide | ×0,5 | ×0,7 | rapide | pattern d'évitement |
| Tank | ×5 | ×1,5 | lente | défense élevée |
| Tireur | ×0,8 | ×1 (à distance) | moyenne | attaque le mur de loin |
| **Soigneur** ✅ | ×0,7 | — | moyenne | **soigne les alliés en avançant (aura/pulsation) ; « au contact », soigne au lieu de frapper** → cible prioritaire |
| **Bouclier-porteur** ✅ | ×0,7 | — | lente | **applique des boucliers aux alliés en avançant et au contact** → cible prioritaire |
| **Accélérateur** ✅ | ×0,7 | — | moyenne | aura : +40 % vitesse des alliés proches |
| Boss | ×15 | ×3 | lente | fin de zone, rares garantis, débloque une arme |

- Les monstres support ✅ sont LE levier de pression : les ignorer = se faire envahir. Supports « farfelus » supplémentaires (Ressusciteur, Voleur de loot, Porte-étendard, Diviseur, Brumeux, Kamikaze) : voir `../Mechanics/Bestiaire.md`.
- Chaque rôle se décline en **variantes élémentaires** (faiblesse/résistances + recoloration).

## 6. Bestiaire par zone ✅

- **Le bestiaire complet existe : `../Mechanics/Bestiaire.md`** — 30 monstres + 6 boss (abstraits), avec multiplicateurs, faiblesses, patterns et skills, prêts pour `DT_Monsters`.
- Introduction par zone : 2 types au stage 1, +1 nouveau par stage, les 5 + boss au stage 5.
- La progression joueur côté armes va de **1 à 100 niveaux** (`../Mechanics/ArmesEtElements.md` §3) : la campagne (30 stages) se calibre sur ~nv 1–40 ; les nv 50–100 et leurs capstones sont le moteur du stage infini 🔶 à recaler au playtest.

## 7. Implémentation (données, pas de code en dur) ✅

- `DT_StageScaling` : par stage → multiplicateurs PV/ATK/quantité, composition, vagues.
- `DT_MonsterRoles` + fiches monstres : rôle, faiblesse, résistances, pattern, drops.
- `E_Element` + multiplicateurs (faiblesse ×1,75 / résistance ×0,5 🔶).
- Stage 31 : mêmes tables + fonction de croissance par vague.

## 8. Prochaines étapes

1. Chiffrer les 7 armes (DPS de référence au niveau 1) — base de toutes les courbes.
2. Bestiaire abstrait de la **zone 1** : 5 monstres + boss (stats, faiblesses, patterns).
3. Prototyper la formule de dégâts (réduction %, faiblesse/résistances, bouclier) dans `HealthComponent`.
4. Playtest zone 1 : TTK 2–5 s, écart ×3,5 bon/mauvais élément (trop punitif ?), rythme 2–3 tentatives.
