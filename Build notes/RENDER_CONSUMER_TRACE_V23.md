# V24 Renderer Consumer Trace

Purpose: stop treating 0x866F6C camB as a second eye and trace the real downstream consumption of CRenderView+0x9B30 records.

Static finding:
- 0x866F6C copies transform-related fields from camA into the main 0x400 record.
- camB is only passed to 0x866DFC, which reads projection/frustum fields and writes a projection block; camB translation is not consumed there.
- 0x5727C5 passes CRenderView+0x9B30 and +0xA330 count to 0x653940.
- 0x64A7EB later reads matrix-looking blocks at record +0x190..+0x1C0.

V24 adds a read-only observer at WHGame+0x653940 with fallback correlation:
1. tier=target: records == selected native-eye target + 0x9B30
2. tier=tracked: records == any tracked CRenderView + 0x9B30
3. tier=generic: sparse samples for unknown record owners

Marker: RENDER-CONSUME-V24

No 0x4E6924 or 0x4E7E64 hooks are enabled. Existing V21 write phases remain so their culling/shadow effects can be correlated with downstream records in one launch.


V24 ABI FIX: WHGame+0x5727BC writes a fifth qword argument at [rsp+0x20] before calling WHGame+0x653940. V23 omitted this parameter and could corrupt downstream renderer execution. V24 forwards it exactly and throttles logging.
