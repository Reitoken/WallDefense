# Progression system — Power sources

> Draft v0.1 — June 9, 2026. Twin document: `Progression_fr.md`.
> Inspired by mobile idle RPGs: power comes from **many independent sources**, each with its own progression track. The player *always* has something to upgrade → constant feeling of progression.

---

## 1. Guiding principle

**Total power is a stack of multipliers.** Each feature below contributes to character stats as **flat values** and/or **percentages**:

```
Final stat = (Base + Σ flat contributions from each feature) × (1 + Σ % bonuses from each feature)
```

Design rules:
1. **Each source progresses independently** — when one source gets too expensive to raise, another stays affordable.
2. **Stars extend every track** — every item/element has a rarity + levels + stars (awakening tiers). A single item lives a long time.
3. **Collection matters** — everything the player *owns* contributes (at a reduced weight), not just what is equipped/active. Owning = progressing.
4. **A global power score is displayed** — a visible aggregate that goes up with every action (psychological reward).
5. **At any moment, at least one upgrade must be reachable short-term** (< a few minutes of play).

---

## 2. The features (isolated power sources)

### F1 — Hero level
- XP earned in combat; each level grants base stats (ATK, DEF, HP, SPD).
- Acts as a **gate** to unlock the other features.

### F2 — Evolving classes
- Class tree: base classes → advanced classes → elite classes (e.g. 2 starting branches that fork).
- Each class has **its own level**; the **total class level** (sum of all leveled classes) grants permanent bonuses — leveling a class you no longer play stays useful.
- Class change unlocked at a hero milestone; each class drives a style (offense, defense, support).
- Typed bonuses: % ATK/DEF/HP/SPD + signature stats (e.g. crit resistance, healing bonus).

### F3 — Skills
- Three families: **active** (triggered), **passive** (permanent), **fragments** (equippable modifiers).
- Each skill has a **level** (raised with resources) and **stars** (rarity/awakening) unlocking extra effects.

### F4 — Equipment
- Multiple slots (weapon, armor, accessories…).
- Each piece: **rarity** (common → legendary) × **upgrade level (+N)** × **stars**.
- Main stats + varied **substats** (accuracy, crit rate/damage, block, healing bonus, masteries/resistances per damage type…).

### F5 — Artifact collection
- Artifacts sorted **by element/type**, with rarities (rare → epic → legendary) and levels (+N).
- **Collection resonance**: owning N artifacts of a type grants global bonus tiers ("resonance level") — even duplicates/unused pieces contribute.
- Two twin stats per element: **offensive** (affinity) and **defensive** (aegis) — the collection feeds both attack AND defense.

### F6 — Companions
- Creature roster: **1 active** (100% bonus), **secondaries** (50%), **all other owned** (20%) — the whole roster contributes permanently.
- Each companion: level (fed with resources), rarity/stars, its own **ability tree** (multiple pages).
- **Roster resonance**: bonus tiers based on the cumulative level of companions.

### F7 — Exploration
- **Permanent** stat bonuses tied to the exploration % of each zone/level — rewards completion.

### F8 — Consumables / cooking
- Semi-permanent stat buffs (dishes, elixirs) with their own recipes to collect and upgrade.

### F9 — Social / guild
- Modest collective bonuses (the group makes the individual progress).

### F10 — Stat cosmetics
- Appearances (faces, skins) with small stats — customization participates in power.

---

## 3. Adaptation to Wall Defense 🔶

Proposed mapping (to validate):

| Generic feature | In Wall Defense |
|---|---|
| F1 Hero | Defender level |
| F2 Classes | Defender specializations (gunner, builder, healer…) |
| F3 Skills | Special shots, lane passives, modifiers |
| F4 Equipment | Defender weapons + armor pieces |
| F5 Artifacts | Relics per damage type (affinity/aegis per element) |
| F6 Companions | Drones/turrets/allied creatures on the lanes |
| F7 Exploration | Arena completion (level stars) |
| F8 Cooking | Siege rations (run buffs) |
| F9 Guild | (post-launch) |
| F10 Cosmetics | Defender and wall skins |
| **The wall** | Extra power source unique to our game: HP, armor, modules — its own progression track |

---

## 4. Proposed implementation order 🔶

1. **Simplified F1 + F4** (hero level + one weapon with rarity/level/stars) — enough to validate the loop against monster scaling.
2. F3 (3–4 skills), then F6 (1 companion), then F5 (collection resonance).
3. The rest post-prototype.

> Numbers and curves (player vs monsters) live in `../Balancing/MonsterScaling_en.md`.
