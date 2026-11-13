Paper Mario PAL Alpha 0.4 Diagnostic (PAL-0.4-DIAG)

This separate diagnostic app is for isolated bug reproduction. Extract this
ZIP at the root of the SD card. It creates App/PaperMarioPALDiagnostic/ and
uses its own data/ and traces/ folders; it does not share saves with the
standard Alpha 0.4 app or prior M1.7 installations.

Supply your own verified Paper Mario (Europe, PAL) ROM at:
  /mnt/SDCARD/Roms/N64/PaperMarioPAL.z64
Required SHA-1:
  2111d39265a317414d359e35a7d971c4dfa5f9e1

In the handheld menu, RENDER TRACE captures a renderer trace and matching
frame.ppm under traces/trace_XXXXXX/. ITEM/TEXEL1 TRACE records the existing
item/TEXEL1 diagnostic profile. The normal launcher log is data/console.log.
These diagnostics are observational; they do not prove a game scene is correct.

Analyzing a reporter's pm64.sav:
1. Record the report's origin and build version. Archive the original unchanged
   and record its SHA-256, file size, and header.
2. Accept only a plausible 131072-byte regular Save-RAM file with the expected
   Paper Mario header. It is not an S6MP save-state and may not open directly
   at the reporter's scene.
3. Assess PAL Alpha 0.4 compatibility before copying only a working copy to
   this app's isolated data/ directory. Never put an untrusted file in a
   personal gameplay installation.
4. Reject malformed, modified, or incompatible files rather than attempting a
   repair. Reproduce with the normal load path, then preserve any trace, log,
   and screenshot evidence separately.

Alpha status: this package passed static release checks but still requires a
hardware acceptance of this exact binary. It contains no ROM, savefile, or
game asset data. See the standard package README for credits and license note.
