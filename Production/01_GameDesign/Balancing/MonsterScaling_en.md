# Monster scaling — Per-stage balancing (MVP)

> v0.2 — June 9, 2026. Twin document: `MonsterScaling_fr.md`.
> Prerequisite: `../Mechanics/Progression_en.md` (stage → loot → upgrade → retry loop).

---

## 1. The model: fixed stages, a rising player

With the "retry the stage after upgrading" loop, balancing changes nature:
- **Monster stats are FIXED per stage** (stage 4 is always stage 4). The player is what moves.
- Within a stage, successive waves increase **count/density**, not stats — readability stays constant during a run.
- The "difficulty wall" is natural: the player fails stage N, farms (earlier stages or their attempts), upgrades, comes back. **The sawtooth IS the defeat → upgrade → victory rhythm.**

The felt variable is still **TTK** (time-to-kill): `TTK = monster_HP(stage) / player_DPS(upgrades)`.

## 2. Per-stage curves 🔶

```
HP_m(s)  = 100 × 1.32^(s−1)      (grunt, stage s)
ATK_m(s) = 10  × 1.22^(s−1)
Speed    = constant per type (speed does NOT scale — it's part of a monster's identity)
```

| Stage | Grunt HP | Grunt ATK | "Expected" player DPS | TTK (right weapon) |
|---|---|---|---|---|
| 1 | 100 | 10 | 40 | 2.5 s |
| 3 | 174 | 15 | 62 | 2.8 s |
| 5 | 304 | 22 | 97 | 3.1 s |
| 7 | 529 | 33 | 152 | 3.5 s |
| 10 | 1,217 | 60 | 305 | 4.0 s |

- **TTK target zone**: 2–5 s for a grunt with the right weapon (×1.5–×2 with the wrong one — that's the switch's punishment/reward), 30–60 s for a boss.
- The "expected" player DPS comes from upgrades (§3): if the player arrives under the curve, the stage pushes them back → farm → return stronger.

### Role multipliers (on top of the stage base)

| Role | HP | ATK | Speed | Note |
|---|---|---|---|---|
| Grunt | ×1 | ×1 | ×1 | reference |
| Fast | ×0.5 | ×0.8 | ×1.8 | breakthrough threat |
| Tank | ×5 | ×0.8 | ×0.55 | armor 🔶: resists everything but the heavy cannon |
| Shooter | ×0.8 | ×1.2 | ×0.9 | ranged attacks |
| Boss (stage end) | ×15 | ×2.5 | ×0.7 | guaranteed materials |

### Waves within a stage
- One stage = **5 waves + 1 boss wave** 🔶.
- Count: `N(wave) = 4 + wave + ⌊stage/2⌋`, capped by lane capacity.
- Composition introduces roles progressively (stage 1: grunts only; fast from stage 2; tank at 3; shooter at 4).

## 3. The player curve (upgrade side)

MVP DPS sources (see Progression §2.2 and §4):
- Weapon level: **+10% base damage per level** (~20 levels) → up to ×3 per weapon.
- Weapon milestones (5/10/15/20): qualitative bonuses ≈ **×1.5 cumulative**.
- Character global damage: +2%/level × 20 → **×1.4**.
- **Right weapon vs right monster: ×1.5–×2 effective** (free — that's skill).

Total available ≈ ×6–9 over the MVP: consistent with the HP curve (×12 at stage 10) **provided** the player uses weapon switching — intended: stats alone aren't quite enough.

### Upgrade costs (to tune with loot)
```
Weapon_cost(level n) = base × 1.18^n   (materials + gold)
```
Pacing target 🔶: beating stage N for the first time ≈ **2–3 runs** (1 instructive defeat + farm + win). If playtests show > 4 runs → lower costs or raise loot, never the sneaky opposite.

## 4. Scaling must not be stats only

- Each **new monster role** appears at a precise stage (§2): difficulty renews through composition, not just numbers.
- Visual variety: monster recolors every ~3 stages 🔶.

## 5. Known traps / current code constraints

1. ⚠️ **Flat-subtraction defense** (`actual damage = incoming − defense`, `HealthComponent`): with ATK ×1.22 per stage, a flat wall defense quickly becomes worthless or blocks everything. MVP recommendation: **wall has no defense stat, HP only** (simple, readable); the tank's "armor" = % reduction against the wrong weapons. 🔶
2. **Speed never scales**: a stage-10 fast monster is as fast as a stage-2 one — its count and context make it dangerous.
3. Everything in **DataTables** (`DT_StageScaling`: HP/ATK/count per stage; `DT_MonsterRoles`: multipliers) — balancing iterates here + in the table, never hardcoded.

## 6. Next steps

1. Set the 3 weapons' stats (→ reference DPS `P_0 = 40` to validate) 🔶
2. Implement: wave spawner reading `DT_StageScaling`, stats applied at spawn.
3. Loot: drop table per monster role (gold + typed material).
4. Playtest stages 1–3: verify 2–5 s TTK and the "2–3 runs per stage" pacing.
