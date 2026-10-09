# Assemblage WoW Compagnon — 9 octobre 2026

Cette révision remplace les références techniques du document du 1er octobre.
Les preuves d'activation sont conservées séparément : un commit et une compilation
ne prouvent pas qu'un binaire est en cours d'exécution.

| Élément | Révision épinglée |
| --- | --- |
| AzerothCore officiel intégré | `7b2cecef92b271a468e39d89831b520b20ae06a8` |
| Fork personnel Playerbots | `be691a93009678e950d512aeba89aae46894ce46` |
| Fork personnel PBC | `5a4955de02deca3f3e7b16a4e6798fe92ad77c29` |

Utiliser `git submodule update --init --recursive` sans `--remote`. Les gitlinks
du cœur identifient l'assemblage ; les branches principales des modules pourront
évoluer ensuite. Ne plus synchroniser les dépôts natifs Playerbots/PBC.

Le cœur conserve `LootTemplate::HasNonQuestItem`, recherche en lecture seule des
sources de matériaux, et l'historique officiel de ses auteurs. Les 85 commits
officiels depuis la précédente base ont été fusionnés sans réécriture.

Les modules conservent les adaptations de compatibilité, le mode compagnons du
même compte, les garde-fous de population, la récupération après la mort, les
vocations naturelles, les talents cohérents, la formation avec vérification
différée, les raffinements de combat et les instantanés factuels de la V3.
PBC contient 93 compositions et 842 passages collectifs, la mémoire d'aventures,
les réactions aux quêtes et les lecteurs documentaires. Il ne contient aucun
nouveau corpus biographique individuel dans cette synchronisation.

Les paramètres d'exploitation restent privés. Le serveur utilisateur conserve
notamment la limite de quatre compagnons, le raffinement de combat activé, les
taux de quêtes 60/70/80 et la politique de parole décidée précédemment. Les valeurs
par défaut distribuées ne sont pas nécessairement ces réglages d'exploitation.

## Validation reproductible

Compiler hors des sources avec CMake, C++20, scripts et modules statiques.
Exécuter les dix projets CMake autonomes de `modules/mod-pbc/tests/` : selfbot,
condensation, adventure, lore, history, mutations, quest_reactions, recovery,
foundation et archetype. Ils utilisent des copies temporaires et aucun modèle
payant. Les treize suites ciblées V3 ont aussi été rejouées dans le stage privé
d'intervention. Leurs preuves sont conservées dans les livrables du projet.

L'activation comprend une sauvegarde complète de la base monde avant les
migrations natives et une comparaison des empreintes des traductions de quêtes
frFR. Les configurations, choix de vocation et corpus collectif sont vérifiés.
Les fiches individuelles, dialogues, souvenirs et journaux ne sont pas publiés.

La compilation et ces validations n'attestent pas de nouveaux achats réels, ni
d'une mesure de charge ou de la qualité en jeu d'un groupe de quatre compagnons.
La proposition de module unique est détaillée dans `WOW_COMPAGNON.md`.
