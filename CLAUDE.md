# WallDefense — Project Instructions

## Repository layout
- `WallDefense/` — the Unreal Engine 5.7 C++ project (code in `Source/`, assets in `Content/`).
- `Production/` — everything game-related outside the engine: game design, art sources, audio, documents, references.

## Documentation language rule (Production/ only)
Pierre is the only developer on the project (solo since June 2026). All Markdown documents in `Production/` are written in **French only** — no `_en`/`_fr` duplicates. Technical files elsewhere (CLAUDE.md, code comments, .vscode, etc.) stay in English.

## Build
- Engine: `C:\Unreal Engine\UE_5.7`
- Build editor target: `& "C:\Unreal Engine\UE_5.7\Engine\Build\BatchFiles\Build.bat" WallDefenseEditor Win64 Development -project="c:\GitHub\WallDefense\WallDefense\WallDefense.uproject" -waitmutex` (VS Code: `Ctrl+Shift+B`)
- External builds of the Editor target FAIL while the Unreal editor is open (Live Coding). Ask to close the editor or use Ctrl+Alt+F11 inside it.

## Key documents (read before designing or coding)
- `Production/01_GameDesign/GDD_WallDefense.md` — THE game design reference (update it when adding mechanics).
- `Production/04_Documents/Technical/ArchitectureTechnique.md` — THE technical reference: classes, components, DataAssets, UI binding, socket conventions, 7-step implementation plan (§12).
- `Production/04_Documents/Technical/Avancement.html` — presentable progress report (French, self-contained HTML). UPDATE IT at every delivered step: roadmap status, implemented classes, "how to test in Unreal" guide, automated test results.
- Pierre gives feedback by writing `## BAK:` comments inside documents — read them, integrate them, then remove them.

## Implementation status (update this section as steps complete)
- **Step 1/7 DONE** (foundations): `Core/WDTypes.h` (EWDElement, FWDDamageEvent, FWDElementalProfile, element colors), `Combat/WDHealthComponent` (elemental weakness ×1.75 / resistance ×0.5, percent defense, shield layer, events), `Core/WDDebugSubsystem` (element-colored debug draw, CVar `wd.Debug.Draw`).
- **Step 2/7 DONE** (top-down hero): `Player/WDHeroMath.h` (input→world math, screen up = world +X), `Player/WDHeroCharacter` (twin-stick: aim-driven rotation, Zelda-like camera onboard, anim state getters), `Player/WDHeroController` (Enhanced Input built at runtime in C++ — WASD+ZQSD/mouse aim via ground-plane intersection, gamepad sticks; placeholder fire/switch draw debug), `Player/WDHeroAnimInstance`, `GameModes/WDSandboxGameMode` (test: GameMode Override in any map with a floor).
- 14 automated tests passing (`WallDefense.Health.*` + `WallDefense.Hero.*`).
- **Next: Step 3** — weapons debug-first: `DA_Weapon`, `WeaponComponent`, projectiles with data-driven behaviors (pierce, bounce, fragments, homing, area) + pool.
- Legacy lane-based code (`LaneGrid`, `LanePlayerCharacter`, old `HealthComponent`, `BaseMonster`) is still in place; it gets replaced/migrated as steps 2–4 land.

## Code conventions
- New classes use the `WD` prefix (`UWDHealthComponent`, `AWDHeroCharacter`...). Code identifiers in English; UI text via `FText` only.
- Debug-first: gameplay must be testable without any art assets (debug draw through `UWDDebugSubsystem`).
- Components are independent (no component includes another component's header); communication via delegates; data via DataAssets (`TSoftObjectPtr`). See ArchitectureTechnique §1 and §10.
- Every step ships with automation tests. Run headless:
  `& "C:\Unreal Engine\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "c:\GitHub\WallDefense\WallDefense\WallDefense.uproject" -ExecCmds="Automation RunTests WallDefense; Quit" -nullrhi -unattended -nopause`
- Asset naming: see `WallDefense/ReadeMe.txt` (BP_, SM_, M_, T_, WBP_, DA_, DT_...).
