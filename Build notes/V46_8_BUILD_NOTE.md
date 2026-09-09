# V46.8 Build Note

- F8 shortened/refocused to 37 seconds total.
- Provisional mono reference: 115° capture FOV / 0.97 framing.
- Four safe sequential/native observer phases; no CustomRes writes and no unsafe second-render call.
- Added dedicated two-second parchment boxing countdown overlay; input is ignored while visible and it auto-closes.
- Added ten-second targeted boxing capture with 250 ms state samples plus millisecond mouse-button edge events.
- Boxing observer records IK CVars, MKB state, optional controller state, pipeline counters and PRESENT count; no joint/IK writes.
- F7 is blocked during the ladder so the menu cannot interfere with capture.
- V46.5 mouse interaction and Augusta/manuscript menu styling retained.
