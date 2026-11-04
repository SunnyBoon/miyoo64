# Paper Mario PAL v0.1-pal-alpha — Installation (Français)

Pour **Miyoo Mini Plus avec OnionOS**. Ce port natif ARM est prêt à installer : **aucune compilation n'est nécessaire**. Vous devez posséder votre propre copie européenne PAL non modifiée de Paper Mario. Aucune ROM ni ressource extraite du jeu n'est incluse ; aucun téléchargement de ROM n'est proposé.

## Télécharger et installer

1. Sur la [page des versions de SunnyBoon/miyoo64](https://github.com/SunnyBoon/miyoo64/releases), choisissez **v0.1-pal-alpha** et téléchargez **PaperMarioPAL-0.1-pal-alpha-OnionOS.zip**. Vérifiez sa somme SHA-256 avec le fichier SHA256SUMS fourni : `763c582d37bff03363a14b2d04fe0f3726f44354f3e1098bef6c83302ca4f794`. Téléchargez aussi `LICENSE`, `NOTICE.md` et `THIRD-PARTY-LICENSES.txt` et conservez-les avec le ZIP.
2. Extrayez le contenu du ZIP à la **racine de la carte SD**, en conservant `App/PaperMarioPAL/`. Les six fichiers doivent être côte à côte : `pm64`, `recipes.bin`, `launch.sh`, `config.json`, `icon.png`, `full_resolution`. Évitez un dossier supplémentaire autour de cette structure. Si `App/PaperMarioPAL/` existe déjà, sauvegardez et vérifiez l'installation existante avant de remplacer quoi que ce soit.
3. Placez votre propre ROM **Paper Mario (Europe, PAL)** sous **`App/PaperMarioPAL/rom.z64`**. Il faut un fichier **Z64 big-endian de 64 Mio (64 MiB, 67 108 864 octets)** avec la somme SHA-1 **`2111d39265a317414d359e35a7d971c4dfa5f9e1`**. Les versions américaine/japonaise, les fichiers modifiés ou un ordre d'octets incorrect ne conviennent pas. Renommer un fichier n'en convertit pas le contenu.
4. Remettez la carte SD dans le Miyoo Mini Plus, ouvrez la liste des applications d'OnionOS et lancez **Paper Mario PAL 0.1-pal-alpha**.
5. Au premier démarrage, l'application vérifie la ROM et crée les données nécessaires localement dans **`App/PaperMarioPAL/data/`**. Attendez la fin de cette opération ; aucune extraction manuelle des ressources ni compilation n'est nécessaire. Laissez la ROM sur la carte pour les lectures pendant le jeu. N'y copiez aucun ancien cache ou état de sauvegarde.
6. Dans le choix de la langue du jeu, sélectionnez **English, German, French ou Spanish** (anglais, allemand, français ou espagnol). Suivez les indications affichées par le jeu. Utilisez les sauvegardes normales de cette version et de nouveaux états de sauvegarde ; la migration des états entre versions n'est pas prise en charge.

## Dépannage

- **Application absente ou impossible à lancer :** vérifiez l'arborescence exacte et les six fichiers. `pm64` et `launch.sh` doivent rester exécutables ; le ZIP indique 0755 pour eux et 0644 pour les autres fichiers. Conservez ces droits si votre outil d'extraction le demande. Utilisez le ZIP complet et son contenu correspondant.
- **ROM introuvable ou refusée :** vérifiez l'emplacement exact de `rom.z64`, sa taille, son format big-endian et la somme SHA-1 ci-dessus. Évitez une double extension. Utilisez votre propre copie correspondante ; aucun lien de téléchargement n'est fourni ici.
- **Premier lancement plus long :** laissez la création locale des données se terminer. La carte SD doit être accessible en écriture et disposer d'espace libre. N'interrompez pas une écriture et n'ajoutez pas de cache d'une autre version.
- **Échec de sauvegarde/chargement, son absent ou problème persistant au démarrage :** confirmez l'appareil/OnionOS, vérifiez le ZIP et conservez ensemble les fichiers de programme et de recettes d'origine. Protégez vos sauvegardes. Notez le message visible et les étapes pour demander de l'aide ; ne remplacez pas les fichiers par ceux d'une autre version et ne partagez ni ROM, ni sauvegardes, ni données privées.

## Tests et problèmes connus

Le **ZIP exact destiné aux utilisateurs a réussi 5/5 vérifications matérielles manuelles rapportées par l'utilisateur** : démarrage depuis ce dossier, quatre langues, menu principal/nouvelle partie, introduction/premier combat, sauvegarde/chargement normal et états de sauvegarde/chargement. Sa base M14.39b a également été jouée de manière stable pendant le prologue jusqu'au début du chapitre 1. Il s'agit d'une **alpha**, pas d'une validation du jeu entier ; les scènes ultérieures et toutes les capacités spéciales n'ont pas été testées de façon exhaustive. Seuls l'appareil et le système indiqués sont couverts par ces résultats.

Problèmes graphiques connus, non corrigés :

- Sur certains grands arbres, de fines jointures droites entre les segments laissent apparaître l'arrière-plan.
- Le cadre de la représentation agrandie d'un ennemi par Goombario est incomplet : deux côtés au lieu de quatre.
- Un morceau d'étoile clignote à tort alors qu'il devrait rester visible de façon statique ; son ombre reste visible pendant sa disparition.

La [PR amont nº 7](https://github.com/mr-finster/miyoo64/pull/7), distincte, a sa propre validation matérielle **8/8** et d'autres sources, programme et recettes. Elle ne remplace pas le résultat **5/5** de ce ZIP. Ne mélangez pas les installations, états ou caches. Consultez KNOWN-ISSUES.md et NOTICE.md.
