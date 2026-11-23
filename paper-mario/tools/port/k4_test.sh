#!/bin/sh
# Compare the native two-cycle fire combiner to Angrylion RDP Plus commit
# 9c8b9ed3e7d7f00dff8bc872ccdd3fba1a3673fc. All 512 raw K4 values,
# four mux configurations and four synthetic texel pairs are included.
# The reference's next TEXEL0 equals the current TEXEL0 for these inputs.
# The isolated M2 build 39B058BF improved the fire on hardware; this
# integrated branch has not been retested on hardware.
set -eu
R=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
INC=${PM64_DECOMP_INCLUDE:-$R/refs/papermario/include}
O=$(mktemp -d)
trap 'rm -rf "$O"' EXIT HUP INT TERM
C="-m32 -fno-pie -no-pie -std=gnu11 -ffunction-sections -fdata-sections -fsigned-char -fno-strict-aliasing -fwrapv -DTARGET_HANDHELD=1 -DPM64=1 -DPM64_REPLAY=1 -DF3DEX_GBI_2=1 -D_LANGUAGE_C=1 -DPORT_NO_FPS=1 -DNON_MATCHING=1 -DAVOID_UB=1"
L="-Wl,--gc-sections -Wl,--unresolved-symbols=ignore-all"
cc $C -I "$R/bench/kit" -I "$INC" -I "$R/tools/port" -w $L -o "$O/k4" "$R/tools/port/k4_test.c" -lm
"$O/k4" > "$O/results"
printf '%s  %s\n' 6b674f6731466226ee8c292051c8d1b3699e8611b8131e9e207d7459db97eaf4 "$O/results" | sha256sum -c -
cc $C -I "$R/bench/kit" -I "$INC" -I "$R/tools/port" -w $L -o "$O/state" "$R/tools/port/k4_state_test.c" "$R/tools/port/k4_state_soft.c" -lm
"$O/state"
echo 'k4_test: 8192 Angrylion reference RGBA outputs and state changes PASS'
