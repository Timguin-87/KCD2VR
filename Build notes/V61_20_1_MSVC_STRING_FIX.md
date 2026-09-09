# V61.20.1 MSVC build fix

Fixes MSVC C2026 (`string too big`) in `src/hud_xml_base.inc`.

The embedded HUD XML was previously emitted as one ~39 KB raw string literal. MSVC rejects a single literal of that size. The XML payload is now split into four adjacent raw string literals, each under 12 KB. C++ concatenates adjacent literals at compile time, so `BaseHudXml()` receives the exact same XML bytes as V61.20.

No VR, HUD geometry, stereo, camera, culling, input, or DOF behavior was changed by this repair.
