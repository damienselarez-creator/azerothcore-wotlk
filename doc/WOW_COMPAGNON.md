# WoW Compagnon — direction technique, 9 octobre 2026

L'ensemble vise un humain et jusqu'à quatre compagnons de son compte, invoqués
manuellement, sans population automatique. Les compagnons associent comportement
de jeu, progression, dialogue, culture et mémoire durable de l'aventure.

## Politique de maintenance

- Suivre uniquement le dépôt officiel AzerothCore pour les évolutions du cœur.
- Maintenir Playerbots et PBC dans les forks de damienselarez-creator. Ne plus
  récupérer les dépôts natifs de ces deux modules, ni leurs branches de staging.
- Conserver les historiques d'origine et l'attribution des auteurs.
- Épingler les modules par révision dans le cœur ; compiler l'ensemble exact.
- Exclure de Git les paramètres privés, clés, comptes, fiches individuelles,
  dialogues, souvenirs, journaux et fichiers de choix persistants.
- Conserver les traductions françaises, les choix de vocation et les souvenirs
  lors de chaque mise à jour. Sauvegarder avant migrations et activation.

## Organisation actuelle

Playerbots possède les actions de jeu : déplacement, combat, équipement,
progression et visite des maîtres. PBC possède les dialogues, les vocations,
les archétypes et la mémoire. La passerelle transmet des instantanés factuels,
avec durée de validité, sans conserver de pointeurs de joueurs.

L'assemblage peut déjà porter le nom WoW Compagnon. Cela ne transforme pas
automatiquement deux modules en un module unique : les dépendances et chemins
de persistance actuels restent réels.

## Fusion physique possible

Recommandation : publier d'abord une distribution cohérente et reproductible,
puis migrer vers `mod-wow-compagnon` dans une intervention distincte.

Le module unique pourrait contenir `gameplay`, `dialogue`, `memory`, `vocations`,
`culture` et une interface commune. Il devrait conserver l'apprentissage,
les rotations et les garde-fous hérités de Playerbots, et isoler l'appel au
modèle afin qu'une panne de dialogue n'empêche pas les actions de jeu.

La migration devra préserver les historiques Git des deux modules, remplacer
les fournisseurs et enregistrements de scripts sans double chargement, adapter
CMake et les inclusions, et accepter les anciens noms de configuration et
chemins de données. Les anciens choix et souvenirs devront rester lisibles.
Un seul mécanisme doit être propriétaire d'une action ou d'une synthèse.

Avant activation : compilation complète, suites autonomes, migration à blanc
des formats sur copies et essai utilisateur à un, deux et quatre compagnons.
Les améliorations artisanat/guilde restent différées selon les décisions du projet.

## Limites

Cette note propose l'architecture de fusion ; elle ne déclare aucune fusion
physique réalisée. La compilation et les tests autonomes ne certifient pas les
achats, les rotations et la coopération en jeu.
