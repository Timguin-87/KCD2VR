// Generated from current production source; run tools/extract_door_entry_h5.py.
static thread_local door4::Request g_door_entry66;
static door4::Request DoorEntry66(const DoorRequestWitness38& w,std::uint32_t target,int side,std::uint64_t now) noexcept {
    door4::Request r{};if(!w.handSource65||side<0||side>1||(!w.input46.token&&!w.acceptedUse62.valid)||
       !kcd2vr::door_hold65::current(w.hand65,now)||w.hand65.side!=side||w.hand65.grip<.30f||!w.hand65.press||
       !DoorPins19c4())return r;
    kcd2vr::door_surface65::Witness surface{};
    if(!kcd2vr::door_surface65::read(target,w.hand65,now,surface))return r;
    const auto& h=w.hand65;r.serial=0x100000000ull|h.press;r.tick=now;r.epoch=w.epoch;r.player=w.player;
    r.id=target;r.side=side;r.entity=reinterpret_cast<std::uintptr_t>(EntityById6694(target));
    r.sourceGeneration=h.sourceGeneration;r.trackingEpoch46=h.trackingEpoch;r.inputSettingsEpoch46=h.settingsEpoch;
    r.inputExperimentEpoch65=h.experimentEpoch;r.inputPress65=h.press;r.surface66=surface.point;
    // Preserve the actual public mode for this synchronous entry proposal.
    // Default2 is not the ordinary0 or legacy3 source family; control65 still
    // rechecks this copied mode and every existing live owner predicate.
    r.mode69=int(kcd2vr::combat_rules178r10::f5Rung.load(std::memory_order_acquire));
    DoorScriptIO19c4r4 io{};kcd2vr::door_hold65::Hand current{};
    if(!io.control65(r,current)||!kcd2vr::door_hold65::sameSource(h,current)||current.press!=h.press)return {};
    return r;
}
static bool OwnedDoorMotion66(std::uintptr_t actor,std::uint32_t target,std::uintptr_t entity,std::uint64_t now,std::uint64_t* serial) noexcept {
    door4::Request r{};kcd2vr::door_hold65::Hand h{};DoorScriptIO19c4r4 io{};
    const auto entry=g_door_entry66;
    if(entry.serial&&entry.player==actor&&entry.id==target&&entry.entity==entity&&
       kcd2vr::door_hold65::fresh(entry.tick,now)&&io.control65(entry,h)&&h.press==entry.inputPress65&&h.grip>=.30f){
        if(serial)*serial=entry.serial;return true;}
    if(!g_door_live_serial66.load(std::memory_order_acquire)||!g_door_request19c4.read(r)||
       r.serial!=g_door_live_serial66.load(std::memory_order_acquire)||r.player!=actor||r.id!=target||r.entity!=entity||
       !io.control65(r,h)||!kcd2vr::door_hold65::fresh(h.tick,now))return false;
    if(serial)*serial=r.serial;return true;
}
using InteractiveActionFn6689=std::uintptr_t(__fastcall*)(void*,const char*,std::uint32_t,bool,float);
static std::uintptr_t DoorNativeCall66(InteractiveActionFn6689 original,void* actor,const char* name,std::uint32_t target,
                                     bool force,float speed,const DoorRequestWitness38& witness,int side,std::uint64_t now) noexcept {
    const auto saved=g_door_entry66;g_door_entry66=DoorEntry66(witness,target,side,now);
    const auto result=original?original(actor,name,target,force,speed):0;g_door_entry66=saved;return result;
}
namespace kcd2vr::frik_pose_probe {
bool DoorNativeMotionOwned66(std::uintptr_t actor,std::uint32_t target,std::uintptr_t entity,std::uint64_t now,std::uint64_t* serial) noexcept {
    return OwnedDoorMotion66(actor,target,entity,now,serial);
}
}
namespace kcd2vr::door66 {
bool OwnedMotionFor(std::uintptr_t actor,std::uint32_t target,std::uintptr_t entity,std::uint64_t now,std::uint64_t* serial) noexcept {
    return frik_pose_probe::DoorNativeMotionOwned66(actor,target,entity,now,serial);
}
}
