#pragma once
#include "../../vrik/motion_owned59c4.h"
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace kcd2vr::routerH3 {
// Separate F12 experiment. 2 changes native window durations ONLY; it does not
// turn an unknown/busy native action into permission for another hit producer.
inline std::atomic<int> mode{0};
// H18: native global timing is an EXPLICIT F12 experiment only. Retired
// automatic hybrid assist made router OFF still lease six native globals.
// Ordinary HYBRID/DIRECT preserve the game's current timings.
inline std::atomic<float> playerSlotModifier{.20f},blockSpeedModifier{1.40f};
inline bool timingSelected() noexcept {return mode.load(std::memory_order_acquire)==2;}
inline std::atomic<float> comboSeconds{3.f},riposteSeconds{1.f},standardSeconds{1.5f},aiSlotModifier{.2f};
inline bool nearValue(float a,float b) noexcept {return std::isfinite(a)&&std::isfinite(b)&&std::fabs(a-b)<.0001f;}
inline float setting(float v,float lo,float hi,float fallback) noexcept {
    return std::isfinite(v)?std::clamp(v,lo,hi):fallback;
}
inline unsigned comboWindow(unsigned configured)noexcept {
    if(!configured||!timingSelected())return configured;
    return std::max(configured,unsigned(setting(comboSeconds.load(),.5f,3.f,3.f)*1000.f));
}
struct Entry {
    const char* name;float original=0,wrote=0;bool owned=false,settled=false;
    unsigned tries=0;
};
struct Windows {
    std::array<Entry,6> entries{{{"wh_cs_ComboSlotDuration"},{"wh_cs_RiposteSlotDuration"},
        {"wh_cs_StandardSlotDuration"},{"wh_cs_AISlotDurationMod"},
        {"wh_cs_PLSlotDurationMod"},{"wh_cs_TimeAlignment_BlockSpeedMod"}}};
    bool wanted=false;std::array<float,6> desired{};std::uint64_t next=0;
    template<class IO,class Log> void service(bool active,std::uint64_t now,IO& io,Log log) noexcept {
        const std::array<float,6> values{{setting(comboSeconds.load(),.5f,3.f,3.f),
            setting(riposteSeconds.load(),.1f,1.5f,1.f),setting(standardSeconds.load(),.1f,2.f,1.5f),
            setting(aiSlotModifier.load(),0.f,1.f,.2f),setting(playerSlotModifier.load(),0.f,1.f,.2f),
            setting(blockSpeedModifier.load(),.8f,2.4f,1.4f)}};
        const bool want=active&&timingSelected();
        if(want!=wanted||values!=desired){
            wanted=want;desired=values;next=0;
            for(auto& e:entries){e.settled=false;e.tries=0;}
        }
        if(next&&now<next)return;
        next=now+250; // only failed/changed entries retry; steady ownership reads NOTHING.
        for(unsigned i=0;i<entries.size();++i){
            auto& e=entries[i];if(e.settled||e.tries>=8)continue;
            if(++e.tries==8)log(e.name,e.owned?"LAST_RETRY_RESTORE_PENDING":"LAST_RETRY",0,0);
            if(e.owned){
                // Always discharge an older/uncertain lease before applying a new
                // setting. Failed read/write/readback retains ownership for retry.
                float current=0;if(!io.Read(e.name,current)||!std::isfinite(current))continue;
                if(!nearValue(current,e.wrote)){e.owned=false;e.settled=true;log(e.name,"OTHER_OWNER",current,current);continue;}
                if(want&&nearValue(e.wrote,desired[i])){e.settled=true;continue;}
                if(!io.Write(e.name,e.original))continue;
                float check=0;if(!io.Read(e.name,check)||!nearValue(check,e.original))continue;
                log(e.name,"RESTORED",e.wrote,check);e.owned=false;
            }
            if(!want){e.settled=true;continue;}
            float current=0;if(!io.Read(e.name,current)||!std::isfinite(current))continue;
            if(nearValue(current,desired[i])){e.settled=true;continue;} // never claim someone else's value
            e.original=current;e.wrote=desired[i];
            // A false return can still mean a side effect. Keep a conservative
            // restore obligation before calling Write, even if readback fails.
            e.owned=true;
            if(!io.Write(e.name,e.wrote))continue;
            float check=0;if(!io.Read(e.name,check)||!nearValue(check,e.wrote))continue;
            e.settled=true;log(e.name,"APPLIED",e.original,check);
        }
    }
    bool owns()const noexcept {for(const auto& e:entries)if(e.owned)return true;return false;}
    void rearmRestore()noexcept {next=0;for(auto& e:entries)if(e.owned){e.tries=0;e.settled=false;}}
};
// Observer runs only at the qualified command boundary, never from raw tip
// speed/contact. The full key prevents a same-number gesture after load/rebind
// being mistaken for an old one. It grants no route or admission permission.
struct Observation {
    motion59c4::Key key{};bool direct=false,known=false,busy=false;int phase=-1,type=-1,kind=0,mode=0;
    double speed=0,scale=0;bool effortKnown=false;
};
struct Observer {
    std::array<motion59c4::Key,32> recent{};unsigned cursor=0,notes=0;
    bool take(const Observation& o) noexcept {
        if(!o.mode||!motion59c4::valid(o.key))return false;
        for(const auto& k:recent)if(motion59c4::same(k,o.key))return false;
        recent[cursor++%recent.size()]=o.key;
        return notes++<160; // bounded per-session evidence; no continuous native readback
    }
};
}
