# Système de progression — Sources de puissance

> Brouillon v0.1 — 9 juin 2026. Document jumeau : `Progression_en.md`.
> Inspiré des RPG idle mobiles : la puissance vient de **nombreuses sources indépendantes**, chacune avec sa propre piste de progression. Le joueur a *toujours* quelque chose à améliorer → sensation de progression constante.

---

## 1. Principe directeur

**La puissance totale est un empilement de multiplicateurs.** Chaque feature ci-dessous contribue aux statistiques du personnage en **valeur brute (flat)** et/ou en **pourcentage** :

```
Stat finale = (Base + Σ contributions flat de chaque feature) × (1 + Σ bonus % de chaque feature)
```

Règles de design :
1. **Chaque source progresse indépendamment** — quand une source coûte trop cher à monter, une autre reste abordable.
2. **Les étoiles allongent chaque piste** — tout objet/élément a une rareté + des niveaux + des étoiles (paliers d'éveil). Un même objet vit longtemps.
3. **La collection compte** — tout ce que le joueur *possède* contribue (avec un poids réduit), pas seulement ce qui est équipé/actif. Posséder = progresser.
4. **Score de puissance global affiché** — un agrégat visible qui monte à chaque action (récompense psychologique).
5. **À tout moment, au moins une amélioration doit être accessible à court terme** (< quelques minutes de jeu).

---

## 2. Les features (sources de puissance isolées)

### F1 — Niveau du héros
- XP gagnée en combat ; chaque niveau donne des stats de base (ATQ, DÉF, PV, VIT).
- Sert de **portail** pour débloquer les autres features.

### F2 — Classes évolutives
- Arbre de classes : classes de base → classes avancées → classes d'élite (ex. 2 branches de départ qui se ramifient).
- Chaque classe a **son propre niveau** ; le **niveau total de classe** (somme de toutes les classes montées) donne des bonus permanents — monter une classe qu'on ne joue plus reste utile.
- Changement de classe déverrouillé à un palier du héros ; chaque classe oriente un style (offense, défense, soutien).
- Bonus typés : % ATQ/DÉF/PV/VIT + stats signature (ex. résistance critique, bonus de soin).

### F3 — Compétences
- Trois familles : **actives** (déclenchées), **passives** (permanentes), **fragments** (modificateurs équipables).
- Chaque compétence a un **niveau** (monte avec des ressources) et des **étoiles** (rareté/éveil) qui débloquent des effets supplémentaires.

### F4 — Équipement
- Slots multiples (arme, armure, accessoires…).
- Chaque pièce : **rareté** (commun → légendaire) × **niveau d'amélioration (+N)** × **étoiles**.
- Stats principales + **sous-stats** variées (précision, taux/dégâts critiques, blocage, bonus de soin, maîtrises/résistances par type de dégâts…).

### F5 — Collection d'artefacts
- Artefacts classés **par élément/type**, avec raretés (rare → épique → légendaire) et niveaux (+N).
- **Résonance de collection** : posséder N artefacts d'un type donne des paliers de bonus globaux (« niveau de résonance ») — même les doublons/pièces non utilisées font progresser.
- Deux stats jumelles par élément : **offensive** (affinité) et **défensive** (égide) — la collection nourrit l'attaque ET la défense.

### F6 — Compagnons
- Roster de créatures : **1 active** (bonus 100 %), **secondaires** (50 %), **toutes les autres possédées** (20 %) — tout le roster contribue en permanence.
- Chaque compagnon : niveau (nourrissage avec ressources), rareté/étoiles, **arbre d'aptitudes** propre (plusieurs pages).
- **Résonance de roster** : paliers de bonus selon le niveau cumulé des compagnons.

### F7 — Exploration
- Bonus de stats **permanents** liés au % d'exploration de chaque zone/niveau — récompense la complétion.

### F8 — Consommables / cuisine
- Buffs de stats semi-permanents (plats, élixirs) avec leurs propres recettes à collectionner et améliorer.

### F9 — Social / guilde
- Bonus collectifs modestes (le groupe fait progresser l'individu).

### F10 — Cosmétiques à stats
- Apparences (visages, skins) avec petites stats — la personnalisation participe à la puissance.

---

## 3. Adaptation à Wall Defense 🔶

Mapping proposé (à valider) :

| Feature générique | Dans Wall Defense |
|---|---|
| F1 Héros | Niveau du défenseur |
| F2 Classes | Spécialisations du défenseur (artilleur, bâtisseur, soigneur…) |
| F3 Compétences | Tirs spéciaux, passifs de lane, modificateurs |
| F4 Équipement | Armes + pièces d'armure du défenseur |
| F5 Artefacts | Reliques par type de dégâts (affinité/égide par élément) |
| F6 Compagnons | Drones/tourelles/créatures alliées sur les lanes |
| F7 Exploration | Complétion des arènes (étoiles de niveau) |
| F8 Cuisine | Rations de siège (buffs de partie) |
| F9 Guilde | (post-launch) |
| F10 Cosmétiques | Skins du défenseur et du mur |
| **Le mur** | Source de puissance supplémentaire propre au jeu : PV, armure, modules — sa propre piste de progression |

---

## 4. Ordre d'implémentation proposé 🔶

1. **F1 + F4 simplifiés** (niveau héros + une arme à rareté/niveau/étoiles) — suffit pour valider la boucle avec l'évolution des monstres.
2. F3 (3–4 compétences), puis F6 (1 compagnon), puis F5 (résonance de collection).
3. Le reste en post-prototype.

> L'équilibrage chiffré (courbes joueur vs monstres) vit dans `../Balancing/MonsterScaling_fr.md`.
