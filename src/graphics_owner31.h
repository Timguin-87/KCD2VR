#pragma once
#include "vr_exposure26.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cctype>
#include <string>
#include <utility>
#include <vector>

namespace kcd2vr::graphics_owner31 {

// Explicit Graphics-page save only. These are the exact retail CVar values;
// no renderer settings are polled or forced after the user's save.
struct UpscalerSelectionR27 {int tech=0,quality=1,preset=0;};
inline bool upscalerSettingsR27(UpscalerSelectionR27 s,std::vector<std::pair<std::string,int>>& out) {
    out.clear();
    if((s.tech!=0&&s.tech!=1&&s.tech!=2&&s.tech!=4)||s.preset<0||s.preset>13||(s.preset>=7&&s.preset<=9))return false; // R151: NGX J..M = 10..13
    if(s.tech==2){if(s.quality<0||s.quality>5)return false;
        out.emplace_back("r_SuperResolution_NVIDIA_DLSS_QualityMode",s.quality);
        out.emplace_back("r_SuperResolution_NVIDIA_DLSS_Preset",s.preset);
    }else if(s.tech==1){if(s.quality<0||s.quality>4)return false;
        out.emplace_back("r_SuperResolution_AMD_FSR_QualityMode",s.quality);}
    else if(s.tech==4){if(s.quality<0||s.quality>6)return false;
        out.emplace_back("r_SuperResolution_INTEL_XeSS_QualityMode",s.quality);}
    out.emplace_back("r_SuperResolution_Mode",s.tech);return true;
}
inline bool userCfgKeyR27(const std::string& line,const std::string& wanted) {
    std::size_t i=0;while(i<line.size()&&(line[i]==' '||line[i]=='\t'))++i;
    if(i<line.size()&&(line[i]==';'||line[i]=='#'||(line[i]=='-'&&i+1<line.size()&&line[i+1]=='-')))return false;
    auto j=i;while(j<line.size()&&line[j]!='='&&line[j]!=' '&&line[j]!='\t')++j;
    if(j-i!=wanted.size())return false;
    for(std::size_t k=0;k<wanted.size();++k){auto a=static_cast<unsigned char>(line[i+k]),b=static_cast<unsigned char>(wanted[k]);
        if(std::tolower(a)!=std::tolower(b))return false;}
    while(j<line.size()&&(line[j]==' '||line[j]=='\t'))++j;
    return j<line.size()&&line[j]=='=';
}
inline std::vector<std::string> updateUserCfgR27(const std::vector<std::string>& lines,const std::vector<std::pair<std::string,int>>& values) {
    std::vector<std::string> out;out.reserve(lines.size()+values.size());
    for(const auto& line:lines){bool changed=false;for(const auto& v:values)if(userCfgKeyR27(line,v.first)){changed=true;break;}
        if(!changed)out.push_back(line);}
    // Remove duplicate active assignments, then append this explicit selection
    // in dependency order. Comments and unrelated user settings are untouched.
    for(const auto& v:values)out.push_back(v.first+" = "+std::to_string(v.second));
    return out;
}

// IO resolves native CVars afresh on transitions; no raw pointer is retained.
// Ready is the exact-retail guard. Read/Write are explicit native API leaves.
enum class Result { Done, Retry, Foreign };
inline bool equal(float a,float b) noexcept {return std::isfinite(a)&&std::isfinite(b)&&std::fabs(a-b)<.001f;}
struct Lease {
    float original=0,last=0,attempted=0,requested=0;bool owned=false,blocked=false,uncertain=false,clamped=false,completedWrite=false;
    template<class IO> Result Set(IO& io,const char* name,bool floating,float target,bool factor=false) noexcept {
        completedWrite=false;
        if(blocked)return Result::Foreign;
        float current=0;if(!io.Read(name,floating,current)||!std::isfinite(current))return Result::Retry;
        if(owned&&!equal(current,last)&&!(uncertain&&equal(current,attempted))){owned=false;blocked=true;return Result::Foreign;}
        if(owned&&uncertain){last=current;uncertain=false;clamped=false;}
        const float baseline=owned?original:current;
        if(factor&&baseline<=0){blocked=true;return Result::Foreign;}
        const float desired=factor?baseline*target:target;
        if(equal(current,desired))return Result::Done;
        // Only a previously proven clamp can satisfy the same request without
        // rewriting it. A first unknown no-op still cannot establish ownership.
        if(owned&&clamped&&equal(current,last)&&equal(desired,requested))return Result::Done;
        const bool wrote=io.Write(name,floating,desired);
        float after=0;
        if(!io.Read(name,floating,after)||!std::isfinite(after)){
            // A setter may have changed the value before failing. Retain the
            // attempted write for a compare-before-restore on the next edge.
            if(!owned){original=baseline;last=current;}attempted=desired;uncertain=true;owned=true;return Result::Retry;
        }
        if(equal(after,desired)){if(!owned)original=baseline;last=desired;requested=desired;owned=true;uncertain=false;clamped=false;completedWrite=wrote;return wrote?Result::Done:Result::Retry;}
        // The completed setter may synchronously clamp its input. Retain that
        // observed result and the original baseline, so OFF can restore it.
        // A faulting setter does not establish ownership of an unexpected value.
        if(wrote&&!equal(after,current)){
            if(!owned)original=baseline;last=after;requested=desired;owned=true;uncertain=false;clamped=true;completedWrite=true;return Result::Done;
        }
        // A changed request may clamp to our already-owned observed value.
        // The completed setter plus the prior clamp proof make this safe; an
        // initial successful unchanged setter remains a Retry below.
        if(wrote&&owned&&clamped&&equal(after,last)){requested=desired;completedWrite=true;return Result::Done;}
        if(!equal(after,current)){owned=false;blocked=true;return Result::Foreign;}
        return Result::Retry;
    }
    template<class IO> bool Restore(IO& io,const char* name,bool floating) noexcept {
        completedWrite=false;
        if(!owned)return true;
        float current=0;if(!io.Read(name,floating,current)||!std::isfinite(current))return false;
        // A prior restore may have succeeded before its readback failed.
        if(equal(current,original)){owned=false;uncertain=false;return true;}
        if(!equal(current,last)&&!(uncertain&&equal(current,attempted))){owned=false;blocked=true;return true;}
        last=current;uncertain=false;
        const bool wrote=io.Write(name,floating,original);
        float after=0;if(!io.Read(name,floating,after)||!std::isfinite(after))return false;
        if(equal(after,original)){owned=false;completedWrite=wrote;return true;}
        if(!equal(after,last)){owned=false;blocked=true;return true;}
        return false;
    }
};
// C49 SHADOW QUALITY (Miki 17 Oct, VrShadowQuality): 0 LOW = only the always-on VR2 cascade layout (e_GsmSizesWH, STEREO
// profile, V66.68) - these 5 CVars stay native; 1 MEDIUM = the C26 cascade fix with a 2048 sun shadow map; 2 HIGH = the
// C26 fix as before (4096 sun shadow map). Only the sun shadow map size differs between MEDIUM and HIGH.
inline constexpr int clampShadowLevel(int level) noexcept {return level<0?0:(level>2?2:level);}
inline constexpr float sunResForLevel(int level) noexcept {return clampShadowLevel(level)>=2?4096.f:2048.f;}
inline constexpr const char* shadowLevelName(int level) noexcept {return clampShadowLevel(level)==0?"LOW":clampShadowLevel(level)==1?"MEDIUM":"HIGH";}
// INI migration: an explicit VrShadowQuality wins; otherwise the old VrShadowTweaks=true (the 4096 fix) -> HIGH, else LOW.
inline constexpr int migrateShadowLevel(int explicitLevel,bool oldTweaks) noexcept {return explicitLevel>=0?clampShadowLevel(explicitLevel):(oldTweaks?2:0);}
struct Shadows {
    inline static constexpr const char* names[5]{"e_ShadowsCascadesCentered","e_ShadowsBlendCascades","e_ShadowsBlendCascadesVal","e_ShadowsMaxTexResSunWH","e_ShadowsPerObject"};
    inline static constexpr float desired[5]{1,1,.3f,4096,1}; // [3] = HIGH; the live target is sunRes
    inline static constexpr bool floating[5]{false,false,true,false,false};
    std::array<Lease,5> lease{};
    bool wanted=false,pending=false,rollback=false,applied=false,foreign=false,gaveUp=false;
    float sunRes=4096;int level=0; // C49: target of e_ShadowsMaxTexResSunWH while on; last requested level
    float target(unsigned i) const noexcept {return i==3?sunRes:desired[i];}
    // C49: level edge. MEDIUM <-> HIGH while on rewrites only the owned sun-resolution lease on the next Service.
    template<class IO> void ServiceLevel(int requested,std::uint64_t now,IO& io) noexcept {
        level=clampShadowLevel(requested);
        if(level>0){const float want=sunResForLevel(level);if(!equal(want,sunRes)){sunRes=want;if(wanted){pending=true;retry=0;misses=0;gaveUp=false;}}}
        Service(level>0,now,io);
    }
    std::uint64_t retry=0;unsigned misses=0;
    bool Owned() const noexcept {for(const auto& l:lease)if(l.owned)return true;return false;}
    // Explicit normal-worker cleanup edge, never a timer or DllMain engine call.
    void RearmRestore() noexcept {wanted=false;pending=Owned();retry=0;misses=0;gaveUp=false;applied=false;}
    template<class IO> bool Restore(IO& io) noexcept {
        bool done=true;for(unsigned i=0;i<lease.size();++i)done=lease[i].Restore(io,names[i],floating[i])&&done;return done;
    }
    template<class IO> void Service(bool on,std::uint64_t now,IO& io) noexcept {
        if(on!=wanted){wanted=on;pending=true;retry=0;misses=0;gaveUp=false;}
        if(!on&&!Owned()){lease={};pending=rollback=applied=foreign=false;return;}
        if(!pending||now<retry)return;
        const auto failed=[&]{if(++misses>=3){gaveUp=true;pending=false;}else retry=now+500;};
        if(!io.Ready()){failed();return;}
        if(!on||rollback){
            if(!Restore(io)){failed();return;} // retain owned/uncertain leases on exhaustion
            rollback=applied=false;
            if(!on){bool completed=false;for(const auto& l:lease)completed|=l.completedWrite;if(completed)misses=0;lease={};pending=foreign=false;return;}
        }
        if(foreign){pending=false;return;}
        bool done=true;
        for(unsigned i=0;i<lease.size();++i){const auto result=lease[i].Set(io,names[i],floating[i],target(i));
            if(result!=Result::Done){done=false;foreign=result==Result::Foreign;break;}}
        if(done){bool completed=false;for(const auto& l:lease)completed|=l.completedWrite;if(completed)misses=0;applied=true;pending=false;return;}
        rollback=true;applied=false;
        if(Restore(io))rollback=false;
        pending=!foreign||rollback;
        if(pending)failed();
    }
};
struct Eye {
    exposure26::State state{};Lease lease{};
    exposure26::Level handled=exposure26::Level::Native;
    exposure26::Level requested=exposure26::Level::Native;
    bool pending=false,episode=false,gaveUp=false,wasEnabled=false;std::uint64_t retry=0;
    unsigned misses=0,restoreMisses=0;
    void RearmRestore() noexcept {
        state={};requested=exposure26::Level::Native;episode=wasEnabled=false;
        pending=lease.owned;retry=0;restoreMisses=0;gaveUp=false;
    }
    template<class IO> void Service(bool enabled,bool scope,bool valid,exposure26::Q q,std::uint64_t now,IO& io,std::uint64_t sourceTick=0) noexcept {
        const bool sourceFresh=!sourceTick||(now>=sourceTick&&now-sourceTick<=100);
        const bool allowed=enabled&&scope&&valid&&sourceFresh&&exposure26::Valid(q);
        const auto desired=allowed?exposure26::Step(state,true,q,sourceTick?sourceTick:now):exposure26::Level::Native;
        if(!allowed)state={};
        if(enabled!=wasEnabled){misses=restoreMisses=0;gaveUp=false;pending=true;retry=0;}
        wasEnabled=enabled;
        if(!allowed&&episode){episode=false;pending=true;retry=0;restoreMisses=0;}
        if(allowed)episode=true;
        if(desired!=requested){requested=desired;pending=true;retry=0;}
        if(desired==exposure26::Level::Native&&!lease.owned){
            lease={};handled=desired;pending=false;return;
        }
        if(!pending||now<retry)return;
        if(desired!=exposure26::Level::Native&&gaveUp){pending=false;return;}
        if(desired==exposure26::Level::Native&&restoreMisses>=3){pending=false;return;}
        const auto failed=[&]{auto& count=desired==exposure26::Level::Native?restoreMisses:misses;
            if(++count>=3){gaveUp=true;pending=false;}else retry=now+500;};
        if(!io.Ready()){failed();return;}
        if(desired==exposure26::Level::Native){
            if(!lease.Restore(io,"r_HDREyeAdaptationSpeed",true)){failed();return;}
            if(lease.completedWrite)misses=restoreMisses=0;
            lease={};handled=desired;pending=false;return;
        }
        const bool slow=desired==exposure26::Level::Slow;
        const auto result=lease.Set(io,"r_HDREyeAdaptationSpeed",true,slow?.25f:.01f,slow);
        if(result==Result::Retry){failed();return;}
        if(result==Result::Done&&lease.completedWrite)misses=0;
        handled=desired;pending=false;
    }
};
}
