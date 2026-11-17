Paper Mario PAL Alpha 0.4.5 — M3.1 yaw reset hardware-tested milestone

Built from M3.0 with the M2.9 renderer changes and M3.0 yaw measurement retained.
The port-only reset clears the four-entry HeartBlockContent yaw history and sets
yawBufferPos to zero at the first heal callback after the fade completes (state 5).
States 2–4 continue to update rotation and yaw history normally.

Enable when generating the PAL Miyoo graph: PM64_HEART_YAW_RESET_M31=1.
Check cc_host flags for -DPM64_HEART_YAW_RESET_M31=1 and inspect the binary metadata
for PAL-0.4.5-HEART-YAW-RESET-M31. Captures emit M31 YAW with yaw2/yaw3.

Hardware result on Miyoo Mini Plus: after refill, M3.1 captured yaw2=0 and yaw3=0
at pos=0; the user judged the heart appearance satisfactory. M3.0 build 047C6674
measured yaw2=270.203918 and yaw3=240.203918 after refill. The yaw trace remains
task_link=unverified, so it does not independently prove association with the
captured framebuffer. Keep M3.0 and M2.9 artifacts untouched.
