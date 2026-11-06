// Uses ORIGINAL Henry joints and the current nested Assetslot's LOCAL pose.
// Never reads a workpiece world matrix already moved by a VR attachment writer.
// Game WORLD is +Z up; the captured plane is fixed in world for this station
// lifetime, so head/room-scale movement cannot drag the floor with the player.
struct SmithWorkpieceD56 {
    std::uintptr_t holder=0,attachment=0,object=0;
    std::uint32_t entity=0;
    QuatT28 inTool{};
};
static bool SmithWorkpieceD56Read(const kcd2vr::smith_tool36::Binding& hand,SmithWorkpieceD56& out) noexcept {
    namespace st=kcd2vr::smith_tool36;
    out={};if(!hand||hand.side!=1||!hand.model||!hand.lifetime||!SmithToolPins36())return false;
    std::uintptr_t holder=0,vft=0;std::uint32_t id=0,host=0;unsigned char attached=0,nested=0;int side=-1;
    if(!Read(reinterpret_cast<void*>(hand.model+st::kWorkpieceSlot),holder)||!holder||
       !Read(reinterpret_cast<void*>(holder),vft)||vft!=WhBase()+st::kWorkpieceVft||
       !Read(reinterpret_cast<void*>(holder+st::kEntity),id)||!id||
       !Read(reinterpret_cast<void*>(holder+st::kHost),host)||
       !Read(reinterpret_cast<void*>(holder+st::kHand),side)||side!=1||
       !Read(reinterpret_cast<void*>(holder+st::kAttached),attached)||attached!=1||
       !Read(reinterpret_cast<void*>(holder+st::kAssetSlot),nested)||nested>1)return false;
    SmithWorkpieceD56 p{};p.holder=holder;p.entity=id;QToQuatT(p.inTool,Q4{0,0,0,1});ToT(p.inTool,{0,0,0});
    if(hand.tool==int(st::Tool::Workpiece)){
        if(nested||holder!=hand.holder||id!=hand.entity||host!=hand.host)return false;
        for(unsigned j=0;j<3;++j){float scale=0;
            if(!Read(reinterpret_cast<void*>(hand.object+0x0C+4*j),scale)||!std::isfinite(scale)||std::fabs(scale-1.f)>.001f)return false;}
        p.attachment=hand.attachment;p.object=hand.object;
    }else if(hand.tool==int(st::Tool::Tongs)){
        // Retail B66676 records the current tongs ENTITY as the nested host;
        // B6669D resolves its character's actual "Assetslot". B66756 sets +20.
        if(!nested||host!=hand.entity)return false;
        void* tongs=EntityById6694(hand.entity);
        if(!tongs||!PhysEntityIdentity135(tongs,hand.entity))return false;
        void* character=CharacterForEntity6689(tongs);
        if(!character||!PhysVft6698(character,0x40602F8))return false;
        void* asset=CharAttachment59c15(character,"Assetslot");void* object=nullptr;std::uint32_t bound=0;
        if(!asset||!PhysVft6698(asset,0x3A657C0)||
           !Read(static_cast<unsigned char*>(asset)+0x20,object)||!object||!PhysVft6698(object,0x3A68F68)||
           !Read(static_cast<unsigned char*>(object)+8,bound)||bound!=id||
           !Read(static_cast<unsigned char*>(asset)+0x30,p.inTool)||!ValidPose6689(p.inTool))return false;
        // The existing pinned 423974 getter composes +30 with parent WORLD.
        // This is a native local socket; no earlier VR world pose participates.
        // Defer uncommon per-object scaling instead of silently using a wrong extent.
        for(unsigned j=0;j<3;++j){float a=0,b=0;
            if(!Read(reinterpret_cast<void*>(hand.object+0x0C+4*j),a)||
               !Read(static_cast<unsigned char*>(object)+0x0C+4*j,b)||
               !std::isfinite(a)||!std::isfinite(b)||std::fabs(a-1.f)>.001f||std::fabs(b-1.f)>.001f)return false;}
        void* objectAgain=nullptr;std::uint32_t boundAgain=0;
        if(EntityById6694(hand.entity)!=tongs||CharAttachment59c15(character,"Assetslot")!=asset||
           !Read(static_cast<unsigned char*>(asset)+0x20,objectAgain)||objectAgain!=object||
           !Read(static_cast<unsigned char*>(object)+8,boundAgain)||boundAgain!=id)return false;
        p.attachment=reinterpret_cast<std::uintptr_t>(asset);p.object=reinterpret_cast<std::uintptr_t>(object);
    }else return false;
    std::uintptr_t again=0;std::uint32_t idAgain=0,hostAgain=0;unsigned char attachedAgain=0,nestedAgain=0;
    if(!Read(reinterpret_cast<void*>(hand.model+st::kWorkpieceSlot),again)||again!=holder||
       !Read(reinterpret_cast<void*>(holder+st::kEntity),idAgain)||idAgain!=id||
       !Read(reinterpret_cast<void*>(holder+st::kHost),hostAgain)||hostAgain!=host||
       !Read(reinterpret_cast<void*>(holder+st::kAttached),attachedAgain)||attachedAgain!=attached||
       !Read(reinterpret_cast<void*>(holder+st::kAssetSlot),nestedAgain)||nestedAgain!=nested)return false;
    out=p;return true;
}
template<class CoherentStateD56> static bool SmithNativeFloorD56(void* character,std::uint64_t epoch,int side,
    const CoherentStateD56& s,std::uint64_t sample,std::uint64_t source,std::uint64_t sourceTick,
    V3& goal,const Q4& orientation,SmithContactHistoryR38& staged,
    float leftTriggerD80=0.f,Q4* assistedOrientationD80=nullptr) noexcept {
    namespace floor=kcd2vr::subsystems::smithing::native_floor_d56;
    namespace st=kcd2vr::smith_tool36;
    using namespace kcd2vr::activity_input163;
    if(side!=1)return false;
    auto& state=staged.nativeFloorD56;const auto now=std::uint64_t(GetTickCount64());
    const auto refuse=[&](){state={};return false;};
    const auto assistHeldD80=[&](int phase) noexcept {
        return kcd2vr::subsystems::smithing::assist_d80::held(
            kcd2vr::crafting_d67::smithSemiAnchors.load(std::memory_order_acquire),
            int(kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)),
            phase,assistedOrientationD80!=nullptr,leftTriggerD80);
    };
    const bool assistD80=assistHeldD80(witness175.SmithPhaseC1());
    if(kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)!=kcd2vr::minigame_hands45::SixDofVrik||
       (!kcd2vr::station_ws_j27::toolCollision.load(std::memory_order_relaxed)&&!assistD80)||SmithAutoHoldReason36(now))return refuse();
    ToolSocket59c10 socket{};st::Binding live{};
    if(!g_tool_socket59c10[1].read(socket)||socket.nativeGrasp13||!socket.smith36||socket.side!=1||
       socket.character!=character||socket.epoch!=epoch||!socket.tick||now<socket.tick||now-socket.tick>80||
       socket.request!=kcd2vr::workstation178r20c1::smithHoldEpoch36.load()||
       socket.modeEpoch67!=kcd2vr::minigame_hands45::modeEpoch67.load()||
       socket.joint<0||unsigned(socket.joint)>=s.count||s.rig.hand_l<0||unsigned(s.rig.hand_l)>=s.count||
       !ReadSmithTool36(socket.attachment,socket.id,reinterpret_cast<std::uintptr_t>(character),live)||
       !st::sameGripIdentity67(socket.smith36,live)||!floor::anvil(live.phase))return refuse();
    const auto station=witness175.smithEpochC1.load(std::memory_order_acquire);
    SmithWorkpieceD56 piece{};if(!SmithWorkpieceD56Read(live,piece))return refuse();
    ModelWorld6692 world{};if(!ReadModelWorld6692(character,world)||!std::isfinite(world.scale)||world.scale<=.1f)return refuse();
    const auto nativeTool=AbsoluteFromRelative(s.original[socket.joint],socket.relative);
    const auto grip=(socket.gripReady67&&st::sameCanonicalGripD56(socket.smith36,live))?socket.grip67:RelativeFromAbsolute(s.original[s.rig.hand_l],nativeTool);
    QuatT28 desiredHand{};QToQuatT(desiredHand,orientation);ToT(desiredHand,goal);
    const auto desiredTool=AbsoluteFromRelative(desiredHand,grip);
    const auto nativePiece=AbsoluteFromRelative(nativeTool,piece.inTool);
    const auto desiredPiece=AbsoluteFromRelative(desiredTool,piece.inTool);
    if(!ValidPose6689(nativePiece)||!ValidPose6689(desiredPiece)||!ValidPose6689(grip))return refuse();
    floor::geometry::Bounds original{},desired{};void* entity=EntityById6694(piece.entity);
    if(!entity||!PhysEntityIdentity135(entity,piece.entity))return refuse();
    // Explicit LT pose assistance needs no collision bounds or learned floor.
    if(!assistD80&&!PhysBounds6698(entity,original.local))return refuse();
    std::copy_n(original.local,6,desired.local);
    const auto matrix=[&](const QuatT28& pose,float (&out)[12]){
        const auto q=QNormalize(QMul(world.q,QFromQuatT(pose)));const auto p=ModelToWorld6692(world,FromT(pose));
        const auto x=Mul(QRotateV(q,{1,0,0}),world.scale),y=Mul(QRotateV(q,{0,1,0}),world.scale),z=Mul(QRotateV(q,{0,0,1}),world.scale);
        const float values[]{x.x,y.x,z.x,p.x,x.y,y.y,z.y,p.y,x.z,y.z,z.z,p.z};std::copy_n(values,12,out);
    };
    matrix(nativePiece,original.world);matrix(desiredPiece,desired.world);
    st::Binding recheck{};SmithWorkpieceD56 pieceAgain{};
    if(!ReadSmithTool36(socket.attachment,socket.id,reinterpret_cast<std::uintptr_t>(character),recheck)||!st::same(live,recheck)||
       !SmithWorkpieceD56Read(recheck,pieceAgain)||pieceAgain.holder!=piece.holder||pieceAgain.entity!=piece.entity||
       pieceAgain.attachment!=piece.attachment||pieceAgain.object!=piece.object||
       std::memcmp(&pieceAgain.inTool,&piece.inTool,sizeof(piece.inTool))||EntityById6694(piece.entity)!=entity)return refuse();
    // D62: bounds and nested-attachment reads can cross a mode/hold/station
    // handover or outlive the source sample. The floor owns only the original
    // qualified solve. A newer same-tool socket cannot renew that deadline.
    const auto commitNow=std::uint64_t(GetTickCount64());ToolSocket59c10 currentSocket{};
    if(kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)!=kcd2vr::minigame_hands45::SixDofVrik||
       (!kcd2vr::station_ws_j27::toolCollision.load(std::memory_order_acquire)&&!assistHeldD80(live.phase))||SmithAutoHoldReason36(commitNow)||
       !witness175.ready.load(std::memory_order_acquire)||witness175.transitionDepth175.load(std::memory_order_acquire)||
       witness175.lifetime175.load(std::memory_order_acquire)!=live.lifetime||
       witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)!=live.instance||
       witness175.smithEpochC1.load(std::memory_order_acquire)!=station||witness175.SmithPhaseC1()!=live.phase||
       socket.request!=kcd2vr::workstation178r20c1::smithHoldEpoch36.load(std::memory_order_acquire)||
       socket.modeEpoch67!=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||
       !sourceTick||commitNow<sourceTick||commitNow-sourceTick>80||
       commitNow<socket.tick||commitNow-socket.tick>80||
       !g_tool_socket59c10[1].read(currentSocket)||currentSocket.nativeGrasp13||currentSocket.side!=1||
       currentSocket.character!=character||currentSocket.epoch!=epoch||currentSocket.id!=socket.id||
       currentSocket.attachment!=socket.attachment||currentSocket.joint!=socket.joint||
       currentSocket.request!=socket.request||currentSocket.modeEpoch67!=socket.modeEpoch67||
       !currentSocket.tick||commitNow<currentSocket.tick||commitNow-currentSocket.tick>80||
       !st::sameGripIdentity67(socket.smith36,currentSocket.smith36)||
       currentSocket.gripReady67!=socket.gripReady67||
       std::memcmp(&currentSocket.relative,&socket.relative,sizeof(socket.relative))||
       (socket.gripReady67&&std::memcmp(&currentSocket.grip67,&socket.grip67,sizeof(socket.grip67))))return refuse();
    if(assistD80){
        if(!assistHeldD80(live.phase)||!sample||!source)return refuse();
        // Invert the CURRENT canonical local grip: allowedWrist * grip equals
        // untouched nativeTool. D83R1 keeps the native movement axis live under
        // LT; re-reading this pose follows the resulting workpiece movement.
        // Never add a separate hand-space slide: that would move the visible
        // iron away from the native position used to evaluate hammer contact.
        const auto nativeQ=QNormalize(QMul(QFromQuatT(nativeTool),QConj(QFromQuatT(grip))));
        const auto nativeP=Sub(FromT(nativeTool),QRotateV(nativeQ,FromT(grip)));
        QuatT28 assisted{};QToQuatT(assisted,nativeQ);ToT(assisted,nativeP);
        if(!ValidPose6689(assisted))return refuse();
        goal=nativeP;*assistedOrientationD80=nativeQ;
        state={};staged.world[1]={};staged.anvil[1]={};return true;
    }
    const floor::Identity id{live,piece.holder,piece.attachment,piece.object,piece.entity,epoch,source,socket.modeEpoch67,station};
    // The option now means HOLD LT, not the retired automatic8mm resting snap.
    const auto result=floor::step(state,id,sample,sourceTick,commitNow,original,desired,false);
    if(!result.owned)return false;
    // One left-hand owner: no learned/world clamp may then pull this same wrist
    // away from the clean native reference. Right hammer contact is unchanged.
    const auto wantedWorld=ModelToWorld6692(world,goal);
    goal=WorldToModel6692(world,{wantedWorld.x,wantedWorld.y,wantedWorld.z+float(result.lift)});
    staged.world[1]={};staged.anvil[1]={};
    if(result.acquired&&kcd2vr::station_ws_j27::captureD47(now)){
        char line[320]{};std::snprintf(line,sizeof(line),"D56 SMITH_NATIVE_FLOOR instance=%p life=%llu station=%llu item=%u nested=%d floorWorldZ=%.6f source=ORIGINAL_NATIVE_IDLE_BOUNDS phase=11 writes=DISPLAY_ONLY",
            reinterpret_cast<void*>(live.instance),(unsigned long long)live.lifetime,(unsigned long long)station,piece.entity,
            live.tool==int(st::Tool::Tongs),result.worldBottom);diagnostic_trace::Write(line);
    }
    return true;
}
