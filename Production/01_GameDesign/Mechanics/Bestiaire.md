# Bestiaire — 6 zones, 30 monstres + 6 boss

> v0.1 — 12 juin 2026. Référencé par le GDD §6.3. Multiplicateurs appliqués sur la base du stage (`../Balancing/MonsterScaling.md`).
> **Bestiaire abstrait** ✅ : IDs (`M101`…), stats, patterns et skills sont définitifs en gameplay ; les designs et noms seront remplacés plus tard (noms actuels = descriptions de travail). Chaque fiche = une ligne de la future DataTable `DT_Monsters`.

---

## 0. Règles transverses

- **Introduction** : 2 types au stage 1 de la zone, +1 nouveau par stage ; stage 5 = les 5 types + le boss.
- **Faiblesse dominante de zone** = l'arme débloquée juste avant d'y entrer → la nouvelle arme brille immédiatement. Zone 1 : faiblesses tournées vers Feu/Glace (futures armes) — re-clear rentable plus tard.
- **Alignement de zone** = l'élément des ressources droppées par ses monstres. (Ressources **Lumière** : droppées par les boss et le stage infini 🔶.)
- Vitesses indicatives 🔶 : lente ≈ 100 · moyenne ≈ 200 · rapide ≈ 350 uu/s. PV/Déf/Dégâts mur = multiplicateurs de la base du stage.
- Les **supports** (soin, bouclier, accélération…) agissent **en avançant** (pulsation) et **au contact du mur** (leur « attaque » est un buff allié). ✅

| Zone | Thème de travail | Faiblesse dominante | Alignement (drops) | Armes possédées en y entrant |
|---|---|---|---|---|
| 1 | Prairies | Feu / Glace (à venir) | varié (faible taux) | Normal |
| 2 | Forêt profonde | **Feu** | Vent | + Feu |
| 3 | Désert ardent | **Glace** | Feu | + Glace |
| 4 | Marais orageux | **Foudre** | Glace | + Foudre |
| 5 | Pics hurlants | **Vent** | Foudre | + Vent |
| 6 | Crypte des ombres | **Lumière** | Ténèbres | + Lumière |

---

## Zone 1 — Prairies (apprentissage)

Faune simple, lisible ; on apprend à viser, switcher viendra après. Peu de résistances.

| ID | Rôle | Pattern | Vitesse | PV | Déf | Dégâts mur | Faiblesse | Résist. | Particularité |
|---|---|---|---|---|---|---|---|---|---|
| M101 « Rongeur » | Standard | droite | moyenne | ×1 | faible | ×1 | Feu | — | le monstre de référence |
| M102 « Lièvre » | Rapide | zigzag | rapide | ×0,5 | faible | ×0,7 | Feu | — | force à décrocher la visée |
| M103 « Sanglier » | Tank | droite | lente | ×4 | moyenne | ×1,5 | Glace | — | premier « gros » |
| M104 « Cracheur » | Tireur | charge-pause | moyenne | ×0,8 | faible | ×1 (distance) | Feu | — | attaque le mur de loin |
| M105 « Apothicaire » | **Soigneur** | sinusoïdal | moyenne | ×0,7 | faible | — | Glace | — | soigne en pulsation ; premier « tue-le d'abord » |
| **B1 « Brute des prairies »** | Boss | droite | lente | ×15 | moyenne | ×3 | Feu | — | charge le mur ; invoque 2× M101 périodiquement |

## Zone 2 — Forêt profonde (boucliers)

Créatures sylvestres coriaces ; la zone enseigne **Feu** (brûlure > régénération) et introduit les boucliers.

| ID | Rôle | Pattern | Vitesse | PV | Déf | Dégâts mur | Faiblesse | Résist. | Particularité |
|---|---|---|---|---|---|---|---|---|---|
| M201 « Liane » | Standard | sinusoïdal | moyenne | ×1,1 | faible | ×1 | Feu | Vent | régénère lentement ses PV |
| M202 « Guêpe » | Rapide | **flanqueur** | rapide | ×0,5 | faible | ×0,7 | Feu | Vent | arrive par les bords |
| M203 « Écorce » | **Bouclier-porteur** | droite | lente | ×0,9 | moyenne | — | Feu | Glace | boucliers alliés en pulsation et au contact |
| M204 « Lanceur d'épines » | Tireur | charge-pause | moyenne | ×0,8 | faible | ×1 (distance) | Feu | Vent | — |
| M205 « Spore » | **Kamikaze** | droite | rapide | ×0,4 | faible | ×4 (explosion) | Feu | — | explose sur le mur — à intercepter |
| **B2 « Ancien sylvestre »** | Boss | droite | lente | ×15 | haute | ×2,5 | Feu | Glace, Vent | s'auto-bouclier par phases ; invoque des M205 |

## Zone 3 — Désert ardent (rythme)

Créatures de braise rapides ou enfouies ; la zone enseigne **Glace** (ralentir ce qui surgit).

| ID | Rôle | Pattern | Vitesse | PV | Déf | Dégâts mur | Faiblesse | Résist. | Particularité |
|---|---|---|---|---|---|---|---|---|---|
| M301 « Salamandre » | Standard | charge-pause | moyenne | ×1 | moyenne | ×1 | Glace | Feu | — |
| M302 « Scarabée » | Tank | droite | lente | ×5 | haute | ×1,5 | Glace | Feu, Vent | carapace : défense très haute |
| M303 « Vipère des sables » | **Fouisseur** | fouisseur | rapide | ×0,6 | faible | ×1 | Glace | Feu | disparaît/réapparaît plus près du mur |
| M304 « Chacal » | **Accélérateur** | sinusoïdal | moyenne | ×0,7 | faible | — | Glace | Feu | aura : +40 % vitesse des alliés proches ✅ |
| M305 « Mirage » | Tireur | spirale | moyenne | ×0,8 | faible | ×1 (distance) | Glace | Feu | — |
| **B3 « Colosse de braise »** | Boss | droite | lente | ×15 | haute | ×3 | Glace | Feu | pose des flaques de lave (zones interdites au joueur) ; enrage sous 25 % PV |

## Zone 4 — Marais orageux (résilience)

La zone enseigne **Foudre** (chaînes contre les groupes) et introduit les renaissances.

| ID | Rôle | Pattern | Vitesse | PV | Déf | Dégâts mur | Faiblesse | Résist. | Particularité |
|---|---|---|---|---|---|---|---|---|---|
| M401 « Limace » | Standard | spirale | lente | ×1,3 | moyenne | ×1,2 | Foudre | Glace | laisse une traînée gluante (ralentit le joueur 🔶) |
| M402 « Grenouille » | Rapide | **sauteur** | rapide | ×0,5 | faible | ×0,7 | Foudre | Glace | intouchable en l'air 🔶 |
| M403 « Chaman des vases » | **Ressusciteur** | sinusoïdal | moyenne | ×0,8 | moyenne | — | Foudre | Ténèbres | ranime 1 allié mort (1 fois chacun) ✅ farfelu |
| M404 « Golem de boue » | Tank | droite | lente | ×5 | moyenne | ×1,5 | Foudre | Glace, Feu | — |
| M405 « Gélatine » | **Diviseur** | droite | moyenne | ×1 | faible | ×0,8 | Foudre | Glace | se scinde en 2 petits (×0,3) à la mort |
| **B4 « Hydre des vases »** | Boss | sinusoïdal | lente | ×15 | moyenne | ×2,5 | Foudre | Glace, Feu | à 50 % PV se divise en 2 mini-hydres (×5) ; ranime des morts |

## Zone 5 — Pics hurlants (saturation)

Essaims et harceleurs ; la zone enseigne **Vent** (regrouper/repousser la masse).

| ID | Rôle | Pattern | Vitesse | PV | Déf | Dégâts mur | Faiblesse | Résist. | Particularité |
|---|---|---|---|---|---|---|---|---|---|
| M501 « Nuée » | **Essaim** | zigzag | rapide | ×0,2 (×8 unités) | faible | ×0,3 | Vent | Foudre | spawn en groupes de 8 |
| M502 « Condor » | Rapide | flanqueur | rapide | ×0,6 | faible | ×0,8 | Vent | Foudre | — |
| M503 « Pie voleuse » | **Voleur** | zigzag | rapide | ×0,5 | faible | — | Vent | — | **ramasse les drops au sol et les emporte** ✅ farfelu — le tuer les rend |
| M504 « Héraut » | **Porte-étendard** | droite | moyenne | ×1 | moyenne | — | Vent | Foudre | aura : +50 % dégâts au mur des alliés proches |
| M505 « Canonnier des cimes » | Tireur | charge-pause | moyenne | ×0,9 | moyenne | ×1,2 (distance) | Vent | Foudre | longue portée |
| **B5 « Roc des tempêtes »** | Boss | spirale | moyenne | ×15 | moyenne | ×2,5 (distance) | Vent | Foudre | bombarde le mur de loin ; invoque des nuées M501 en continu |

## Zone 6 — Crypte des ombres (maîtrise)

Tout ce que le jeu a appris, en plus dur ; la zone enseigne **Lumière** (briser les boucliers). Boucliers fréquents.

| ID | Rôle | Pattern | Vitesse | PV | Déf | Dégâts mur | Faiblesse | Résist. | Particularité |
|---|---|---|---|---|---|---|---|---|---|
| M601 « Spectre » | Standard | sinusoïdal | moyenne | ×1,2 | moyenne | ×1,2 | Lumière | Ténèbres | **bouclier** natif (×0,5 PV) |
| M602 « Ombre véloce » | Rapide | zigzag | très rapide | ×0,5 | faible | ×0,8 | Lumière | Ténèbres | brèves phases d'intangibilité 🔶 |
| M603 « Voile » | **Brumeux** | spirale | moyenne | ×0,8 | moyenne | — | Lumière | Ténèbres | voile les alliés proches : intouchables 1 s (pulsation) ✅ farfelu |
| M604 « Gardien du caveau » | Tank | droite | lente | ×6 | haute | ×1,5 | Lumière | Ténèbres, Feu | **bouclier** régénérant |
| M605 « Nécrophore » | **Soigneur++** | flanqueur | moyenne | ×0,8 | moyenne | — | Lumière | Ténèbres | soigne ET pose un petit bouclier |
| **B6 « Seigneur des ombres »** | Boss final | charge-pause | moyenne | ×18 | haute | ×3 | Lumière | Ténèbres, Glace, Feu | bouclier régénérant par phases ; voile ses alliés ; sous 30 % : pluie de projectiles |

---

## 7. Implémentation

- **`DT_Monsters`** : 1 ligne par fiche (rôle, pattern, vitesse, multiplicateurs PV/Déf/dégâts mur, faiblesse, résistances, bouclier, skill support, drops).
- **`DT_StageComposition`** : par stage → liste des IDs + quantités par vague (règle d'introduction §0).
- Skills support = composants réutilisables (`AuraHealComponent`, `AuraShieldComponent`, `AuraSpeedComponent`…) paramétrés par la fiche — un seul code pour toutes les zones.
- Les particularités 🔶 (intangibilité, vol de loot, traînée gluante) se prototypent en debug-first comme les armes (GDD §10).
- **Stage 31 (infini)** : puise dans TOUT le bestiaire, par tranches thématiques de vagues (rotation des zones), multiplicateurs de la courbe infinie.
