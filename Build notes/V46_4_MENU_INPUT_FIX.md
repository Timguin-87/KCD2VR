# MIS V46.4 — Menu input fix

V46.3.2 rendered the panel in D3D12 backbuffer coordinates but hit-tested raw Win32 client coordinates. If the client rectangle and captured 1920x1080 backbuffer were not 1:1, the visible software cursor and the invisible click rectangles could drift apart.

V46.4 maps both:
- WM mouse coordinates -> current client rect -> latest captured backbuffer size -> panel coordinates
- OS cursor used for the in-HMD cross -> same mapping

While F8 runs, menu edits remain intentionally locked. V46.4 makes this explicit in the parchment UI: `STEREO TEST RUNNING` / `menu edits are locked`. `Good Mono 110°` remains the safe cancel action.
