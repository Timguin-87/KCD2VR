// Actual Mod point/ray producer, also executed by the connected host fixture.
// Retain trigger only for an existing native-selected Alchemy palm lock.
bool alchemyHeldTrigger32[2]{};
if(alchemyPointer59c2&&pointerEnabled32&&(f5Mode==0||f5Mode==2||(f5Mode==3&&guided_crafting55::active(std::uint64_t(GetTickCount64()))))){
    const auto& witness32=activity_input163::witness175;
    const auto life32=witness32.lifetime175.load(std::memory_order_acquire);
    const auto instance32=witness32.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire);
    const auto epoch32=workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire);
    const bool ready32=witness32.ready.load(std::memory_order_acquire)&&!witness32.transitionDepth175.load(std::memory_order_acquire);
    for(int side=0;side<2;++side)alchemyHeldTrigger32[side]=ready32&&point123.owner==side&&
        alchemyPalmLock27.owns32(instance32,life32,epoch32,side);
    if(!witness32.ready.load(std::memory_order_acquire)||witness32.transitionDepth175.load(std::memory_order_acquire)||
       witness32.lifetime175.load(std::memory_order_acquire)!=life32||
       witness32.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire)!=instance32||
       workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire)!=epoch32)
        alchemyHeldTrigger32[0]=alchemyHeldTrigger32[1]=false;
}
palm59c9::Pair palms59c9{};
door_hold65::Frame doorFrame65{};bool doorContact65[2]{};
tracking59c6::Stamp doorStamp65{};
const bool doorSampled65=xr_manager_&&xr_manager_->CopyInputSampleStamp59c6(doorStamp65);
const auto doorNow65=std::uint64_t(GetTickCount64());
const bool doorContext65=door_use46::inputAvailable(f5Mode,comparison114)&&pointerEnabled32&&!alchemyPointer59c2&&pi.enabled&&!pi.menu&&
    door_use46::enabled.load(std::memory_order_acquire)&&doorSampled65&&door_hold65::fresh(doorStamp65.tick,doorNow65);
const auto doorGeneration65=doorContext65?interaction_alignment::DoorAcquisitionGeneration65():0;
for(int side=0;side<2;++side)if(xr_manager_){
    const auto& c=side?xr_manager_->LeftController():xr_manager_->RightController();
    const bool hoverAvailable=c.active&&itemState116[side]==0&&!(reserved114&(1u<<side))&&
        !(gripMask116&(side?1u:2u));
    const bool available=alchemyPointer59c2?c.active:hoverAvailable;
    bool builtDoor65=false;
    if(doorContext65&&hoverAvailable&&pi.hand[side].tracked&&pi.hand[side].empty&&buildPalm59c9(side,palms59c9.ray[side])){
        builtDoor65=true;const auto& ray65=palms59c9.ray[side];auto& held65=doorFrame65.hand[side];
        held65.available=true;held65.side=side;held65.grip=c.grip;held65.tick=doorNow65;held65.poseTick=ray65.tick;
        held65.press=held114.owner==side?heldUse114.pressed:0;held65.poseEpoch=ray65.sourceEpoch;
        held65.trackingEpoch=doorStamp65.epoch;held65.sourceGeneration=doorGeneration65;
        held65.settingsEpoch=door_use46::settingsEpoch.load(std::memory_order_acquire);
        held65.experimentEpoch=workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire);
        held65.player=ray65.sourcePlayer;held65.point={ray65.ox,ray65.oy,ray65.oz};
        // Regrip the exact still-owned paused operation without a second Use.
        // The worker, not this copied message, validates its native lifetime.
        door_hold65::Operation operation65{};
        const bool session65=held65.press&&held65.grip>=.30f&&door_hold65::operation.read(operation65)&&
           door_hold65::current(operation65,doorNow65)&&door_hold65::sameSource(operation65.source,held65);
        if(session65&&operation65.paused)
            (void)door_surface65::offer(door_surface65::request(operation65.request.id,held65,operation65.tick,operation65.source.point),doorNow65);
        door_surface65::Witness touch65{};
        doorContact65[side]=held65.press&&(door_surface65::read(0,held65,doorNow65,touch65)||
            (session65&&!operation65.paused&&operation65.source.press==held65.press));
    }
    const hand_point123r1::Hand h{available,c.grip,c.trigger,alchemyHeldTrigger32[side],doorContact65[side]};
    if(available&&hand_point123r1::TriggerEligible32(h,point123.owner,side)&&(builtDoor65||buildPalm59c9(side,palms59c9.ray[side])))
        palms59c9.eligible[side]=true;
}
(void)door_hold65::frame.publish(doorFrame65); // release remains available and pauses; context loss is unavailable
hand_point123r1::Input pointer123{};pointer123.now=doorNow65;pointer123.enabled=pointerEnabled32;
for(int side=0;side<2;++side)if(xr_manager_){
    const auto& c=side?xr_manager_->LeftController():xr_manager_->RightController();
    pointer123.hand[side]={palms59c9.eligible[side],c.grip,c.trigger,alchemyHeldTrigger32[side],doorContact65[side],fallbackPressH40[side]};
}
const auto pointResult123=hand_point123r1::Update(point123,pointer123);
palms59c9.locked=pointResult123.owner>=0?pointResult123.owner:(po.ray_active?po.ray_owner:-1);
for(int side=0;side<2;++side)if(xr_manager_){
    const auto& c=side?xr_manager_->LeftController():xr_manager_->RightController();
    palms59c9.eligible[side]=pointer123.enabled&&palms59c9.eligible[side]&&(c.grip<.58f||palms59c9.locked==side);
}
#include "interaction_palm_lock27.inl"
#include "interaction_ordinary_lock64.inl"
// C46 ordinary-door pulse lock. It cannot override an alchemy owner or survive
// a physical release, changed producing player/reference space, lost palm, or
// the input producer's context/epoch retirement. It selects no action itself.
if(!alchemyPointer59c2&&door_use46::inputMode(f5Mode)&&pointer123.enabled&&!palms59c9.lockedEntity){
    door_use46::Intent door46{};
    if(door_use46::read(door46,ladder_now_ms)&&door46.side>=0&&door46.side<2){
        const auto& ray46=palms59c9.ray[door46.side];const auto& c46=pointer123.hand[door46.side];
        if(palms59c9.eligible[door46.side]&&point123.owner==door46.side&&c46.grip>=.30f&&
           ray46.sourceEpoch==door46.poseEpoch&&ray46.sourcePlayer==door46.player&&
           interaction_alignment::DoorAcquisitionCurrent65(door46.sourceGeneration)){
            palms59c9.locked=door46.side;palms59c9.lockedEntity=door46.target;
        }
    }
}
// The lock producer rechecks the native witness after ray construction. If an
// activity transition raced that work, retire the trigger-only permission too.
for(int side=0;side<2;++side)if(alchemyHeldTrigger32[side]&&!palms59c9.lockedEntity){
    palms59c9.eligible[side]=false;
    if(point123.owner==side){point123.owner=-1;point123.armed[side]=false;}
    if(palms59c9.locked==side)palms59c9.locked=-1;
}
