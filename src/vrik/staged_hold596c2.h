#pragma once
#include <cmath>
#include <cstdint>
namespace kcd2vr::staged596c2 {
enum class Decision { Hold, Drop, Stow };
struct Hold {
    std::uint64_t epoch=0,last=0,entered=0;
    bool outside=false;
    void begin(std::uint64_t e,std::uint64_t now) noexcept { *this={e,now,0,false}; }
    Decision update(std::uint64_t e,std::uint64_t now,bool eligible,double grip,double chestDistance) noexcept {
        if(!eligible||e!=epoch||!last||now<last||now-last>80||!std::isfinite(grip)||
           !std::isfinite(chestDistance)||grip<.30) return Decision::Drop;
        last=now;
        if(chestDistance>.33){outside=true;entered=0;}
        if(!outside||chestDistance>.22){entered=0;return Decision::Hold;}
        if(!entered)entered=now;
        return now-entered>=300?Decision::Stow:Decision::Hold;
    }
};
// A real native nock receipt is required. A gesture does not create ammunition.
enum class Reload { Empty, Hand, Ready, AwaitHand, AwaitNock, NockUnconfirmed };
enum class Command { None, Retrieve, Nock, RemoveHand };
struct Loader {
    Reload state=Reload::Empty;
    bool neutral=false,leftQuiver=false,heldArrow40=false;
    // Refusal deadline only: neither stale tracking grace nor authority to
    // retry/delete a visual or accept a logical ammunition key as attachment.
    static constexpr std::uint64_t NockAckBoundMs=500;
    std::uint64_t last=0,nearSince=0,nockSince=0;
    Command update(std::uint64_t now,bool eligible,bool nativeNocked,bool handExists,
                   double grip,bool quiver,double nockGap,bool handAttached=true,bool nativeBindingPending=false,bool releaseToNock40=false) noexcept {
        if(!eligible||!now||(last&&(now<last||now-last>80))||!std::isfinite(grip)||!std::isfinite(nockGap)){
            *this={};return handExists?Command::RemoveHand:Command::None;
        }
        last=now;
        if(nativeNocked){state=Reload::Ready;nearSince=0;nockSince=0;return handExists?Command::RemoveHand:Command::None;}
        if(state==Reload::Ready){state=Reload::Empty;neutral=false;leftQuiver=false;}
        if(grip<.30)neutral=true;
        // A submitted nock is not an attachment receipt. Never repeat the native
        // remove/bind transaction while its actual attachment is still unconfirmed.
        // Entering this mode with a logical key but no physical arrow binding
        // has the same refusal/recovery contract as our own submitted request.
        if(nativeBindingPending&&!handExists&&state!=Reload::AwaitNock&&state!=Reload::NockUnconfirmed){
            state=Reload::AwaitNock;nockSince=now;
        }
        if(state==Reload::AwaitNock||state==Reload::NockUnconfirmed){
            if(!nockSince)nockSince=now;
            if(state==Reload::AwaitNock&&now-nockSince>=NockAckBoundMs)state=Reload::NockUnconfirmed;
            // No deletion or re-nock follows an unconfirmed transaction. Native
            // emptiness plus neutral permits a new shoulder gesture; a later
            // actual attachment permits Ready above, without an expired ticket.
            if(grip<.30&&!handExists&&!nativeBindingPending){state=Reload::Empty;neutral=true;leftQuiver=false;nockSince=0;}
            return Command::None;
        }
        if(handExists){
            state=handAttached?Reload::Hand:Reload::AwaitHand;
            if(grip<.30){
                const bool nock=releaseToNock40&&heldArrow40&&handAttached&&leftQuiver&&nockGap<=.14;
                heldArrow40=false;nearSince=0;
                if(nock){state=Reload::AwaitNock;nockSince=now;return Command::Nock;}
                state=Reload::Empty;return Command::RemoveHand;
            }
            if(grip>=.58)heldArrow40=true;
            if(!handAttached){nearSince=0;return Command::None;}
            if(!quiver)leftQuiver=true;
            if(leftQuiver&&nockGap<=.14){
                if(!nearSince)nearSince=now;
                if(!releaseToNock40&&now-nearSince>=80){nearSince=0;state=Reload::AwaitNock;nockSince=now;return Command::Nock;}
            }else nearSince=0;
        }else if(state==Reload::Hand||state==Reload::AwaitHand){state=Reload::Empty;neutral=false;nearSince=0;}
        else if(neutral&&!nativeBindingPending&&grip>=.58&&quiver){neutral=false;leftQuiver=false;return Command::Retrieve;}
        return Command::None;
    }
};
}
