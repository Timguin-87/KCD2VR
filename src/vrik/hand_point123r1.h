#pragma once
#include <cmath>
#include <cstdint>
namespace kcd2vr::hand_point123r1 {
struct Hand {bool eligible=false;float grip=0,trigger=0;bool acceptedHeldTrigger32=false,doorContact65=false;std::uint64_t resumePressD48=0;};
struct Input {bool enabled=false;std::uint64_t now=0;Hand hand[2]{};};
struct State {int owner=-1;bool armed[2]{false,false};std::uint64_t tick=0,resumedPressD48[2]{};};
struct Output {int owner=-1;bool simultaneous=false;};
// Trigger is normally a fist/combat owner. Only an already accepted native
// workstation grip may retain its existing palm; trigger never acquires one.
inline bool TriggerEligible32(const Hand& h,int owner,int side) noexcept {
    return std::isfinite(h.grip)&&std::isfinite(h.trigger)&&h.trigger>=0&&h.trigger<=1&&
        (h.trigger<.15f||(h.acceptedHeldTrigger32&&owner==side&&h.grip>=.30f)||h.doorContact65);
}
inline Output Update(State& s,const Input& in) noexcept {
    Output out{};
    if(!in.enabled||!in.now||(s.tick&&(in.now<s.tick||in.now-s.tick>80))){s={};return out;}
    s.tick=in.now;bool edge[2]{};
    for(int i=0;i<2;++i){const auto& h=in.hand[i];
        const bool valid=h.eligible&&TriggerEligible32(h,s.owner,i);
        if(!valid){s.armed[i]=false;if(s.owner==i)s.owner=-1;continue;}
        if(h.grip<.30f){s.armed[i]=true;s.resumedPressD48[i]=0;if(s.owner==i)s.owner=-1;}
        if(h.grip>.58f&&s.armed[i]){edge[i]=true;s.armed[i]=false;}
        // Only a current same-door physical surface capability supplied by the
        // caller may acquire a held Grip after reaching it. Real weapon/fist
        // reservations are checked before that capability is produced.
        if(h.doorContact65&&h.grip>=.30f&&s.owner<0){edge[i]=true;s.armed[i]=false;}
        // Transfer the original unentered H40 intent after its no-door reply.
        // Caller revalidates the exact press, player and tracking epoch.
        if(h.resumePressD48&&h.resumePressD48!=s.resumedPressD48[i]&&in.now>=h.resumePressD48&&
           in.now-h.resumePressD48<=300&&h.grip>=.30f&&h.trigger<.15f&&s.owner<0){
            edge[i]=true;s.armed[i]=false;s.resumedPressD48[i]=h.resumePressD48;
        }
    }
    if(edge[0]&&edge[1]){out.simultaneous=true;s.owner=-1;}
    else if(s.owner<0&&(edge[0]||edge[1]))s.owner=edge[0]?0:1;
    out.owner=s.owner;return out;
}
}
