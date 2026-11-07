#ifdef _MSC_VER
#include <windows.h>
// The complete production TU contains native SEH finalizers.
#undef __try
#undef __except
#endif
#include "../src/diagnostics/interaction_cycle_h4.h" // parent's separately staged F5 include is not part of52b1917
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {} void Drain(bool) noexcept {}}
using namespace kcd2vr::frik_pose_probe;namespace q=kcd2vr::quick_talk_owner_h4;
#include <type_traits>
static unsigned total=0,fail=0;static void ck(bool b,const char*s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
using Producer=void(*)(const ph::Input&,const PhysEquipment6698&,ph::Output&,std::array<ph::HolsterState,8>&,unsigned) noexcept;
static_assert(std::is_standard_layout_v<q::Mailbox>);
// A deterministic concurrent-read/publication scheduling leaf. Mailbox is a
// standard-layout C++ class whose first declared member is this actual flag.
// No game/native memory, fake mailbox or production visibility change.
static std::atomic_flag& lockbox(){return *reinterpret_cast<std::atomic_flag*>(&q::mailbox);}
struct Case {
 PhysEquipment6698 gear{};ph::Input in{};std::array<ph::HolsterState,8> state{};ph::Output out{};ph::Vec3 mount{};int side;
 Case(int s):side(s){g_pose_epoch6689=1;g_weapon_request_epoch6694=1;g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;
  gear.valid=true;gear.epoch=gear.request=1;gear.player=reinterpret_cast<void*>(123);gear.count=1;gear.selected=1;gear.handKnown[0]=gear.handKnown[1]=true;
  auto& item=gear.items[0];item.profile.side=s;item.profile.classId=s?9:7;item.profile.key=1901;item.profile.generation=12;item.set=1;item.slot126=0;
  in.epoch=in.bodyEpoch=1;in.tick=10000;in.sample=1;in.requirePrimaryGrip114=in.allowed=true;
  in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};ph::holster_mount126(in,item.profile,mount);in.hands[s].p=mount; // current accepted shoulder/back acquisition volume
 }
 void step(Producer f,int mask=0,bool busy=false,unsigned doors=3){in.tick+=20;++in.sample;hostTickCount64=in.tick;gear.tick=in.tick;g_phys_equipment6698.publish(gear);
  if(mask>=0)ck(q::mailbox.publish({in.tick,in.bodyEpoch,in.epoch,reinterpret_cast<std::uintptr_t>(gear.player),unsigned(mask)}),"publish actual same-owner stamp");
  if(busy)ck(!lockbox().test_and_set(std::memory_order_acquire),"hold real mailbox lock for modeled scheduling collision");
  out={};f(in,gear,out,state,doors);
  if(busy)lockbox().clear(std::memory_order_release);
 }
 void latch(Producer f){step(f);in.grip[side]=1;step(f);for(int i=0;i<3;++i)step(f);ck(state[0].comparison114.latched,"real producer latches eligible Grip and original dwell");in.hands[side].p=mount+ph::Vec3{0,.35,0};}
};

static void MailboxContentionH4(){
 q::Stamp first{};ck(q::mailbox.read(first),"never-published Talk mailbox is readable");
 ck(!first.mask&&!q::current(first,10000,1,1,123),"never-published Talk means known no current owner");
 for(int s=0;s<2;++s){
  Case c(s);c.latch(PhysHolsterGestures6698);const auto before=c.state[0].comparison114;
  c.step(PhysHolsterGestures6698,0,true);
  ck(!c.out.commandCount&&c.state[0].comparison114.latched&&c.state[0].comparison114.neutral==before.neutral,
     "actual transient Talk contention defers new draw without erasing valid neutral/latch");
  c.step(PhysHolsterGestures6698,0);
  ck(c.out.commandCount==1&&c.out.commands[0].kind==ph::CommandKind::Draw&&c.out.commands[0].key==1901&&c.out.commands[0].tick==c.in.tick&&c.out.commands[0].sample==c.in.sample,
     "next known no-owner snapshot delivers exactly one draw with current occurrence identity");
  c.step(PhysHolsterGestures6698,0);ck(!c.out.commandCount,"same held Grip cannot repeat resumed draw");
  Case claim(s);claim.latch(PhysHolsterGestures6698);claim.step(PhysHolsterGestures6698,1u<<s,true);
  ck(!claim.out.commandCount&&claim.state[0].comparison114.latched,"unknown current Talk claim cannot grant new equipment");
  claim.step(PhysHolsterGestures6698,1u<<s);
  ck(claim.out.commandCount==1&&claim.out.commands[0].kind==ph::CommandKind::Draw&&claim.state[0].comparison114.pending,
     "known later Talk cannot steal the already latched held holster Grip");
  claim.step(PhysHolsterGestures6698,0);ck(!claim.out.commandCount,"Talk revocation cannot duplicate the pending native draw");
  for(unsigned change=0;change<8;++change){Case x(s);x.latch(PhysHolsterGestures6698);
   if(change==0)x.in.allowed=false;if(change==1)x.in.tracked[s]=false;if(change==2)x.in.intentTracked[s]=false;
   if(change==3)++x.gear.items[0].profile.key;if(change==4)++x.gear.items[0].profile.generation;if(change==5)++x.in.epoch;
   if(change==6)x.gear.handKey[s]=999;if(change==7)x.in.grip[s]=0;
   x.step(PhysHolsterGestures6698,0,true);if(x.out.commandCount||x.state[0].comparison114.latched)std::printf("CONTEXT A side=%d change=%u commands=%u latched=%d pending=%d handKey=%llu item=%llu\n",s,change,x.out.commandCount,x.state[0].comparison114.latched,x.state[0].comparison114.pending,(unsigned long long)x.gear.handKey[s],(unsigned long long)x.gear.items[0].profile.key);ck(!x.out.commandCount&&!x.state[0].comparison114.latched,
      "unknown observation retains actual context/tracking/item/request/current-hand/raw-release invalidation");
   x.in.allowed=true;x.in.tracked[s]=x.in.intentTracked[s]=true;x.gear.handKey[s]=0;x.in.grip[s]=1;x.step(PhysHolsterGestures6698,0);
   if(x.out.commandCount)std::printf("CONTEXT B side=%d change=%u commands=%u kind=%d latched=%d pending=%d handKey=%llu item=%llu\n",s,change,x.out.commandCount,int(x.out.commands[0].kind),x.state[0].comparison114.latched,x.state[0].comparison114.pending,(unsigned long long)x.gear.handKey[s],(unsigned long long)x.gear.items[0].profile.key);ck(!x.out.commandCount,"known recovery cannot replay acquisition invalidated during contention");
  }
  for(unsigned owner=0;owner<4;++owner){Case x(s);x.latch(PhysHolsterGestures6698);
   if(owner==0)x.in.trigger[s]=1;if(owner==1)x.in.grip[1-s]=x.in.trigger[1-s]=1;
   x.step(PhysHolsterGestures6698,0,true,owner==2?(3u&~(1u<<s)):3u);
   ck(!x.out.commandCount&&x.state[0].comparison114.latched==(owner==1||owner==3),
      "unknown leaves independent same-fist/door arbitration and opposite-hand independence intact");
  }
  Case pending(s);pending.latch(PhysHolsterGestures6698);pending.step(PhysHolsterGestures6698,0);
  ck(pending.out.commandCount==1,"pending control comes from actual new draw, not injected FSM state");
  const auto sent=pending.state[0].comparison114.sent;
  while(pending.in.tick+20<sent+2000)pending.step(PhysHolsterGestures6698,0,true);
  ck(pending.state[0].comparison114.pending&&pending.state[0].comparison114.sent==sent,"contended copies preserve exact pending original2s deadline");
  pending.step(PhysHolsterGestures6698,0,true);
  ck(!pending.out.commandCount&&!pending.state[0].comparison114.pending&&pending.state[0].comparison114.sent==sent,
     "existing absolute2s expiry observes unknown without retry/deadline refresh");
  pending.step(PhysHolsterGestures6698,0);ck(!pending.out.commandCount,"read recovery cannot replay timed-out command");
  Case ack(s);ack.latch(PhysHolsterGestures6698);ack.step(PhysHolsterGestures6698,0);const auto originalSent=ack.state[0].comparison114.sent;
  ack.gear.items[0].held=true;ack.gear.handKey[s]=1901;ack.step(PhysHolsterGestures6698,0,true);
  ck(!ack.out.commandCount&&!ack.state[0].comparison114.pending&&ack.state[0].comparison114.sent==originalSent,
     "exact native hand-key ACK retires original pending draw even with contended Talk");
  ack.in.grip[s]=0;ack.step(PhysHolsterGestures6698,0,true,0);
  ck(ack.out.commandCount==1&&ack.out.commands[0].kind==ph::CommandKind::Holster,
     "held Grip release retains immediate native stow despite unknown Talk/door claim");
  ack.step(PhysHolsterGestures6698,0,true,0);ck(!ack.out.commandCount,"held release cannot repeat native stow");

  {Case x(s);x.latch(PhysHolsterGestures6698);
   x.gear.count=2;x.gear.items[1]=x.gear.items[0];x.gear.items[1].set=2;x.gear.items[1].profile.key=1902;
   x.gear.selected=2;x.step(PhysHolsterGestures6698,0,true);
   ck(!x.out.commandCount&&!x.state[0].comparison114.latched,"unknown uncommitted acquisition observes actual selected-loadout replacement");
   x.gear.selected=1;x.step(PhysHolsterGestures6698,0);ck(!x.out.commandCount,"returning selected set cannot revive its invalidated old Grip latch");
  }
  if(s==0){Case x(0);x.latch(PhysHolsterGestures6698);const bool reload=g_bow_manual_reload146.exchange(true);
   x.gear.count=2;x.gear.items[1]=x.gear.items[0];x.gear.items[1].profile.key=1902;x.gear.items[1].profile.side=1;x.gear.items[1].profile.classId=9;x.gear.items[1].held=true;
   x.gear.handKey[1]=1902;x.step(PhysHolsterGestures6698,0,true);
   ck(!x.out.commandCount&&!x.state[0].comparison114.latched,"unknown uncommitted acquisition observes actual manual-bow quiver ownership");
   g_bow_manual_reload146=reload;x.gear.count=1;x.gear.handKey[1]=0;x.step(PhysHolsterGestures6698,0);
   ck(!x.out.commandCount,"quiver ownership ending cannot revive a rejected Grip latch");
  }
  for(unsigned invalid=0;invalid<2;++invalid){Case x(s);if(invalid)x.gear.items[0].profile.side=-1;else x.gear.items[0].profile.classId=12;
   x.step(PhysHolsterGestures6698,0,true);ck(!x.out.commandCount,"unsupported or invalid native side cannot enter unknown hand observation");}
 }

 // Uncommitted legacy acquisition retains its original unknown-copy reset.
 // Its issued-request observer above remains separate; no legacy continuation
 // policy is introduced by the primary-handling correction.
 for(unsigned nativeHand=0;nativeHand<3;++nativeHand){Case x(1);x.in.requirePrimaryGrip114=false;x.gear.items[0].profile.classId=8;
  PhysGripBank6698 bank{};bank.grip[0].key=1901;bank.grip[0].generation=12;bank.grip[0].mountReady=true;
  bank.grip[0].mount.p={0,0,0};g_phys_grips6698.publish(bank);x.in.hands[1].p={0,0,0};
  x.step(PhysHolsterGestures6698,0);x.in.grip[1]=1;x.step(PhysHolsterGestures6698,0);
  ck(x.state[0].latched,"legacy control starts from actual original native-mount acquisition latch");
  if(nativeHand==1)x.gear.handKnown[1]=false;if(nativeHand==2)x.gear.handKey[1]=999;
  x.in.hands[1].p={0,.4,0};x.step(PhysHolsterGestures6698,0,true);
  ck(!x.out.commandCount&&x.state[0].latched==(nativeHand!=0)&&!x.state[0].pending&&bool(x.state[0].key)==(nativeHand!=0),
     "legacy unknown copy retains exact parent known-empty versus unknown/occupied native-hand filtering");
  x.step(PhysHolsterGestures6698,0);ck(!x.out.commandCount&&x.state[0].latched==(nativeHand!=0),
     "legacy known recovery retains literal parent reset/observation without extending its acquisition policy");
 }
 // Current integrated knife supply uses left chest, so hip.stock is compile-
 // time false in this producer. Both proposed Equipment and the pure retained
 // Stock selection FSM are still checked without pretending Stock is active.
 for(unsigned change=0;change<8;++change){Case h(0);h.gear.items[0].profile.classId=3;
  const auto axes=ph::body_axes(h.in);const auto centre=h.in.shoulders[0]-axes.up*.50+axes.right*.03;
  h.in.hands[0].p=centre;h.step(PhysHolsterGestures6698,0);h.in.grip[0]=1;h.step(PhysHolsterGestures6698,0);
  ck(h.state[0].hipStateT10.owner==kcd2vr::hip_t10::Owner::Equipment&&h.state[0].hipStateT10.closed,"actual current hip Equipment selection starts from a real open/close cycle");
  if(change==0){h.in.grip[0]=0;h.step(PhysHolsterGestures6698,0,true);ck(!h.out.hipGrantT10.key&&h.out.hipGrantT10.owner==kcd2vr::hip_t10::Owner::None,"unknown raw release publishes no hip grant");h.in.grip[0]=1;h.in.hands[0].p=centre+ph::Vec3{0,.7,0};}
  if(change==1)++h.in.bodyEpoch;if(change==2)++h.in.epoch;if(change==3)++h.gear.items[0].profile.key;if(change==4)++h.gear.items[0].profile.generation;
  if(change==5)h.in.allowed=false;if(change==6)h.in.tick+=101;if(change==7)h.in.sample=0;
  h.step(PhysHolsterGestures6698,0,true);
  ck(h.out.hipGrantT10.owner==kcd2vr::hip_t10::Owner::None&&!h.out.hipGrantT10.key,"unknown current hip observation never publishes Stock/Equipment grant");
  h.in.allowed=true;h.in.hands[0].p=centre;h.step(PhysHolsterGestures6698,0);
  ck(h.out.hipGrantT10.owner==kcd2vr::hip_t10::Owner::None,"raw cycle/source/context/age/sample transition cannot resurrect previous hip grant");
 }
 {namespace hip=kcd2vr::hip_t10;hip::State state{};hip::Input input{};input.allowed=input.stock=true;input.body=input.request=input.sample=1;input.tick=100;input.distance[0]=1;input.distance[1]=0;input.grip=0;
  (void)hip::update(state,input);input.tick+=20;++input.sample;input.grip=1;(void)hip::update(state,input);ck(state.owner==hip::Owner::Dagger,"retained pure Stock cycle has a real selected family");
  input.tick+=20;++input.sample;input.grip=0;(void)hip::update(state,input);input.tick+=20;++input.sample;input.grip=1;input.distance[1]=1;(void)hip::update(state,input);
  input.tick+=20;++input.sample;input.distance[1]=0;ck(hip::update(state,input).owner==hip::Owner::None,"retained pure Stock release/reclose observes changed reach instead of resurrecting closed owner");
 }
}
// R15: priority applies to the current same-hand acquisition, not lost tracking,
// released Grip or another hand. These use the actual modern producer/adapter.
static void PriorityLossR15(){
 for(int side=0;side<2;++side){
  for(unsigned loss=0;loss<5;++loss){Case c(side);c.latch(PhysHolsterGestures6698);
   if(loss==0)c.in.tracked[side]=false;
   if(loss==1)c.in.intentTracked[side]=false;
   if(loss==2)c.in.grip[side]=0;
   if(loss==3)c.in.allowed=false;
   if(loss==4)c.in.tracked[1-side]=c.in.intentTracked[1-side]=false;
   c.step(PhysHolsterGestures6698,1u<<side);
   if(loss==4){
    ck(c.out.commandCount==1&&c.out.commands[0].kind==ph::CommandKind::Draw,
       "opposite-hand tracking loss does not steal current holster Grip");
    c.step(PhysHolsterGestures6698,0);ck(!c.out.commandCount,"opposite-hand recovery cannot repeat issued Draw");
   }else{
    ck(!c.out.commandCount&&!c.state[0].comparison114.latched,
       "real same-hand tracking/intent/release/gameplay loss cancels unissued draw despite later Talk");
    c.in.tracked[side]=c.in.intentTracked[side]=c.in.allowed=true;c.in.grip[side]=1;
    c.step(PhysHolsterGestures6698,0);ck(!c.out.commandCount,
       "context recovery cannot replay the cancelled outward pull from still-held Grip");
   }
  }
  {Case other(side);other.latch(PhysHolsterGestures6698);other.step(PhysHolsterGestures6698,1u<<(1-side));
   ck(other.out.commandCount==1&&other.out.commands[0].side==side,
      "opposite-hand Talk claim does not steal this holster acquisition");}
  {Case first(side);first.in.grip[side]=1;first.step(PhysHolsterGestures6698,1u<<side);
   ck(!first.out.commandCount&&!first.state[0].comparison114.latched,
      "a preexisting same-hand Talk claim still prevents a new holster acquisition");
   first.step(PhysHolsterGestures6698,0);ck(!first.out.commandCount&&!first.state[0].comparison114.latched,
      "preexisting-owner release does not turn its consumed Grip into a deferred acquisition");}
 }
}
static void SwitchRepresentationR15(){
 for(unsigned mode=0;mode<6;++mode){Case c(0);c.gear.selected=0;
  c.gear.items[0].set=1;c.gear.count=2;
  auto& held=c.gear.items[1];held.profile.key=902;held.profile.generation=3;held.profile.classId=4;held.profile.side=0;held.set=0;held.slot126=0;held.held=true;
  c.gear.handKey[0]=902;
  if(mode==0)c.gear.handKey[0]=999; // no represented held item
  if(mode==2)c.gear.handKey[1]=902; // one two-handed native item
  if(mode==3||mode==4){c.gear.count=3;c.gear.items[2]=held;c.gear.items[2].profile.key=903;c.gear.items[2].profile.side=1;c.gear.items[2].profile.classId=8;
   c.gear.handKey[1]=mode==3?903:999;}
  if(mode==5)held.held=false; // key alone is not current native ownership
  c.in.grip[0]=1;c.step(PhysHolsterGestures6698);for(unsigned n=0;n<4;++n)c.step(PhysHolsterGestures6698);
  c.in.hands[0].p=c.mount+ph::Vec3{0,-.35,0};c.step(PhysHolsterGestures6698);
  const bool valid=mode==1||mode==2||mode==3;
  if(valid){
   ck(c.out.commandCount==(mode==3?2u:1u)&&c.state[0].switchHS2.active,
      "represented current held keys retain native stow half of cross-loadout switch");
   for(unsigned j=0;j<c.out.commandCount;++j)ck(c.out.commands[j].kind==ph::CommandKind::Holster&&c.out.commands[j].key!=1901,
      "legitimate switch stows represented current equipment, never its wanted item");
   for(unsigned n=0;n<5;++n){c.step(PhysHolsterGestures6698);ck(!c.out.commandCount,"represented switch cannot repeat native stow while waiting");}
   c.gear.handKey[0]=c.gear.handKey[1]=0;for(int j=1;j<c.gear.count;++j)c.gear.items[j].held=false;
   c.step(PhysHolsterGestures6698);ck(c.out.commandCount==1&&c.out.commands[0].kind==ph::CommandKind::Draw&&c.state[0].comparison114.pending,
      "native hand clearance advances represented switch to one pending Draw");
   c.step(PhysHolsterGestures6698);ck(!c.out.commandCount,"same held Grip cannot repeat deferred Draw");
  }else{
   ck(!c.out.commandCount&&!c.state[0].comparison114.latched&&!c.state[0].switchHS2.active,
      "unrepresented native hand key cannot acquire or issue a stowless switch");
   c.gear.handKey[0]=c.gear.handKey[1]=0;for(int j=1;j<c.gear.count;++j)c.gear.items[j].held=false;
   c.step(PhysHolsterGestures6698);ck(!c.out.commandCount,"unrepresented-owner recovery cannot replay held Grip draw");
  }
 }
 // Native key publication can precede held attachment ACK by one frame.
 {Case c(0);c.latch(PhysHolsterGestures6698);c.step(PhysHolsterGestures6698);const auto sent=c.state[0].comparison114.sent;
  c.gear.handKey[0]=1901;c.step(PhysHolsterGestures6698,1);
  ck(!c.out.commandCount&&c.state[0].comparison114.pending&&c.state[0].comparison114.sent==sent,
     "matching native hand key before held attachment preserves original pending clock");
  c.gear.items[0].held=true;c.step(PhysHolsterGestures6698,1);
  ck(!c.out.commandCount&&!c.state[0].comparison114.pending&&c.state[0].comparison114.sent==sent,
     "real held-item ACK follows matching native key without resubmission");
 }
 // The mismatch must retire even when the new owner IS represented: an issued
 // Draw cannot borrow the fresh-switch eligibility exception.
 {Case c(0);c.latch(PhysHolsterGestures6698);c.step(PhysHolsterGestures6698);
  ck(c.out.commandCount==1&&c.state[0].comparison114.pending,"pending owner control issues actual Draw");
  c.gear.count=2;auto& held=c.gear.items[1];held.profile.key=902;held.profile.generation=3;held.profile.classId=4;held.profile.side=0;held.set=0;held.slot126=0;held.held=true;c.gear.handKey[0]=902;
  c.step(PhysHolsterGestures6698,1);
  ck(!c.out.commandCount&&!c.state[0].comparison114.pending,"represented different native hand owner retires issued Draw without new switch");
 }
}
int main(){
 SwitchRepresentationR15();
 PriorityLossR15();
 MailboxContentionH4();
 g_pose_epoch6689=1;g_weapon_request_epoch6694=1;g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;
 for(int side=0;side<2;++side)for(bool queueFull:{false,true}){g_phys_commands6698.head=g_phys_commands6698.tail=0;PhysEquipment6698 gear{};gear.valid=true;gear.epoch=gear.request=1;gear.player=reinterpret_cast<void*>(123);gear.count=1;gear.selected=1;gear.handKnown[0]=gear.handKnown[1]=true;
  auto& item=gear.items[0];item.profile.side=side;item.profile.classId=side?9:7;item.profile.key=901;item.profile.generation=2;item.set=1;item.slot126=0;
  ph::Input in{};in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;in.requirePrimaryGrip114=in.allowed=true;in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};ph::Vec3 mount{};ph::holster_mount126(in,item.profile,mount);in.hands[side].p=mount;
  std::array<ph::HolsterState,8> states{};ph::Output out{};
  auto step=[&](unsigned mask){in.tick+=20;++in.sample;hostTickCount64=in.tick;gear.tick=in.tick;g_phys_equipment6698.publish(gear);PublishQuickTalkOwnerH4(mask,in.tick);out={};PhysHolsterGestures6698(in,gear,out,states);};
  step(0);in.grip[side]=1;step(0);ck(states[0].comparison114.latched,"actual ordinary empty-hand holster-zone grab latches before arbitration");
  in.hands[side].p.y+=.45;for(int n=0;n<4;++n)step(1u<<side);
  ck(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Draw&&states[0].comparison114.pending,
     "later same-hand Talk preserves the already latched holster Grip through original dwell and pull");
  const auto original=states[0].comparison114;const auto issued=out.commands[0];
  if(queueFull){ph::Command filler{};for(unsigned n=0;n<g_phys_commands6698.q.size()-1;++n)(void)g_phys_commands6698.push(filler);}
  ck(g_phys_commands6698.push(issued)!=queueFull,"actual producer command enqueue reports success or actual full-ring failure");
  for(int n=0;n<4;++n)step(1u<<side);
  const auto& observed=states[0].comparison114;
  ck(!out.commandCount&&observed.pending&&observed.wantDrawn&&observed.sent==original.sent&&observed.key==original.key&&observed.generation==original.generation&&observed.epoch==original.epoch,"later Talk observes issued primary draw without erasing/recreating its identity or original sent clock");
  ck(kcd2vr::equipment114::PendingDrawBlocks(observed,item.profile.key,item.profile.generation,in.epoch,in.tick,false),"issued draw retains actual cross-item pending exclusion under later Talk");
  ph::Command queued{};unsigned actualDraws=0;while(g_phys_commands6698.pop(queued)){if(queued.kind==ph::CommandKind::Draw){++actualDraws;ck(queued.tick==issued.tick&&queued.sample==issued.sample&&queued.key==issued.key,"producer observation never rewrites actual ring command");}}
  ck(actualDraws==(queueFull?0u:1u),"later pending observation neither invents a failed enqueue nor duplicates a successful enqueue");
  // A failed ring insertion has the same pre-existing pending FSM, not native
  // ownership. Observation may only expire it at the original deadline.
  while(in.tick+20<original.sent+2000)step(1u<<side);
  ck(states[0].comparison114.pending&&states[0].comparison114.sent==original.sent,"pending exists only until original2s deadline, independent of Talk and enqueue success");
  step(1u<<side);
  ck(!out.commandCount&&!states[0].comparison114.pending&&states[0].comparison114.sent==original.sent,"later Talk cannot refresh original2s pending timeout or create a retry after absent queue delivery");
  step(0);ck(!out.commandCount,"Talk close cannot replay timed-out issued command from held Grip");
  q::Stamp s{};q::mailbox.read(s);ck(!s.mask,"actual source publisher revokes mask when no gesture owns hand");
  PublishQuickTalkOwnerH4(1u<<side,in.tick);q::mailbox.read(s);ck(s.mask==(1u<<side)&&s.player==123&&s.body==1&&s.request==1,"actual publisher pins same copied current empty equipment owner");
  g_ctx_menu=true;PublishQuickTalkOwnerH4(1u<<side,in.tick);q::mailbox.read(s);ck(!s.mask,"actual menu context prevents physical holster claim publication");g_ctx_menu=false;
  gear.handKey[side]=901;g_phys_equipment6698.publish(gear);PublishQuickTalkOwnerH4(1u<<side,in.tick);q::mailbox.read(s);ck(!s.mask,"actual item becoming held revokes former empty-hand talk claim");
  in.grip[side]=0;step(0);in.grip[side]=1;item.held=true;step(1u<<side);in.grip[side]=0;step(1u<<side);ck(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Holster,"actual newly held item retains ordinary deliberate Grip-release holster after failed-request neutral rearm");
 }
 // Native ACK and source changes are observed by the same production FSM;
 // pending is not a utility lease or proof that a native command was accepted.
 for(unsigned change=0;change<4;++change){PhysEquipment6698 gear{};gear.valid=true;gear.epoch=gear.request=1;gear.player=reinterpret_cast<void*>(123);gear.count=1;gear.selected=1;gear.handKnown[0]=gear.handKnown[1]=true;
  auto& item=gear.items[0];item.profile.side=0;item.profile.classId=7;item.profile.key=901;item.profile.generation=2;item.set=1;item.slot126=0;
  ph::Input in{};in.epoch=in.bodyEpoch=1;in.tick=2000;in.sample=1;in.allowed=in.requirePrimaryGrip114=true;in.grip[0]=1;in.tracked[0]=in.intentTracked[0]=true;
  std::array<ph::HolsterState,8> states{};auto& pending=states[0].comparison114;pending.initialized=pending.pending=pending.wantDrawn=true;pending.key=901;pending.generation=2;pending.epoch=1;pending.tick=1980;pending.sent=1950;const auto sent=pending.sent;
  if(change==0){item.held=true;gear.handKey[0]=901;}if(change==1)++item.profile.generation;if(change==2)++in.epoch;if(change==3)gear.handKey[0]=902;
  hostTickCount64=in.tick;gear.tick=in.tick;g_phys_equipment6698.publish(gear);PublishQuickTalkOwnerH4(1,in.tick);ph::Output out{};PhysHolsterGestures6698(in,gear,out,states);
  if(out.commandCount||pending.pending)std::printf("CONTEXT C change=%u commands=%u pending=%d handKey=%llu item=%llu\n",change,out.commandCount,pending.pending,(unsigned long long)gear.handKey[0],(unsigned long long)item.profile.key);ck(!out.commandCount&&!pending.pending,"actual native ACK or original generation/request/current-hand mismatch retires pending without native retry");
  if(change==0)ck(pending.sent==sent,"native ACK retains original sent clock; no replacement request is synthesized");
 }
 // Retained legacy producer: same existing observation path, original timeout,
 // no fabricated Draw while a later Talk claim owns new acquisition.
 {PhysEquipment6698 gear{};gear.valid=true;gear.epoch=gear.request=1;gear.player=reinterpret_cast<void*>(123);gear.count=1;gear.handKnown[0]=gear.handKnown[1]=true;auto& item=gear.items[0];item.set=0;item.profile.side=1;item.profile.classId=8;item.profile.key=902;item.profile.generation=3;
  PhysGripBank6698 bank{};bank.grip[0].key=902;bank.grip[0].generation=3;bank.grip[0].mountReady=true;bank.grip[0].mount.p={0,0,0};g_phys_grips6698.publish(bank);
  ph::Input in{};in.epoch=in.bodyEpoch=1;in.tick=3000;in.sample=1;in.allowed=true;in.tracked[1]=in.intentTracked[1]=true;in.grip[1]=1;in.hands[1].p={0,.4,0};
  std::array<ph::HolsterState,8> states{};auto& pending=states[0];pending.pending=pending.expectedDrawn=true;pending.epoch=1;pending.key=902;pending.last=2950;pending.lastTick=2980;
  for(unsigned n=0;n<3;++n){hostTickCount64=in.tick;gear.tick=in.tick;g_phys_equipment6698.publish(gear);PublishQuickTalkOwnerH4(2,in.tick);ph::Output out{};PhysHolsterGestures6698(in,gear,out,states);ck(!out.commandCount&&pending.pending&&pending.last==2950&&pending.key==902,"later Talk preserves legacy pending original identity/deadline without emitting new command");in.tick+=20;}
 }
 // Actual producer: neutral outside -> Grip-only reach -> original dwell/pull.
 // Fist, door and current item/source identity retain their existing owners.
 for(int side=0;side<2;++side)for(unsigned mode=0;mode<7;++mode){
  g_pose_epoch6689=1;g_weapon_request_epoch6694=1;
  PhysEquipment6698 gear{};gear.valid=true;gear.epoch=gear.request=1;gear.player=reinterpret_cast<void*>(123);
  gear.count=1;gear.selected=1;gear.handKnown[0]=gear.handKnown[1]=true;
  auto& item=gear.items[0];item.profile.side=side;item.profile.classId=side?9:7;
  item.profile.key=1901;item.profile.generation=12;item.set=1;item.slot126=0;
  ph::Input in{};in.epoch=in.bodyEpoch=1;in.tick=5000+side*1000+mode*200;
  in.sample=1;in.requirePrimaryGrip114=in.allowed=true;
  in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;
  in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};
  ph::Vec3 mount{};ph::holster_mount126(in,item.profile,mount);
  in.hands[side].p=mount+ph::Vec3{0,.9,0};
  std::array<ph::HolsterState,8> states{};ph::Output out{};unsigned doorMask=3;
  auto step=[&](){in.tick+=20;++in.sample;hostTickCount64=in.tick;gear.tick=in.tick;
   gear.request=in.epoch;g_phys_equipment6698.publish(gear);PublishQuickTalkOwnerH4(0,in.tick);
   out={};PhysHolsterGestures6698(in,gear,out,states,doorMask);};
  step();in.grip[side]=1;
  if(mode==1)in.trigger[side]=1; // this hand's qualified fist
  if(mode==2)in.grip[1-side]=in.trigger[1-side]=1; // opposite fist is independent
  step();in.hands[side].p=mount;step(); // actual current volume, before outward pull
  for(unsigned n=0;n<3;++n)step();
  if(mode==3)doorMask&=~(1u<<side);
  if(mode==4)++item.profile.generation;
  if(mode==5){++in.epoch;g_weapon_request_epoch6694=in.epoch;}
  if(mode==6)in.intentTracked[side]=false;
  in.hands[side].p=mount+ph::Vec3{0,.35,0};step();
  const bool expected=mode==0||mode==2;
  ck((out.commandCount==1)==expected,"actual carried-Grip draw preserves own-fist/other-fist/door/generation/request/tracking policies");
  if(expected){
   ck(out.commandCount&&out.commands[0].kind==ph::CommandKind::Draw&&out.commands[0].side==side&&
      out.commands[0].key==item.profile.key&&out.commands[0].sample==in.sample&&out.commands[0].tick==in.tick&&
      out.commands[0].epoch==in.epoch&&out.commands[0].bodyEpoch==in.bodyEpoch,
      "actual draw keeps exact item, side, body, request, sample and occurrence clock");
   const auto sent=states[0].comparison114.sent;step();
   ck(!out.commandCount&&states[0].comparison114.pending,"same held Grip produces no duplicate before native ACK");
   item.held=true;gear.handKey[side]=item.profile.key;step();
   ck(!out.commandCount&&!states[0].comparison114.pending&&states[0].comparison114.sent==sent,
     "actual native hand-key ACK retires original draw without a new request");
   doorMask=0;in.trigger[side]=1;in.grip[side]=0;step();
   ck(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Holster,
     "held release stows once despite door admission and Trigger; it is not empty-hand fist acquisition");
   step();ck(!out.commandCount,"held release cannot duplicate stow before native ACK");
  }
 }
 std::printf("quick_talk_holster_h4=%u/%u actualProduction=HOLSTER_AND_PUBLISH nativeCalls=NONE\n",total-fail,total);return fail?1:0;
}
