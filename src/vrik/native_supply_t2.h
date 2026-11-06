#pragma once
#include "../subsystems/throw_drop/release_rearm_t15.h"
#include "../subsystems/throw_drop/inventory_lifecycle_t8.h"
#include <atomic>
#include <cstdint>
#include <cmath>
namespace kcd2vr::supply_t2 {
inline std::atomic<bool> enabled{false};
inline std::atomic<bool> nativeLease{false}; // retained real hand/pending unit needs frame witnesses even after toggle-off
// Complete native-frame publication. Invisible reach offsets are body-relative;
// no pelvis lookup, script code or native receiver is queued.
struct Input {
    std::uintptr_t manager=0,player=0,character=0;
    std::uint64_t body=0,request=0,sample=0,tick=0;
    std::uint32_t entity=0;
    std::uint64_t hand[2]{};
    float grip=0,pos[2][3]{},angles[3]{},offset[2][3]{};
    bool allowed=false,tracked=false,pouchNear[2]{},bone=false;
    float grip18[2]{},trigger18[2]{},wrist18[2][7]{},offset18[2][2][3]{};
    bool pose18[2]{},near18[2][2]{};
    bool chestKnivesT26=false; // original completed-source route, never inferred from hand key
    bool rightHipPouchD66=false; // location metadata; does not change LEFT-only native transport
    std::uint64_t poseTick18[2]{};
    // T12 display metadata: actual equipped WUID, no equipment mutation.
    std::uint64_t beltKey=0;int beltClass=0;bool beltShow=false;
    float beltPos[3]{},beltAngles[3]{};
    // R109: raw controller facts of the same frame, published even when the packet is not the
    // exact completed source (grip18/pose18 are zeroed then). they never admit a draw; they only
    // let the pouch say why a squeeze did nothing instead of staying silent. not part of SameFrame.
    float gripRaw18[2]{};bool zoneRaw18=false; // zoneRaw18: left hand inside the knife zone (hip or chest)
};
// Original completed-frame inputs only. This grants no native combat result.
inline bool QualifiedFistChordT25(const Input& i,int side) noexcept {
    return side>=0&&side<2&&std::isfinite(i.grip18[side])&&std::isfinite(i.trigger18[side])&&
        i.grip18[side]>.60f&&i.trigger18[side]>.60f;
}
inline bool StockGestureAllowedT25(const Input& i,int side) noexcept {
    return side>=0&&side<2&&std::isfinite(i.grip18[side])&&std::isfinite(i.trigger18[side])&&
        !QualifiedFistChordT25(i,side);
}
struct Lease {
    std::uintptr_t manager=0,player=0,character=0,attachment=0;
    std::uint64_t key=0,body=0,request=0,serial=0,tick=0;
    std::uint32_t entity=0;int side=-1,classId=-1;
    bool held=false;
};
inline bool fresh(std::uint64_t now,std::uint64_t then,std::uint64_t max=100) noexcept {
    return then&&now>=then&&now-then<=max;
}
inline bool live(const Input& i,std::uint64_t now) noexcept {
    return i.allowed&&i.tracked&&i.manager&&i.player&&i.character&&i.entity&&
        i.body&&i.request&&i.sample&&fresh(now,i.tick)&&std::isfinite(i.grip);
}
inline bool owns(const Lease& l,const Input& i,std::uint64_t key,int side,std::uint64_t now) noexcept {
    return side>=0&&side<2&&l.held&&l.key==key&&l.side==side&&l.manager==i.manager&&l.player==i.player&&
        l.character==i.character&&l.body==i.body&&l.request==i.request&&l.attachment&&l.entity&&
        fresh(now,l.tick,150)&&live(i,now)&&i.hand[side]==key;
}
// Script-owner-only gesture state. Legacy hip/axe pulls require open in zone,
// close, then pull12cm/80ms; legacy LEFT chest uses its K35 neutral/squeeze rule.
// D66 RIGHT-hip trial allows held-Grip entry once per real Grip occurrence.
// No attachment/pelvis bone is needed; a knife only claims its actual hand.
struct Gesture {
    std::uintptr_t player=0;std::uint64_t body=0,request=0,lastSample=0,started=0;
    bool armed=false,open[2]{};int pulling=-1;float origin[3]{};
    bool rightHipPouchD66=false,hipSpentD66=false;
    int next(const Input& i,std::uint64_t now,bool selected,bool reserved,int side=0,bool dispatchReady=true) noexcept {
        const bool sameOwnerD66=player==i.player&&body==i.body&&request==i.request&&player;
        if(player!=i.player||body!=i.body||request!=i.request){
            // Actor/source replacement cannot manufacture an opening of the
            // same physical Grip after an issued draw or qualified fist.
            const bool spentD66=hipSpentD66;*this={};hipSpentD66=spentD66;
        }
        player=i.player;body=i.body;request=i.request;
        // A location switch cannot carry an undecided squeeze to another pouch.
        // Retained native records/flights are owned outside this gesture state.
        if(rightHipPouchD66!=i.rightHipPouchD66){
            armed=false;open[0]=open[1]=false;pulling=-1;
            if(sameOwnerD66)hipSpentD66=true; // a setting switch is not a new physical squeeze
        }
        rightHipPouchD66=i.rightHipPouchD66;
        if(!selected||!live(i,now)){
            // R109: on the LEFT pouch route a sample whose completed source is not current (the T27
            // preflight flips READY <-> CURRENT_SOURCE every few seconds) used to drop the arm, so the
            // open hand -> squeeze sequence had to be unbroken-live or the squeeze did nothing, with
            // no line. keep the arm; a known closed Grip elsewhere still consumes it.
            // only while the PACKET is the problem (not the exact completed source, or older than the 100 ms
            // window). the switch OFF, a real hand owner or a fist still clear the arm as before.
            if(i.chestKnivesT26&&side==1&&!i.rightHipPouchD66&&(!i.allowed||!fresh(now,i.tick))){
                if(std::isfinite(i.grip)&&i.grip>.55f&&!i.pouchNear[0])armed=false;
                open[0]=open[1]=false;pulling=-1;return -1;
            }
            armed=false;open[0]=open[1]=false;pulling=-1;return -1;
        }
        // A combat fist consumes this side's utility pull. Opening only Trigger
        // while Grip stays closed cannot replay it; a new neutral Grip is required.
        if(!StockGestureAllowedT25(i,side)){
            armed=false;open[0]=open[1]=false;pulling=-1;lastSample=i.sample;
            if(i.chestKnivesT26&&i.rightHipPouchD66)hipSpentD66=true;
            return -1;
        }
        if(i.sample==lastSample)return -1;lastSample=i.sample;
        // K35: the finite LEFT chest pouch is a zone Grip, not the legacy
        // 12 cm pull gesture. A real neutral Grip may be observed before entry;
        // squeezing elsewhere consumes it, so a carried closed fist cannot draw.
        if(i.chestKnivesT26){
            if(side!=1){armed=false;open[0]=open[1]=false;pulling=-1;return -1;}
            if(i.rightHipPouchD66){
                // D66 trial: a held Grip can enter the eligible hip zone. Only
                // a completed/request-retained draw, real hand owner or fist
                // spends that occurrence; pure no-record admission can wait.
                open[0]=open[1]=false;pulling=-1;
                if(i.grip<.25f){hipSpentD66=false;armed=true;return -1;}
                if(reserved||i.hand[0]){hipSpentD66=true;armed=false;return -1;}
                if(i.grip<=.55f||hipSpentD66||!i.pouchNear[0]||!dispatchReady)return -1;
                hipSpentD66=true;armed=false;return 0;
            }
            if(i.grip<.25f){armed=true;open[0]=open[1]=false;pulling=-1;return -1;}
            if(reserved||i.hand[0]){armed=false;open[0]=open[1]=false;pulling=-1;return -1;}
            if(i.grip<=.55f)return -1;
            const bool draw=armed&&i.pouchNear[0];
            // A current pouch Grip may wait for dispatch without acquiring any
            // native/hand ownership. Leaving the pouch, releasing, a real owner,
            // tracking loss or a fist chord still ends this exact intent.
            if(draw&&!dispatchReady)return -1;
            armed=false;open[0]=open[1]=false;pulling=-1;return draw?0:-1;
        }

        if(i.grip<.25f){armed=true;open[0]=i.pouchNear[0];open[1]=i.pouchNear[1];pulling=-1;return -1;}
        if(reserved||i.hand[0]){armed=false;open[0]=open[1]=false;pulling=-1;return -1;}
        if(i.grip<=.55f){pulling=-1;return -1;}
        if(pulling<0){
            if(!armed)return -1;
            const int k=i.pouchNear[0]&&open[0]?0:i.pouchNear[1]&&open[1]?1:-1;
            armed=false;open[0]=open[1]=false;if(k<0)return -1;
            for(int j=0;j<3;++j)if(!std::isfinite(i.offset[k][j]))return -1;
            pulling=k;started=now;for(int j=0;j<3;++j)origin[j]=i.offset[k][j];return -1;
        }
        if(now<started||now-started>2000){pulling=-1;return -1;}
        double moved=0,from=0,to=0;
        for(int j=0;j<3;++j){const double v=i.offset[pulling][j];if(!std::isfinite(v)){pulling=-1;return -1;}
            const double d=v-origin[j];moved+=d*d;from+=double(origin[j])*origin[j];to+=v*v;}
        if(now-started<80||moved<=.12*.12||std::sqrt(to)<=std::sqrt(from)+.02)return -1;
        const int k=pulling;pulling=-1;return k;
    }
};
}
