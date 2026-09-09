# V27 — Isolated consumed-parent causality

V26 proved two stable mode-0 parents are consumed, but its per-call reversible writes meant the two parents were never simultaneously displaced. V27 isolates each candidate independently.

Phases after target selection:

- 0–5 s: baseline
- 5–10 s: parent A +100 mm, parent B unchanged
- 10–15 s: parent B +100 mm, parent A unchanged
- 15–20 s: parent A -100 mm, parent B unchanged
- 20–25 s: parent B -100 mm, parent A unchanged
- 25 s+: XR split: A=-IPD/2, B=+IPD/2

Interpretation:
- If A-only doubles the color image but B-only does not, B is auxiliary rather than a second eye candidate.
- If both A-only and B-only independently move/double the color image, both are genuine visible-color contributors and deserve downstream render-target tracing.
- Shadows/culling-only changes identify an auxiliary pass.

Markers: PARENT-ISOLATE-V27 selected-A / selected-B / baseline / apply, RENDER-CONSUME-V27.
