# Progression, loot et économie

> v0.4 — 12 juin 2026. Référencé par le GDD §7.

---

## 1. La boucle ✅

```
STAGE : tuer des monstres → drops (× multiplicateur d'étoiles)
  └─> MENU : dépenser → améliorer (armes / personnage / mur) / débloquer (skins, équipements)
        └─> retenter ou avancer (progression linéaire ; boss de zone → nouvelle arme)
```

**Règle d'or : perdre fait progresser.** Tout le loot est conservé, victoire ou défaite.

## 2. Les drops ✅

| Drop | Sert à | Notes |
|---|---|---|
| **Argent (or)** | monnaie universelle : améliorations, déblocages | droppé par tous |
| **Expérience** | niveaux du personnage | 🔶 stats par niveau à définir |
| **Ressources élémentaires** | améliorer l'arme de l'élément correspondant | un monstre droppe la ressource de SON élément → farmer le bon stage a du sens. **3 tiers par mode** ✅ v0.6 : **Fragments** (Normal) · **Cristaux** (Hard) · **Noyaux** (Enfer) |
| **Objets rares** | débloquer équipements et **skins gagnables** | table de drop **par monstre** ; boss = rares garantis |

### Multiplicateur d'étoiles ✅
- Seuils ✅ (répartition équitable) : 3★ = 100 % PV mur · 2★ ≥ 66 % · 1★ ≥ 33 % · 0★ < 33 %.
- Les drops du stage sont **multipliés par les étoiles** ✅ : 0★ ×1 · 1★ ×1,25 · 2★ ×1,5 · 3★ ×2, + chances de rare accrues.
- **Récompense fixe de fin de stage** selon les étoiles, + **bonus spécial à 3★** (objet rare garanti 🔶).

### Ramassage ✅ (v0.5)
- Les drops **expirent au sol** (🔶 ~10 s + clignotement) → tension ramasser vs tuer.
- **Aimant automatique** : les drops volent vers le joueur ; la **portée d'attraction monte avec le niveau du personnage** — c'est LA stat de confort de la montée en XP.

## 3. Les dépenses ✅ structure / 🔶 valeurs

1. **Améliorer les armes** — ressources élémentaires (de l'élément de l'arme) + or ; **niveaux 1 → 100**, palier majeur tous les 5 (voir `ArmesEtElements.md` §4). **Les tiers structurent la courbe** ✅ v0.6 : nv ~1–40 = **Fragments** (mode Normal), ~41–70 = **Cristaux** (Hard), ~71–100 = **Noyaux** (Enfer) — monter une arme au max impose de maîtriser les 3 modes. Courbe de coûts 🔶 à chiffrer (mandat BAK). *(Les armes ne s'achètent pas : elles se gagnent sur les boss.)*
2. **Améliorer le mur** ✅ — **PV et défense** contre des monstres de plus en plus forts (or 🔶 + ressources ?).
3. **Power up le personnage** — niveaux via XP ; 🔶 stats concernées (vitesse, rayon de ramassage, bonus globaux ?).
4. **Équipements** 🔶 — accessoires passifs à préciser.
5. **Skins gagnables** — cosmétiques débloquables par le jeu (objets rares + or).

## 4. Sauvegarde ✅

- **5 slots** de partie indépendants ; **sauvegarde et chargement automatiques** ; nouvelle partie / recommencer de zéro à tout moment.
- ⚙️ Base existante : `WallDefenseSaveGame` + `AutoSaveComponent` → à étendre (slots, méta-progression complète : armes, niveaux, étoiles par stage, record du stage infini).

## 5. Garde-fous d'économie 🔶

- **Rythme cible** : battre un stage en 2–3 tentatives quand on est à jour ; sinon augmenter les drops (jamais l'inverse en douce).
- **Pas de farm subi** : rejouer un stage = choix d'optimisation (étoiles manquantes, ressource ciblée). Les étoiles ×2 rendent le re-clear des vieux stages rapide ET rentable — c'est voulu.
- 4 familles de monnaies maximum, ne pas en ajouter.
- Le **stage 31 (infini)** est la zone de farm de fin de jeu : généreux en loot, record personnel comme moteur.
