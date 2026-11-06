// Actual production PostStart/Client/Current owner/container checks. Native
// engine memory and the retained equipment flag are explicit fixture leaves.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
static unsigned checksH36=0;
static void ckH36(bool value,const char* message){
    if(!value){std::cerr<<"FAIL "<<message<<'\n';std::abort();}++checksH36;
}
static void prepareH36(Fixture& a,int primary=0){
    equipmentSuspendedH36Fixture(false);a.source.primary=primary;
    g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;x::revokedNativeLease=0;
    g_henry_ctrl=a.controller.data();x::registrationReader=&FixtureRegistrationXR34;
    x::identityReader=&FixtureIdentityXR34;x::RefreshRegistration();x::SetSpatialReader(&spatial);
    x::PublishSource(a.source);
}
static void originalStartH36(Fixture& a){
    ++a.starts; // Native original already returned and seeded its speed.
    a.put(a.p(a.child)+0x5C,2.5f);XR34PostStartNative(a.parent.data(),true);
}
int main(){
    {Fixture a;prepareH36(a);equipmentSuspendedH36Fixture(true);originalStartH36(a);
        ckH36(a.starts==1,"native original is neither rejected nor replayed");
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"LEFT debt refuses first optional clock write");
        ckH36(a.get<float>(a.p(a.child)+0x5C)==2.5f,"native rate survives refusal");
        ckH36(a.cocks==0&&a.loads==0&&a.finals==0,"no fabricated reload events");
        ckH36(!x::cleanupInterested.load()&&!x::activeNativeLease.load(),"no spurious optional owner published");
        // A new body/request cannot erase an old retained equipment job.
        ++a.source.body;++a.source.request;currentEquipmentH36Fixture(a.source);x::PublishSource(a.source);originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"new source identity does not forgive old debt");
    }
    {Fixture a;prepareH36(a,1);equipmentSuspendedH36Fixture(true);originalStartH36(a);
        ckH36(g_crossbow_xr34.pending()&&a.writes==1,"RIGHT working hand stays independent");
        XR34BeforeStopNative(a.parent.data());
        ckH36(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f,"native stop restores despite LEFT debt");
    }
    {Fixture a;prepareH36(a);originalStartH36(a);
        ckH36(g_crossbow_xr34.pending()&&a.writes==1,"debt-free LEFT working hand admits");
        equipmentSuspendedH36Fixture(true);const auto writes=a.writes;
        // No second original is fabricated: exercise refusal against existing
        // optional ownership to prove cleanup is not gated by new admission.
        XR34PostStartNative(a.parent.data(),true);
        ckH36(!g_crossbow_xr34.pending(),"old optional clock restored while successor denied");
        ckH36(a.writes==writes+1&&a.get<float>(a.p(a.child)+0x5C)==2.5f,"exactly one restoration, no new zero");
        ckH36(!x::cleanupInterested.load()&&!x::activeNativeLease.load(),"old lease withdrawal published");
        equipmentSuspendedH36Fixture(false);originalStartH36(a);
        ckH36(g_crossbow_xr34.pending(),"actual debt release allows later native start");
    }
    {Fixture a;prepareH36(a);originalStartH36(a);equipmentSuspendedH36Fixture(true);
        a.partialWrite=true;a.writeFailure=true;XR34PostStartNative(a.parent.data(),true);
        ckH36(g_crossbow_xr34.pending(),"failed restore retains original cleanup debt");
        ckH36(g_crossbow_xr34.status().phase==x::Phase::Retiring,"failed restore is retiring, not a new owner");
        a.writeFailure=false;XR34BeforeStopNative(a.parent.data());
        ckH36(!g_crossbow_xr34.pending(),"native stop can finish pending restore under LEFT debt");
        ckH36(a.get<float>(a.p(a.child)+0x5C)==2.5f,"original speed finally restored");
    }
    {Fixture a;prepareH36(a);originalStartH36(a);equipmentSuspendedH36Fixture(true);
        a.put(a.p(a.child)+0x5C,.75f);const auto writes=a.writes;XR34PostStartNative(a.parent.data(),true);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==writes,"foreign speed is not overwritten");
        ckH36(a.get<float>(a.p(a.child)+0x5C)==.75f,"foreign owner remains authoritative");
    }
    {Fixture a;prepareH36(a);originalStartH36(a);equipmentSuspendedH36Fixture(true);
        const auto writes=a.writes;XR34BeforeCompleteNative(a.parent.data(),a.child.data());
        ckH36(!g_crossbow_xr34.pending()&&a.writes==writes+1,"native completion cleanup remains ungated");
        ckH36(a.get<float>(a.p(a.child)+0x5C)==2.5f,"completion restores original rate");
    }
    ckH36(checksH36==21,"all original21 root checks preserved before extensions");
    namespace h=kcd2vr::frik_pose_probe;namespace ac=kcd2vr::ammo_carry_t18;
    const auto heldDebt=[](Fixture& a,bool successor=false){auto& o=successor?h::g_left_equipment_successors_h36[3]:h::g_left_equipment_obligation_h36;
        o.command.side=1;o.command.bodyEpoch=a.source.body;o.command.epoch=a.source.request;o.active=true;
        o.player=reinterpret_cast<void*>(a.source.player);o.manager=reinterpret_cast<void*>(a.source.manager);o.character=a.source.character;};
    // Real query (bit4 OFF) must carry original actor/manager debt, not FakeFree.
    for(bool successor:{false,true}){Fixture a;prepareH36(a);heldDebt(a,successor);originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"exact retained original/successor owner refuses first clock without global bit4");
        ++a.source.body;++a.source.request;currentEquipmentH36Fixture(a.source);x::PublishSource(a.source);originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"new body/request does not retire original actor/manager operation");}
    {Fixture a;prepareH36(a,1);heldDebt(a);originalStartH36(a);
        ckH36(g_crossbow_xr34.pending(),"actual retained LEFT query preserves opposite RIGHT working hand");}
    {Fixture a;prepareH36(a);h::g_native_held_equipment_pending_h36=true;originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"raw retained flag with unknown record refuses admission");}
    {Fixture a;prepareH36(a);h::g_left_equipment_overflow_h36=true;originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"actual bounded retained overflow remains UNKNOWN");}
    {Fixture a;prepareH36(a);heldDebt(a);h::g_left_equipment_obligation_h36.manager=reinterpret_cast<void*>(a.source.manager+8);originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"foreign original manager is UNKNOWN rather than ignored");}
    {Fixture a;prepareH36(a);h::g_weapon_request_epoch6694=a.source.request+1;originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"query uses ORIGINAL request rather than retagging current atomics");}
    {Fixture a;prepareH36(a);h::g_committed_character6689=reinterpret_cast<void*>(a.source.character+8);originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"query uses ORIGINAL character before native clock mutation");}
    for(int primary:{0,1}){Fixture a;prepareH36(a,primary);const int hand=1-primary;
        ac::Claim claim{};claim.serial=501;claim.player=a.source.player;claim.manager=a.source.manager;claim.character=a.source.character;
        claim.body=a.source.body;claim.request=a.source.request;claim.stage=ac::Stage::Pending;
        ac::hands=1u<<hand;ac::claims[hand].publish(claim);originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"REAL T24 classifier excludes same working hand on both anatomical sides");}
    {Fixture a;prepareH36(a);ac::hands=2;ac::claims[1].publish({});originalStartH36(a);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"partial T24 publication remains UNKNOWN, never free");}
    {Fixture a;prepareH36(a);kcd2vr::native_weapon_body_t4::control=8;originalStartH36(a);
        ckH36(g_crossbow_xr34.pending(),"actual bit4 getter does not reinterpret independent T23bit8 as H36 debt");}
    {Fixture a;prepareH36(a);originalStartH36(a);heldDebt(a);const auto writes=a.writes;
        XR34PostStartNative(a.parent.data(),true);
        ckH36(!g_crossbow_xr34.pending()&&a.writes==writes+1,"OLD restore precedes refusal by exact retained actor query");
        ckH36(a.get<float>(a.p(a.child)+0x5C)==2.5f,"actual retained query never gates native rate restoration");}
    {Fixture a;prepareH36(a);std::atomic<unsigned> phase{0};std::thread other([&]{std::unique_lock<std::recursive_mutex> lock(h::g_phys_native_mutex6698);
        phase.store(1,std::memory_order_release);while(phase.load(std::memory_order_acquire)!=2)std::this_thread::yield();});
        while(phase.load(std::memory_order_acquire)!=1)std::this_thread::yield();originalStartH36(a);phase.store(2,std::memory_order_release);other.join();
        ckH36(!g_crossbow_xr34.pending()&&a.writes==0,"actual retained-query trylock contention cannot grant new clock");}
    equipmentSuspendedH36Fixture(false);
    std::cout<<"XR34 retained working-hand debt: "<<checksH36<<" checks; actual adapter/client, native engine NOT_RUN\n";
}
