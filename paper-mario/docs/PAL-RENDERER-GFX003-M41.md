# Paper Mario PAL: GFX003 and experimental M4.1

## GFX003: star pieces and TEXEL1 UV coordinates

Star pieces were sometimes invisible. In the generic TEXEL1-only path, the UV repeat drop used the TEXEL0 coordinate frame even though TEXEL1 uses its own sampler and coordinates. The guard in `gfx_emit_triangle` disables that drop for this state; the existing linked TEXEL0/TEXEL1 handling remains in place.

Commit `b87fb593ec26107a687b89cea08424200268922e` preserves the tested guard on baseline `64bc67c0974f7fe05c15bba29a4fcbd2a0bb1ddf`. On Miyoo Mini Plus, build `A2A0B224` displayed the star piece correctly according to the hardware observation. Original tile-0 state was not fully measured and no extra instrumented A/B trace of the fix build was captured. This is a hardware-confirmed fix candidate, not a claim that every UV state has been verified.

## M4.1: experimental texture-edge blend

A leaf fragment measured on hardware had RGBA `(11,36,7,90)` over framebuffer RGB `(132,181,222)` and the existing software path produced `(89,129,146)`. A conspicuous blue-green fringe was visible along foliage contours.

`PM64_EDGE_AB=1` enables an experiment only when the decoded state is one-cycle and OtherModeL equals `0x00553078` (`G_RM_AA_ZB_TEX_EDGE | G_RM_AA_ZB_TEX_EDGE2`). Passing fragments use the existing unblended edge write after the same alpha and depth tests. Texture sampling is unchanged. With the switch absent or set to `0`, the prior path remains active. This does not enable the separate `PM64_EDGEOPAQUE` option and is not a complete N64 coverage implementation. Settings are read at process startup; use a full restart for A/B.

In a Miyoo A/B comparison of build `635A2249`, a narrow blue-green color threshold counted 52 pixels in A and zero in B in the 72×50 region x=92–163, y=56–105. The counts describe all pixels in that region, not a semantic edge mask. The images were well aligned but came from different game frames; animation can explain some changes. B produced darker, sometimes harder edges, and other scenes, transparent objects and motion still need regression testing. A differently scaled and compressed RetroArch/ParaLLEl-RDP image also lacked similarly conspicuous blue fringes; it is a visual reference, not proof of exact RDP or original hardware behavior.
