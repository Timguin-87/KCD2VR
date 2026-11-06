#pragma once
// R93R1: optional index-finger pointing is OFF by default. Honor an explicit
// saved ON without migrating it away; controller X remains independent.
// Also: which prompt glyph family a startup headset preset implies (PSVR2 -> PlayStation, everything else -> Xbox).
#include <cwchar>

namespace kcd2vr::gesture_talk_policy_d33 {
inline constexpr int kVersion=1;
// savedVersion: VrGestureTalkVersion from the INI (0 when absent); savedTalk: VrGestureTalk (false when absent).
inline bool loadTalk(int savedVersion,bool savedTalk) noexcept {(void)savedVersion;return savedTalk;}

// native_vr_pad_premap glyph style: 2 = Xbox prompt family, 4 = PlayStation prompt family.
inline constexpr int kGlyphXbox=2,kGlyphPlayStation=4;
inline int glyphForHeadset(const wchar_t* id) noexcept {
    return id&&std::wcscmp(id,L"psvr2")==0?kGlyphPlayStation:kGlyphXbox;
}
}
