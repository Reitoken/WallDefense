# WallDefense — Project Instructions

## Repository layout
- `WallDefense/` — the Unreal Engine 5.7 C++ project (code in `Source/`, assets in `Content/`).
- `Production/` — everything game-related outside the engine: game design, art sources, audio, documents, references.

## Bilingual documentation rule (Production/ only)
Every Markdown document in `Production/` MUST exist in two versions, kept in sync:
- `<Name>_fr.md` — French version (for Pierre)
- `<Name>_en.md` — English version (to share with the team)

Rules:
- When creating a `.md` file in `Production/`, always create both `_fr` and `_en` versions with the same content translated.
- When editing one version, ALWAYS apply the same change to the other version in the same commit.
- This applies ONLY to `Production/` — technical files elsewhere (CLAUDE.md, code comments, .vscode, etc.) stay single-language.

## Build
- Engine: `C:\Unreal Engine\UE_5.7`
- Build editor target: `& "C:\Unreal Engine\UE_5.7\Engine\Build\BatchFiles\Build.bat" WallDefenseEditor Win64 Development -project="c:\GitHub\WallDefense\WallDefense\WallDefense.uproject" -waitmutex` (VS Code: `Ctrl+Shift+B`)
- External builds of the Editor target FAIL while the Unreal editor is open (Live Coding). Ask to close the editor or use Ctrl+Alt+F11 inside it.

## Conventions
- Asset naming: see `WallDefense/ReadeMe.txt` (BP_, SM_, M_, T_, WBP_...).
- Gameplay rules live in `Production/01_GameDesign/GDD_WallDefense_fr.md` / `_en.md` — update the GDD when adding mechanics.
