# Wall Defense — Game Design Document

> **Document status**: draft v0.1 — June 9, 2026
> Rules marked ✅ are already implemented in the project. Those marked 🔶 are still to be decided/validated.
> This document is the **single source of truth** for the game rules: any mechanic added to the code must be described here.

---

## 1. Vision

### 1.1 Pitch
🔶 *To refine — starting proposal:*
A defense game where the player, alone on a battlefield divided into **lanes**, protects a **wall** against waves of monsters. The player moves from lane to lane in the style of *Mega Man Battle Network* and takes down attackers before they reach (or destroy) the wall.

### 1.2 Design pillars
🔶 *Starting proposal, to validate:*
1. **Readability** — you understand at a glance which lane is threatened and by what.
2. **Mobility = survival** — positioning matters as much as aiming.
3. **Rising tension** — each wave is more dangerous, the wall doesn't repair itself.

### 1.3 References
- *Mega Man Battle Network* — grid/lane-based movement.
- 🔶 Other references to list in `Production/05_References/`.

---

## 2. Core Loop

🔶 *To validate:*

```
Main menu
   └─> Run
        └─> Monster wave announced
             └─> Combat: move between lanes / shoot / dodge
                  └─> Wave cleared → reward / preparation   🔶
                       └─> Next wave (harder)
                            └─> Defeat (wall or player destroyed) or Victory 🔶
```

- 🔶 **Defeat condition**: wall destroyed? player dead? both?
- 🔶 **Victory condition**: survive N waves? endless mode with score?
- 🔶 **Between waves**: preparation pause, shop, wall repair?

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

### 4.2 Shooting ✅
- The player carries a weapon (`Weapon` + `WeaponComponent`) that fires projectiles (`Bullet`).
- Projectiles use an **object pool** (`BulletPool`) for performance.
- 🔶 Fire rate, damage, range of the base weapon: values to set in `Balancing/`.
- 🔶 Multiple weapons? Upgrade system? Limited ammo?

### 4.3 Player health ✅ / 🔶
- The player has HP via `HealthComponent` (default: 100 HP, 0 defense).
- 🔶 Can the player die, or does only the wall matter? Respawn?

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

## 7. Progression and waves 🔶

Progression is the heart of the intended game feel: player power is a **stack of multipliers** coming from many independent sources (level, classes, skills, starred equipment, artifact collection, companions, exploration…) — there is *always* something to upgrade. Against it, monsters scale on a **sawtooth** model (their growth slightly outpaces the player's passive progression; investing a step — star, rarity — gives a power spike back).

- **Player power sources (split into features F1–F10)**: see `Mechanics/Progression_en.md`.
- **Monster scaling: formulas, prototype values, TTK table**: see `Balancing/MonsterScaling_en.md`.
- 🔶 Detailed wave composition (what, how many, on which lanes, at what pace).
- 🔶 End-of-wave rewards (currencies, equipment loot, XP).

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

1. Exact victory/defeat condition (§2).
2. Detailed wall behavior: HP, segments, repair (§3.2).
3. Can the player die? (§4.3)
4. First bestiary: 3–4 monsters for a playable prototype (§5.5).
5. Minimal wave system to complete a full game loop (§7).
6. Final camera angle (§8).
7. Defense formula: flat subtraction (current) vs % reduction — blocking for defensive equipment (see `Balancing/MonsterScaling_en.md` §5).
8. Final mapping of progression features onto Wall Defense (see `Mechanics/Progression_en.md` §3).
