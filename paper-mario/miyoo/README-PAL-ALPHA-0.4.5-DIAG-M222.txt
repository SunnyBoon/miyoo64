Paper Mario PAL Alpha 0.4.5 Diagnostic M2.2.2 (PAL-0.4.5-DIAG-M222)

This separate diagnostic app is for isolated bug reproduction. Extract this
ZIP at the root of the SD card. It creates App/PaperMarioPALAlpha045DiagM222/ and
uses its own data/ and traces/ folders; it does not share saves with the
standard Alpha 0.4 app or prior M1.7 installations.

Supply your own verified Paper Mario (Europe, PAL) ROM at:
  /mnt/SDCARD/Roms/N64/PaperMarioPAL.z64
Required SHA-1:
  2111d39265a317414d359e35a7d971c4dfa5f9e1

The handheld menu has one DIAGNOSTIC row and shows READY, ARMED, RECORDING,
BUSY, SAVED, or FAILED. Left/Right selects RENDER FOCUS, TIMELINE, or RAW
COMPAT; A arms the selected bounded capture. While TIMELINE is recording, a
second A requests a safe early stop. A rejected start keeps the menu open and
reports BUSY. Each capture
stores a frame.ppm, trace.log and metadata.json under traces/trace_XXXXXX/.
RENDER FOCUS prioritizes cache reset/import/reimport and SoftTex upload facts,
then records the cache fingerprint, imported and actually bound SoftTex sizes
with each TexRect geometry, combiner and tile sampler state. It keeps a tiny
reset/import history across the save-state reload boundary while coalescing
unrelated cache hits. A verified save-state load between two focus captures can
link them as an observed before/after group; otherwise grouping remains neutral.
TIMELINE records game/swap progress and Merlee phase hooks, then stops on a
script completion, record limit or timeout. A regular app exit finalizes an
active session as partial/app_exit and never claims a missing screenshot. RAW COMPAT keeps the bounded
historical display-list trace. These diagnostics are observational; they do
not prove a game scene is correct or a cause has been established.

Analyzing a reporter's pm64.sav:
1. Record the report's origin and build version. Archive the original unchanged
   and record its SHA-256, file size, and header.
2. Accept only a plausible 131072-byte regular Save-RAM file with the expected
   Paper Mario header. It is not an S6MP save-state and may not open directly
   at the reporter's scene.
3. Assess PAL Alpha 0.4.5 compatibility before copying only a working copy to
   this app's isolated data/ directory. Never put an untrusted file in a
   personal gameplay installation.
4. Reject malformed, modified, or incompatible files rather than attempting a
   repair. Reproduce with the normal load path, then preserve any trace, log,
   and screenshot evidence separately.

Alpha status: this diagnostic version requires a build and hardware acceptance
of the exact resulting binary. It contains no ROM, savefile, or
game asset data. See the standard package README for credits and license note.


M2.2.2 Scheduler-Diagnose
------------------------
Ergaenzt die zeitlich begrenzte Scheduler-Probe fuer die
Vulkan-Stachelball-Sequenz auf Karte kzn_17.

Die Probe zeichnet maximal 16 qualifizierte Scheduler-Durchlaeufe
mit Vorher-/Nachherwerten auf.

Diagnosedatei: scheduler_probe.json

Ein leeres Probe-Array beweist keinen Scheduler-Stillstand.
Es kann auch bedeuten, dass keine qualifizierten Durchlaeufe
aufgezeichnet wurden.

Entwicklungsstand:
Offline-Scheduler-Tests bestanden.
ARM32-Kompilierung erfolgreich.
Hardwarevalidierung noch ausstehend.
