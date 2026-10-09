# WoW Compagnon

Le fork utilise un seul module optionnel, `modules/mod-wow-compagnon`, qui réunit
Playerbots et PBC avec leurs historiques. Le cœur demeure compilable sans module.

Le même module est compilé avec le cœur officiel AzerothCore 7b2cecef92b271a468e39d89831b520b20ae06a8
sans modification et avec ce fork. Les deux cœurs sont aussi construits sans le
module. Aucune API privée du cœur n'est requise.

Les configurations AiPlayerbot.*, Playerbots.* et PBC.*, les tables et les formats
de souvenirs restent compatibles. Les fichiers privés d'exploitation sont placés
dans env/dist/data/wow-compagnon ; ils ne sont pas versionnés.

Installer uniquement le nouveau module. Les anciens mod-playerbots et mod-pbc
ne doivent pas participer à la même construction. Leurs dépôts GitHub restent
des archives de provenance ; aucun suivi de leurs dépôts natifs n'est effectué.

Maintenance : intégrer les évolutions officielles AzerothCore, adapter le module
fusionné, valider sur cœur officiel et fork. La compilation et les tests autonomes
ne remplacent pas l'essai en jeu des dialogues, achats et combats.

Source du module : https://github.com/damienselarez-creator/mod-wow-compagnon
