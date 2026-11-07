// D76 review: exact visibility owner, preserved strict melee graph, scoped native call, full
// physical-mode dispatch prefix, T4 and crossbow weapon attachment callbacks.
// Entity registry/writes, tracking/equipment publication and physical solver
// output are host leaves; this is not a renderer or headset test.
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "core/snapshot_mailbox.h"
#include "vrik/physical_weapon.h"
#include "subsystems/physics/runtime/native_weapon_body_t4.h"
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include "subsystems/ranged_reload/ranged_draw_policy_d72.h"
#include "subsystems/ranged_reload/weapon_draw_scope_d75.h"
#include "subsystems/ranged_reload/weapon_kind_xr32.h"
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;
namespace heldT4=kcd2vr::native_weapon_body_t4;namespace xcXR34=kcd2vr::crossbow_native_xr34;
struct Block {std::array<unsigned char,0xB00> data{};void* ptr(){return data.data();}};
struct Item {Block item{},link{},entity{},object{},attachment{},definition{},classification{};
    std::uint64_t key=100,generation=1;unsigned id=10;int side=0,classId=4;
    bool present=true,hidden=false,placed=false;};
static Item items[3];static Block itemManager{},player{},slots{},charManager{},character{};
static std::vector<Block*> memory;
static std::uintptr_t denied=0;static bool identityReads=true;
static constexpr std::uintptr_t base=0x10000000,kCEntityVftRva=0x4072C78;
static std::uintptr_t WhBase(){return base;}
template<class T>static void put(Block& b,std::size_t offset,const T& value){std::memcpy(b.data.data()+offset,&value,sizeof(T));}
template<class T>static bool Read(const void* p,T& out){
    if(reinterpret_cast<std::uintptr_t>(p)==denied||!identityReads)return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    for(auto* block:memory){const auto lo=reinterpret_cast<std::uintptr_t>(block->data.data());
        if(a>=lo&&a-lo<=block->data.size()&&sizeof(T)<=block->data.size()-(a-lo)){std::memcpy(&out,p,sizeof(T));return true;}}
    return false;
}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t v=0;return p&&Read(p,v)&&v==base+rva;}
static Item* forEntity(void* e){for(auto& i:items)if(i.entity.ptr()==e)return &i;return nullptr;}
static void* EntityById6694(unsigned id){for(auto& i:items)if(i.present&&i.id==id)return i.entity.ptr();return nullptr;}
static bool hiddenReads=true,hideInvokes=true,hideWorks=true,showWorks=true;
static unsigned hides=0,shows=0;
static bool EntityHiddenFlag59c14(void* e,bool& h){auto* i=forEntity(e);if(!i||!hiddenReads)return false;h=i->hidden;return true;}
static bool EntityHide59c14(void* e,bool h,bool* invoked=nullptr){if(invoked)*invoked=hideInvokes;
    if(!hideInvokes)return false;auto* i=forEntity(e);if(!i)return false;
    if(h){++hides;if(hideWorks)i->hidden=true;return hideWorks;}
    ++shows;if(showWorks)i->hidden=false;return showWorks;
}
static std::uint64_t now=100;static std::uint64_t GetTickCount64(){return now;}
static std::atomic<int> g_weapon_control_mode{4};
static std::atomic<bool> g_phys_fault6698{false},g_gameplay_allowed{true};
static std::atomic<std::uint64_t> g_pose_epoch6689{3},g_weapon_request_epoch6694{4};
static std::atomic<void*> g_henry_item_attach_manager{nullptr},g_henry_char_attachment_manager{nullptr};
static std::atomic<std::uintptr_t> g_player_char37{0};
static bool retail=true;
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail;}}
static std::atomic<void*> g_native_side_item[2]{},g_native_side_owner[2]{};
static std::atomic<int> g_native_side_item_state[2]{};
static bool immersive=true,tracked=true,passenger=false,lockAvailable=true;
static bool StowedBowImmersive54(){return immersive;}
static bool TrackingHeadUsable111(std::uint64_t){return tracked;}
static bool TrackingHandUsable111(int,std::uint64_t){return tracked;}
static bool NativePassengerCurrentA34(std::uint64_t){return passenger;}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{1};}
struct PhysTryLock6698 {bool owned=lockAvailable;};
struct ReadScope101R2{};static void InvalidateReadCache101R2(){}
struct PhysEquipmentItem6698 {ph::Profile profile{};void* item=nullptr;void* entity=nullptr;void* worldLink54=nullptr;bool held=false;};
struct PhysEquipment6698 {std::array<PhysEquipmentItem6698,8>items{};int count=0;
    void* player=nullptr;void* manager=nullptr;std::uint64_t epoch=0,request=0,tick=0;
    bool valid=false,handKnown[2]{};std::uint64_t handKey[2]{};};
static kcd2vr::core::SnapshotMailbox<PhysEquipment6698> g_phys_equipment6698;
#include "native_hand.inc"
#include "candidate_item.inc"
#include "item_world.inc"
#include "item_description.inc"
#include "entity_identity.inc"
static std::atomic<void*> g_hidden_bow_entity59c14{nullptr};
static std::atomic<std::uint64_t> g_hidden_bow_seen59c14{0},g_bow_shows59c14{0};
static unsigned g_hidden_bow_id59c14=0;
namespace diagnostic_trace {static void Write(const char*){}}
#include "stowed_forget.inc"
#include "stowed_release.inc"
static void StowedBowHide59c14(void* e,std::uint64_t){if(auto* i=forEntity(e))i->hidden=true;}
#include "subsystems/ranged_reload/first_draw_binding_d75.inl"
#include "subsystems/ranged_reload/ranged_draw_visibility_d72.inl"

struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
static ph::Pose PhysPose6698(const QuatT28& q){return {{q.q[0],q.q[1],q.q[2],q.q[3]},{q.t[0],q.t[1],q.t[2]}};}
struct ModelWorld6692{};
struct CommittedFrame6695 {pm::Stamp stamp{};QuatT28 allowed[2]{};bool valid[2]{true,true};std::uint64_t publicationR24=2;};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};ph::Profile profiles[2]{};
    struct Correlation {pm::Stamp stamp{};}correlation126[2];
    std::uint64_t request=0,publicationT23=2,trackingEpochT23=1;};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
static std::atomic<std::uint64_t> g_phys_ready_sample6698{10};
static std::atomic<bool> g_committed_pose_valid6689{true};
static std::uint64_t publication=2;static std::uint64_t ReadCompletedPublicationT23(){return publication;}
static bool writeOK=true,readbackOK=true;static unsigned writes=0,readbacks=0;
static void (*afterReadback)()=nullptr;
static bool PlaceBothConsumers6689(void*,void* e,const float (&)[8],bool& child){++writes;child=false;
    if(auto* i=forEntity(e)){if(writeOK)i->placed=true;return writeOK;}return false;}
static bool PlacementReadback6689(void*,void* e,unsigned id,const float (&)[8],bool& checked){++readbacks;checked=false;
    auto* i=forEntity(e);const bool good=readbackOK&&i&&i->placed&&i->id==id;if(afterReadback)afterReadback();return good;}
static bool t4On=false,t4Receipt=true,t4Current=true;static heldT4::Achieved achieved{};
namespace kcd2vr::native_weapon_body_t4 {
    bool HasUnretiredCollider() noexcept{return t4On;}
    bool ReadRetainedOwner(Owner& o) noexcept{o=achieved.owner;return t4Receipt;}
    bool Read(Achieved& a) noexcept{a=achieved;return t4Receipt;}
    void Retire(const Owner&) noexcept{t4Current=false;}
}
namespace kcd2vr::native_held_pose_t4 {static bool fresh(std::uint64_t n,std::uint64_t t){return t&&n>=t&&n-t<=80;}}
struct NativeHeldSourceT4 {heldT4::Input input{};float attachmentScale=1;bool valid=false;};
static kcd2vr::core::SnapshotMailbox<NativeHeldSourceT4> g_native_held_source_t4;
static std::atomic<std::uintptr_t> g_native_held_attachment_self_t4{0},g_native_held_attachment_t4{0};
static bool NativeHeldSourceCurrentT4(const heldT4::Input&){return t4Current;}
static void NativeHeldInvalidateT4(){t4Current=false;}
enum class PhysicalAttachmentResult101 {NotMine,Waiting,Placed,Restored,Busy,Fault};
#include "native_solver_attachment.inc"

static bool crossbowOn=false,crossbowCurrent=true;static xcXR34::Status crossbowState{};static xcXR34::Source crossbowSource{};
static CommittedFrame6695 body{};
static bool RangedOwnerXR34(const xcXR34::Owner&){return crossbowCurrent;}
static bool ReloadPoseWriterCurrentXR34(const xcXR34::Owner&){return crossbowCurrent;}
static bool RangedItemEntityXR34(void* item,unsigned& id,void*& entity){for(auto& i:items)if(i.item.ptr()==item){id=i.id;entity=i.entity.ptr();return i.present;}return false;}
static bool RangedBoundWeaponIdentityD72(const xcXR34::Owner& o,unsigned id,void* entity,void*& bound){for(auto& i:items)if(i.key==o.source.key&&i.id==id&&i.entity.ptr()==entity){bound=i.attachment.ptr();return true;}return false;}
static bool RangedSourceXR34(xcXR34::Source& s){s=crossbowSource;return crossbowCurrent;}
static bool ReadCommittedPacket6695(CommittedFrame6695& b){b=body;return g_committed_pose_valid6689;}
static bool PhysAttachmentWorld164(void*,std::uintptr_t,ModelWorld6692&){return true;}
static bool ReadAttachmentQs6689(void*,float (&natural)[8]){natural[3]=natural[7]=1;return true;}
static ph::Pose PhysCurrentWorld164(ph::Pose model,const ModelWorld6692&){return model;}
#include "crossbow_wrist.inc"
#include "crossbow_weapon_attachment.inc"
static bool CrossbowAttachmentXR34(void* self,void* attachment){unsigned id=0;return crossbowOn&&Read(static_cast<unsigned char*>(self)+8,id)&&CrossbowWeaponAttachmentD72(crossbowState,self,attachment,id);}

static bool physicalReady=false;static unsigned nativeCalls=0,nativeVisible=0;
static void OriginalAttachment(void* self,void*){++nativeCalls;unsigned id=0;Read(static_cast<unsigned char*>(self)+8,id);
    for(auto& i:items)if(i.id==id&&i.present){i.placed=false;if(!i.hidden)++nativeVisible;break;}}
static auto g_entity_attachment_update_original=&OriginalAttachment;
static PhysicalAttachmentResult101 PhysicalAttachmentUpdate101(void* self,void* attachment){
    if(!physicalReady)return PhysicalAttachmentResult101::Waiting;
    PhysEquipment6698 e{};if(!g_phys_equipment6698.read(e))return PhysicalAttachmentResult101::Waiting;
    unsigned id=0;Read(static_cast<unsigned char*>(self)+8,id);
    for(int n=0;n<e.count;++n){const auto& i=e.items[n];if(i.profile.entity!=id)continue;
        const auto binding=RangedDrawIdentityD72(e,i,self,attachment);RangedDrawBeginD72(binding);
        bool child=false,checked=false;float wanted[8]{0,0,0,1,1,2,3,1};
        const bool wrote=PlaceBothConsumers6689(self,i.entity,wanted,child);
        const bool verified=wrote&&PlacementReadback6689(self,i.entity,id,wanted,checked);
        RangedDrawPlacedD72(binding,wrote,verified);return PhysicalAttachmentResult101::Placed;
    }
    return PhysicalAttachmentResult101::Waiting;
}
#define __fastcall
namespace kcd2vr::hookcost178r22 {enum Kind{AttachmentUpdate};struct Scope{explicit Scope(Kind){}};struct Original{explicit Original(Kind){}};}
static void StationWeaponAfterAttachmentD56(void*,void*){}
struct AttachmentObservation15 {void* self;void* attachment;};
static bool ObserveNativeActivityHand46(void*,void*){return false;}
static bool ManualArrowAttachment64(void*,void*){return false;}
static bool ToolAttachment59c10(void*,void*){return false;}
static std::atomic<void*> g_attachment_pending_self6695{nullptr},g_attachment_pending_object6695{nullptr};
static std::atomic<std::uint64_t> g_phys_waiting_native157{0};
static bool PhysicalAttachmentPending6698(){return false;}
struct AttachmentLock6689 {bool owned=true;};
static struct {void* self=nullptr;void* attachment=nullptr;bool changed=false;}g_attachment6689;
static void MarkWeaponPending6695(void*,void*){}
#include "physical_mode_dispatch.inc"
static bool publishHandBefore=true,nativeAttachOK=true;
static void setHand(int side,std::uint64_t key){put(slots,0x260+side*8,key);}
static bool OriginalAttach(void* owner,void* item,int side){
    for(auto& i:items)if(i.item.ptr()==item){const int actual=side==2?i.side:side;
        if(publishHandBefore&&nativeAttachOK&&owner==itemManager.ptr())setHand(actual,i.key);
        EntityAttachmentUpdateDetourV6688(i.object.ptr(),i.attachment.ptr());
        if(!publishHandBefore&&nativeAttachOK&&owner==itemManager.ptr())setHand(actual,i.key);
        break;}
    return nativeAttachOK;
}
static auto g_item_attach_operation_original=&OriginalAttach;
#include "native_attach_scope.inc"

static unsigned checks=0;
static void ck(bool ok,const char* message){++checks;if(!ok){std::fprintf(stderr,"FAIL %u: %s\n",checks,message);std::exit(1);}}
static void publish(Item& i,bool held=true){
    PhysEquipment6698 e{};e.player=player.ptr();e.manager=itemManager.ptr();e.epoch=g_pose_epoch6689;e.request=g_weapon_request_epoch6694;e.tick=now;e.valid=true;
    for(int side=0;side<2;++side){e.handKnown[side]=NativeHandKey6697(itemManager.ptr(),side,e.handKey[side]);}
    e.count=1;e.items[0].item=i.item.ptr();e.items[0].entity=i.entity.ptr();e.items[0].worldLink54=i.link.ptr();e.items[0].held=held;
    auto& p=e.items[0].profile;p.key=i.key;p.generation=i.generation;p.entity=i.id;p.classId=i.classId;p.side=i.side;
    g_phys_equipment6698.publish(e);
    g_native_side_item[i.side]=i.item.ptr();g_native_side_owner[i.side]=itemManager.ptr();g_native_side_item_state[i.side]=1;
}
static rdrawD72::Identity binding(Item& i){return {reinterpret_cast<std::uintptr_t>(i.entity.ptr()),reinterpret_cast<std::uintptr_t>(i.object.ptr()),
    reinterpret_cast<std::uintptr_t>(i.attachment.ptr()),i.key,i.generation,g_pose_epoch6689.load(),g_weapon_request_epoch6694.load(),i.id,i.side};}
static void initItem(Item& i,unsigned n,int kind,int side){i={};i.key=100+n;i.generation=1+n;i.id=10+n;i.side=side;i.classId=kind;
    for(auto* block:{&i.item,&i.link,&i.entity,&i.object,&i.attachment,&i.definition,&i.classification})memory.push_back(block);
    put(i.item,0,base+0x3A2DA88);put(i.item,0x30,i.key);put(i.item,0x48,i.definition.ptr());put(i.item,0x88,i.link.ptr());
    put(i.definition,0,base+0x3A4E390);put(i.definition,0xE8,i.classification.ptr());put(i.definition,0x58,2.f);
    put(i.classification,8,kind);put(i.classification,0x24,static_cast<unsigned char>(0));put(i.classification,0x25,static_cast<unsigned char>(side==0));
    put(i.link,0,base+0x3EC85E0);put(i.link,0x30,i.id);put(i.link,0x38,i.entity.ptr());put(i.link,0x58,i.item.ptr());
    put(i.entity,0,base+kCEntityVftRva);put(i.entity,0x0C,i.id);
    put(i.object,0,base+0x3A68F68);put(i.object,8,i.id);
    put(i.attachment,0,base+0x3A657C0);put(i.attachment,0x20,i.object.ptr());put(i.attachment,0x28,charManager.ptr());
    std::memcpy(i.attachment.data.data()+0x100,side?"hand_left":"hand_right",side?10:11);
    put(i.attachment,0x10,i.attachment.data.data()+0x100);
}
static void reset(int kind=4,int side=0){
    now=100;memory.clear();for(auto* b:{&itemManager,&player,&slots,&charManager,&character}){*b={};memory.push_back(b);}
    put(itemManager,0,base+0x3EC9958);put(itemManager,0x38,player.ptr());put(player,0,base+0x3AA4B98);
    put(player,0x9D0,itemManager.ptr());put(player,0x990,slots.ptr());put(charManager,0x70,character.ptr());
    for(unsigned n=0;n<3;++n)initItem(items[n],n,kind,side);
    g_henry_item_attach_manager=itemManager.ptr();g_henry_char_attachment_manager=charManager.ptr();
    put(character,0,base+0x40602F8);g_player_char37=reinterpret_cast<std::uintptr_t>(character.ptr());retail=true;
    for(int s=0;s<2;++s){g_native_side_item[s]=nullptr;g_native_side_owner[s]=nullptr;g_native_side_item_state[s]=0;
        g_ranged_draw_d72[s]={};g_weapon_draw_accepted_d75[s]={};g_weapon_draw_attach_serial_d75[s]=0;}
    g_weapon_draw_cleanup_d75={};g_ranged_draw_mask_d73=0;g_phys_equipment6698.publish({});
    immersive=tracked=lockAvailable=identityReads=hiddenReads=hideInvokes=hideWorks=showWorks=writeOK=readbackOK=true;
    passenger=false;g_weapon_control_mode=4;g_phys_fault6698=false;g_gameplay_allowed=true;animation_filter::nativeBodyOwned112=false;
    g_pose_epoch6689=3;g_weapon_request_epoch6694=4;tracking59c6::epoch=1;
    denied=0;hides=shows=nativeCalls=nativeVisible=writes=readbacks=0;physicalReady=false;afterReadback=nullptr;
    t4On=crossbowOn=false;t4Receipt=t4Current=crossbowCurrent=true;achieved={};crossbowState={};crossbowSource=xcXR34::Source{};body={};
    publication=2;g_committed_pose_valid6689=true;g_phys_ready_sample6698=10;g_phys_frame6698.publish(PhysFrame6698{});g_native_held_source_t4.publish({});
    xcXR34::sourceMailbox.publish(xcXR34::Source{});xcXR34::statusMailbox.publish({});xcXR34::activeNativeLease=0;
    publishHandBefore=nativeAttachOK=true;g_hidden_bow_entity59c14=nullptr;g_hidden_bow_id59c14=0;g_hidden_bow_seen59c14=0;
}
static void draw(Item& i){EntityAttachmentUpdateDetourV6688(i.object.ptr(),i.attachment.ptr());}
static void familyTests(){
    for(int kind:{0,1,2,3,4,5,6,7,8,9,10,11,13,14,15,16,17})for(int side:{0,1}){
        reset(kind,side);auto& i=items[0];setHand(side,i.key);publish(i);draw(i);
        ck(i.hidden&&nativeCalls==1&&nativeVisible==0,"every eligible family/hand concealed before native WAITING callback");
        const auto b=binding(i);RangedDrawPlacedD72(b,false,true);ck(i.hidden,"skipped readback never acknowledges first placement");
        now=100000;RangedDrawTickD72(now);ck(i.hidden,"long delay and stale metadata never reveal");
        publish(i);physicalReady=true;readbackOK=false;draw(i);ck(i.hidden,"failed actual readback retains hide");
        readbackOK=true;draw(i);ck(!i.hidden&&i.placed&&WeaponDrawAcceptedForD75(b),"first successful current pose reveals immediately");
        const auto before=hides;draw(i);ck(hides==before,"verified same binding is not hidden again every callback");
        setHand(side,0);publish(i,false);RangedDrawTickD72(now);ck(!WeaponDrawAcceptedForD75(b),"positive stow invalidates redraw readiness");
        physicalReady=false;ck(DrawNativeOperationD75(itemManager.ptr(),i.item.ptr(),2),"native draw original succeeds once");
        ck(i.hidden,"same-item same-pointer redraw does not reuse preceding placement readiness");
    }
    for(int unsupported:{-1,12,18,31}){reset(unsupported);auto& i=items[0];setHand(0,i.key);publish(i);draw(i);
        ck(!i.hidden&&!g_ranged_draw_d72[0].owned,"fists/unrelated/unknown classes not concealed");}
}
static void firstCallbackTests(){
    for(std::uint64_t changed:{std::uint64_t(0),std::uint64_t(101)}){
        reset();auto& stale=items[0];setHand(0,stale.key);publish(stale);setHand(0,changed);
        RangedDrawBeginD72(binding(stale));
        ck(!stale.hidden&&!g_ranged_draw_d72[0].owned,"fresh cached held metadata cannot hide after live native hand departure");
    }
    for(bool before:{true,false}){
        reset();auto& i=items[0];publishHandBefore=before;
        ck(DrawNativeOperationD75(itemManager.ptr(),i.item.ptr(),0),"actual scoped original operation returns its native result");
        ck(nativeCalls==1&&i.hidden&&g_ranged_draw_d72[0].owned&&!g_ranged_draw_d72[0].binding.generation,
            "first binding hides with exact native proof before metadata publication");
        if(before)ck(nativeVisible==0,"inside-original callback hides before authored placement");
        publish(i);physicalReady=true;draw(i);ck(!i.hidden&&i.placed,"provisional debt joins real equipment generation without timed reveal");
    }
    reset();auto& stale=items[0];publish(stale,false);setHand(0,stale.key);
    PhysEquipment6698 oldEquipment{};g_phys_equipment6698.read(oldEquipment);
    ck(!WeaponDrawMayStowD75(oldEquipment,oldEquipment.items[0]),"stale stowed snapshot cannot hand newly held native binding back");
    setHand(0,0);denied=reinterpret_cast<std::uintptr_t>(slots.ptr())+0x260;
    ck(!WeaponDrawMayStowD75(oldEquipment,oldEquipment.items[0]),"unknown native hand cannot declare stow");
    denied=0;ck(WeaponDrawMayStowD75(oldEquipment,oldEquipment.items[0]),"positive actual native non-membership allows stow");
    reset();nativeAttachOK=false;auto& i=items[0];
    ck(!DrawNativeOperationD75(itemManager.ptr(),i.item.ptr(),0)&&!i.hidden&&!g_ranged_draw_d72[0].owned,"failed original without hand membership grants no hide");
    reset();auto& j=items[0];setHand(0,j.key);publish(j);put(j.attachment,0x20,items[1].object.ptr());draw(j);
    ck(!j.hidden,"replaced attachment binding cannot acquire hide");
    reset();auto& k=items[0];setHand(0,k.key);publish(k);g_henry_char_attachment_manager=items[1].attachment.ptr();draw(k);
    ck(!k.hidden,"foreign character attachment cannot acquire hide");
}
static void strictGraphD76Tests(){
    for(bool metadata:{false,true})for(int fault=0;fault<8;++fault){
        reset();auto& i=items[0];setHand(0,i.key);if(metadata)publish(i);
        switch(fault){
            case 0:retail=false;break;
            case 1:put(i.attachment,0x10,items[1].attachment.data.data()+0x100);items[1].attachment.data[0x105]='l';break;
            case 2:put(i.link,0,base+0x3EC85E1);break;
            case 3:put(i.link,0x38,items[1].entity.ptr());break;
            case 4:put(i.link,0x58,items[1].item.ptr());break;
            case 5:g_player_char37=reinterpret_cast<std::uintptr_t>(items[1].entity.ptr());break;
            case 6:put(character,0,base+0x40602F9);break;
            case 7:denied=reinterpret_cast<std::uintptr_t>(i.attachment.data.data()+0x10);break;
        }
        if(metadata)draw(i);else (void)DrawNativeOperationD75(itemManager.ptr(),i.item.ptr(),0);
        ck(!i.hidden&&!g_ranged_draw_d72[0].owned,"strict D76 melee graph rejects foreign/unknown graph with or without metadata");
    }
    for(int fault=0;fault<4;++fault){
        reset();auto& i=items[0];setHand(0,i.key);publish(i);draw(i);physicalReady=true;
        if(fault==0)afterReadback=[](){put(items[0].link,0x38,items[1].entity.ptr());};
        if(fault==1)afterReadback=[](){put(items[0].link,0x58,items[1].item.ptr());};
        if(fault==2)afterReadback=[](){g_player_char37=reinterpret_cast<std::uintptr_t>(items[1].entity.ptr());};
        if(fault==3)afterReadback=[](){items[0].attachment.data[0x100]='x';};
        draw(i);ck(i.hidden,"strict D76 melee graph is rechecked after native placement/readback");
        afterReadback=nullptr;g_weapon_control_mode=0;RangedDrawTickD72(now);
        ck(!i.hidden,"mode exit restores only our exact original-visible debt despite changed graph");
    }
}
static void lifecycleTests(){
    reset();auto& i=items[0];setHand(0,i.key);publish(i);i.hidden=true;draw(i);physicalReady=true;draw(i);
    ck(i.hidden&&shows==0&&!g_ranged_draw_d72[0].owned,"pre-hidden native entity never adopted or shown after placement");
    reset();auto& a=items[0];setHand(0,a.key);publish(a);draw(a);const auto b=binding(a);
    tracked=false;now=10000;g_phys_equipment6698.publish({});RangedDrawTickD72(now);ck(a.hidden,"tracking/missing equipment retain pending hide");
    tracked=true;publish(a);tracking59c6::epoch=2;RangedDrawPlacedD72(b,true,true);ck(a.hidden,"old tracking epoch cannot reveal");
    physicalReady=true;draw(a);ck(!a.hidden,"new exact observation plus accepted current placement completes after tracking reset");
    reset();auto& c=items[0];setHand(0,c.key);publish(c);draw(c);physicalReady=true;showWorks=false;draw(c);
    ck(c.hidden&&g_ranged_draw_d72[0].owned&&!g_ranged_draw_d72[0].cleanup,"failed first show retains presentation debt");
    showWorks=true;now+=1000;RangedDrawTickD72(now);ck(c.hidden,"old successful placement cannot cause delayed unverified reveal on tick");
    publish(c);draw(c);ck(!c.hidden,"a new accepted placement retries failed show");
    reset();auto& d=items[0];setHand(0,d.key);publish(d);draw(d);d.present=false;g_weapon_control_mode=0;RangedDrawTickD72(now);
    ck(d.hidden&&g_ranged_draw_d72[0].owned,"OFF retains cleanup debt while exact entity lookup unknown");
    now+=100000;d.present=true;RangedDrawTickD72(now);ck(!d.hidden&&!g_ranged_draw_d72[0].owned,"OFF eventually restores only original owned hide without expiry");
    reset();auto& old=items[0];auto& next=items[1];setHand(0,old.key);publish(old);draw(old);old.present=false;
    setHand(0,next.key);publish(next);draw(next);ck(next.hidden&&old.hidden&&g_weapon_draw_cleanup_d75[0].owned,"new weapon can be protected while unknown old hide debt remains recorded");
    physicalReady=true;draw(next);ck(!next.hidden&&old.hidden,"new accepted placement does not show unknown old entity");
    old.present=true;RangedDrawTickD72(now);ck(!old.hidden&&!g_weapon_draw_cleanup_d75[0].owned,"retired original debt restores once positively identified");
    reset();auto& replaced=items[0];setHand(0,replaced.key);publish(replaced);draw(replaced);replaced.present=false;
    auto& replacement=items[1];replacement.id=replaced.id;put(replacement.entity,0x0C,replacement.id);replacement.hidden=true;
    RangedDrawTickD72(now);ck(replacement.hidden&&shows==0&&!g_ranged_draw_d72[0].owned,"positive entity replacement retires identity without showing replacement");
    reset();auto& paired=items[0];setHand(0,paired.key);publish(paired);draw(paired);animation_filter::nativeBodyOwned112=true;RangedDrawTickD72(now);
    ck(!paired.hidden,"explicit native paired-action ownership restores our hide");
    reset();auto& rebound=items[0];setHand(0,rebound.key);publish(rebound);draw(rebound);auto oldBinding=binding(rebound);
    put(rebound.attachment,0x20,items[1].object.ptr());RangedDrawPlacedD72(oldBinding,true,true);ck(rebound.hidden,"readback for replaced attachment cannot reveal");
    reset();auto& refused=items[0];setHand(0,refused.key);publish(refused);hideInvokes=false;draw(refused);
    ck(!refused.hidden&&!g_ranged_draw_d72[0].owned,"uninvoked native Hide creates no ownership");
}
static void finalRevealTests(){
    reset();auto& i=items[0];setHand(0,i.key);publish(i);draw(i);physicalReady=true;
    afterReadback=[](){setHand(0,0);};draw(i);
    ck(i.hidden,"post-readback live hand change beats still-fresh equipment snapshot");
    afterReadback=nullptr;setHand(0,i.key);draw(i);ck(!i.hidden,"current live hand proof permits next actual placement");
    reset();auto& j=items[0];setHand(0,j.key);publish(j);draw(j);physicalReady=true;
    afterReadback=[](){put(items[0].link,0x30,items[1].id);};draw(j);
    ck(j.hidden,"post-readback CItem world-link replacement blocks reveal");
    afterReadback=nullptr;put(j.link,0x30,j.id);draw(j);ck(!j.hidden,"restored exact current link permits fresh readback reveal");
}
static void t4Tests(){
    reset();auto& i=items[0];setHand(0,i.key);publish(i);t4On=true;
    achieved.owner={i.key,i.generation,g_weapon_request_epoch6694.load(),g_pose_epoch6689.load(),
        reinterpret_cast<std::uintptr_t>(player.ptr()),reinterpret_cast<std::uintptr_t>(itemManager.ptr()),reinterpret_cast<std::uintptr_t>(character.ptr()),
        reinterpret_cast<std::uintptr_t>(i.entity.ptr()),reinterpret_cast<std::uintptr_t>(i.attachment.ptr()),reinterpret_cast<std::uintptr_t>(i.object.ptr()),i.id,0};
    achieved.colliderRetired=false;achieved.colliderLease=11;achieved.worldGeneration=2;achieved.bodyGeneration=3;
    NativeHeldSourceT4 s{};s.valid=true;s.input.owner=achieved.owner;s.input.colliderLease=11;g_native_held_source_t4.publish(s);
    draw(i);ck(i.hidden&&nativeCalls==0,"actual T4 pending callback cannot bypass early draw guard");
    achieved.state=heldT4::State::Active;achieved.poseValid=true;
    PhysFrame6698 f{};f.input.sample=10;f.input.tick=now;f.input.bodyEpoch=g_pose_epoch6689;f.request=g_weapon_request_epoch6694;
    f.profiles[0].key=i.key;f.profiles[0].generation=i.generation;f.correlation126[0].stamp.actor=reinterpret_cast<std::uintptr_t>(character.ptr());
    f.output.solverOwnedT4[0]=true;f.output.solverLeaseT4=11;f.output.solverWorldGenT4=2;f.output.solverBodyGenT4=3;f.output.solverSourceTickT4=now;
    g_phys_frame6698.publish(f);readbackOK=false;draw(i);ck(i.hidden&&nativeCalls==0,"actual T4 write without successful readback stays hidden");
    readbackOK=true;afterReadback=[](){setHand(0,0);};draw(i);
    ck(i.hidden,"actual T4 final live hand proof rejects membership changed by readback");
    afterReadback=nullptr;setHand(0,i.key);draw(i);
    ck(!i.hidden&&i.placed&&readbacks>=3,"actual T4 current write/readback completes first visibility");
}
static void crossbowTests(){
    for(int kind:{10,14,15}){reset(kind,1);auto& i=items[0];setHand(1,i.key);publish(i);crossbowOn=true;
        auto& s=crossbowSource;s.key=i.key;s.generation=i.generation;s.body=g_pose_epoch6689;s.request=g_weapon_request_epoch6694;s.tracking=1;
        s.primary=1;s.classId=kind;s.weaponItem=reinterpret_cast<std::uintptr_t>(i.item.ptr());s.character=reinterpret_cast<std::uintptr_t>(character.ptr());s.sample=10;s.tick=now;s.instantD72=true;
        crossbowState.owner.source=s;body.stamp={s.body,s.sample,s.tick,s.character};readbackOK=false;draw(i);
        ck(i.hidden&&nativeCalls==0,"actual crossbow weapon writer failed readback cannot reveal native draw pose");
        readbackOK=true;afterReadback=[](){setHand(1,0);};draw(i);
        ck(i.hidden,"actual crossbow final live hand proof rejects membership changed by readback");
        afterReadback=nullptr;setHand(1,i.key);publication=4;draw(i);
        ck(i.hidden,"actual crossbow rejects readback from a superseded body publication");
        body.publicationR24=publication;draw(i);
        ck(!i.hidden&&i.placed,"actual crossbow current readback reveals all crossbow classes");
        WeaponDrawBeforeAttachmentD75(i.object.ptr(),i.attachment.ptr());
        g_weapon_draw_accepted_d75[1]={};WeaponDrawBeforeAttachmentD75(i.object.ptr(),i.attachment.ptr());
        crossbowState.owner.serial=17;crossbowState.clockOwned=true;
        xcXR34::activeNativeLease=17;xcXR34::statusMailbox.publish(crossbowState);
        animation_filter::nativeBodyOwned112=true;now+=100000;RangedDrawTickD72(now);
        ck(i.hidden&&WeaponDrawReloadAttachmentD75(i.object.ptr(),i.attachment.ptr(),now),"exact live reload lease retains hide when pose source becomes unavailable/stale");
        xcXR34::activeNativeLease=0;RangedDrawTickD72(now);
        ck(!i.hidden,"positively ended reload lease permits deliberate native handoff");
    }
}
static void stowedTests(){
    reset(9,1);auto& i=items[0];i.hidden=true;g_hidden_bow_entity59c14=i.entity.ptr();g_hidden_bow_id59c14=i.id;g_hidden_bow_seen59c14=now;
    i.present=false;now+=100000;ck(!StowedBowRelease59c14(now)&&g_hidden_bow_entity59c14==i.entity.ptr(),"slow stowed bow cleanup never forgets unknown original entity by age");
    i.present=true;ck(StowedBowRelease59c14(now)&&!i.hidden&&!g_hidden_bow_entity59c14,"late real bow placement can still restore original stowed hide");
}
int main(){familyTests();firstCallbackTests();strictGraphD76Tests();lifecycleTests();finalRevealTests();t4Tests();crossbowTests();stowedTests();
    std::printf("D76 reviewed weapon draw %u/%u PASS; actual native binding + scoped callback/dispatch + T4/crossbow writers, host engine leaves; pixels untested\n",checks,checks);
}
