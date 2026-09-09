# V51 implementation notes

- Default camera injection moved from repeated frustum mutation to reversible `PublishViewCamera` injection.
- Late QPC-now sampling remains only as F8 legacy phase 5.
- F8 contains a position-only control to distinguish rotational camera injection from general OpenXR tracking.
- F8 phase 6 uses existing AFR machinery only as an eye/IPD diagnostic; it remains sequential, not same-frame native stereo.
- F7 menu copy now accepts typed/typeless BGRA8 family compatibility.
- No live KCD2 resolution switching is performed.
- True target remains two real same-frame eye renders at runtime-sized per-eye targets plus separate UI layers.
