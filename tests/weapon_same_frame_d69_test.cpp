// Executes the actual production native adapter with bounded, explicit native
// memory/contract doubles. It does not emulate engine thread safety or pixels.
#include "vrik/physical_weapon.h"
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <functional>
namespace ph=kcd2vr::physical;
using namespace kcd2vr;
static int checks=0,failed=0,reads=0,calls=0,getters=0,readbacks=0;
static void ck(bool b,const char*n){++checks;if(!b)++failed;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static constexpr std::uintptr_t base=0x10000000,kCEntityVftRva=0x4072C78;
static std::uintptr_t WhBase(){return base;}
static bool retail=true,head=true,hands[2]{true,true},passenger=false,reference=false,collider=false,locked=false,writeOk=true;
static std::uint64_t now=1020,pub=2,handKey[2]{7,0};
static std::function<void()> getterAction;
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail;}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace kcd2vr::diagnostics::lab {static bool NativeReference(){return reference;}}
namespace heldT4 {static bool HasUnretiredCollider(){return collider;}}
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{5};}
static std::atomic<int> g_weapon_control_mode{4};
static std::atomic<bool> g_phys_fault6698{false},g_gameplay_allowed{true},g_committed_pose_valid6689{true};
static std::atomic<std::uint64_t> g_pose_policy_generation57{2},g_phys_ready_sample6698{10},g_weapon_request_epoch6694{9},g_pose_epoch6689{3};
static std::atomic<void*> g_henry_item_attach_manager{nullptr},g_henry_char_attachment_manager{nullptr},g_committed_character6689{nullptr};
static std::uint64_t GetTickCount64(){return now;} static unsigned GetCurrentThreadId(){return 123;}
static bool NativePassengerCurrentA34(std::uint64_t){return passenger;}
static bool TrackingHeadUsable111(std::uint64_t){return head;}
static bool TrackingHandUsable111(int s,std::uint64_t){return hands[s];}
static std::uint64_t ReadCompletedPublicationT23(){return pub;}
struct PhysTryLock6698 {bool owned=!locked;~PhysTryLock6698()=default;};
struct PhysCorrelation126{pose_core::Stamp stamp{};};
struct PhysFrame6698{ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};std::array<PhysCorrelation126,2> correlation126{};void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;float world[8]{};std::uint64_t trackingEpochT23=0,characterT23=0,publicationT23=0;};
struct PhysEquipmentItem6698{ph::Profile profile{};void* entity=nullptr;bool held=false;};
struct PhysEquipment6698{std::array<PhysEquipmentItem6698,8> items{};int count=0;void* player=nullptr;void* manager=nullptr;std::uint64_t request=0,tick=0,epoch=0;bool valid=false;};
struct CommittedFrame6695{pose_core::Stamp stamp{};float world[8]{};bool valid[2]{};};
struct PhysPlacement6698{void* self=nullptr;void* attachment=nullptr;std::uint64_t key=0;bool changed=false;void* entity=nullptr;void* manager=nullptr;std::uint32_t entityId=0;int side=-1;std::uint64_t generation=0,sourceTick146=0,epoch146=0,request146=0;float wanted[8]{};};
static std::array<PhysPlacement6698,2> g_phys_placement6698;
template<class T>struct Mail{T data{};bool ok=true;bool read(T&v){if(!ok)return false;v=data;return true;}};
static Mail<PhysFrame6698> g_phys_frame6698;
static Mail<PhysEquipment6698> g_phys_equipment6698;
static CommittedFrame6695 body;
static bool bodyOk=true;
static bool PhysReadVisualBodyR24(std::uint64_t p,std::uint64_t e,std::uint64_t t,std::uint64_t c,CommittedFrame6695&out){if(!bodyOk||p!=pub||e!=g_pose_epoch6689.load()||t!=tracking59c6::epoch.load()||c!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()))return false;out=body;return true;}
static bool NativeHandKey6697(void* manager,int side,std::uint64_t&key){key=handKey[side];return manager==g_henry_item_attach_manager.load();}
static std::unordered_map<std::uintptr_t,std::vector<unsigned char>> memory;
template<class T>static void store(std::uintptr_t p,const T&v){auto&b=memory[p];b.resize(sizeof(v));std::memcpy(b.data(),&v,sizeof(v));}
template<class T>static bool Read(const void*p,T&v){++reads;auto i=memory.find(reinterpret_cast<std::uintptr_t>(p));if(i==memory.end()||i->second.size()!=sizeof(v))return false;std::memcpy(&v,i->second.data(),sizeof(v));return true;}
static bool PhysVft6698(void*p,std::uintptr_t r){std::uintptr_t v=0;return p&&Read(p,v)&&v==base+r;}
static bool PhysEntityIdentity135(void*p,std::uint32_t id){std::uint32_t got=0;return PhysVft6698(p,kCEntityVftRva)&&Read(static_cast<unsigned char*>(p)+0xc,got)&&id==got;}
static void* child=nullptr;
static void* CharacterForEntity6689(void*){++getters;if(getterAction)getterAction();return child;}
static ph::Pose PhysWorld6698(const float*q){return {{q[0],q[1],q[2],q[3]},{q[4],q[5],q[6]}};}
static float placed[8]{};
static bool PlaceBothConsumers6689(void*,void*,const float(&qs)[8],bool&has){++calls;std::copy_n(qs,8,placed);has=child!=nullptr;return writeOk;}
static bool PlacementReadback6689(void*,void*,std::uint32_t,const float(&)[8],bool&checked){++readbacks;checked=child!=nullptr;return writeOk;}
static void InvalidateReadCache101R2(){} struct PerfScope101R2{explicit PerfScope101R2(int){}};
namespace kcd2vr::diagnostics::motion_d68 {
 enum Stage {Attachment=10};struct Event{std::uint64_t actor=0,object=0,sample=0,sourceNs=0,sourceTick=0,poseEpoch=0,trackingEpoch=0,publication=0,request=0,cycle=0,session=0,ns=0;int side=0;unsigned thread=0,flags=0,count=0;float v[48]{};};
 static bool active=false;static Event last{};static int pushed=0;
 static bool requested(){return active;}static bool begin(Event&e,Stage){e.session=99;e.ns=123;return active;}static bool inWindow(const Event&){return true;}
 struct Cadence{bool due(std::uint64_t,std::uint64_t){return true;}};
 static bool push(Event&e){last=e;++pushed;return true;}
}
#include "vrik/weapon_same_frame_d69.inl"
static void reset(){
 namespace wt=kcd2vr::weapon_trials_d69;wt::setSameFrame(false);wt::setSameFrame(true);
 memory.clear();reads=calls=getters=readbacks=0;getterAction={};retail=head=hands[0]=hands[1]=bodyOk=writeOk=true;
 passenger=reference=collider=locked=false;child=nullptr;now=1020;pub+=2;
 g_weapon_control_mode=4;animation_filter::nativeBodyOwned112=false;g_phys_fault6698=false;g_gameplay_allowed=true;g_committed_pose_valid6689=true;
 g_pose_policy_generation57=2;g_phys_ready_sample6698=10;g_weapon_request_epoch6694=9;g_pose_epoch6689=3;tracking59c6::epoch=5;handKey[0]=7;
 auto self=reinterpret_cast<void*>(0x1000),att=reinterpret_cast<void*>(0x2000),entity=reinterpret_cast<void*>(0x3000),manager=reinterpret_cast<void*>(0x4000),charmanager=reinterpret_cast<void*>(0x5000),character=reinterpret_cast<void*>(0x6000),player=reinterpret_cast<void*>(0x7000);
 g_henry_item_attach_manager=manager;g_henry_char_attachment_manager=charmanager;g_committed_character6689=character;
 g_phys_frame6698={};g_phys_equipment6698={};g_phys_placement6698={};body={};
 auto&f=g_phys_frame6698.data;f.input.allowed=true;f.input.sample=10;f.input.tick=1010;f.input.timeNs=1010000000;f.input.bodyEpoch=3;f.input.tracked[0]=true;
 f.output.active[0]=f.output.owned[0]=true;f.output.weapons[0].p={.2,.3,1.2};
 f.profiles[0].key=7;f.profiles[0].generation=8;f.profiles[0].primary=true;f.profiles[0].entity=20;f.profiles[0].side=0;
 f.correlation126[0].stamp.actor=reinterpret_cast<std::uintptr_t>(character);f.characterT23=reinterpret_cast<std::uintptr_t>(character);f.publicationT23=pub;f.trackingEpochT23=5;f.request=9;f.player=player;f.manager=manager;
 f.world[3]=f.world[7]=1;f.world[4]=100;f.world[5]=200;f.world[6]=300;
 body.stamp.actor=f.characterT23;body.stamp.sample=10;body.stamp.tick=1010;body.stamp.epoch=3;body.valid[0]=true;std::copy_n(f.world,8,body.world);
 auto&e=g_phys_equipment6698.data;e.valid=true;e.count=1;e.player=player;e.manager=manager;e.request=9;e.tick=1010;e.epoch=3;e.items[0].profile=f.profiles[0];e.items[0].entity=entity;e.items[0].held=true;
 auto&r=g_phys_placement6698[0];r.self=self;r.attachment=att;r.key=7;r.changed=true;r.entity=entity;r.manager=manager;r.entityId=20;r.side=0;r.generation=8;r.sourceTick146=1001;r.epoch146=3;r.request146=9;r.wanted[3]=r.wanted[7]=1;
 store(0x1000,base+0x3A68F68);store(0x1008,std::uint32_t(20));store(0x2000,base+0x3A657C0);store(0x2020,self);store(0x2028,charmanager);store(0x5070,character);store(0x6000,base+0x40602F8);store(0x3000,base+kCEntityVftRva);store(0x300c,std::uint32_t(20));
 store(base+kCEntityVftRva+0x168,base+0x509810);store(base+kCEntityVftRva+0x328,base+0x6411AC);store(base+0x40602F8+0x190,base+0x423C30);
 kcd2vr::diagnostics::motion_d68::active=false;kcd2vr::diagnostics::motion_d68::pushed=0;
}
static void run(){PhysSameFrameD69(pub,10,99,88);}
int main(){
 reset();kcd2vr::weapon_trials_d69::setSameFrame(false);run();ck(reads==0&&calls==0&&getters==0,"OFF no extra native reads or engine calls");
 reset();const auto saved=g_phys_placement6698[0];run();ck(calls==1&&std::abs(placed[4]-100.2f)<1e-5&&std::abs(placed[5]-200.3f)<1e-5,"same completed frame converted with exact packet world");
 ck(std::memcmp(&saved,&g_phys_placement6698[0],sizeof(saved))==0,"trial does not renew attachment source/grace clocks");run();ck(calls==1,"same publication attempted at most once");
 const std::vector<std::function<void()>> refuse{
 []{retail=false;},[]{g_weapon_control_mode=3;},[]{animation_filter::nativeBodyOwned112=true;},[]{passenger=true;},[]{reference=true;},[]{g_phys_fault6698=true;},[]{g_gameplay_allowed=false;},[]{head=false;},[]{hands[0]=false;},[]{collider=true;},[]{locked=true;},[]{g_pose_policy_generation57=3;},[]{g_phys_ready_sample6698=9;},[]{g_phys_frame6698.ok=false;},[]{g_phys_equipment6698.data.valid=false;},[]{g_phys_frame6698.data.input.allowed=false;},[]{g_weapon_request_epoch6694=10;},[]{g_pose_epoch6689=4;},[]{g_phys_frame6698.data.player=nullptr;},[]{now=1100;},[]{bodyOk=false;},[]{body.stamp.sample=9;},[]{body.world[4]+=1;},[]{body.valid[0]=false;},[]{g_phys_frame6698.data.output.owned[0]=false;},[]{g_phys_frame6698.data.output.solverOwnedT4[0]=true;},[]{g_phys_frame6698.data.profiles[0].generation++;},[]{g_phys_placement6698[0].sourceTick146=900;},[]{g_phys_equipment6698.data.items[0].held=false;},[]{handKey[0]=8;},[]{store(0x2020,reinterpret_cast<void*>(0x1234));},[]{store(0x5070,reinterpret_cast<void*>(0x1234));},[]{store(0x1000,base+0x3A68F69);},[]{store(base+kCEntityVftRva+0x168,base+0x509811);},[]{store(base+kCEntityVftRva+0x328,base+0x6411AD);},[]{g_phys_frame6698.data.output.weapons[0].p.x=std::nan("");}
 };
 for(auto&f:refuse){reset();f();run();ck(calls==0,"incomplete/foreign/stale/native/menu/loss authority produces no placement");}
 for(int mode=0;mode<12;++mode){reset();getterAction=[mode]{switch(mode){case 0:g_gameplay_allowed=false;break;case 1:g_pose_policy_generation57+=2;break;case 2:handKey[0]=9;break;case 3:kcd2vr::weapon_trials_d69::setSameFrame(false);break;case 4:pub+=2;break;case 5:g_weapon_control_mode=3;break;case 6:retail=false;break;case 7:g_weapon_request_epoch6694=99;break;case 8:g_phys_fault6698=true;break;case 9:collider=true;break;case 10:now+=200;break;case 11:store(0x2028,reinterpret_cast<void*>(0x9000));break;}};run();ck(calls==0,"native getter cannot preserve revoked preflight authority");}
 reset();child=reinterpret_cast<void*>(0x8000);store(0x8000,base+0x40602F8);run();ck(calls==1,"qualified character-bearing weapon uses existing dual consumer leaf");
 reset();child=reinterpret_cast<void*>(0x8000);store(0x8000,base+0x40602F8);store(base+0x40602F8+0x190,base+1);run();ck(calls==0,"wrong child update slot rejects before first write");
 reset();writeOk=false;run();run();ck(calls==1,"failed native setter is not retried within publication or given new grace");
 reset();namespace md=kcd2vr::diagnostics::motion_d68;md::active=true;run();ck(md::pushed==1&&md::last.cycle==88&&md::last.session==99&&md::last.publication==pub&&md::last.sample==10&&(md::last.flags&0x10000)&&readbacks==1,"finite ShiftF2 receipt joins CPU cycle/publication plus actual write/readback status");
 reset();md::active=true;PhysSameFrameD69(pub,10,98,88);ck(calls==1&&md::pushed==0,"different diagnostic session cannot borrow old cycle");
 std::printf("D69 same-frame adapter %d/%d\n",checks-failed,checks);return failed?1:0;
}
