# Production — WallDefense

Everything game-related that lives **outside** the Unreal project (`/WallDefense`):
game design, asset sources, documents, references.

## Organization

| Folder | Contents |
|---|---|
| `01_GameDesign/` | GDD, game mechanics, balancing, level design |
| `02_Art/` | Concepts, visual references, 2D/3D source files (.psd, .blend...), UI |
| `03_Audio/` | Music, sound effects and their source files |
| `04_Documents/` | Technical documentation, roadmap, production notes |
| `05_References/` | Inspirations, screenshots, useful links |

## Rules

- Assets **imported into the game** (.uasset) go to `WallDefense/Content/` — here we only keep **source files** (editable).
- Heavy files (images, 3D, audio, archives) automatically go through **Git LFS** (see `.gitattributes` at the repo root).
- Game asset naming convention: see `WallDefense/ReadeMe.txt`.
- Every Markdown document in this folder exists in two versions: `_fr` (French) and `_en` (English), kept in sync.
