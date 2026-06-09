# Progression — Reduced scope (MVP)

> v0.2 — June 9, 2026. Twin document: `Progression_fr.md`.
> **Scope decision**: we keep the *spirit* of idle RPGs (multipliers, constant progression) but with a **minimal, achievable core**. Extended systems move to the backlog (§5) and must NOT be implemented for the MVP.

---

## 1. The full loop (MVP)

```
MENU (hub)
 ├─ Upgrade weapons (with looted materials)
 ├─ Upgrade the character (with gold)
 └─ Launch a stage
      └─> Monster waves → they rush the WALL
           ├─ VICTORY (all waves cleared) → next stage unlocked + bonus loot
           └─ DEFEAT (wall destroyed) → back to menu — YOU KEEP ALL THE LOOT
                └─> upgrade → retry the stage
```

**The pillar: losing always means progressing.** Every run yields loot, even a lost one. The player beats a stage *because* they farmed/upgraded — never stuck, just "not strong enough yet".

- Defeat = wall destroyed. The player cannot die (MVP) — at worst they take knockbacks/slows 🔶.
- Victory = all waves of the stage cleared.

## 2. Weapons — heart of the gameplay AND of progression

The player carries **3 weapons** and **switches on the fly** (wheel / buttons). All combat skill = killing as fast as possible by using the right weapon at the right moment.

### 2.1 Why switch? (the moment-to-moment fun)
Two complementary mechanisms 🔶 (to validate in prototype):
1. **Overheat/reload**: continuous fire overheats a weapon → optimal DPS comes from **rotating** between weapons.
2. **Distinct roles**: each weapon excels against a monster profile.

| Weapon (MVP) | Role | Strong vs | Weak vs |
|---|---|---|---|
| Machine gun | sustained single-target DPS | fast | tanks (armor) |
| Heavy cannon | slow burst, armor-piercing | tanks, bosses | swarms |
| Shockwave | area damage | swarms/groups | isolated targets |

### 2.2 Weapon upgrades (the main progression)
- Each weapon has a **level (+N)**: cost = **materials** (loot) + **gold**.
- Each level: **+10% base damage** 🔶.
- **Qualitative milestones** at levels 5/10/15/20: tangible bonus (fire rate, armor-piercing, area width…) — that's the "power spike" of the sawtooth.
- No stars, no rarity, no substats in the MVP (→ backlog).

## 3. Loot

- **Every monster killed** drops: **gold** (universal currency) + a chance of **upgrade materials**.
- Materials typed per monster: e.g. the tank drops `Plates`, the fast one drops `Fibers`… → upgrading the anti-tank weapon requires killing tanks: farming is meaningful.
- **Stage boss**: guaranteed materials + first kill = big bonus.
- Loot is kept on defeat (golden rule §1).
- 3–4 material types maximum in the MVP.

## 4. The character (light)

A few stats raised in the menu **with gold only** (no materials):

| Stat | Effect | Levels |
|---|---|---|
| Wall HP | +X HP per level | ~20 |
| Global damage | +2% per level (all weapons) | ~20 |
| Movement speed | +2% per level | ~10 |
| Cooling | slower overheat | ~10 |

No classes, no tree, no companions in the MVP.

## 5. Post-MVP backlog (the former full list)

The idle-RPG-inspired systems, **to ship only if the core is fun** — by likely value:
1. Weapon stars/rarity (extends every upgrade track).
2. New weapons to unlock (4th, 5th…).
3. A single companion/turret.
4. Wall modules (traps, armor).
5. Classes/specializations, artifacts + collection resonance, exploration, cooking, guild, cosmetics — see this file's git history for the full detail (version v0.1).

## 6. MVP content targets 🔶

| Content | Quantity |
|---|---|
| Weapons | 3 |
| Monster types | 5 (grunt, fast, tank, shooter, boss) |
| Stages | 10 |
| Arena | 1 (color variations) |
| Loot materials | 3–4 |

> Numbers (per-stage curves, costs, TTK) live in `../Balancing/MonsterScaling_en.md`.
