#pragma once
#include <cstdint>
#include <cmath>
namespace kcd2vr::physical_hand_owner55 {
// A completed current holster acquisition only suppresses a later same-hand
// QuickTalk gesture. It cannot certify native held equipment, contact or a pose.
template<class Frame,class Equipment>
inline bool holster(const Frame& f,const Equipment& e,int side,std::uint64_t now,
                    std::uint64_t ready,std::uint64_t request,std::uint64_t epoch,
                    std::uintptr_t character) noexcept {
 const auto itemCount=e.count;
 if(side<0||side>1||!ready||!request||!epoch||!character||f.characterT23!=character||
    f.input.sample!=ready||f.request!=request||f.input.epoch!=request||f.input.bodyEpoch!=epoch||
    !f.input.allowed||!f.input.tracked[side]||!f.input.intentTracked[side]||!std::isfinite(f.input.grip[side])||f.input.grip[side]<=.55||
    !f.input.tick||now<f.input.tick||now-f.input.tick>80||!e.valid||e.request!=request||e.epoch!=epoch||
    !e.tick||now<e.tick||now-e.tick>80||!e.player||f.player!=e.player||f.manager!=e.manager||!e.manager||
    itemCount<0||itemCount>static_cast<int>(e.items.size())||!e.handKnown[side])return false;
 const auto key=f.holsterGripKeyR15[side],generation=f.holsterGripGenerationR15[side];
 if(!key||!generation)return false;
 for(int i=0;i<e.count;++i){const auto& item=e.items[i];
    if(item.profile.key==key&&item.profile.generation==generation&&item.set>=0)return true;
 }
 return false;
}
// No native reads. A native hand slot can be empty while that hand actually
// holds the other weapon's support socket or the bow string. Only the completed
// current physical publication can certify that additional owner.
// Retained intent is a NEGATIVE voice/utility veto, not a pose/physics grant.
template<class Input,class Output,class Profile,class State>
inline bool retained(const Input& in,const Output& out,const Profile& p,const State& s,int owner) noexcept {
 const int other=1-owner;
 return owner>=0&&owner<2&&p.side==owner&&p.key&&p.generation&&p.twoHanded&&!p.bow()&&
   in.allowed&&in.tracked[owner]&&in.tracked[other]&&in.grip[other]>.25&&
   out.active[owner]&&out.owned[owner]&&s.key==p.key&&s.generation==p.generation&&
   s.support&&s.supportGripReady169r2;
}
template<class Frame,class Equipment>
inline int read(const Frame& f,const Equipment& e,int side,std::uint64_t now,
                std::uint64_t ready,std::uint64_t request,std::uint64_t epoch,
                std::uintptr_t character) noexcept {
 const auto itemCount=e.count;
 if(side<0||side>1||!ready||!request||!epoch||!character||!f.input.sample||
    f.input.sample!=ready||f.request!=request||f.input.epoch!=request||
    f.input.bodyEpoch!=epoch||!f.input.allowed||!f.input.tracked[side]||
    !f.input.tick||now<f.input.tick||now-f.input.tick>80||
    !e.valid||e.request!=request||e.epoch!=epoch||!e.tick||now<e.tick||now-e.tick>80||
    !e.player||f.player!=e.player||f.manager!=e.manager||!e.manager||
    itemCount<0||itemCount>static_cast<int>(e.items.size())||!e.handKnown[side])return -1;
 bool held=false;
 for(int owner=0;owner<2;++owner){
   const auto& p=f.profiles[owner];
   if(!f.output.active[owner])continue;
   // An independent one-hand item cannot make the other native-empty hand
   // unknown. Validate this owner's ancestry only when it actually uses the
   // queried hand; two-hand, retained support and bow-string ownership stay coupled.
   const bool usesSide=side==owner||f.output.support[owner]||
       (owner==1&&side==0&&f.output.bowHand169r2)||
       (f.retainedSupport55[owner]&&p.twoHanded&&!p.bow()&&f.input.grip[side]>.25);
   if(!usesSide)continue;
   if(!p.key||!p.generation||p.side!=owner||!f.output.owned[owner]||!e.handKnown[owner]||
      e.handKey[owner]!=p.key)return -1;
   const int i=e.heldIndex[owner];
   if(i<0||i>=e.count||!e.items[i].held||e.items[i].profile.key!=p.key||
      e.items[i].profile.generation!=p.generation||e.items[i].profile.entity!=p.entity)return -1;
   const auto& c=f.correlation126[owner];
   if(!c.socket.valid||c.socket.key!=p.key||c.socket.generation!=p.generation||
      reinterpret_cast<std::uintptr_t>(c.socket.character)!=character||c.stamp.actor!=character||
      c.stamp.epoch!=epoch||c.stamp.sample!=ready||c.stamp.tick!=f.input.tick)return -1;
   if(side==owner)held=true;
   else if(f.retainedSupport55[owner]&&p.twoHanded&&!p.bow()&&f.input.grip[side]>.25){
     // The fixed-length reach solve can leave the wrist at a free fallback while
     // preserving an actual held grasp. It is still occupied for gestures.
     held=true;
   }else if(f.output.support[owner]||(owner==1&&side==0&&f.output.bowHand169r2)){
     if(!f.output.owned[side])return -1;held=true;
   }
 }
 if(held)return 1;
 // An owned wrist without a qualified item ancestry is not an empty hand.
 return f.output.owned[side]?-1:0;
}
}
