# Monster scaling — Balancing against player progression

> Draft v0.1 — June 9, 2026. Twin document: `MonsterScaling_fr.md`.
> Prerequisite: `../Mechanics/Progression_en.md` (the player's power sources).

---

## 1. The problem to solve

The player stacks power multipliers constantly. If monsters don't scale at the right pace:
- **too slow** → the game becomes trivial, progression no longer *feels* like anything (everything dies instantly no matter what);
- **too fast** → frustration wall, progression feels pointless.

The goal is NOT for monsters to exactly track the player: it is to create a controlled **sawtooth**.

## 2. The sawtooth model

We define:
- `P(w)`: expected offensive power of an "on-curve" player at wave `w` (effective DPS).
- `HP_m(w)`: hit points of a base monster at wave `w`.
- **TTK** (time-to-kill) = `HP_m(w) / P(w)`: time to kill a monster. This is THE felt variable.

**Golden rule: monsters grow slightly faster than the player's "passive" progression.**

```
HP_m(w)  = HP_0  × g_hp^w          (geometric growth per wave)
ATK_m(w) = ATK_0 × g_atk^w         with g_atk < g_hp
P(w)     = P_0   × g_p^w  × M(w)   M(w) = "step" multipliers (stars, rarity, awakening)
```

- The ratio `r = g_hp / g_p > 1` slowly raises the TTK → growing pressure.
- When TTK leaves the comfort zone, the player invests in a **step** (star, new rarity, awakening) → `M(w)` jumps (×1.5 to ×2) → TTK drops back → **feeling of power**. That's the sawtooth.
- `g_atk < g_hp`: difficulty must come through **attrition** (monsters take longer to kill, the wall takes hits), not through one-shotting the player.

### Difficulty wall cadence

TTK doubles every `N = ln(2) / ln(r)` waves if the player doesn't invest.

| Chosen `r` | TTK doubles every… | Feel |
|---|---|---|
| 1.02 | ~35 waves | very gentle, chill game |
| **1.035** | **~20 waves** | **recommended for the prototype** |
| 1.05 | ~14 waves | demanding, tense game |

## 3. Proposed prototype values 🔶

Anchored to current code defaults (`BaseMonster`: 100 HP, strength 10; `HealthComponent`: 100 player HP):

| Parameter | Value | Note |
|---|---|---|
| `HP_0` (grunt, wave 1) | 100 | current code default |
| `ATK_0` | 10 | current code default |
| `P_0` (player DPS, wave 1) | 40 | 🔶 e.g. 10 damage × 4 shots/s — to set with the weapon |
| `g_hp` | 1.10 / wave | |
| `g_atk` | 1.07 / wave | attrition > burst |
| `g_p` (on-curve player) | 1.065 / wave | → `r ≈ 1.033`, wall ~every 21 waves |
| Monster speed | +0.5% / wave, **capped at +50%** | speed must never make a lane unplayable |
| Count per wave | `6 + ⌊w/2⌋`, capped by lane capacity | density is part of the difficulty |

### Role multipliers (applied on top of the wave base)

| Role | HP | ATK | Appears |
|---|---|---|---|
| Grunt | ×1 | ×1 | every wave |
| Fast | ×0.6 | ×0.8 (speed ×1.8) | from wave 3 |
| Tank | ×4 | ×0.7 (speed ×0.6) | every 5 waves |
| Elite | ×4 | ×2 | every 5 waves |
| Boss | ×15 | ×3 | every 10 waves |

### Control table (grunt, "on-curve" player, no step invested)

| Wave | Monster HP | Player DPS | TTK |
|---|---|---|---|
| 1 | 110 | 43 | 2.6 s |
| 5 | 161 | 55 | 2.9 s |
| 10 | 259 | 75 | 3.5 s |
| 15 | 418 | 103 | 4.1 s |
| 20 | 673 | 141 | 4.8 s |
| 30 | 1,745 | 265 | 6.6 s |
| 40 | 4,526 | 497 | 9.1 s |
| 50 | 11,739 | 932 | 12.6 s |

**Reading**: without step investment, TTK drifts from 2.6 s to 12.6 s. Each purchased step (×1.5–×2) brings it back into the **target zone: 2 to 6 s for a grunt** (boss: 30–60 s). If the player over-invests, they steamroll for 10–15 waves — intended, that's the reward.

## 4. Scaling must not be stats only 🔶

Numbers alone become invisible. **Qualitative** milestones:
- **Every ~10 waves**: new behavior (more aggressive sinusoidal pattern, shooters, lane jumps 🔶…).
- **Visual variants**: recolors between milestones, a new monster at major milestones.
- The bestiary (GDD §5.5) should be designed as a **role × tier grid**.

## 5. Known traps / current code constraints

1. ⚠️ **Flat-subtraction defense** (`actual damage = incoming − defense`, `HealthComponent`): with geometrically growing ATK, flat defense quickly becomes worthless — or invincible in the other direction. If we keep the formula, player/wall defense must grow on the **same geometric curve** as `g_atk`; otherwise switch to % reduction (`damage × 100/(100+DEF)`). 🔶 decision needed before implementing defensive equipment.
2. **Speed** must never scale uncapped (an uncrossable lane is frustration, not difficulty).
3. Everything must be **data-driven**: `DT_MonsterScaling` DataTable (HP/ATK/speed/count curves per wave) + role multipliers, never hardcoded constants — balancing will iterate through this file and the DataTable.

## 6. Next steps

1. Set the base weapon stats (→ real `P_0`) 🔶
2. Implement scaling at spawn: `Stats = Base × Curve(wave) × RoleMultiplier` (DataTable + wave spawner).
3. Playtest: verify the 2–6 s TTK zone over the first 10 waves, adjust `g_hp`.
