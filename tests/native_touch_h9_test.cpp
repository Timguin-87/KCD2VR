// Actual committed-contact publisher, actual native touch consumer and actual
// current-character/shape resolver. Native GetCharacter/GetStatus/AddImpact and
// readable engine memory are explicit host leaves. No engine movement claim.
#define KCD2VR_TOUCH_NATIVE_HOST_TEST
#include "subsystems/physics/touch_react_service60.h"
#include "vrik/part_namespace62.h"
#include "vrik/part_anchor59c7g.h"
#include "vrik/lab58.h"
#include "subsystems/physics/grab_over_talk_ds.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <limits>
namespace pc=kcd2vr::pose_core;
namespace tr=kcd2vr::touch_react_service60;
namespace ns=kcd2vr::part_namespace62;
namespace ph=kcd2vr::collab_h21;
namespace lab=kcd2vr::lab58;
using Address=std::uintptr_t;
constexpr Address base=0x10000000,entity=0x1000,proxy=0x3000,character=0x4000,model=0x6000,
    joints=0x7000,mainBody=0x8000,info=0x9000,primary=0x9100,secondary=0x9200,geometry=0xA000,sourceActor=0x7777;
static std::unordered_map<Address,unsigned char> memory;
static unsigned checks=0,failed=0,characterCalls=0,statusCalls=0,impactCalls=0;
static unsigned capsuleCalls=0,capsuleRootReads=0;
static int capsuleResult=1;static Address capsuleBody=0,capsuleFunction=0;
struct CapsulePacket {int type;float impulse[3],angular[3],point[3];int part,index,applyTime,source;};
static_assert(sizeof(CapsulePacket)==56);
static CapsulePacket capsulePacket{};
static std::uint64_t now=1000,serial=0,lastSourceTick=0;
static Address currentEntity=entity,currentCharacter=character;
static bool retail=true,statusOk=true;static int impactResult=1,mutate=0;
static int currentPart=745,impactJoint=-1;
static Address currentGeometry=geometry,currentProxyGeometry=geometry,impactPose=0;
static std::unordered_map<int,Address> nativePrimaryParts;
static float impactPoint[3]{},impactImpulse[3]{};
static std::vector<std::string> lines;
static bool seeded[2]{};
static void check(bool ok,const char* why){++checks;if(!ok){++failed;std::printf("FAIL %s\n",why);}}
template<class T>static void put(Address p,const T& value){const auto* b=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof(T);++i)memory[p+i]=b[i];}
template<class T>static bool Read(void* p,T& value) noexcept {
    auto* b=reinterpret_cast<unsigned char*>(&value);const auto at=Address(p);
    if(at==proxy+0x38){++capsuleRootReads;
        if(mutate==5&&capsuleRootReads==2)put(proxy+0x38,Address(0xBAD0));
        if(mutate==6&&capsuleRootReads==2)ph::setPhysicsMode(0);}
    for(unsigned i=0;i<sizeof(T);++i){const auto it=memory.find(at+i);if(it==memory.end())return false;b[i]=it->second;}return true;
}
static std::uint64_t GetTickCount64() noexcept{return now;}
static Address WhBase() noexcept{return base;}
static bool SupportedWhBuild(Address value) noexcept{return retail&&value==base;}
static void* FlinchEntityById178r11(std::uint32_t id) noexcept{return id==456?reinterpret_cast<void*>(currentEntity):nullptr;}
static void* CharacterForEntity6689(void* e) noexcept {
    if(Address(e)!=currentEntity)return nullptr;
    ++characterCalls;
    if(mutate==1&&characterCalls==3)put(proxy+0x38,Address(0xBAD0));
    if(mutate==2&&characterCalls==3)currentCharacter=0xBAD0;
    return reinterpret_cast<void*>(currentCharacter);
}
static bool ValidateCharInstance(void* ci,void*& anim) noexcept{anim=ci;return Address(ci)==character;}
static bool PartAnchorPins59c7g() noexcept{return retail;}
static bool PartStatus47(Address body,kcd2vr::partanchor59c7g::Status& out) noexcept {
    ++statusCalls;Address vft=0,fn=0;
    if(!statusOk||body!=mainBody||!Read(reinterpret_cast<void*>(body),vft)||vft!=base+0x3A48B30||
       !Read(reinterpret_cast<void*>(vft+0x30),fn)||fn!=base+0x8A9348)return false;
    if(out.part==currentPart){out.geometry=currentGeometry;out.proxyGeometry=currentProxyGeometry;}
    else{const auto it=nativePrimaryParts.find(out.part);if(it==nativePrimaryParts.end())return false;out.geometry=out.proxyGeometry=it->second;}
    if(mutate==3)put(character+0x7A0,Address(0xBAD0));
    if(mutate==4)put(character+0x7B4,static_cast<unsigned char>(0x20));
    return true;
}
static int HostTouchAddImpact(void* pose,Address fn,int part,const float* p,const float* j) noexcept {
    ++impactCalls;impactPose=Address(pose);impactJoint=part;
    std::memcpy(impactPoint,p,sizeof(impactPoint));std::memcpy(impactImpulse,j,sizeof(impactImpulse));
    return fn==base+0x37C66A4?impactResult:-1;
}
static int HostTouchCapsuleH11(Address body,Address fn,const void* packet) noexcept {
    ++capsuleCalls;capsuleBody=body;capsuleFunction=fn;std::memcpy(&capsulePacket,packet,sizeof capsulePacket);return capsuleResult;
}
namespace kcd2vr::combat_diag_probe {static bool NativeCharacterEntryQualified63(Address) noexcept{return false;}}
namespace kcd2vr::diagnostic_trace {static void Write(const char* text){lines.emplace_back(text);}}
namespace kcd2vr::diagnostics::lab {static bool reference=false;static bool NativeReference() noexcept{return reference;}}
namespace kcd2vr::frik_pose_probe {
namespace pose_core=::kcd2vr::pose_core;
namespace gore32n {static std::atomic<bool(*)(const char*)noexcept> g_lua_exec43{nullptr};}
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(sourceActor)};
static std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(0x7770)};
static std::atomic<std::uint64_t> g_pose_epoch6689{2},g_weapon_request_epoch6694{3},g_contact_control_epoch6695{13};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false};
#if defined(KCD2VR_TOUCH_CAPSULE_PARENT)
#include "touch_capsule_parent.inl"
#elif defined(KCD2VR_TOUCH_PARENT)
#include "touch_parent_body.inl"
#else
#include "../src/vrik/touch_react43.inl"
#endif
static void Dispatch() noexcept {
#if defined(KCD2VR_TOUCH_PARENT)
#include "touch_parent_dispatch.inc"
#else
    touch43::HandContactsH9(std::uint64_t(GetTickCount64()));
#endif
}
}
namespace fp=kcd2vr::frik_pose_probe;
static void fixture(){
    memory.clear();lines.clear();now+=1000;retail=statusOk=true;mutate=0;impactResult=1;
    currentEntity=entity;currentCharacter=character;currentPart=745;currentGeometry=currentProxyGeometry=geometry;
    nativePrimaryParts.clear();nativePrimaryParts[5]=geometry;
    seeded[0]=seeded[1]=false;
    characterCalls=statusCalls=impactCalls=0;impactJoint=-1;impactPose=0;
    capsuleCalls=capsuleRootReads=0;capsuleResult=1;capsuleBody=capsuleFunction=0;capsulePacket={};
    fp::touch43::g_on=false;fp::touch43::g_latched=false;fp::touch43::g_calls=fp::touch43::g_fail=0;
    fp::touch43::g_last[0]=fp::touch43::g_last[1]=0;fp::touch43::g_lastTarget[0]=fp::touch43::g_lastTarget[1]=0;
    fp::g_committed_character6689=reinterpret_cast<void*>(sourceActor);fp::g_pose_epoch6689=2;fp::g_weapon_request_epoch6694=3;fp::g_contact_control_epoch6695=13;
    fp::g_gameplay_allowed=true;fp::g_ctx_menu=fp::g_ctx_dialogue=fp::g_ctx_cutscene=false;
    kcd2vr::diagnostics::lab::reference=false;
    ph::setPhysicsMode(0);ph::setPhysicsMode(3);lab::rung=3;lab::livingHitImpact=true;
    put(base+0x6411AC,ns::characterEntry);put(base+0x1AA6BC0,ns::poseGetter);put(base+0x1A77FD0,ns::physicsGetter);put(base+0x1AAF020,ns::countGetter);
    put(entity,base+0x4072C78);put(entity+0xC,std::uint32_t(456));put(base+0x4072C78+0x328,base+0x6411AC);
    put(entity+0xA8,Address(0x2000));put(0x2008,Address(0x2100));put(0x2119,static_cast<unsigned char>(0));put(0x2120,unsigned(1));put(0x2128,proxy);
    put(proxy,base+0x3AC3478);put(proxy+0x30,entity);put(proxy+0x38,Address(0xB000));
    put(character,base+0x40602F8);put(base+0x40602F8+0x38,base+0x1AA6BC0);
    put(character+0x698,base+0x3A71028);put(base+0x3A71028+0x30,base+0x1A77FD0);put(base+0x3A71028+0x58,base+0x37C66A4);
    put(character+0x8A8,character);put(character+0x7A0,mainBody);put(character+0x7B4,static_cast<unsigned char>(0));
    put(character+0x8E0,base+0x3A71510);put(character+0x940,unsigned(8));put(character+0xA70,model);
    put(model,base+0x406B930);put(base+0x406B930+8,base+0x1AAF020);put(model+8,joints);put(joints-4,unsigned(8));
    for(unsigned i=0;i<8;++i){put(joints+i*0x30+0x1C,int(-1));put(joints+i*0x30+0x20,Address(0));}
    put(joints+5*0x30+0x1C,currentPart);put(joints+5*0x30+0x20,info);put(info,primary);put(primary,geometry);put(primary+0x50,Address(0));
    put(mainBody,base+0x3A48B30);put(base+0x3A48B30+0x30,base+0x8A9348);
    put(0xB000,base+0x3A82570);put(0xB020,entity);put(base+0x3A82570+0x38,base+0x845B90);
    constexpr unsigned char livingEntry[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x55,0x57,0x41,0x54,0x41};
    constexpr unsigned char livingType2[]{0x41,0x83,0x3E,0x02,0x0F,0x84,0x0F,0x01,0x00,0x00};
    constexpr unsigned char livingPacket[]{0x41,0x83,0x7E,0x34,0x02,0xF2,0x41,0x0F,0x10,0x46,0x04,0x41,0x8B,0x46,0x0C};
    put(base+0x845B90,livingEntry);put(base+0x845D5A,livingType2);put(base+0x845EDE,livingPacket);
}
static tr::Evidence publish(int side=0,std::uint64_t tick=0,double speed=.5,const kcd2vr::npc_contact172::Witness* observed=nullptr,float grip=0.f){
    kcd2vr::passive169r1::Sample s{};s.source=kcd2vr::passive169r1::Source::HandWorld;s.status=kcd2vr::passive169r1::Status::Contact;s.side=side;
    s.actor=sourceActor;s.key=100;s.generation=7;s.epoch=2;s.request=13;s.sequence=++serial;s.tick=tick?tick:now;
    s.eligible=s.worldCoherent=true;s.point={.1,.2,1.1};s.normal={0,0,1};s.requested=s.allowed={.1,.2,1.1};
    kcd2vr::npc_contact172::Witness w{};w.ownerRead=true;w.owner=entity;w.entityId=456;w.collider=mainBody;w.body=kcd2vr::npc_contact172::Body::Articulated;w.part=currentPart;
    if(observed)w=*observed;
    if(!seeded[side]){auto prior=s;prior.status=kcd2vr::passive169r1::Status::Clear;prior.tick=s.tick-20;
        prior.requested.z+=speed*.020;prior.allowed=prior.requested;tr::publishPlan(prior,w,false,grip);seeded[side]=true;
        s.sequence=++serial;
    }
    tr::publishPlan(s,w,false,grip);tr::Evidence e{};check(tr::mailbox[side].read(e),"real committed mailbox publish/read");lastSourceTick=s.tick;
    check(!s.measuredMotion&&!s.motion.valid&&e.sample.motion.valid,"actual endpoint-only producer derives original committed motion");
    // The old production caller is also supplied the identical original contact.
    kcd2vr::grab_over_talk_ds::HandTouch old{};old.entity=456;old.part=currentPart;old.side=side;old.tick=s.tick;old.sample=s.sequence;
    old.speed=float(e.sample.motion.incoming);for(int k=0;k<3;++k){old.point[k]=float(k==0?s.point.x:k==1?s.point.y:s.point.z);old.normal[k]=float(k==2?1.:0.);}
    (void)kcd2vr::grab_over_talk_ds::handTouch.publish(old);return e;
}
static void send(tr::Evidence e){(void)tr::mailbox[e.sample.side].publish(e);}
static tr::Evidence capsule(int side=0,double speed=.5,int part=-1,float grip=0.f){
    kcd2vr::npc_contact172::Witness w{};w.ownerRead=true;w.owner=entity;w.entityId=456;w.collider=0xB000;
    w.body=kcd2vr::npc_contact172::Body::Living;w.vft=0x3A82570;w.part=part;
    return publish(side,now-17,speed,&w,grip);
}
struct QueryState {bool requested=false,prepared=false;unsigned preparedOwner=0;};
static kcd2vr::npc_contact172::Witness HostExistingRay(const QueryState& q,unsigned& calls,
        kcd2vr::npc_contact172::Body observed=kcd2vr::npc_contact172::Body::Articulated){
    ++calls;kcd2vr::npc_contact172::Witness w{};
    if(q.requested){w.mask=kcd2vr::npc_contact172::BodiesAndWorld;
        const bool living=observed==kcd2vr::npc_contact172::Body::Living;
        w.body=kcd2vr::npc_contact172::classify(living?0x3A82570:0x3A48B30);
        w.collider=living?Address(0xB000):mainBody;w.ownerRead=true;w.owner=entity;w.entityId=456;w.part=living?-1:currentPart;}
    return w;
}
int main(){
    // D51 intentionally suspends living native actuation. Preserve the older
    // adapter cases below as historical coverage, not current success claims.
    if(!lab::livingReactionsAvailableD51()){
        fixture();lab::livingHitImpact=lab::livingTouch=true;fp::touch43::g_on=true;
        publish(0);publish(1);fp::Dispatch();
        check(!impactCalls&&!capsuleCalls&&!characterCalls&&!statusCalls,"paused mapped touch performs no living native work");
        fixture();lab::livingTouch=true;capsule(0);capsule(1);fp::Dispatch();
        check(!impactCalls&&!capsuleCalls,"paused capsule contacts leave native body response alone");
        float v[3]{};fp::touch43::CapsuleImpulseH11 packet{};
        check(fp::touch43::CallAddImpact(nullptr,base+0x37C66A4,5,v,v)==0&&!impactCalls,"direct skeletal native leaf also refuses retired actuation");
        check(fp::touch43::CallCapsuleH11(base,0xB000,packet)==0&&!capsuleCalls,"direct capsule native leaf also refuses retired actuation");
        std::printf("native_touch_h9 D51 suspension: %u/%u assertions passed; historical actuator cases NOT_RUN\n",checks-failed,checks);
        return failed?1:0;
    }
    fixture();QueryState query{false,true,77};unsigned rayCalls=0;
    {tr::ScopedNpcQueryH9 scope(query,false);check(query.requested&&!query.prepared,"F9 selects NPC bodies for existing free-hand rays in otherwise world-only setup");
        const auto hit=HostExistingRay(query,rayCalls);publish(0,0,.5,&hit);}
    check(!query.requested&&query.prepared&&query.preparedOwner==77,"free-hand query restores the complete old outer/weapon context");
    fp::Dispatch();check(impactCalls==1&&impactJoint==5&&rayCalls==1,"F9-only existing ray witness reaches actual mapped native consumer once");
    fixture();query={false,true,77};lab::rung=0;
    {tr::ScopedNpcQueryH9 scope(query,false);check(!query.requested&&query.prepared&&query.preparedOwner==77,"F9 OFF leaves exact world-only query policy");}
    fixture();query={false,true,77};
    {tr::ScopedNpcQueryH9 scope(query,true);check(!query.requested&&query.prepared,"native reference cannot acquire F9 body-query ownership");}
    fixture();query={true,true,88};
    {tr::ScopedNpcQueryH9 scope(query,false);check(query.requested&&query.prepared&&query.preparedOwner==88,"pre-existing body-query owner is not reset or prepared twice");}
    fixture();const auto original=publish(0,now-37);fp::Dispatch();
    check(impactCalls==1,"F9 ON plus manual OFF reaches mapped native consumer once");check(impactJoint==5,"actual primary geometry maps raw745 to joint5");
    check(impactPose==character+0x698,"native call uses original current skeleton pose");
    check(std::fabs(impactPoint[0]-.1f)<1e-6&&std::fabs(impactImpulse[2]+3.f)<1e-5,"original point and measured speed determine native impulse");
    check(!fp::touch43::g_on,"F9 touch does not flip separate manual switch");
    check(original.sample.tick==lastSourceTick&&original.sample.tick==now-37,"original source clock retained, not freshened");
    fp::Dispatch();check(impactCalls==1,"same committed sample never replays");
    fixture();currentPart=5;put(joints+5*0x30+0x1C,currentPart);publish();fp::Dispatch();
    check(impactCalls==1&&impactJoint==5&&statusCalls==1,"primary native target reuses the original actual query without redundant polling");
    fixture();publish();fp::touch43::g_on=true;fp::Dispatch();check(impactCalls==1&&impactJoint==5,"F9 maps actual shape even when separate manual switch is ON");
    fixture();auto slow=publish(0,0,.1);fp::Dispatch();
    check(impactCalls==1&&std::fabs(impactImpulse[2]+.6f)<1e-5,"small real touch retains its measured energy without a fabricated minimum");
    fixture();currentPart=203;put(joints+3*0x30+0x1C,999);put(joints+3*0x30+0x20,info);put(primary,Address(0xA100));put(primary+0x50,secondary);put(secondary,geometry);
    nativePrimaryParts.clear();nativePrimaryParts[3]=0xA100;
    // Remove the unrelated joint5 mapping to the same shape descriptor.
    put(joints+5*0x30+0x20,Address(0));publish();fp::Dispatch();check(impactCalls==1&&impactJoint==3,"actual secondary shape plus native secondary ID maps joint3");
    for(int kind=0;kind<21;++kind){fixture();auto e=publish();
        switch(kind){
        case 0:currentGeometry=currentProxyGeometry=0xFFFF;break;
        case 1:e.collider=0xB000;break;
        case 2:currentEntity=0xDEAD;break;
        case 3:mutate=1;break;
        case 4:mutate=2;break;
        case 5:mutate=3;break;
        case 6:mutate=4;break;
        case 7:put(character+0x7B4,static_cast<unsigned char>(0x20));break;
        case 8:e.sample.tick=now-81;break;
        case 9:e.sample.tick=now+1;break;
        case 10:e.sample.epoch=99;break;
        case 11:e.sample.request=99;break;
        case 12:e.sample.actor=0x999;break;
        case 13:ph::setPhysicsMode(0);ph::setPhysicsMode(3);break;
        case 14:put(joints+6*0x30+0x1C,currentPart);put(joints+6*0x30+0x20,info);break;
        case 15:currentProxyGeometry=0;put(primary,Address(0));break;
        case 16:statusOk=false;break;
        case 17:retail=false;break;
        case 18:put(joints+6*0x30+0x1C,999);put(joints+6*0x30+0x20,info);break;
        case 19:nativePrimaryParts.clear();break;
        case 20:nativePrimaryParts[5]=0xDEAD;break;
        }
        send(e);fp::Dispatch();check(impactCalls==0&&capsuleCalls==0,"unmapped/stale/replaced current native owner cannot be borrowed or replaced by a capsule");
    }
    fixture();publish(0);publish(1);fp::Dispatch();check(impactCalls==2,"both actual committed hands remain independent");
    fixture();auto e=publish();lab::rung=0;fp::Dispatch();check(impactCalls==0,"F9 OFF does not call new touch route");
    fp::touch43::g_on=true;const double p[3]{0,0,1},n[3]{0,0,1};
    check(!fp::touch43::OnContact(0,456,5,p,n,.5,now)&&impactCalls==0,"manual switch cannot authorize an unproven physics-part to joint conversion");
    fixture();publish();lab::livingHitImpact=false;fp::Dispatch();check(impactCalls==0,"F9 sub-switch OFF stays off");
    fixture();publish();kcd2vr::diagnostics::lab::reference=true;fp::Dispatch();check(impactCalls==0,"native reference retires queued F9 native touch");
    kcd2vr::diagnostics::lab::reference=false;fp::Dispatch();check(impactCalls==0,"no cached F9 touch replays after native reference exits");
    fixture();e=publish();e.fist=true;send(e);fp::Dispatch();check(impactCalls==0,"combat fist does not double as free-hand touch");
    fixture();e=publish();e.sample.worldCoherent=false;send(e);fp::Dispatch();check(impactCalls==0,"failed coherent pose is not a native touch");
    fixture();publish();fp::g_ctx_menu=true;fp::Dispatch();check(impactCalls==0,"current menu ownership retires proposed touch");
    fp::g_ctx_menu=false;fp::Dispatch();check(impactCalls==0,"retired proposed touch cannot replay after menu closes");
    fixture();publish();impactResult=0;fp::Dispatch();fp::Dispatch();check(impactCalls==1&&fp::touch43::g_calls==0,"native refusal is authoritative and is not retried");
    fixture();publish();impactResult=-1;fp::Dispatch();fp::Dispatch();check(impactCalls==1&&fp::touch43::g_latched,"native fault is retained once without replay");
    fixture();const auto bodyTouch=capsule();fp::Dispatch();
    check(capsuleCalls==1&&impactCalls==0&&characterCalls==0&&statusCalls==0,"same achieved PE_LIVING contact reaches its native whole-body consumer, not a guessed joint");
    check(capsuleBody==0xB000&&capsuleFunction==base+0x845B90,"current native entity capsule is the original observed collider");
    check(capsulePacket.type==2&&capsulePacket.part==int(0x80000000u)&&capsulePacket.index==int(0x80000000u)&&capsulePacket.applyTime==2&&capsulePacket.source==0,"native whole-body packet preserves AddImpulse scheduling and unused part domains");
    check(std::fabs(capsulePacket.impulse[2]+3.f)<1e-5&&std::fabs(capsulePacket.point[0]-.1f)<1e-6&&std::fabs(capsulePacket.point[2]-1.1f)<1e-6,"capsule impulse uses original committed point,normal and measured energy");
    check(bodyTouch.sample.tick==now-17&&!fp::touch43::g_on,"capsule touch preserves original clock and does not change manual touch selection");
    fp::Dispatch();check(capsuleCalls==1,"same committed capsule contact never replays");
    fixture();capsule(0);capsule(1);fp::Dispatch();check(capsuleCalls==2&&impactCalls==0,"both original free-hand capsule contacts remain independent");
    fixture();capsule(0,.1);fp::Dispatch();check(capsuleCalls==1&&std::fabs(capsulePacket.impulse[2]+.6f)<1e-5,"gentle capsule touch has no invented minimum impulse or speed");
    fixture();capsule(0,.5,0);fp::Dispatch();check(capsuleCalls==1&&capsulePacket.part==int(0x80000000u),"capsule's numeric contact part never becomes skeleton or limb identity");
    for(int kind=0;kind<15;++kind){fixture();auto c=capsule();
        switch(kind){
        case 0:put(proxy+0x38,Address(0xBAD0));break;
        case 1:put(proxy+0x30,Address(0xBAD0));break;
        case 2:put(0xB000,base+0x3A48B30);break;
        case 3:put(0xB020,Address(0xDEAD));break;
        case 4:currentEntity=0xDEAD;break;
        case 5:mutate=5;break;
        case 6:mutate=6;break;
        case 7:retail=false;break;
        case 8:put(base+0x3A82570+0x38,Address(0xBAD0));break;
        case 9:put(base+0x845D5A,static_cast<unsigned char>(0x90));break;
        case 10:c.sample.tick=now-81;break;
        case 11:c.sample.request=99;break;
        case 12:ph::setPhysicsMode(0);ph::setPhysicsMode(3);break;
        case 13:c.fist=true;break;
        case 14:c.sample.worldCoherent=false;break;
        }
        send(c);fp::Dispatch();check(capsuleCalls==0&&impactCalls==0,"replacement/lost native capsule or retired original contact cannot be substituted");
    }
    fixture();capsule();capsuleResult=0;fp::Dispatch();fp::Dispatch();check(capsuleCalls==1&&fp::touch43::g_calls==0,"native capsule refusal remains authoritative without replay");
    fixture();capsule();capsuleResult=-1;fp::Dispatch();fp::Dispatch();check(capsuleCalls==1&&fp::touch43::g_latched,"capsule native fault is retained once without replay");
    fixture();capsule();lab::rung=0;fp::Dispatch();check(capsuleCalls==0,"F9 OFF leaves capsule wholly native");
    fixture();capsule();lab::livingHitImpact=false;fp::Dispatch();check(capsuleCalls==0,"same F9 living sub-switch owns capsule and articulated touch");
    fixture();query={false,true,77};rayCalls=0;
    {tr::ScopedNpcQueryH9 scope(query,false);const auto hit=HostExistingRay(query,rayCalls,kcd2vr::npc_contact172::Body::Living);publish(0,0,.5,&hit);}
    fp::Dispatch();check(capsuleCalls==1&&impactCalls==0&&rayCalls==1,"actual scoped body query and committed publisher reach capsule without another ray");
    check(!query.requested&&query.prepared&&query.preparedOwner==77,"capsule query restores original prepared outer/weapon context");
    const auto continueCapsule=[](tr::Evidence original,double inward){
        auto next=original.sample;next.sequence=++serial;next.tick=now;next.requested.z-=inward;
        next.measuredMotion=false;next.motion={}; // actual producer input is raw endpoints, not a consumed motion result
        kcd2vr::npc_contact172::Witness w{};w.ownerRead=true;w.owner=entity;w.entityId=456;w.collider=0xB000;
        w.body=kcd2vr::npc_contact172::Body::Living;w.part=-1;
        tr::publishPlan(next,w,false);tr::Evidence result{};check(tr::mailbox[0].read(result),"fresh original capsule sample republishes through actual measured-motion producer");return result;
    };
    fixture();auto timed=capsule();now+=81;fp::Dispatch();check(capsuleCalls==0,"expired original capsule sample cannot be retimed into a push");
    timed=continueCapsule(timed,.02);fp::Dispatch();check(capsuleCalls==0,"first sample after timeout establishes continuity without old-motion replay");
    now+=20;timed=continueCapsule(timed,.01);fp::Dispatch();check(capsuleCalls==1,"new genuine inward motion after timeout reaches original current capsule");
    fp::Dispatch();check(capsuleCalls==1,"recovered capsule motion submits once");
    fixture();auto rejoined=capsule();ph::setPhysicsMode(0);ph::setPhysicsMode(3);fp::Dispatch();
    check(capsuleCalls==0,"F9 OFF/ON cannot borrow the old physics contact epoch");
    now+=20;rejoined=continueCapsule(rejoined,.01);fp::Dispatch();check(capsuleCalls==0,"first fresh reacquisition sample does not manufacture prior velocity");
    now+=20;rejoined=continueCapsule(rejoined,.01);fp::Dispatch();check(capsuleCalls==1,"current physics reacquisition admits new measured motion without a permanent latch");
    std::printf("native_touch_h9: %u/%u assertions passed\n",checks-failed,checks);return failed?1:0;
}
