# Current-main FX readout

Base: main 70ed4e4. These are actual 240×240 framebuffer captures in Instrument
Teal. `orbit-motion.gif` is a three-second playback capture, not a separate mockup.
The Macro view has one large combined plot; the Individual view has one large
selected-effect drawing. Four numbered knob assignments remain below both.

SELECT browses; SEL loads. ALGORITHM right opens Individual and left returns
Macros. SAVE preserves main's sixteen-slot user preset workflow. Nothing in the
readout changes the native Motion On/Off preference, sound patches or transport.

Run `sh tools/test_fx.sh`, then `python3 tools/render_fx_screen.py` with Pillow.
The screen captures use a RAM-only emulator. Hardware verification is pending.
