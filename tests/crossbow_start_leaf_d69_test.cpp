// Actual production adapter against its exact pre-diagnostic function.
// Explicit native leaves only; no native engine execution or admission relaxation.
#define KCD2VR_XR34_NATIVE_NO_MAIN 1
#include "crossbow_native_xr34_test.cpp"
#include "fixtures/xr34_current_before_d69.inl"
#include <functional>
static unsigned checksD69=0;
#define CHECK69(x) do {++checksD69;if(!(x)){std::cerr<<"FAIL line "<<__LINE__<<" "<<#x<<'\n';std::exit(1);}}while(0)
static x::Owner prepareD69(Fixture& a){
    g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;x::revokedNativeLease=0;
    g_base=base;g_henry_ctrl=a.controller.data();x::registrationReader=&FixtureRegistrationXR34;
    x::identityReader=&FixtureIdentityXR34;x::RefreshRegistration();x::SetSpatialReader(&spatial);x::PublishSource(a.source);
    CHECK69(XR34ConfigureNative());
    x::Owner o{};o.source=a.source;o.controller=a.p(a.controller);o.main=a.p(a.main);o.parent=a.p(a.parent);o.child=a.p(a.child);o.context=a.p(a.context);o.token=17;
    return o;
}
static void currentCaseD69(const char* why,const std::function<void(Fixture&,x::Owner&)>& alter){
    Fixture a;auto o=prepareD69(a);alter(a,o);a.traceReadsD69=true;
    const bool before=XR34CurrentBeforeD69(o);const auto reads=a.readsD69;
    const auto resolve=a.resolveCallsD69,equipment=a.equipmentCallsD69;
    a.readsD69.clear();a.resolveCallsD69=a.equipmentCallsD69=0;
    const bool after=XR34Current(o);
    CHECK69(after==before);CHECK69(a.readsD69==reads);
    CHECK69(a.resolveCallsD69==resolve&&a.equipmentCallsD69==equipment);
    CHECK69(why?(!after&&g_xr34_current_leaf_d69&&std::strcmp(why,g_xr34_current_leaf_d69)==0):(after&&!g_xr34_current_leaf_d69));
}
static std::uintptr_t failReadAtD69=0;
static bool readExceptD69(std::uintptr_t p,void* v,std::size_t n) noexcept {return p!=failReadAtD69&&XR34ReadBytes(p,v,n);}
static bool admitFalseD69(const x::Owner&) noexcept{return false;}
static bool currentTrueD69(const x::Owner&) noexcept{return true;}
int main(){
    currentCaseD69(nullptr,[](auto&,auto&){});
    currentCaseD69("CURRENT_NO_BASE",[](auto&,auto&){g_base=0;});
    currentCaseD69("CURRENT_NO_IDENTITY_READER",[](auto&,auto&){x::identityReader=nullptr;});
    currentCaseD69("CURRENT_PASSENGER_IDENTITY",[](auto&,auto&o){++o.source.key;});
    currentCaseD69("CURRENT_SOURCE_NOT_ALLOWED",[](auto&,auto&o){o.source.allowed=false;});
    currentCaseD69("CURRENT_NOT_OWNER_THREAD",[](auto&a,auto&){x::identityReader=&currentTrueD69;a.current=false;});
    currentCaseD69("CURRENT_CONTROLLER_REPLACED",[](auto&a,auto&){g_henry_ctrl=a.player.data();});
    currentCaseD69("CURRENT_MEMBERSHIP_UNKNOWN",[](auto&a,auto&){a.put(a.p(a.controller)+0x34,25u);});
    currentCaseD69("CURRENT_CHILD_ABSENT",[](auto&a,auto&){a.put(a.p(a.controller)+0x50,a.p(a.queued));});
    currentCaseD69("CURRENT_PLAYER_CLASS",[](auto&a,auto&){a.put(a.p(a.player),std::uintptr_t(0));});
    currentCaseD69("CURRENT_PLAYER_ID",[](auto&a,auto&){a.put(a.p(a.player)+0x30,42u);});
    currentCaseD69("CURRENT_WEAPON_SLOT",[](auto&a,auto&){a.put(a.p(a.equipment)+0x40,std::uintptr_t(0));});
    currentCaseD69("CURRENT_WEAPON_CLASS",[](auto&a,auto&){a.put(a.p(a.item),std::uintptr_t(0));});
    currentCaseD69("CURRENT_WEAPON_KEY",[](auto&a,auto&){a.put(a.p(a.item)+0x30,std::uint64_t(0));});
    currentCaseD69("CURRENT_AMMO_SLOT",[](auto&a,auto&){a.put(a.p(a.equipment)+0x48,std::uintptr_t(0));});
    currentCaseD69("CURRENT_AMMO_CLASS",[](auto&a,auto&){a.put(a.source.ammoItem,std::uintptr_t(0));});
    currentCaseD69("CURRENT_AMMO_KEY",[](auto&a,auto&){a.put(a.source.ammoItem+0x30,std::uint64_t(0));});
    currentCaseD69("CURRENT_MAIN_SLOT",[](auto&a,auto&){a.put(a.p(a.equipment)+0x50,std::uintptr_t(0));});
    currentCaseD69("CURRENT_MAIN_ACTION_CLASS_OR_UNREADABLE",[](auto&a,auto&o){o.main=0;a.put(a.p(a.equipment)+0x50,std::uintptr_t(0));});
    currentCaseD69("CURRENT_MAIN_ACTION_CLASS_OR_UNREADABLE",[](auto&a,auto&){a.put(a.p(a.main),std::uintptr_t(0));});
    currentCaseD69("CURRENT_MAIN_PARENT",[](auto&a,auto&){a.put(a.p(a.main)+0x98,std::uintptr_t(0));});
    currentCaseD69("CURRENT_PARENT_CLASS",[](auto&a,auto&){a.put(a.p(a.parent),std::uintptr_t(0));});
    currentCaseD69("CURRENT_PARENT_MAIN",[](auto&a,auto&){a.put(a.p(a.parent)+0x80,std::uintptr_t(0));});
    currentCaseD69("CURRENT_PARENT_CHILD",[](auto&a,auto&){a.put(a.p(a.parent)+0x70,std::uintptr_t(0));});
    currentCaseD69("CURRENT_CHILD_CLASS",[](auto&a,auto&){a.put(a.p(a.child),std::uintptr_t(0));});
    currentCaseD69("CURRENT_CHILD_CONTEXT",[](auto&a,auto&){a.put(a.p(a.child)+8,std::uintptr_t(0));});
    currentCaseD69("CURRENT_CHILD_TOKEN",[](auto&a,auto&){a.put(a.p(a.child)+0x54,99u);});
    currentCaseD69("CURRENT_CHILD_STATUS",[](auto&a,auto&){a.put(a.p(a.child)+0x28,2);});
    currentCaseD69("CURRENT_CONTROLLER_CONTEXT",[](auto&a,auto&){a.put(a.p(a.controller)+0x20,std::uintptr_t(0));});
    currentCaseD69("CURRENT_CONTROLLER_CHARACTER",[](auto&a,auto&){a.put(a.p(a.context)+0x10,std::uintptr_t(0));});
    currentCaseD69("CURRENT_NATIVE_ACTOR",[](auto&a,auto&){a.actorOKD69=false;});
    currentCaseD69("CURRENT_NATIVE_EQUIPMENT",[](auto&a,auto&){a.equipmentOKD69=false;});
    const auto startCase=[](x::StartReasonD69 expected,const std::function<void(Fixture&,x::Owner&,x::Ops&,float&)>& alter){
        Fixture a;auto o=prepareD69(a);float saved=2.5f;
        x::Ops io{&XR34ReadBytes,&XR34WriteBytes,&x::ReadSource,&XR34Spatial,&XR34NativeClip,&XR34NativeClock,&XR34Current,&XR34NewAdmissionT25};
        alter(a,o,io,saved);g_crossbow_xr34.configure(base,io);
        const bool result=g_crossbow_xr34.startNewAfterRestore(o,saved,a.now);
        CHECK69(g_crossbow_xr34.startReasonD69()==expected);CHECK69(result==(expected==x::StartReasonD69::Taken));
        CHECK69(a.cocks==0&&a.loads==0&&a.starts==0&&a.finals==0);
        g_crossbow_xr34.resetStartReasonD69();CHECK69(g_crossbow_xr34.startReasonD69()==x::StartReasonD69::NotAttempted);
    };
    startCase(x::StartReasonD69::NoCurrentReader,[](auto&,auto&,auto&io,auto&){io.current=nullptr;});
    startCase(x::StartReasonD69::CurrentOwner,[](auto&a,auto&,auto&,auto&){a.put(a.p(a.main),std::uintptr_t(0));});
    startCase(x::StartReasonD69::SourceInvalid,[](auto&,auto&o,auto&,auto&){o.source.tick=1;});
    startCase(x::StartReasonD69::SpeedInvalid,[](auto&,auto&,auto&,auto&s){s=0;});
    startCase(x::StartReasonD69::MembershipUnknown,[](auto&a,auto&,auto&io,auto&){io.current=&currentTrueD69;a.put(a.p(a.controller)+0x34,25u);});
    startCase(x::StartReasonD69::NotQueued,[](auto&a,auto&,auto&io,auto&){io.current=&currentTrueD69;a.put(a.p(a.controller)+0x50,a.p(a.queued));});
    startCase(x::StartReasonD69::ActiveUnreadable,[](auto&a,auto&,auto&io,auto&){failReadAtD69=a.p(a.child)+0x10;io.read=&readExceptD69;});
    startCase(x::StartReasonD69::AlreadyActive,[](auto&a,auto&,auto&,auto&){a.put(a.p(a.child)+0x10,.1f);});
    startCase(x::StartReasonD69::HandAdmission,[](auto&,auto&,auto&io,auto&){io.newAdmission=&admitFalseD69;});
    startCase(x::StartReasonD69::InitialPauseFailed,[](auto&a,auto&,auto&io,auto&){io.newAdmission=nullptr;a.writeFailure=true;});
    startCase(x::StartReasonD69::Taken,[](auto&,auto&,auto&io,auto&){io.newAdmission=nullptr;});
    {Fixture a;auto o=prepareD69(a);g_crossbow_xr34.configure(0,{});
        CHECK69(!g_crossbow_xr34.startNewAfterRestore(o,2.5f,a.now));CHECK69(g_crossbow_xr34.startReasonD69()==x::StartReasonD69::NoBase);}
    {Fixture a;auto o=prepareD69(a);x::Ops io{&XR34ReadBytes,&XR34WriteBytes,nullptr,nullptr,nullptr,nullptr,&XR34Current};
        g_crossbow_xr34.configure(base,io);CHECK69(g_crossbow_xr34.startNewAfterRestore(o,2.5f,a.now));
        const auto writes=a.writes;CHECK69(!g_crossbow_xr34.startNewAfterRestore(o,2.5f,a.now));
        CHECK69(g_crossbow_xr34.startReasonD69()==x::StartReasonD69::PendingRestore&&a.writes==writes);}
    {Fixture a;prepareD69(a);namespace rd=kcd2vr::reload_diagnostics_d48;
        rd::Event e{};while(rd::pop(e)){};
        rd::cap::gate.select(rd::cap::Domain::Reload,true,a.now);
        a.put(a.p(a.main),std::uintptr_t(0));XR34PostStartNative(a.parent.data(),true);
        bool seen=false;while(rd::pop(e))if(e.stage==1){seen=true;CHECK69(std::strcmp(e.why,"CURRENT_MAIN_ACTION_CLASS_OR_UNREADABLE")==0);}
        CHECK69(seen&&a.writes==0&&!g_crossbow_xr34.pending());
        a.put(a.p(a.main),base+0x3E7B1B0);XR34PostStartNative(a.parent.data(),true);
        seen=false;while(rd::pop(e))if(e.stage==1){seen=true;CHECK69(std::strcmp(e.why,"TAKEN")==0);}
        CHECK69(seen&&g_crossbow_xr34.startReasonD69()==x::StartReasonD69::Taken&&a.writes==1);
        const auto writes=a.writes;XR34PostStartNative(a.parent.data(),false);
        CHECK69(!rd::pop(e)&&a.writes==writes&&g_crossbow_xr34.startReasonD69()==x::StartReasonD69::NotAttempted);
        rd::cap::gate.select(rd::cap::Domain::Reload,false,a.now);
    }
    std::cout<<"D69 XR34 leaf diagnostics "<<checksD69<<" checks PASS; old/new reads+native leaf calls identical; nativeEngine=MOCKED\n";
}
