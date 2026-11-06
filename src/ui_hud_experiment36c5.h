#pragma once
#include "ui_palm171.h"
#include <limits>

namespace kcd2vr::ui_hud_experiment36c5 {
// Session-only. Recording can expire without changing an explicitly selected HUD.
class Ladder {
    unsigned rung_=0;
public:
    explicit Ladder(unsigned startup=0) noexcept:rung_(startup<3?startup:0){} // C26: rung 3 opens the map - never at startup
    unsigned Press() noexcept {rung_=(rung_+1)%4;return rung_;} // C26 (Miki): 0 off, 1 record, 2 corrected HUD, 3 = palm HUD + VR MAP
    unsigned Rung() const noexcept{return rung_;}
    // C34 (Claude, Miki 16 Oct log: F2 pressed onto rung 1 at 22:22:52 -> corrected=0 for the rest of the session = the old
    // black square, no vitals, no prompts; VrAutoCorrectHud defaults to false so the C32 keepAutoHud path does not cover it).
    // The corrected HUD is the baseline: F2 only changes recording / palm HUD / map. The parameter stays for the callers.
    bool Corrected(bool keepAutoHud=false) const noexcept{(void)keepAutoHud;return true;}
};
class PalmReveal {
    ui_palm171::Gaze gaze_;
    struct Hand {bool gaze=false,revealed=false,fired=false;std::uint64_t since=0,lookSince=0;};
    // C22b (Miki): a passing glance must not reveal; the OPEN palm must be looked at for 1 s.
public: static inline std::uint64_t dwellMs=1000; // tests of the pre-C22b timer set 0
    // R115 (Miki 6 Nov: "the compass and health can stay longer (like 7 seconds)"): a revealed palm group stays
    // 7 s from the reveal, also after you look away / close the hand (before: hidden at once, max 3 s).
    static inline std::uint64_t showMs=7000;
private:
    std::array<Hand,2> hands_{};
    bool haveClock_=false;std::uint64_t last_=0;
public:
    ui_contextual_hud162r1::Output Update(const ui_contextual_hud162r1::Input& in,
                                       const std::array<bool,2>& open) noexcept {
        using namespace ui_contextual_hud162r1;
        const bool active=in.enabled&&in.gameplay;
        if(!active||!in.headValid||!ValidQuat(in.headOrientation)||
           !std::isfinite(in.head.x)||!std::isfinite(in.head.y)||!std::isfinite(in.head.z)||
           (haveClock_&&in.now<last_)){
            gaze_={};hands_={};haveClock_=false;return {active,false,false};
        }
        haveClock_=true;last_=in.now;
        const auto seen=gaze_.Update(in,open); // raw gaze stays instantaneous for other HUD callers
        const std::array<bool,2> looking{seen.left,seen.right};
        std::array<bool,2> shown{};
        for(unsigned h=0;h<2;++h){auto& s=hands_[h];const auto& p=in.palm[h];
            if(!p.valid||!ValidQuat(p.orientation)||!std::isfinite(p.position.x)||
               !std::isfinite(p.position.y)||!std::isfinite(p.position.z)){s={};continue;}
            if(looking[h]){
                if(!s.gaze){s.lookSince=in.now;s.fired=false;}
                if(!s.fired&&in.now>=s.lookSince&&in.now-s.lookSince>=dwellMs){s.revealed=true;s.since=in.now;s.fired=true;}
            } else {s.fired=false;if(s.revealed&&(in.now<s.since||in.now-s.since>=showMs))s.revealed=false;}
            s.gaze=looking[h];
            // R115: shown for showMs (7 s) from the reveal, looking or not; a continuous glance needs a new
            // glance after that. no latch: it always ends after 7 s.
            shown[h]=s.revealed&&in.now>=s.since&&in.now-s.since<showMs;
            if(!shown[h]&&!looking[h])s.revealed=false;
        }
        return {active,shown[0],shown[1]};
    }
};
// The palm timer must own native visibility as well as alpha. Otherwise a
// native timeline restoring alpha makes a palm-hidden clip visible again.
// Use the semantic Left/Right groups: quick selectors are deliberately not
// members of Left in ui_hud_visibility175::Group, so they remain native.
inline unsigned VisibilityMask(unsigned preferences,ui_contextual_hud162r1::Output reveal) noexcept {
    using namespace ui_contextual_hud162r1;
    if(!reveal.active)return preferences;
    return (preferences&~(HideCompass163|HideLower163))|
        (reveal.left?0u:unsigned(Left))|(reveal.right?0u:unsigned(Right));
}
// Cache desktop SCREEN pixels, not client pixels. Window movement and an
// externally moved cursor must both invalidate a stationary VR-ray position.
inline bool NeedsMove(long x,long y,bool have,long oldX,long oldY,bool actualValid,long actualX,long actualY) noexcept {
    return !have||x!=oldX||y!=oldY||!actualValid||x!=actualX||y!=actualY;
}
}
