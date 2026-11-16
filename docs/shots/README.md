# MOD GUI shots

Every image: real mod-ui 1.15.0 (build v1.15.0.3371, commit 391c1ed, github.com/moddevices/mod-ui) running in a container, no audio host, drawn in Chromium; label "mod-ui 1.15.0 in a container".

- `mod-ui-1.15.0-container-comp-before.png`: omx-comp as main ships it (before this change).
- `mod-ui-1.15.0-container-comp-vintage.png`, `-comp-console.png`, `-comp-3d.png`: omx-comp, advanced tier, the pedal starting in each look.
- `mod-ui-1.15.0-container-strip-vintage-easy.png`, `-strip-vintage-advanced.png`, `-strip-console-advanced.png`, `-strip-3d-advanced.png`: omx-strip. These are drawn from a bundle whose come-up values ENGAGE the filters and two bands (HPF on at 80 Hz, LPF on at 18 kHz, band 1 +3 dB, band 2 -3 dB) so the static EQ curve shows them; the shipped plugin comes up with hpfOn=false, lpfOn=false and every band off.
- `mod-ui-1.15.0-container-comp-live-vintage-needles.png`: omx-comp, vintage, the needles mid-lag: the output ports (compIn, compOut, compKey, compGr) are driven from the page through mod-ui's own output_set path, a scripted level sweep, not an audio tone.
- `mod-ui-1.15.0-container-comp-live-switch-console.png`, `-switch-3d.png`, `-switch-vintage.png`: the same pedal after a click on the header's look switch; the size and the ports do not change.
- `mod-ui-1.15.0-container-comp-vintage-metal-closeup.png`: omx-comp, vintage, a close-up (device scale 4) of the header and the first section: the knob film strips in the console's aluminium metal (cap-hi to face, rim, knurl, carved pointer), the recessed glass, the lit scribbles, the metal button caps and the footswitch.
- Retaken after the label fit (a label breaks only between words, one declared gap to its value strip), the console's relief and metal in the vintage look, the raised-section 3d look, and omx-strip's EQ Bands as a selector.
