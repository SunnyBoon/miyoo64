# Paper Mario PAL — 0.1-pal-alpha

Unofficial native Paper Mario PAL port for Miyoo Mini Plus / OnionOS. Pre-release. No ROM or extracted game assets are included.

## Installation (Deutsch)

1. `PaperMarioPAL-0.1-pal-alpha-OnionOS.zip` herunterladen. SHA-256: `763c582d37bff03363a14b2d04fe0f3726f44354f3e1098bef6c83302ca4f794`. Mit SHA256SUMS abgleichen.
2. Auf die Wurzel der SD-Karte entpacken: `App/PaperMarioPAL/pm64`, `recipes.bin`, `launch.sh`, `config.json`, `icon.png`, `full_resolution`. Keine zusätzlichen Verzeichnisebenen. Vorhandene Test-Apps nicht überschreiben.
3. Eigene **Paper Mario (Europe, PAL)**-ROM als `App/PaperMarioPAL/rom.z64` bereitstellen. Kanonisches .z64, 67.108.864 Bytes, SHA-1 `2111d39265a317414d359e35a7d971c4dfa5f9e1`. Die App prüft die ROM-Identität. Die ROM ist kein Bestandteil des Downloads.
4. `launch.sh` und `pm64` müssen ausführbar sein (0755), andere Dateien 0644. In Onion unter Apps **Paper Mario PAL 0.1-pal-alpha** starten. Der erste Start rekonstruiert die Daten aus der eigenen ROM; abwarten und ROM auf der Karte lassen.
5. Eigener Laufzeitordner: `App/PaperMarioPAL/data`. Keine alten Caches oder Save-States hineinkopieren. Kein gemeinsamer Save-/Cachepfad mit bisherigen Test-Apps. Keine Save-State-Migration zwischen Builds. Erst Stabilität prüfen, dann neue Testspielstände speichern/laden.

## Installation (English)

1. Download `PaperMarioPAL-0.1-pal-alpha-OnionOS.zip` and verify its SHA-256 against SHA256SUMS.
2. Extract at the SD-card root. The executable must be `App/PaperMarioPAL/pm64`, beside the other five app files. Keep existing test apps intact.
3. Supply your own **Paper Mario (Europe, PAL)** dump as `App/PaperMarioPAL/rom.z64`: canonical big-endian .z64, 67,108,864 bytes, SHA-1 `2111d39265a317414d359e35a7d971c4dfa5f9e1`. The app checks ROM identity. No ROM is included.
4. Keep `pm64` and `launch.sh` executable (0755), other files 0644. Start **Paper Mario PAL 0.1-pal-alpha** in Onion Apps. Let the initial asset reconstruction finish and keep the ROM on the card.
5. Runtime data stays in `App/PaperMarioPAL/data`. Do not copy old caches or save states into it. Save-state migration between builds is unsupported. Test stability before saving/loading new test progress.

## Installation (Français)

1. Téléchargez `PaperMarioPAL-0.1-pal-alpha-OnionOS.zip` et vérifiez sa somme SHA-256 avec SHA256SUMS.
2. Extrayez le ZIP à la racine de la carte SD : `App/PaperMarioPAL/pm64` et les cinq autres fichiers doivent être côte à côte. Ne remplacez pas les applications de test existantes.
3. Fournissez votre propre ROM **Paper Mario (Europe, PAL)** sous `App/PaperMarioPAL/rom.z64` : format .z64 big-endian canonique, 67 108 864 octets, SHA-1 `2111d39265a317414d359e35a7d971c4dfa5f9e1`. L'application vérifie l'identité de la ROM. Aucune ROM n'est fournie.
4. Conservez les droits exécutables de `pm64` et `launch.sh` (0755), les autres fichiers en 0644. Lancez **Paper Mario PAL 0.1-pal-alpha** depuis les applications Onion. Attendez la reconstruction initiale des données et laissez la ROM sur la carte.
5. Les données restent dans `App/PaperMarioPAL/data`. N'y copiez ni anciens caches ni anciens états de sauvegarde. La migration des états entre versions n'est pas prise en charge. Vérifiez la stabilité avant de sauvegarder/charger une nouvelle partie de test.

## Instalación (Español)

1. Descargue `PaperMarioPAL-0.1-pal-alpha-OnionOS.zip` y compruebe su SHA-256 con SHA256SUMS.
2. Extraiga el ZIP en la raíz de la tarjeta SD: `App/PaperMarioPAL/pm64` debe quedar junto a los otros cinco archivos. No sobrescriba las aplicaciones de prueba existentes.
3. Proporcione su propia ROM **Paper Mario (Europe, PAL)** como `App/PaperMarioPAL/rom.z64`: formato .z64 big-endian canónico, 67.108.864 bytes, SHA-1 `2111d39265a317414d359e35a7d971c4dfa5f9e1`. La aplicación comprueba la identidad de la ROM. No se incluye ninguna ROM.
4. Mantenga `pm64` y `launch.sh` ejecutables (0755), y los demás archivos con permisos 0644. Abra **Paper Mario PAL 0.1-pal-alpha** desde las aplicaciones de Onion. Espere a que termine la reconstrucción inicial de los datos y conserve la ROM en la tarjeta.
5. Los datos se guardan en `App/PaperMarioPAL/data`. No copie allí cachés ni estados de guardado antiguos. No se admite migrar estados entre versiones. Compruebe la estabilidad antes de guardar/cargar una nueva partida de prueba.

## Status

The four PAL languages are **English, German, French, Spanish**. See TEST-STATUS.md for the exact evidence and outstanding final-folder test. Confirmed gameplay coverage ends at the start of Chapter 1, not the full game. Known graphical issues remain. No affiliation with Nintendo, Miyoo, OnionOS or the upstream maintainers is implied.

This preparation has **not been published**. The final end-user ZIP still needs a separate manual hardware smoke test. Licensing/provenance review items in LICENSE-AND-PROVENANCE.md must also be resolved before publication.

# Known issues / Bekannte Probleme

- Segment seams on some large trees / Gerade Segmentnähte an einigen großen Bäumen.
- Incomplete frame in Goombario's enemy inspection view / Unvollständiger Rahmen in Gumbarios Gegneransicht.
- The Star Piece incorrectly blinks even though it should remain continuously visible / Der Sternensplitter blinkt fälschlicherweise, obwohl er statisch sichtbar bleiben sollte.
- Other small graphical issues may exist. This alpha adds no renderer fixes.
- Confirmed hardware gameplay coverage ends at the start of Chapter 1. Later chapters, later boss scenes and the repeated-use behavior of all special abilities have not been exhaustively validated.
- Save-state compatibility is tied to this build. Do not migrate snapshots from another build. No cross-build save-state migration is offered.
- Only Miyoo Mini Plus is confirmed in the supplied hardware-test evidence. Other Miyoo devices/OS versions are not established by that evidence.

# License and provenance review

Upstream project: https://github.com/mr-finster/miyoo64 . Its LICENSE applies MIT to its own port code only. That exact notice is supplied as LICENSE and in the app's existing config.json metadata, so the binary download carries the notice without adding unapproved ZIP members. Existing copyright/license notices remain in the source trees.

Decompilation: https://github.com/pmret/papermario at ee321921df2debdaae4c9c00426b6ea84f26adb5. The local pinned Git tree contains no LICENSE/LICENCE/COPYING/NOTICE file and its README does not grant a general permissive license. This audit does **not** relicense the decompiled game code as MIT. Publication rights for decompiled/derived components require explicit review; the absence of ROM bytes does not establish those rights.

The renderer's source retains its Irrlicht clipping attribution, Copyright (C) 2002-2006 Nikolaus Gebhardt/Alten Thomas. The official Irrlicht license page is https://irrlicht.sourceforge.io/?page_id=294 . Broader Fast3D/SM64-derived renderer/audio lineage is described by upstream; a blanket MIT grant for all inherited components is not established here. A source-only license in a different SM64 variant is not automatically treated as governing this Paper Mario binary. Resolve the actual applicable lineage/terms before publication. SDL/glibc remain dynamically linked system dependencies; their binaries are not bundled.

The release source preserves the existing six local PAL commits and upstream history. New integration commits use the user-supplied SunnyBoon identity. The historical gameplay fixes originate in upstream 69fa3b5 and 01494e6; they are omitted from the separate current-main PR because general overlay restoration supersedes them. No invented original-author or Signed-off-by claims are added.

Historical tracked recipe JSONs were audited: five in the historical release tree, six in current upstream, including the later Paper Mario Miyoo recipe. All inspected recipe kinds are numeric RAW/RELOCS/MIO0/UNPACK descriptions with zero LITERAL/data/bytes payload fields. Their Git blob IDs and introducing commits are recorded; they remain unchanged inherited upstream objects. No new PAL recipe JSON, assets.bin, ROM, save or cache is committed. The downloadable recipes.bin has zero literal bytes and passes the exact pair and leak gates.

License status: **PARTIAL / publication review required** for inherited decomp/renderer lineage. MIT notice preservation, retained source notices and technical data-leak gates pass; this is not blanket legal clearance.

## Source build prerequisites

Use the pinned pmret/papermario commit ee321921df2debdaae4c9c00426b6ea84f26adb5 and apply game/patches/decomp.patch. Do not version the materialized refs tree. The historical build used the offline pm64hh:decomp image sha256:0c42a871ebcd90da2a7cde0ce981fd02dbd53683f9ff771c86af7b21ad1b0289 (extends the validated ARM build image and supplies PyYAML/Splat).

Mount paper-mario at /work and the user's canonical PAL ROM read-only at /work/refs/papermario/ver/pal/baserom.z64. Before ./configure pal, run `python3 -B tools/port/prepare_splat_provider.py`. It checks the installed splat64 0.31.0 provider and reproduces the exact validated Makefile adapter; it never downloads packages or replaces a different provider. Then run ./configure pal in refs/papermario, and its generated_code_pal / inc_img_bins_pal Ninja targets. Only after that run `python3 -B tools/port/pal_build.py --rom refs/papermario/ver/pal/baserom.z64` from /work.

The build forces game-object recompilation, emits a new nonzero buildstamp, and derives fresh recipes into ignored build/game/pal. No inherited US recipe or runtime cache is reused. Package with miyoo/game-pal.json, run pal_release_gates.py and perform an independent hardware test. The provided historical alpha ZIP is a repackaging of the validated binary, not a fresh build from the new release metadata. Newly built binaries are not claimed to have its exact checksum or hardware validation.
