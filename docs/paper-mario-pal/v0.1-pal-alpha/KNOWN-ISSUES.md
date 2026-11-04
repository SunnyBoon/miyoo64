# Known issues — v0.1-pal-alpha

The following alpha graphics issues are known and **not fixed**:

1. Some large trees have narrow straight segment seams through which the background is visible.
2. The frame around Goombario's enlarged enemy display is incomplete: two sides instead of four.
3. A Star Piece incorrectly blinks although it should remain statically visible. Its shadow remains visible while the Star Piece disappears.

Other small graphics issues may exist. Graphics/audio testing is not a claim of pixel-perfect rendering.

The exact standalone end-user ZIP passed 5/5 manual user hardware smoke checks on Miyoo Mini Plus with OnionOS. Its M14.39b base was played through the prologue to the beginning of Chapter 1. Complete game stability, later chapters/boss scenes and all repeated special-ability uses are not established. Other devices and OS versions are not validated by this evidence; the exact tested OnionOS version and test date were not supplied.

Save states are tied to the build. Do not migrate states/caches or mix pm64 and recipes.bin from another build. Keep the independent PR test app separate: upstream PR #7 has another source/binary pair and its own 8/8 manual result. Its generic package folder can share the name PaperMarioPAL; do not overwrite this alpha or share runtime data inadvertently.
