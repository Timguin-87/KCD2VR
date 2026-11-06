#pragma once
#include <cstdint>

namespace kcd2vr::subsystems::smithing::stroke_receipt_d57 {
// Diagnostic correlation only. No input, animation time, quality or hit policy
// reads this state. The caller supplies only an active finite F5 capture.
struct Receipt {
    std::uintptr_t instance=0;
    std::uint64_t session=0,lifetime=0,id=0,pose=0,source=0,modeEpoch=0;
    std::uint64_t startTick=0,releaseTick=0,startSample=0,releaseSample=0,sample=0,sampleTick=0,tick=0;
    int phase=-1,mode=-1;
    bool nativeHeight=false,nativeFallSeen=false;
};
struct Input {
    bool capture=false,eligible=false,sampleValid=false,start=false,release=false,abort=false,nativeHeight=false;
    std::uintptr_t instance=0;
    std::uint64_t session=0,lifetime=0,pose=0,source=0,modeEpoch=0,sample=0,sampleTick=0,now=0;
    int phase=-1,mode=-1;
};
inline bool anvil(int phase) noexcept {return phase==11||phase==14||phase==16||phase==17;}
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {return tick&&now>=tick&&now-tick<=80;}
class Tracker {
    Receipt current_{};
    std::uint64_t session_=0,next_=0,lastSample_=0,lastTick_=0;
public:
    Receipt step(const Input& in) noexcept {
        if(!in.capture||!in.session){if(session_)*this={};return {};}
        if(session_!=in.session){*this={};session_=in.session;}
        const bool valid=in.eligible&&in.sampleValid&&!in.abort&&in.instance&&in.lifetime&&
            in.pose&&in.source&&in.modeEpoch&&in.sample&&in.now&&fresh(in.sampleTick,in.now)&&anvil(in.phase);
        if(!valid){current_={};lastSample_=lastTick_=0;return {};}
        const bool changed=current_.id&&(current_.instance!=in.instance||current_.lifetime!=in.lifetime||
            current_.pose!=in.pose||current_.source!=in.source||current_.modeEpoch!=in.modeEpoch||
            current_.mode!=in.mode||current_.nativeHeight!=in.nativeHeight);
        const bool discontinuity=lastSample_&&(in.sample<lastSample_||in.sampleTick<lastTick_||in.sampleTick-lastTick_>80);
        if(changed||discontinuity)current_={};
        const bool newSample=in.sample!=lastSample_;
        lastSample_=in.sample;lastTick_=in.sampleTick;
        // Only a genuine newly observed raise edge creates an ID. Turning F5 on
        // halfway through native/manual work cannot adopt that ongoing stroke.
        if(in.start&&newSample){
            current_={};if(next_>=128)return {};
            current_.instance=in.instance;current_.lifetime=in.lifetime;
            current_.session=in.session;current_.id=++next_;current_.pose=in.pose;current_.source=in.source;
            current_.modeEpoch=in.modeEpoch;current_.mode=in.mode;current_.nativeHeight=in.nativeHeight;
            current_.startTick=in.now;current_.startSample=in.sample;
        }
        if(!current_.id)return {};
        if(in.release&&newSample&&!current_.releaseTick){current_.releaseTick=in.now;current_.releaseSample=in.sample;}
        if(current_.releaseTick&&(in.phase==16||in.phase==17))current_.nativeFallSeen=true;
        if(current_.nativeFallSeen&&in.phase==11){current_={};return {};}
        current_.phase=in.phase;current_.sample=in.sample;current_.sampleTick=in.sampleTick;current_.tick=in.now;
        return current_;
    }
};
// Join to the EXISTING qualified native projected-point callback and placed
// hammer packet. No native action ID is available here: this is temporal
// correlation, not proof of the same native stroke, quality or accepted damage.
inline const char* join(const Receipt& r,std::uint64_t session,std::uint64_t now,
    std::uintptr_t instance,std::uint64_t lifetime,bool hammerJoined,
    std::uint64_t pose,std::uint64_t source,std::uint64_t modeEpoch) noexcept {
    if(!r.id)return "NO_VR_STROKE";
    if(r.session!=session)return "CAPTURE_CHANGED";
    if(r.instance!=instance||r.lifetime!=lifetime)return "STATION_CHANGED";
    if(!fresh(r.tick,now)||!fresh(r.sampleTick,now))return "INPUT_STALE";
    if(!r.releaseTick||r.releaseTick<r.startTick||r.releaseTick>now)return "NO_DOWN_REVERSAL";
    if(!hammerJoined)return "NO_PLACED_HAMMER";
    if(r.pose!=pose||r.source!=source||r.modeEpoch!=modeEpoch)return "EPOCH_CHANGED";
    return "CORRELATED_NATIVE_POINT_ONLY";
}
}
