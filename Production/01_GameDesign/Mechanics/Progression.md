# Progression, loot et économie

> v0.3 — 12 juin 2026. Référencé par le GDD §7.

---

## 1. La boucle ✅

```
STAGE : tuer des monstres → drops
  └─> MENU : dépenser → débloquer / améliorer / s'équiper
        └─> retenter ou avancer (stage suivant, jusqu'au 30, puis l'infini 31)
```

**Règle d'or : perdre fait progresser.** Tout le loot ramassé est conservé, victoire ou défaite.

## 2. Les drops (ce que lâche un monstre vaincu) ✅

| Drop | Sert à | Notes |
|---|---|---|
| **Argent (or)** | monnaie universelle : améliorations, déblocages | droppé par tous |
| **Expérience** | niveaux du personnage | 🔶 stats montées par niveau à définir |
| **Ressources élémentaires** | améliorer les armes de l'élément correspondant | un monstre droppe la ressource de SON élément → farmer le bon stage a du sens |
| **Objets rares** | débloquer équipements, armes, **skins gagnables** | table de drop **par monstre** (les boss/élites droppent les plus rares) |

🔶 Taux de drop, quantités par stage et coûts : à construire dans `../Balancing/` quand le bestiaire existera.

## 3. Les dépenses ✅ structure / 🔶 valeurs

1. **Débloquer des armes** — la matrice catégorie × élément s'ouvre progressivement (objets rares + or).
2. **Améliorer les armes** — niveaux +N : ressources élémentaires (de l'élément de l'arme) + or. Paliers qualitatifs (voir `ArmesEtElements.md` §4).
3. **Power up le personnage** — niveaux via XP ; 🔶 stats concernées (PV du mur ? vitesse ? capacité de loadout ?).
4. **Équipements** 🔶 — à préciser : accessoires passifs ? (GDD §13.)
5. **Skins gagnables** — cosmétiques débloquables par le jeu (≠ boutique, voir GDD §9).

## 4. Le loadout (s'équiper avant l'aventure) ✅

- On compose son loadout au menu : quelles catégories, quels éléments (proposition de départ : 2 catégories × 3 éléments 🔶).
- Choix stratégique informé : l'écran de sélection de stage devrait afficher les éléments/types des monstres attendus 🔶 (ou seulement après une première tentative ? — découverte vs préparation).

## 5. Les étoiles de stage ✅ / 🔶 usage

- 0–3 ⭐ selon les PV du mur (GDD §2.1). Total max : 90 ⭐ sur 30 stages.
- 🔶 Usage proposé : paliers de récompenses globales (à 15/30/45/60/75/90 ⭐), et/ou exigence d'accès à certains stages, et/ou défis du battle pass gratuit.

## 6. Garde-fous d'économie 🔶

- **Rythme cible** : battre un stage en 2–3 tentatives quand on est « à jour » ; si les playtests montrent plus → augmenter les drops, pas l'inverse en douce.
- **Pas de farm obligatoire massif** : rejouer un stage doit être un choix d'optimisation (étoiles manquantes, ressource ciblée), pas une corvée imposée.
- 4 familles de monnaies, c'est déjà beaucoup pour un petit jeu : ne pas en ajouter.
- Le stage 31 (infini) doit rester généreux en loot : c'est aussi LA zone de farm de fin de jeu.
