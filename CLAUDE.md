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

## Conventions
- Asset naming: see `WallDefense/ReadeMe.txt` (BP_, SM_, M_, T_, WBP_...).
- Gameplay rules live in `Production/01_GameDesign/GDD_WallDefense.md` — update the GDD when adding mechanics.
