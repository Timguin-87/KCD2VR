// Complete production action-owner TU: actual resource event scan -> actual
// Attach pause -> actual return refusal recovery -> actual Update/manual A.
// Only native memory/cancel/Detach/home leaves are explicit providers.
#define KCD2VR_GESTURE_RETURN67_HOST_TEST
#define KCD2VR_ALCHEMY_SEMI46_NO_MAIN
#include "../../alchemy_semi46_native_test.cpp"

// Host-only providers for unrelated weapon/physics/script services. Alchemy
// ownership, Update/Queue, cancellation and lease cleanup remain production code.
namespace kcd2vr::native_weapon_body_t4 {
bool ThrowAdmissionSuspendedT23() noexcept{return false;}
bool HasUnretiredCollider() noexcept{return false;}
bool EquipmentAdmissionSuspendedH36() noexcept{return false;}
std::uint64_t AdmissionSerial() noexcept{return 0;}
}
namespace kcd2vr::native_script_t2 {bool Context::Current() const noexcept{return false;}}
namespace kcd2vr::frik_pose_probe {
bool HoverHaptic178r14(int,std::uint64_t) noexcept{return false;}
int CombatCensusMode() noexcept{return 0;}
int WeaponControlMode() noexcept{return 0;}
std::uint64_t PoseEpochH24() noexcept{return 0;}
bool NativeReloadHandRemoveEntryQualifiedT30(std::uintptr_t) noexcept{return false;}
ThrowAuthorityT22 ThrowNativeAuthorityT22(int,std::uintptr_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept{return ThrowAuthorityT22::Denied;}
ThrowAuthorityT22 ThrowHandAdmissionT22(int,std::uintptr_t,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept{return ThrowAuthorityT22::Denied;}
ThrowAuthorityT22 ThrowEquipmentAdmissionT25(int,std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept{return ThrowAuthorityT22::Denied;}
bool ReadThrowModelGripT18(void*,float (&)[7]) noexcept{return false;}
bool HandClaimBlocksT24(int) noexcept{return false;}
}
namespace kcd2vr::alchemy_finish_cb1 {bool Last(Result&) noexcept{return false;}}

namespace kcd2vr::action_time_owner59c11 { namespace {
static unsigned cancel67=0,detach67=0,home67=0;
bool HostAlchemyCancel67(void* receiver) noexcept {
    ++cancel67;assert(receiver==alchemy46.data()+0x78);
    assert(!g_semi_alchemy46.written); // restored before the local native call
    return true; // native helper returned, but deliberately retained this slot
}
bool HostAlchemyDetach67(void*,const char*) noexcept {++detach67;return false;}
bool HostAlchemyHome67(void*,int) noexcept {++home67;return false;}
static void SeedReturnPins67(){
    const auto putBytes=[](std::uintptr_t at,std::initializer_list<unsigned char> bytes){
        std::copy(bytes.begin(),bytes.end(),image.begin()+at);
    };
    // Synthetic executable-memory leaf. Exact retail bytes are independently
    // checked by verify_return_retail67.py; this fixture never executes them.
    putBytes(0x15fced0, {0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x63,0xda,0x48,0x8b,0xf9,0x8b,0xd3,0xe8,0x11,0xfd,0x3c,0xff,0x84,0xc0,0x74,0x21});
    putBytes(0x9cc9d4, {0x40,0x55,0x53,0x56,0x57,0x41,0x56,0x48,0x8b,0xec,0x48,0x83,0xec,0x30,0x48,0x8b,0x41,0x38});
    putBytes(0x10853cc, {0x48,0x8b,0xc4,0x48,0x89,0x58,0x20,0x55,0x56,0x57,0x41,0x56,0x41,0x57,0x48,0x8d,0x6c,0x24,0x90,0x48,0x81,0xec,0x70,0x01,0x00,0x00});
    putBytes(0x6c531c, {0x83,0xfa,0x15,0x74,0x50,0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x63,0xc2,0x4c,0x8b,0xc1,0x8b,0xbc,0x81,0x50,0x01,0x00,0x00});
    putBytes(0x6c2a53, {0x48,0x8b,0x41,0x58,0x48,0x8b,0x48,0x18,0x48,0x8b,0x01,0xff,0x90,0x10,0x06,0x00,0x00,0x4c,0x8b,0x05,0x35,0x98,0x9e,0x03,0x45,0x33,0xc9,0x48,0x8b,0xc8,0xc6,0x44,0x24,0x20,0x01,0x41,0x8d,0x51,0x01,0xe8,0x4d,0x29,0x9c,0x00});
}
// Exercise the ACTUAL native admission consumer, not a duplicate of its kind
// predicate. A native left-held mortar may drive the right-pestle work clock;
// mounted Bellows is instead proved by native left membership18, never by an
// invented entity_left attachment. Prepare46 provides only native memory and
// Queue/Update leaves; SemiAlchemyInput46/QueueDetour are production code.
static void NativeToolClockGates68(){
    namespace br=kcd2vr::subsystems::alchemy::bridge67;
    const auto setup=[](unsigned kind,int type,int side){
        Prepare46(kind,2);
        minigame_hands45::setMode(minigame_hands45::Alchemy,minigame_hands45::Gesture);
        br::Control c{};c.instance=reinterpret_cast<std::uintptr_t>(alchemy46.data());
        c.parent=reinterpret_cast<std::uintptr_t>(parent46.data());c.lifetime=input46.lifetime;
        c.tick=1000;c.poseEpoch=8;c.sourceEpoch=9;c.modeEpoch=minigame_hands45::modeEpoch67.load();
        c.enabled=true;c.entity=72;c.type=type;c.side=side;
        put(alchemy46.data(),0x150+4*type,std::uint32_t(72));
        put(alchemy46.data(),0x2A4,std::uint32_t(kind==semi46::Bellows?18:2));
        input46.hand=kind==semi46::Mortar?0:1;semi46::control.publish(input46);br::control.publish(c);
        return c;
    };
    for(unsigned kind:{unsigned(semi46::Mortar),unsigned(semi46::Bellows)}){
        const int type=kind==semi46::Mortar?2:18;
        auto c=setup(kind,type,1);semi46::WorkWitness w{};semi46::Control received{};
        ck46(SemiAlchemyParent46(controller.data(),w)&&w.kind==kind&&w.parent==c.parent);
        ck46(SemiAlchemyInput46(w,1000,received)&&received.hand==(kind==semi46::Mortar?0:1));
        QueueDetour(controller.data(),action.data(),0);
        ck46(g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
        InstallQueued46();Sample46(1020,.01);UpdateDetour(controller.data(),.02f);
        ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f);
        // Each refusal starts from a clean actual native fixture. No stale
        // paused lease, another hand, or old Control can make it pass.
        for(unsigned fault=0;fault<(kind==semi46::Bellows?9u:7u);++fault){
            c=setup(kind,type,1);ck46(SemiAlchemyParent46(controller.data(),w));
            switch(fault){
            case 0:c.side=0;break;
            case 1:c.passenger=true;break;
            case 2:c.entity=73;break;
            case 3:c.poseEpoch=7;break;
            case 4:c.sourceEpoch=8;break;
            case 5:c.modeEpoch++;break;
            case 6:input46.manualUse=true;semi46::control.publish(input46);break;
            case 7:put(alchemy46.data(),0x2A4,std::uint32_t(21));break;
            case 8:c.parent++;break;
            }
            br::control.publish(c);ck46(!SemiAlchemyInput46(w,1000,received));
            QueueDetour(controller.data(),action.data(),0);
            ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
        }
    }
}
static void UpdateWithoutScopeR28(void* ctrl,float dt){
    ck46(!g_alchemy_update_controller_r27&&!g_owner_transaction_depth19c4);
    NativeUpdate46(ctrl,dt);
}
static void QueueWithoutScopeR28(void* ctrl,void* a,float time){
    ck46(!g_alchemy_update_controller_r27&&!g_owner_transaction_depth19c4);
    NativeQueue46(ctrl,a,time);
}
static void MigratedQueueAndScopeR28(){
    Prepare46(semi46::Bellows,2);minigame_hands45::setMode(minigame_hands45::Alchemy,minigame_hands45::SemiPassenger);g_owner_thread13.store(99);
    ck46(!OnOwnerThread13()&&!OnAlchemyUpdateOwnerR27());
    {AlchemyUpdateScopeR27 noTransaction(controller.data());ck46(!OnAlchemyUpdateOwnerR27());}
    {OwnerTransaction19c4 tx;ck46(bool(tx));
     {AlchemyUpdateScopeR27 exact(controller.data());ck46(OnAlchemyUpdateOwnerR27()&&!OnOwnerThread13());
      {AlchemyUpdateScopeR27 other(parent46.data());ck46(!OnAlchemyUpdateOwnerR27());}
      ck46(OnAlchemyUpdateOwnerR27());}
     ck46(!OnAlchemyUpdateOwnerR27());
     const auto base=g_base.exchange(0);
     {AlchemyUpdateScopeR27 unqualified(controller.data());ck46(!OnAlchemyUpdateOwnerR27());}
     g_base=base;
    }
    ck46(!g_owner_transaction_depth19c4&&!OnAlchemyUpdateOwnerR27());
    g_queue_original=QueueWithoutScopeR28;QueueDetour(controller.data(),action.data(),0);
    ck46(g_semi_alchemy46.written&&g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    ck46(!OnOwnerThread13()&&!OnAlchemyUpdateOwnerR27());
    InstallQueued46();Sample46(1020,.01);g_update_original=UpdateWithoutScopeR28;UpdateDetour(controller.data(),.02f);
    ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f);
    input46.manualUse=true;semi46::control.publish(input46);UpdateDetour(controller.data(),.01f);
    ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    ck46(!g_owner_transaction_depth19c4&&!OnAlchemyUpdateOwnerR27());
    std::printf("migrated Queue/Update, nested/foreign/no-transaction/fingerprint scope and manual cleanup checks PASS\n");
}
static int ConnectedReturn67(bool migrated=false){
    std::fprintf(stderr,"BEGIN migrated=%d checks=%u\n",migrated,checks46);
    namespace br=kcd2vr::subsystems::alchemy::bridge67;
    namespace rt=kcd2vr::subsystems::alchemy::return67;
    NativeToolClockGates68();
    ResourceFixture46();SeedReturnPins67();
    minigame_hands45::setMode(minigame_hands45::Alchemy,minigame_hands45::Gesture);
    const auto a=reinterpret_cast<std::uintptr_t>(alchemy46.data());
    const auto ch=reinterpret_cast<std::uintptr_t>(character.data());
    const auto life=kcd2vr::activity_input163::witness175.lifetime175.load();
    put(alchemy46.data(),0x28,std::uint32_t(777));
    put(alchemy46.data(),0x150+4*13,std::uint32_t(72));put(alchemy46.data(),0x2A4,std::uint32_t(21));
    put(attachment.data(),0x10,static_cast<const char*>("entity_left"));
    put(attachment.data(),0x20,reinterpret_cast<std::uintptr_t>(attachmentObject.data()));
    put(manager.data(),0x4C,int(1));put(manager.data(),0x50,std::uint32_t(72));
    br::Control c{};c.instance=a;c.lifetime=life;c.tick=1000;c.poseEpoch=8;c.sourceEpoch=9;
    c.modeEpoch=minigame_hands45::modeEpoch67.load();c.enabled=true;c.entity=72;c.side=1;c.type=13;
    br::control.publish(c);input46.hand=1;semi46::control.publish(input46);
    SemiResourceScanDetour46(testSkel,false,.14f,.16f,&eventInfo46,nullptr,0);
    ck46(nativeAttach46==1&&nativeUse46==0&&g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    rt::Request req{};const auto reader=[](std::uintptr_t at,auto& value){return SehCopy(reinterpret_cast<void*>(at),&value,sizeof(value));};
    ck46(kcd2vr::alchemy_tool18::read(reader,g_base.load(),a,reinterpret_cast<std::uintptr_t>(player.data()),ch,
        reinterpret_cast<std::uintptr_t>(attachment.data()),72,req.binding));
    req.binding.lifetime=life;req.child=reinterpret_cast<std::uintptr_t>(action.data());req.token=55;req.serial=100;
    req.tick=1000;req.poseEpoch=8;req.sourceEpoch=9;req.modeEpoch=c.modeEpoch;
    Sample46(1000,.03); // outstanding credit must not drain after release abort
    rt::result.publish({});rt::request.publish(req);rt::interested=true;g_alchemy_return67={};g_alchemy_return_retired67={};
    cancel67=detach67=home67=0;
    g_mode=Off; // the independent saved Gesture owner is still authoritative
    if(migrated){g_owner_thread13.store(99);g_update_original=UpdateWithoutScopeR28;UpdateDetour(controller.data(),.01f);}
    else TickGestureAlchemyReturn67(controller.data(),1000);
    std::fprintf(stderr,"RETURN migrated=%d checks=%u calls=%u owner=%lu\n",migrated,checks46,cancel67,(unsigned long)g_owner_thread13.load());
    rt::Result result{};ck46(rt::result.read(result)&&result.status==rt::Status::CancelRefusedPausedNativeA);
    ck46(cancel67==1&&detach67==0&&home67==0&&g_semi_pending46&&g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    ck46(g_semi_alchemy46.consumedSeconds==.03&&g_semi_alchemy46.consumed==input46.sequence);
    const float before=Rd<float>(action.data(),kActActiveTime,-1);
    Sample46(1010,.03);c.tick=1010;br::control.publish(c);UpdateDetour(controller.data(),.01f);
    ck46(nativeUse46==0&&Rd<float>(action.data(),kActActiveTime,-1)==before&&g_semi_pending46);
    // Physical native A explicitly chooses the passenger continuation. The
    // ACTUAL native owner must restore its real lease before original Update.
    input46.manualUse=true;semi46::control.publish(input46);c.passenger=true;br::control.publish(c);
    UpdateDetour(controller.data(),.01f);
    ck46(!g_semi_pending46&&!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    ck46(cancel67==1&&detach67==0&&home67==0&&nativeUse46==0);
    // Only the retained REAL native event scan performs the Use after resume.
    SemiResourceScanDetour46(testSkel,false,.40f,.43f,&eventInfo46,nullptr,0);
    ck46(nativeUse46==1);
    std::printf("native_return67_connected: %u actual natural-mortar/Bellows-membership clock gates, scan/Attach/refused-cancel/paused-Update/native-A/Use checks PASS\n",checks46);
    return 0;
}
}}
int main(){using namespace kcd2vr::action_time_owner59c11;ConnectedReturn67();ConnectedReturn67(true);MigratedQueueAndScopeR28();std::printf("R28 connected alchemy: %u checks PASS\n",checks46);}
