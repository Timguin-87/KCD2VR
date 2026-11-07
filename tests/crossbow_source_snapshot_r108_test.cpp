// The runner supplies the actual native adapter/client fixture and literal
// production RangedSpatialXR34. Native bindings/geometry are explicit leaves.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "native_fixture.cpp"
#include "vrik/physical_weapon.h"
namespace geometryR108 {
namespace xcXR34=kcd2vr::crossbow_native_xr34;
namespace pm=kcd2vr::pose_core;namespace ph=kcd2vr::physical;
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
struct Matrix34V6550 {float m[12]{1,0,0,0,0,1,0,0,0,0,1,0};};
struct ManualItems596 {void* weapon=nullptr;void* component=nullptr;std::uint64_t weaponKey=0,ammoKey=0,nockedKey=0;bool loaded=false;};
struct RangedPlacedXR34 {x::Owner owner{};bool valid=false,bolt=false;std::uint64_t sample=0,tick=0,toolKey=0;
    unsigned id=0;void* toolItem=nullptr;pm::Vec3 worldGrip{};};
struct EmptyPlacement {bool read(RangedPlacedXR34&)const{return false;}}g_ranged_placed_xr34;
enum class Hook {None,Publish,CancelAndRequalify,OwnerLost,BindingLost,Expire};
static Hook hook=Hook::None;static bool binding=true;static unsigned acceptedSample=0,spatialSample=0;
static bool RangedOwnerXR34(const x::Owner& owner) noexcept {
    const auto action=hook;hook=Hook::None;
    if(action==Hook::Publish){++f->source.sample;x::PublishSource(f->source);}
    if(action==Hook::CancelAndRequalify){const x::Source withdrawn{};x::PublishSource(withdrawn);++f->source.sample;x::PublishSource(f->source);}
    if(action==Hook::OwnerLost)f->current=false;
    if(action==Hook::BindingLost)binding=false;
    if(action==Hook::Expire)f->now+=81;
    x::Source current{};
    return f->current&&x::ReadSource(current)&&x::fresh(current,f->now)&&x::sameSource(current,owner.source);
}
static bool ManualItemsRead596(void*,ManualItems596& items){items.weapon=f->item.data();items.component=f->item.data();
    items.weaponKey=f->source.key;items.ammoKey=f->source.ammoKey;return true;}
static bool RangedItemEntityXR34(void*,unsigned& id,void*& entity){id=123;entity=f->item.data();return true;}
static bool GetEntityWorldTmV6552(void*,unsigned,Matrix34V6550& matrix){matrix={};return true;}
static bool RangedDefaultsXR34(void*,int,pm::Vec3& rail,pm::Vec3& rest,QuatT28*,int*,void**){rail={0,.1,0};rest={0,.3,0};return true;}
static ph::Pose PhysWorld6698(const float* p){return {{p[0],p[1],p[2],p[3]},{p[4],p[5],p[6]}};}
static ph::Pose PhysPose6698(const QuatT28& p){return {{p.q[0],p.q[1],p.q[2],p.q[3]},{p.t[0],p.t[1],p.t[2]}};}
static pm::Vec3 RangedPointMatrixXR34(const Matrix34V6550& m,pm::Vec3 p){
    return {m.m[0]*p.x+m.m[1]*p.y+m.m[2]*p.z+m.m[3],m.m[4]*p.x+m.m[5]*p.y+m.m[6]*p.z+m.m[7],m.m[8]*p.x+m.m[9]*p.y+m.m[10]*p.z+m.m[11]};}
static bool RangedNativeBindingXR34(const x::Owner& owner,x::Spatial& out){if(!binding)return false;
    out=f->spatial;out.serial=owner.serial;return true;}
template<class T>static bool Read(const void* address,T& out){return f->read(reinterpret_cast<std::uintptr_t>(address),&out,sizeof(out));}
static bool ManualNockAttached19c4r4(const ManualItems596&){return false;}
static bool RangedPlacementStillCurrentXR34(const RangedPlacedXR34&){return false;}
static void RangedBoltAlignmentD78(const x::Owner&,const ManualItems596&,void*,int,const QuatT28&,
    const Matrix34V6550&,const RangedPlacedXR34&,x::Spatial&){}
static void RangedMechanismSpatialD78(const x::Owner&,const x::Source&,x::Spatial&,const RangedPlacedXR34*){}
@RANGED_SPATIAL@
#if HAS_SOURCE_SNAPSHOT_R108
static bool bridge(const x::Owner& owner,const x::Source& accepted,x::Spatial& result) noexcept {
    acceptedSample=static_cast<unsigned>(accepted.sample);
    const bool ok=RangedSpatialXR34(owner,accepted,result);
#else
static bool bridge(const x::Owner& owner,x::Spatial& result) noexcept {
    acceptedSample=static_cast<unsigned>(f->source.sample);
    const bool ok=RangedSpatialXR34(owner,result);
#endif
    spatialSample=static_cast<unsigned>(result.sample);return ok;
}
}
static unsigned checks108=0;static std::vector<std::string> failed108;
static void check108(bool ok,const char* name){++checks108;if(!ok)failed108.emplace_back(name);}
static void begin108(Fixture& a,int kind){
    a.source.classId=kind;a.source.physicalD81=true;a.source.chestKnown=true;
    a.source.handInChest[0]=true;a.source.grip[0]=.9;a.source.instantD72=false;
    geometryR108::hook=geometryR108::Hook::None;geometryR108::binding=true;
    setup(a);x::SetSpatialReader(&geometryR108::bridge);
}
static void restart108(Fixture& a){
    XR34BeforeStopNative(a.parent.data());a.current=true;geometryR108::binding=true;
    a.put(a.p(a.controller)+0x50,a.p(a.queued)+8);a.put(a.p(a.scope)+0x148,std::uintptr_t(0));
    a.put(a.p(a.child)+0x28,1);a.put(a.p(a.child)+0x10,0.f);a.put(a.p(a.child)+0x30,std::uintptr_t(0));
    a.put(a.p(a.child)+0x54,a.get<unsigned>(a.p(a.child)+0x54)+1);a.put(a.entry()+0x44,0.f);a.remaining=5;
    ++a.now;++a.source.sample;a.source.tick=a.now;a.source.entryGripTickD72=a.now;a.source.completedLease=0;
    setup(a,false,false);x::SetSpatialReader(&geometryR108::bridge);
}
int main(){
    for(int kind:{10,14,15}){
        {Fixture a;begin108(a,kind);XR34PreControllerNative(a.controller.data(),.02f);
            check108(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().clockOwned,"ordinary exact source retains manual owner");
            check108(geometryR108::spatialSample==geometryR108::acceptedSample,"ordinary geometry uses accepted sample");XR34BeforeStopNative(a.parent.data());}
        {Fixture a;begin108(a,kind);const auto accepted=a.source.sample;
            geometryR108::hook=geometryR108::Hook::Publish;
            XR34PreControllerNative(a.controller.data(),.02f);
            check108(a.source.sample==accepted+1,"controlled publication genuinely advances mailbox");
            check108(geometryR108::spatialSample==accepted,"geometry remains on accepted source despite newer mailbox");
            check108(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().clockOwned,
                "fresh same-owner publication handoff does not abandon manual reload");
            check108(a.originalEvents==0&&a.cocks==0&&a.loads==0,"snapshot handoff manufactures no native resource event");
            a.input(.9);geometryR108::hook=geometryR108::Hook::None;XR34PreControllerNative(a.controller.data(),.02f);
            check108(g_crossbow_xr34.pending(),"next fresh source resumes same action after publication handoff");
            for(int attempt=0;attempt<2;++attempt){const auto prior=g_crossbow_xr34.status().owner.serial;restart108(a);
                XR34PreControllerNative(a.controller.data(),.02f);
                check108(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial>prior,
                    "real native Stop permits next reload even with reused action addresses");}
            XR34BeforeStopNative(a.parent.data());}
        for(auto loss:{geometryR108::Hook::CancelAndRequalify,geometryR108::Hook::OwnerLost,
                       geometryR108::Hook::BindingLost,geometryR108::Hook::Expire}){
            Fixture a;begin108(a,kind);geometryR108::hook=loss;
            XR34PreControllerNative(a.controller.data(),.02f);
            check108(!g_crossbow_xr34.pending()&&!g_crossbow_xr34.status().clockOwned,
                "real cancellation owner binding or expiry still relinquishes manual clock");
            check108(a.get<float>(a.p(a.child)+0x5C)==2.5f,"genuine loss restores exact native speed");
            check108(a.originalEvents==0&&a.cocks==0&&a.loads==0,"real loss grants no event or ammunition credit");
            restart108(a);XR34PreControllerNative(a.controller.data(),.02f);
            check108(g_crossbow_xr34.pending(),"fresh valid native reload can recover after exact loss");
            XR34BeforeStopNative(a.parent.data());}
    }
    std::printf("{\"checks\":%u,\"failures\":%zu,\"failed_checks\":[",checks108,failed108.size());
    for(size_t i=0;i<failed108.size();++i)std::printf("%s\"%s\"",i?",":"",failed108[i].c_str());
    std::puts("]}");return failed108.empty()?0:1;
}
