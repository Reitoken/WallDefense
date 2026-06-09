# Wall Defense — Game Design Document

> **Document status**: v0.2 (reduced MVP scope) — June 9, 2026
> Rules marked ✅ are already implemented in the project. Those marked 🔶 are still to be decided/validated.
> This document is the **single source of truth** for the game rules: any mechanic added to the code must be described here.

---

## 1. Vision

### 1.1 Pitch ✅ *(decided v0.2)*
A **small, snappy game**: tower defense with an RPG flavor. Monsters rush a **wall** across **lanes** (*Mega Man Battle Network*-style movement); the player must kill them **as fast as possible** by **switching between weapons** to maximize damage. Killed monsters drop **loot** which is spent in the menu to **upgrade weapons and the character** — losing is never an end: you come back stronger and beat the stage.

**Deliberately reduced scope**: no system stacking — one single, short, generous loop, detailed in `Mechanics/Progression_en.md`.

### 1.2 Design pillars
🔶 *Starting proposal, to validate:*
1. **Readability** — you understand at a glance which lane is threatened and by what.
2. **Mobility = survival** — positioning matters as much as aiming.
3. **Rising tension** — each wave is more dangerous, the wall doesn't repair itself.

### 1.3 References
- *Mega Man Battle Network* — grid/lane-based movement.
- 🔶 Other references to list in `Production/05_References/`.

---

## 2. Core Loop ✅ *(decided v0.2)*

```
MENU (hub): upgrade weapons (materials) / character (gold) / pick a stage
   └─> STAGE: monster waves rushing the wall
        ├─ Combat: switch weapons + stand on the right lane = kill fast
        ├─ VICTORY (all waves cleared) → next stage + bonus loot
        └─ DEFEAT (wall destroyed) → back to menu, LOOT KEPT
             └─> upgrade → retry the stage
```

- **Defeat**: the wall is destroyed. **Victory**: all waves of the stage cleared.
- **Losing means progressing**: loot from killed monsters is always kept.
- Full loop and scope detail: `Mechanics/Progression_en.md`.

---

## 3. The arena

### 3.1 The lane grid ✅
The playfield is a grid of **parallel lanes** (implemented by `LaneGrid`):

| Parameter | Default value | Note |
|---|---|---|
| Number of lanes | **3** | configurable per level |
| Lane spacing | 250 uu | |
| Lane length | 2000 uu | |
| Columns per lane | **8** | subdivisions along the lane (planned for future column jumps 🔶) |

- Monsters advance **along** the lanes toward the wall.
- The player jumps **between** lanes (lateral) and moves **along** their current lane.
- The grid can be freely oriented in the level; all rules follow its orientation.

### 3.2 The wall 🔶
*Heart of the concept, not implemented yet:*
- The wall sits at the end of the lanes, on the player's side.
- It has hit points; monsters that reach it attack it.
- 🔶 HP per lane segment or a single global HP pool?
- 🔶 Repairs possible? (cost, between waves?)
- 🔶 Wall upgrades (armor, traps, turrets?)

### 3.3 Zones ✅
- **Monster spawn zone** (`MonsterSpawnZone`) — at the end opposite the wall.
- **Monster movement zone** (`MonsterMovementZone`) — bounds where monsters can roam.

---

## 4. The player

### 4.1 Movement (MMBN style) ✅
Implemented by `LanePlayerCharacter`:
- **Left / Right**: jump from one lane to another (1 lane per press, input threshold 0.5).
- **Up / Down**: move forward/backward along the current lane (600 uu/s), the character faces the move direction.
- The player stays **snapped to the centerline** of their lane (fast interpolation, tunable).
- Starting lane: index 1 (center lane on a 3-lane grid).

### 4.2 Weapons and switching ✅ base code / 🔶 design decided v0.2
- The player carries **3 weapons** and **switches on the fly** — that's the core skill: the right weapon against the right monster (machine gun/single-target, heavy cannon/armor-piercing, shockwave/area).
- Reason to switch (to validate in prototype 🔶): overheat/rotation + distinct roles. Detail: `Mechanics/Progression_en.md` §2.
- Existing code: `Weapon` + `WeaponComponent`, `Bullet` projectiles with an **object pool** (`BulletPool`). Multi-weapon and switching still to implement.
- Weapons are upgraded in the menu with **loot** (+N levels, qualitative milestones).

### 4.3 Player health ✅ *(decided v0.2)*
- **The player cannot die (MVP)**: only the wall matters. At worst, knockback/slow 🔶.
- The existing `HealthComponent` will serve the wall and the monsters.

---

## 5. Monsters

### 5.1 Base behavior ✅
All monsters inherit from `BaseMonster`:

| Stat | Default | Description |
|---|---|---|
| HP | 100 | via `HealthComponent` |
| Speed | 200 uu/s | |
| Strength (damage) | 10 | |
| Attack range | 200 uu | |
| Attack cooldown | 1.5 s | |

### 5.2 Movement patterns ✅
- **Straight line** or **sinusoidal / zig-zag** (tunable amplitude and frequency).
- Pathing: direct to target, or **locked to a lane** from spawn (default mode).

### 5.3 Attack modes ✅
- **Melee**: strikes on contact (attack animation montage).
- **Ranged**: fires projectiles (800 uu/s speed, 1200 uu range by default).

### 5.4 Combat feedback ✅
- White flash + mesh shake on hit, Niagara effects and sounds (hit, death), hit-react and death montages.

### 5.5 Bestiary 🔶
*To design — each monster = a sheet in `Mechanics/`:*

| Monster | Role | Status |
|---|---|---|
| Test monster (imported from old game) | validate the pipeline | ✅ in game |
| 🔶 Basic grunt | cannon fodder | to define |
| 🔶 Fast | forces repositioning | to define |
| 🔶 Tank | damage sponge, threatens the wall | to define |
| 🔶 Shooter | attacks the player at range | to define |
| 🔶 Boss | end of a wave sequence | to define |

---

## 6. Combat — damage rules

- Damage goes through `HealthComponent.ApplyDamage`: **actual damage = incoming damage − defense** (defense is a stat on the component). ✅
- Healing possible via `Heal`. ✅
- 🔶 Damage types / elemental weaknesses?
- 🔶 Critical hits?

---

## 7. Progression, loot and stages ✅ *(decided v0.2)* / 🔶 values

Deliberately reduced scope: **a single progression loop** (weapons + light character), no system stacking. The "always something to upgrade" spirit is kept, but concentrated.

- **Loot**: every killed monster drops gold + materials typed per monster (farming is meaningful). Kept even on defeat.
- **Upgrades**: weapons (+10%/level, qualitative milestones at lv 5/10/15/20) and 4 character stats. Detail: `Mechanics/Progression_en.md`.
- **Fixed stages**: monster stats are frozen per stage; the player is what rises. Curves, TTK and costs: `Balancing/MonsterScaling_en.md`.
- MVP targets: 3 weapons, 5 monsters, 10 stages, 1 arena.
- 🔶 Exact values to tune in playtest (target pacing: a stage beaten in 2–3 runs).

---

## 8. Camera and game feel ✅ / 🔶

- Dedicated game camera (`GameCamera`) with **camera shake** (`GameCameraShake`). ✅
- 🔶 Final camera angle: side view? three-quarter back view (like MMBN)? fixed or following the player?

---

## 9. Interface (UI) 🔶

*To design — mockups in `Production/02_Art/UI/`:*
- Wall health bar (and/or per lane segment).
- Player health bar.
- Wave number / progression.
- Main menu (a `MenuGameMode` already exists ✅), pause, defeat/victory screens.

---

## 10. Meta and save system ✅ / 🔶

- Save system in place: `WallDefenseSaveGame` + `AutoSaveComponent` (auto-save). ✅
- 🔶 What exactly do we save? (level progression, upgrades, options, high scores)

---

## 11. Production conventions

- Asset naming: see `WallDefense/ReadeMe.txt` (BP_, SM_, M_, T_, WBP_, etc.).
- C++ code by modules: `Core`, `Player`, `Lanes`, `Monsters`, `Weapons`, `Combat`, `GameModes`, `Camera`, `Arena`.
- Any new gameplay rule → written here first, then implemented.

---

## 12. Open questions (next decisions)

*Resolved in v0.2: victory/defeat conditions (wall), player mortality (no), progression scope (weapons + light character).*

1. Detailed wall behavior: global HP or per lane segment? repair? (§3.2)
2. Switch mechanism: overheat only, roles only, or both? → settle in prototype (§4.2).
3. Exact stats of the 3 weapons (→ reference DPS, `Balancing/MonsterScaling_en.md` §6).
4. Sheets for the 5 MVP monsters: grunt, fast, tank, shooter, boss (§5.5).
5. Final camera angle (§8).
6. Tank "armor": % reduction against wrong weapons — validate the value (`Balancing/MonsterScaling_en.md` §5).
