# Production — WallDefense

Tout ce qui est lié au jeu **en dehors** du projet Unreal (`/WallDefense`) vit ici :
game design, sources d'assets, documents, références.

## Organisation

| Dossier | Contenu |
|---|---|
| `01_GameDesign/` | GDD, mécaniques de jeu, équilibrage, level design |
| `02_Art/` | Concepts, références visuelles, fichiers sources 2D/3D (.psd, .blend...), UI |
| `03_Audio/` | Musiques, effets sonores et leurs fichiers sources |
| `04_Documents/` | Documentation technique, roadmap, notes de production |
| `05_References/` | Inspirations, captures d'écran, liens utiles |

## Règles

- Les assets **importés dans le jeu** (.uasset) vont dans `WallDefense/Content/` — ici on garde uniquement les **fichiers sources** (modifiables).
- Les fichiers lourds (images, 3D, audio, archives) passent automatiquement par **Git LFS** (voir `.gitattributes` à la racine).
- Convention de nommage des assets du jeu : voir `WallDefense/ReadeMe.txt`.
