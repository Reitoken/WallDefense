# Évolution des monstres — 30 stages + stage infini

> v0.3 — 12 juin 2026. Référencé par le GDD §6 et §8. Prérequis : `../Mechanics/Progression.md`.

---

## 1. Le modèle ✅

- **Stages 1–30 : stats FIXES par stage.** Le stage 12 est toujours le stage 12 ; c'est le joueur qui monte (loot → améliorations). Le « mur de difficulté » naturel : échouer → farmer/améliorer → repasser.
- **Stage 31 (infini) : croissance continue par vague**, sans plafond. Personne ne le « finit » ; on y mesure sa build → score → leaderboard Steam.
- La variable ressentie reste le **TTK** (temps pour tuer), modulé par le **bon élément** : c'est lui qui crée l'écart de skill.

## 2. Dégâts : la formule élémentaire 🔶 à valider

```
Dégâts = DégâtsArme × MultiplicateurÉlémentaire × (100 / (100 + Défense))
```

| Situation | Multiplicateur proposé |
|---|---|
| **Faiblesse** du monstre | ×1,75 |
| Neutre | ×1,0 |
| **Résistance** du monstre | ×0,5 |
| Élément Normal | ×1,0 partout (jamais résisté, jamais super-efficace) — la valeur sûre, compensée par de meilleures stats brutes |

- ⚠️ On remplace la défense en **soustraction flat** du `HealthComponent` actuel par une réduction en % (`100/(100+DEF)`) : indispensable pour que la défense reste pertinente sur 30 stages de croissance géométrique.
- **Bouclier** 🔶 : couche de PV au-dessus des PV (à percer d'abord) ; proposition : certains éléments sont plus efficaces contre les boucliers (Lumière ?).
- Bien joué (bonne arme + faiblesse) vs mal joué (résistance) = écart **×3,5** : le switch n'est pas optionnel.

## 3. Courbes des stages 1–30 🔶 valeurs de départ

```
PV_m(s)    = 100 × 1,17^(s−1)     → ×1 (st.1) … ×96 (st.30)
ATK_mur(s) = 10  × 1,12^(s−1)     → ×1 … ×27
Vitesse    : ne scale JAMAIS (identité du monstre)
Quantité par vague : croît avec le stage, plafonnée par la lisibilité
```

- La progression joueur attendue (armes +N, niveaux, nouvelles armes/éléments) doit suivre ≈ ×60–100 de DPS sur les 30 stages — à caler quand les grilles d'amélioration existeront.
- **Zone cible TTK** (monstre standard, bonne arme) : 2–5 s ; boss : 30–60 s.
- Rythme cible : un stage battu en **2–3 tentatives** quand on est à jour.
- Chaque stage a une **composition élémentaire dominante** (annoncée ou découverte 🔶) → le choix du loadout est la première décision du stage.

## 4. Stage 31 : la courbe infinie ✅ modèle / 🔶 valeurs

```
PV_m(vague v)  = PV_base(≈ stage 25) × 1,05^v
ATK_mur(v)     = ATK_base × 1,035^v
Quantité(v)    = plafonnée à partir d'un seuil (perf + lisibilité) — au-delà, seules les stats montent
Composition    : rotation des éléments par tranches de vagues → tous les loadouts sont testés
```

- Croissance **douce mais sans fin** : la mort du mur est inévitable ; le skill + la build déterminent QUAND.
- **Score** 🔶 proposition : `vague atteinte` comme mesure principale (simple, lisible, comparable), kills en départage.
- Anti-triche : sans serveur, un leaderboard Steam est pollué tôt ou tard. Niveau d'ambition à choisir (GDD §13.12) — au minimum, des bornes de plausibilité côté client.

## 5. Rôles de monstres (multiplicateurs sur la base du stage) 🔶

| Rôle | PV | Dégâts au mur | Vitesse | Particularité |
|---|---|---|---|---|
| Standard | ×1 | ×1 | moyenne | — |
| Rapide | ×0,5 | ×0,7 | rapide | pattern d'évitement |
| Tank | ×5 | ×1,5 | lente | défense élevée |
| Tireur | ×0,8 | ×1 (à distance) | moyenne | attaque le mur de loin |
| **Healer** | ×0,7 | ×0,3 | moyenne | soigne les monstres proches → cible prioritaire |
| **Porteur de bouclier** | ×1 + bouclier | ×1 | lente | le bouclier se perce (élément efficace 🔶) |
| Boss | ×15 | ×3 | lente | fin de stage, drops rares garantis |

Chaque rôle existe en **variantes élémentaires** (résistance/faiblesse + recoloration) → la grille `rôles × éléments` fournit le volume de bestiaire pour 30 stages sans exploser le coût de production (GDD §13.10).

## 6. Implémentation (données, pas de code en dur) ✅

- `DT_StageScaling` : par stage → multiplicateurs PV/ATK/quantité, composition élémentaire, vagues.
- `DT_MonsterRoles` : par rôle → multiplicateurs, particularités.
- `E_Element` + table des multiplicateurs élémentaires (faiblesse/résistance).
- Stage 31 : mêmes tables + fonction de croissance par vague.
- L'équilibrage s'itère dans CE fichier + les DataTables, jamais dans le code.

## 7. Prochaines étapes

1. Trancher la table élémentaire (qui est faible contre quoi) — bloque le bestiaire ET les stages.
2. Fixer les stats des premières armes (DPS de référence).
3. Bestiaire des stages 1–5 (3–4 monstres) + prototype de la formule de dégâts.
4. Playtest : valider TTK 2–5 s et l'écart ×3,5 bon/mauvais élément (trop punitif ?).
