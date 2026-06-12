# Propositions à valider — éléments auxquels penser

> v1.0 — 12 juin 2026. Liste proposée par Claude, **à valider par Pierre** (annote chaque ligne : garde / vire / plus tard).
> Une fois tranchées, les propositions retenues seront intégrées au GDD et à l'architecture, et ce document nettoyé.

## A. Indispensables (je recommande fortement de tout garder)

1. **Menu pause** — pause en partie : reprendre / options / abandonner le stage. On ne l'a jamais spécifié ! Règle à trancher : abandonner = défaite (loot conservé, cohérent avec « perdre fait progresser »).
2. **Écran de résumé de fin de stage** — victoire OU défaite : étoiles animées, loot détaillé (× multiplicateur), progression d'XP, premier kill de nouveaux monstres (« découverte ! »). C'est l'écran de dopamine du jeu, il mérite d'exister en tant que tel.
3. **Télégraphes d'attaque** — indicateurs au sol avant les attaques dangereuses (tirs des tireurs, zones de boss, lave du Colosse). En top-down c'est LA convention de lisibilité ; sans ça, la difficulté paraît injuste.
4. **Alerte « mur en danger »** — signal visuel/sonore directionnel quand le mur prend des dégâts hors écran ou qu'un kamikaze approche. Le joueur regarde sa cible, pas son mur.
5. **Barres de vie des monstres** — avec l'icône de faiblesse affichée une fois découverte (lien encyclopédie). Boss : grande barre dédiée en haut d'écran avec phases.
6. **Accessibilité daltonisme** — nos 7 éléments reposent sur la couleur ! Doubler chaque couleur d'une **forme/icône** (la signalétique devient couleur + symbole). Presque gratuit si prévu maintenant, très cher après.
7. **Remapping des touches** — attendu sur PC, natif avec Enhanced Input si prévu dès le départ. + option intensité/désactivation du camera shake (déjà côté GameFeel).
8. **Auto-pause** — manette déconnectée ou fenêtre qui perd le focus → pause. Standard, trivial, évite des défaites injustes.

## B. Forte valeur, coût raisonnable

9. **Aperçu de la vague suivante** — bandeau « Vague 3/6 » + icônes des types qui arrivent (synergie : préparer le switch, lisibilité du danger). Variante : icônes `?????` pour les monstres jamais rencontrés.
10. **Intro de boss** — bandeau nom + court zoom caméra à l'arrivée du boss de zone. Très « arcade », vend le moment fort de chaque zone pour pas cher.
11. **Musique dynamique** — 2–3 couches d'intensité (vague calme / assaut / mur < 33 %). MetaSounds rend ça simple ; énorme effet sur la tension.
12. **Succès Steam** — fonctionnent hors ligne, gratuits côté plateforme, et nos jalons existent déjà (boss, étoiles, niveaux 100, record infini). Bonne visibilité boutique.
13. **Cloud save Steam** — config quasi nulle (Steam Auto-Cloud sur le dossier de sauvegarde), évite la perte des 5 slots.
14. **Statistiques** — kills par monstre, dégâts par arme, etc. Alimente l'encyclopédie (« 12 tués ») et les succès. Se résume à des compteurs dans la Progression.
15. **Confirmations et achats multiples** — confirmation des grosses dépenses + bouton « améliorer ×5/×10 » (à 100 niveaux par arme, cliquer 100 fois est une torture).

## C. À débattre (utiles mais pas sûrs pour CE jeu)

16. **Préréglage graphique auto au premier lancement** — benchmark rapide → qualité suggérée. Sympa, mais UE fait un défaut correct ; à voir.
17. **Compteur de série (kill streak)** — combo de kills sans que le mur soit touché → petit bonus d'or. Ajoute du « flow », mais ajoute aussi du bruit à l'écran ; à prototyper.
18. **Vitesse ×2 en re-clear** — accélérer le temps quand on refarme un stage déjà 3-étoilé. Confort de farm énorme… mais risque de cannibaliser le « vrai » jeu ; à trancher selon les playtests.
19. **Mode photo** — pour le marketing/communauté. Coût non nul, valeur marketing réelle mais tardive. Plutôt post-launch.
20. **Replay du tutoriel** — revoir le guide depuis l'encyclopédie. Petit, mais utile après une pause de jeu.

## D. Je déconseille (pour cadrer le scope)

- **Minimap** — le terrain tient à l'écran en top-down, inutile.
- **New Game+ au-delà d'Enfer**, classements locaux, mode coop — hors scope du « petit jeu complet ».
