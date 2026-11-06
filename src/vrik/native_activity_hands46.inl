// C46R1: observe the game's current hand attachment independently of the F5
// placement experiment. Called on the existing attachment callback. This path
// copies identity only; a true result tells the outer hook to run native Update.
struct NativeActivityHandWitness46 {
    kcd2vr::alchemy_tool18::Binding alchemy{};
    kcd2vr::smith_tool36::Binding smith{};
    std::uint64_t epoch=0,tick=0;
};
static kcd2vr::core::SnapshotMailbox<NativeActivityHandWitness46> g_native_activity_hand46[2];

static bool NativeActivityHandOwned46(bool alchemy,std::uintptr_t character,
                                     std::uint64_t epoch,int side,std::uint64_t now,
                                     kcd2vr::smith_tool36::Binding* smithOut=nullptr,
                                     bool requireSmithFixture=true) noexcept {
    if(side<0||side>1||!character||!epoch||
       !(kcd2vr::workstation178r20c1::semiPassengerOwns45(now)||
         (!alchemy&&kcd2vr::workstation178r20c1::gestureSmith67(now))||
         (alchemy&&kcd2vr::subsystems::alchemy::bridge67::visualPassenger71(now)))||
       WorkstationToolReason19c4r2(now)||diagnostics::lab::NativeReference()||
       g_pose_restore_fault6689.load(std::memory_order_acquire))return false;
    NativeActivityHandWitness46 w{};
    if(!g_native_activity_hand46[side].read(w)||w.epoch!=epoch||!w.tick||now<w.tick||now-w.tick>80)return false;
    if(alchemy){
        kcd2vr::alchemy_tool18::Binding current{};
        return w.alchemy&&w.alchemy.character==character&&w.alchemy.side==side&&
            ReadAlchemyTool18(reinterpret_cast<void*>(w.alchemy.attachment),w.alchemy.entity,character,current)&&
            kcd2vr::alchemy_tool18::same(w.alchemy,current);
    }
    kcd2vr::smith_tool36::Binding current{};
    const bool joined=w.smith&&w.smith.character==character&&w.smith.side==side&&
        (!requireSmithFixture||kcd2vr::subsystems::smithing::bridge67::fixtureOwnedH21(w.smith))&&
        ReadSmithTool36(reinterpret_cast<void*>(w.smith.attachment),w.smith.entity,character,current)&&
        kcd2vr::smith_tool36::sameGripIdentity67(w.smith,current);
    // Native phase is re-read above. Geometry may survive its 11->14 ack;
    // no action/phase permission is borrowed from the old witness.
    if(joined&&smithOut)*smithOut=current;
    return joined;
}

static bool ObserveNativeActivityHand46(void* self,void* attachment) noexcept {
    using namespace kcd2vr::activity_input163;
    const auto now=std::uint64_t(GetTickCount64());
    if(!self||!attachment||!kcd2vr::workstation178r20c1::ownsActivityWrist45(now)||
       WorkstationToolReason19c4r2(now)||diagnostics::lab::NativeReference()||
       g_pose_restore_fault6689.load(std::memory_order_acquire)||!retail_build112::Ready(WhBase()))return false;
    const bool alchemy=witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)!=0;
    const bool smith=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)!=0;
    if(alchemy==smith)return false;
    // Filter to the already identified Henry attachment manager before reading
    // membership. An environment/NPC attachment cannot borrow a minigame allow.
    void* manager=nullptr;void* character=nullptr;std::uint32_t entity=0;
    const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    if(!epoch||!PhysVft6698(attachment,0x3A657C0)||!PhysVft6698(self,0x3A68F68)||
       !Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       manager!=g_henry_char_attachment_manager.load(std::memory_order_acquire)||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||!PhysVft6698(character,0x40602F8)||
       !Read(static_cast<unsigned char*>(self)+8,entity)||!entity)return false;
    NativeActivityHandWitness46 next{};next.epoch=epoch;next.tick=now;
    const auto owner=reinterpret_cast<std::uintptr_t>(character);
    int side=-1;
    if(alchemy){
        if(!ReadAlchemyTool18(attachment,entity,owner,next.alchemy)||
           next.alchemy.object!=reinterpret_cast<std::uintptr_t>(self))return false;
        side=next.alchemy.side;
    }else{
        if(!ReadSmithTool36(attachment,entity,owner,next.smith)||
           next.smith.object!=reinterpret_cast<std::uintptr_t>(self))return false;
        side=next.smith.side;
    }
    if(side<0||side>1||epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       manager!=g_henry_char_attachment_manager.load(std::memory_order_acquire)||
       !kcd2vr::workstation178r20c1::ownsActivityWrist45(std::uint64_t(GetTickCount64())))return false;
    (void)g_native_activity_hand46[side].publish(next);
    if(alchemy&&kcd2vr::workstation178r20c1::fullVrAlchemy63(now)){
        const auto& a=next.alchemy;kcd2vr::subsystems::alchemy::bridge67::NativeTool t{};
        t.instance=a.instance;t.parent=a.action;t.character=a.character;t.lifetime=a.lifetime;
        t.tick=now;t.poseEpoch=epoch;t.entity=a.entity;t.side=a.side;t.type=a.type;t.joined=true;t.binding=a;
        (void)kcd2vr::subsystems::alchemy::bridge67::tools[side].publish(t);
        // Semantic work permission and tracked presentation are independent.
        // Only explicit native recovery keeps the native early-return owner.
        return kcd2vr::alchemy_tool18::nativeFixtureD56(a.type)||
            kcd2vr::subsystems::alchemy::bridge67::visualPassenger71(now);
    }
    if(smith&&kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)!=kcd2vr::minigame_hands45::Native){
        kcd2vr::subsystems::smithing::bridge67::NativeTool t{};
        t.binding=next.smith;t.tick=next.tick;t.poseEpoch=next.epoch;
        t.modeEpoch=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire);t.joined=true;
        (void)kcd2vr::subsystems::smithing::bridge67::tools[side].publish(t);
        { // D83R2 smith: copy the workpiece record's native map once per minigame.
            namespace sb=kcd2vr::subsystems::smithing::bridge67;
            sb::WorkpieceMapD83R2 map{};
            const bool have=sb::workpieceMapD83R2.read(map)&&map.valid&&map.instance==next.smith.instance&&
                map.lifetime==next.smith.lifetime&&map.model==next.smith.model;
            if(!have){
                const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
                (void)sb::readWorkpieceMapD83R2(rd,WhBase(),next.smith,now,map);
                (void)sb::workpieceMapD83R2.publish(map);
            }
        }
        // D56: the blank is deliberately detached in native axe heating.
        // Keep only that exact support stage authored; all other tracked
        // tools still use the same committed body/tool placement owner.
        return false; // D73: detached forge blank does not own the attached tongs wrist
    }
    // Publication contention is not permission for the old free-placement owner.
    // Native Update remains the only transform writer for this verified tool.
    return true;
}
