# V55.2 implementation notes

- Raw filter mode is now a true no-op with respect to QPC/filter state.
- `XrManager::SetFlatUiMode` is transition-based; repeated calls no longer recapture the anchor or spam one line per XR/control frame.
- Flat UI source shrink increased from 1.28 to 1.58 for inventory/codex/quest readability without the previous zoomed crop.
- F6 now toggles native-menu flat presentation instead of immediately forcing itself back off.
- No desktop resolution changes.
- No claim of automatic menu detection in this build.
