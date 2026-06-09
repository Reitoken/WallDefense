# Progression — Scope réduit (MVP)

> v0.2 — 9 juin 2026. Document jumeau : `Progression_en.md`.
> **Décision de scope** : on garde *l'esprit* des RPG idle (multiplicateurs, progression constante) mais avec un **cœur minimal et réalisable**. Les systèmes étendus sont déplacés en backlog (§5) et ne doivent PAS être implémentés pour le MVP.

---

## 1. La boucle complète (MVP)

```
MENU (hub)
 ├─ Améliorer les armes (avec les matériaux lootés)
 ├─ Améliorer le personnage (avec l'or)
 └─ Lancer un stage
      └─> Vagues de monstres → ils foncent sur le MUR
           ├─ VICTOIRE (toutes les vagues nettoyées) → stage suivant débloqué + loot bonus
           └─ DÉFAITE (mur détruit) → retour menu — ON GARDE TOUT LE LOOT
                └─> améliorer → retenter le stage
```

**Le pilier : perdre fait toujours progresser.** Chaque run rapporte du loot, même perdue. Le joueur bat un stage *parce qu'il* a farmé/amélioré — jamais bloqué, juste « pas encore assez fort ».

- Défaite = mur détruit. Le joueur ne meurt pas (MVP) — il encaisse au pire des knockbacks/ralentissements 🔶.
- Victoire = toutes les vagues du stage nettoyées.

## 2. Les armes — cœur du gameplay ET de la progression

Le joueur porte **3 armes** et **switche à la volée** (molette / boutons). Tout le skill en combat = tuer le plus vite possible en utilisant la bonne arme au bon moment.

### 2.1 Pourquoi switcher ? (le fun moment-à-moment)
Deux mécanismes complémentaires 🔶 (à valider en prototype) :
1. **Surchauffe/rechargement** : tirer en continu surchauffe l'arme → le DPS optimal s'obtient en **rotation** entre les armes.
2. **Rôles distincts** : chaque arme excelle contre un profil de monstre.

| Arme (MVP) | Rôle | Forte contre | Faible contre |
|---|---|---|---|
| Mitrailleuse | DPS soutenu mono-cible | rapides | tanks (armure) |
| Canon lourd | burst lent, perce-armure | tanks, boss | essaims |
| Onde de choc | dégâts de zone | essaims/groupes | cibles isolées |

### 2.2 Amélioration des armes (la progression principale)
- Chaque arme a un **niveau (+N)** : coût = **matériaux** (loot) + **or**.
- Chaque niveau : **+10 % de dégâts de base** 🔶.
- **Paliers qualitatifs** aux niveaux 5/10/15/20 : bonus tangible (cadence, perce-armure, largeur de zone…) — c'est le « pic de puissance » de la dent de scie.
- Pas d'étoiles, pas de rareté, pas de sous-stats au MVP (→ backlog).

## 3. Le loot

- **Chaque monstre tué** lâche : **or** (monnaie universelle) + chance de **matériaux d'amélioration**.
- Matériaux typés par monstre : ex. le tank lâche des `Plaques`, le rapide des `Fibres`… → améliorer l'arme anti-tank demande de tuer des tanks : le farm a du sens.
- **Boss de stage** : matériaux garantis + premier kill = gros bonus.
- Le loot est conservé en cas de défaite (règle d'or §1).
- 3–4 types de matériaux maximum au MVP.

## 4. Le personnage (léger)

Quelques stats à monter au menu **avec l'or seul** (pas de matériaux) :

| Stat | Effet | Niveaux |
|---|---|---|
| PV du mur | +X PV par niveau | ~20 |
| Dégâts globaux | +2 % par niveau (toutes armes) | ~20 |
| Vitesse de déplacement | +2 % par niveau | ~10 |
| Refroidissement | surchauffe plus lente | ~10 |

Pas de classes, pas d'arbre, pas de compagnons au MVP.

## 5. Backlog post-MVP (l'ex-liste complète)

Les systèmes inspirés des RPG idle, **à ne sortir que si le cœur est fun** — par ordre de valeur probable :
1. Étoiles/rareté d'armes (allonge chaque piste d'amélioration).
2. Nouvelles armes à débloquer (4e, 5e…).
3. Compagnon/tourelle unique.
4. Modules du mur (pièges, armure).
5. Classes/spécialisations, artefacts + résonance de collection, exploration, cuisine, guilde, cosmétiques — voir l'historique git de ce fichier pour le détail complet (version v0.1).

## 6. Cibles de contenu MVP 🔶

| Contenu | Quantité |
|---|---|
| Armes | 3 |
| Types de monstres | 5 (grunt, rapide, tank, tireur, boss) |
| Stages | 10 |
| Arène | 1 (variations de couleur) |
| Matériaux de loot | 3–4 |

> L'équilibrage chiffré (courbes par stage, coûts, TTK) vit dans `../Balancing/MonsterScaling_fr.md`.
