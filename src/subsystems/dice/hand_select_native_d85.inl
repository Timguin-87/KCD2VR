// D85 dice: included inside activity_input163's owner, beside the D84 observer.
// Runs on the outer native frame end (game thread, after the native update).
// The only native write is the game's own cursor setter 2E951BC(C_Dice,index,false),
// the same call the d-pad/stick handler (2E9007C) and the die hover lambda (202AF04) make.
// Decisions live in subsystems/dice/hand_select_d85.h (host tested); this file only
// supplies the native leaves.
struct DiceMemD85 {
    template<class T> bool read(std::uintptr_t at,T& v) noexcept {return AlchemyInputRead19c4(at,v);}
    bool same(std::uintptr_t at,const unsigned char* bytes,std::size_t n) noexcept {
        __try{return !std::memcmp(reinterpret_cast<const void*>(at),bytes,n);}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    // Entity registry: global 492D8A0, vft 4077048, slot +70 = 4B4F98 (same pins as doors).
    std::uintptr_t entity(std::uint32_t id) noexcept {
        const auto b=base175.load(std::memory_order_acquire);if(!b||!id)return 0;
        std::uintptr_t system=0,vf=0,fn=0;
        if(!read(b+0x492D8A0,system)||!system||!read(system,vf)||vf!=b+0x4077048||!read(vf+0x70,fn)||fn!=b+0x4B4F98||
           !same(b+0x4B4F98,kcd2vr::native_door_pins19c4::entityLookup,sizeof(kcd2vr::native_door_pins19c4::entityLookup)))return 0;
        return EntityLookupD85(fn,system,id);
    }
    static std::uintptr_t EntityLookupD85(std::uintptr_t fn,std::uintptr_t system,std::uint32_t id) noexcept {
        void* e=nullptr;using Lookup=void*(__fastcall*)(void*,std::uint32_t);
        __try{e=reinterpret_cast<Lookup>(fn)(reinterpret_cast<void*>(system),id);}
        __except(EXCEPTION_EXECUTE_HANDLER){return 0;}
        return reinterpret_cast<std::uintptr_t>(e);
    }
};
static bool DiceSetCursorD85(std::uintptr_t b,std::uintptr_t a,int index) noexcept {
    using Set=void(__fastcall*)(void*,int,bool);
    __try{reinterpret_cast<Set>(b+0x2E951BC)(reinterpret_cast<void*>(a),index,false);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    int again=-1;return AlchemyInputRead19c4(a+0x4F0,again)&&again==index;
}
// F5 observes the existing selector/renderer handover; it never repeats native reads.
static void DiceMarkerReceiptR93(const kcd2vr::dice_hands_d85::Witness& w,bool pins,bool turnKnown,
                                 const kcd2vr::dice_marker_r92::Selection& selection,std::uint64_t now) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    namespace dm=kcd2vr::dice_marker_r92;
    if(!w.instance||!cap::active(cap::Domain::Interaction,now))return;
    dm::Presented prepared{},hidden{},presented{};
    const bool selected=dm::fresh(selection,now);
    const bool preparedJoined=dm::prepared.read(prepared)&&dm::ownsFlatCursor(selection,prepared,now);
    const bool hiddenJoined=dm::hidden.read(hidden)&&dm::ownsFlatCursor(selection,hidden,now);
    const bool presentedJoined=dm::presented.read(presented)&&dm::ownsFlatCursor(selection,presented,now);
    const bool flatLease=dm::flatOwnerReady.load(std::memory_order_acquire);
    kcd2vr::dice_hud_depth_r92r3::Receipt hudDepth{};
    const bool hudDepthJoined=kcd2vr::dice_hud_depth_r92r3::applied.read(hudDepth)&&
        kcd2vr::dice_hud_depth_r92r3::joined(selection,hudDepth,now);
    const bool handPointing=kcd2vr::dice_hands_d85::enabled.load(std::memory_order_acquire);
    const unsigned bits=unsigned(w.ready)|(unsigned(w.retail)<<1)|(unsigned(w.quiet)<<2)|(unsigned(w.dice)<<3)|
        (unsigned(pins)<<4)|(unsigned(turnKnown)<<5)|(unsigned(selected)<<6)|(unsigned(preparedJoined)<<7)|
        (unsigned(hiddenJoined)<<8)|(unsigned(presentedJoined)<<9)|(unsigned(flatLease)<<10)|(unsigned(handPointing)<<11)|(unsigned(hudDepthJoined)<<12);
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate notes;
    const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
    if(!notes.observe(session,now,{w.instance,bits,selection.index,0},100,512,1000))return;
    const char* stage=!w.ready?"LIFECYCLE_NOT_READY":!w.retail?"RETAIL_UNQUALIFIED":!w.quiet?"ACTIVITY_TRANSITION":
        !w.dice?"NOT_CURRENT_DICE":!pins?"SELECTOR_PINS_UNQUALIFIED":!turnKnown?"NATIVE_CHOOSING_TURN_UNAVAILABLE":
        !selected?"NATIVE_SELECTED_DIE_UNAVAILABLE":!preparedJoined?"XR_POSE_OR_ATLAS_NOT_PREPARED":
        !hiddenJoined||!flatLease?"NATIVE_CURSOR_HIDE_NOT_JOINED":!presentedJoined?"XR_END_NOT_YET_JOINED":"XR_END_JOINED";
    char line[768]{};
    std::snprintf(line,sizeof(line),
        "R93 F5_DICE_MARKER session=%llu tick=%llu instance=%llX visit=%llu life=%llu stage=%s nativeTurn=%u nativeSelected=%u index=%d world=(%.5f,%.5f,%.5f) handPointing=%u prepared=%u hidden=%u flatLease=%u xrEndJoined=%u hudDepthJoined=%u hudDepth=%.5f hudSurface=(%.5f,%.5f) nativeReads=NONE pixelScanout=NOT_OBSERVED",
        (unsigned long long)session,(unsigned long long)now,(unsigned long long)w.instance,
        (unsigned long long)w.visit,(unsigned long long)w.lifetime,stage,unsigned(turnKnown),unsigned(selected),selection.index,
        double(selection.world[0]),double(selection.world[1]),double(selection.world[2]),unsigned(handPointing),
        unsigned(preparedJoined),unsigned(hiddenJoined),unsigned(flatLease),unsigned(presentedJoined),unsigned(hudDepthJoined),
        double(hudDepthJoined?hudDepth.depth:0.f),double(hudDepthJoined?hudDepth.width:0.f),double(hudDepthJoined?hudDepth.height:0.f));
    diagnostic_trace::Write(line);
}
static void DiceHandsNativeFrameEndD85() noexcept {
    namespace dh=kcd2vr::dice_hands_d85;
    static dh::Frame frame{};
    static std::uint64_t notedVisit=0;static const char* notedText=nullptr;
    const bool on=dh::enabled.load(std::memory_order_acquire);
    dh::Witness w{};
    w.instance=witness175.dice.load(std::memory_order_acquire);
    w.ready=witness175.ready.load(std::memory_order_acquire);
    w.base=base175.load(std::memory_order_acquire);
    w.visit=witness175.diceStart.load(std::memory_order_acquire);
    w.lifetime=witness175.lifetime175.load(std::memory_order_acquire);
    if(w.instance&&w.ready){
        w.retail=retail_build112::Ready(w.base)&&DiceSnapshotPinsD84(w.base);
        w.quiet=!witness175.transitionDepth175.load(std::memory_order_acquire);
        w.dice=ActiveKind175(reinterpret_cast<void*>(w.instance))==Kind175::Dice;
    }
    dh::Ray r{};if(on)(void)dh::ray.read(r);
    DiceMemD85 m{};
    const auto status=dh::frameEnd(frame,on,w,m,r,std::uint64_t(GetTickCount64()),
        [&](int index){return DiceSetCursorD85(w.base,w.instance,index);},
        [&](const char* text){ // one KCD2VR.log line per reason per dice visit
            if(notedVisit==w.visit&&notedText==text)return;
            notedVisit=w.visit;notedText=text;
            kcd2vr::visible_failure::publishLogOnlyD80(text,std::uint64_t(GetTickCount64()));});
    dh::status.store(unsigned(status),std::memory_order_release);
    // The native selection marker also follows gamepad selection with hand
    // pointing OFF. Read only during the human's actual choosing turn.
    static std::uintptr_t markerBase=0;static bool markerPins=false;
    if(markerBase!=w.base){markerBase=w.base;markerPins=false;}
    if(!markerPins&&w.retail)markerPins=dh::pins(m,w.base);
    dh::Turn turn{};
    dice_marker_r92::Selection selection{};
    const bool turnKnown=w.ready&&w.retail&&w.quiet&&w.dice&&markerPins&&dh::readTurn(m,w.base,w.instance,turn);
    const auto now=std::uint64_t(GetTickCount64());
    if(turnKnown)selection=dice_marker_r92::selection(turn,w.visit,w.lifetime,now);
    (void)dice_marker_r92::selected.publish(selection);
    DiceMarkerReceiptR93(w,markerPins,turnKnown,selection,now);
}
