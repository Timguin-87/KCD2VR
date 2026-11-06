// Calibration has no destination or timestamp. It cannot authorize a write;
// only a fresh coherent target can. Protect with g_ranged_pose_mutex_xr34.
struct RangedGripXR34 {RangedSocketXR34 socket{};QuatT28 grip{};bool valid=false;};
static RangedGripXR34 g_ranged_grip_xr34;
struct RangedPlacedXR34 {xcXR34::Owner owner{};void* entity=nullptr;void* toolItem=nullptr;std::uint32_t id=0;std::uint64_t toolKey=0,sample=0,tick=0;pm::Vec3 worldGrip{},localGrip{};bool bolt=false,valid=false;
    bool mountedD78=false;xcXR34::Clock mechanismClockD78{};QuatT28 mechanismPoseD78{};
    bool helperFrameD78=false,poseBasisD78=false;QuatT28 helperD78{};Matrix34V6550 achievedD78{};
    RangedSocketXR34 socketR95{};std::uint64_t bodyPublicationR95=0;};
struct RangedHeavyGripD72 {xcXR34::Owner owner{};unsigned id=0;ph::Pose grip{};bool ready=false;
    std::uint64_t key=0;std::uintptr_t attachment=0;QuatT28 relative{};int joint=-1;
    void* toolItem=nullptr;void* entity=nullptr;void* self=nullptr;void* character=nullptr;void* skeleton=nullptr;};
static RangedHeavyGripD72 g_ranged_heavy_grip_d72;
struct RangedMechanismD78 {xcXR34::Owner owner{};xcXR34::Clock clock{};
    std::uint64_t sample=0,tick=0,observed=0,toolKey=0;std::uintptr_t toolItem=0,toolEntity=0,attachment=0;
    unsigned toolId=0;void* character=nullptr;void* skeleton=nullptr;void* pose=nullptr;std::int16_t joint=-1;
    QuatT28 relative{},toolModel{},stringModel{};int stringJoint=-1;bool valid=false,stringValid=false;};
static core::SnapshotMailbox<RangedMechanismD78> g_ranged_mechanism_d78;
static std::atomic<std::uintptr_t> g_ranged_mechanism_character_d78{0};
static bool RangedReadMechanismD78(const xcXR34::Owner&,const xcXR34::Source&,const xcXR34::Spatial&,RangedMechanismD78&) noexcept;
static void RangedMechanismSpatialD78(const xcXR34::Owner&,const xcXR34::Source&,xcXR34::Spatial&,const RangedPlacedXR34*) noexcept;
static void RangedBoltAlignmentD78(const xcXR34::Owner&,const ManualItems596&,void*,int,const QuatT28&,
    const Matrix34V6550&,const RangedPlacedXR34&,xcXR34::Spatial&) noexcept;
static core::SnapshotMailbox<RangedSocketXR34> g_ranged_socket_xr34;
static core::SnapshotMailbox<RangedPlacedXR34> g_ranged_placed_xr34;
static core::SnapshotMailbox<RangedPlacedXR34> g_ranged_primary_placed_r95;
static std::recursive_mutex g_ranged_pose_mutex_xr34;
static bool RangedReadXR34(std::uintptr_t p,void* out,std::size_t n) noexcept {
    if(!p||!out||!n||!Safe(reinterpret_cast<void*>(p),n))return false;
    __try {std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool RangedOwnerXR34(const xcXR34::Owner& o) noexcept {
    xcXR34::Source source{};xcXR34::Status state{};void* main=nullptr;void* parent=nullptr;void* child=nullptr;
    if(!ReloadContextXR34(o,GetTickCount64())||!xcXR34::ReadStatus(state)||!state.clockOwned||!xcXR34::sameOwner(o,state.owner))return false;
    const xcXR34::Ops io{&RangedReadXR34};const auto member=xcXR34::membership(io,WhBase(),o);
    if(member!=xcXR34::Member::Queued&&member!=xcXR34::Member::Root)return false;
    unsigned token=0;void* context=nullptr;
    return xcXR34::ReadStatus(state)&&state.clockOwned&&xcXR34::sameOwner(o,state.owner)&&
        xcXR34::ReadSource(source)&&xcXR34::fresh(source,GetTickCount64())&&xcXR34::sameSource(source,o.source)&&
        Read(reinterpret_cast<unsigned char*>(o.source.equipment)+0x50,main)&&reinterpret_cast<std::uintptr_t>(main)==o.main&&
        Read(static_cast<unsigned char*>(main)+0x98,parent)&&reinterpret_cast<std::uintptr_t>(parent)==o.parent&&PhysVft6698(parent,0x3E78CF8)&&
        Read(static_cast<unsigned char*>(parent)+0x70,child)&&reinterpret_cast<std::uintptr_t>(child)==o.child&&
        Read(static_cast<unsigned char*>(child)+0x54,token)&&token==o.token&&
        Read(static_cast<unsigned char*>(child)+8,context)&&reinterpret_cast<std::uintptr_t>(context)==o.context;
}
static bool RangedItemEntityXR34(void* item,std::uint32_t& id,void*& entity) noexcept {
    id=0;entity=nullptr;void* pickable=nullptr;
    return PhysVft6698(item,0x3A2DA88)&&Read(static_cast<unsigned char*>(item)+0x88,pickable)&&pickable&&
        Read(static_cast<unsigned char*>(pickable)+0x30,id)&&id&&Read(static_cast<unsigned char*>(pickable)+0x38,entity)&&
        PhysVft6698(entity,0x4072C78)&&EntityById6694(id)==entity;
}
static bool RangedBoundHandIdentityXR34(const xcXR34::Owner& o,std::uint32_t id,void* entity,int& side,void*& attachment) noexcept {
    side=-1;attachment=nullptr;void* model=nullptr;void* player=nullptr;
    if(!id||!entity||!Read(reinterpret_cast<unsigned char*>(o.source.player)+0x1B0,model)||!model||
       !Read(static_cast<unsigned char*>(model)+0x20,player)||reinterpret_cast<std::uintptr_t>(player)!=o.source.player)return false;
    for(int s=0;s<2;++s){std::uint32_t current=0;const char* name=nullptr;
        if(!Read(static_cast<unsigned char*>(model)+0x2C+0x24*s,current)||current!=id)continue;
        if(!Read(reinterpret_cast<void*>(WhBase()+0x40AC298+8*s),name)||!name)return false;
        void* found=ManualFindHandAttachment19c4r4(model,name);
        if(!ManualBoundEntity19c4r4(found,id,entity)||side!=-1)return false;side=s;attachment=found;}
    return side==1-o.source.primary;
}
// Exact retail heavy lever is bound to the equipped crossbow's "lever"
// socket by native callback1E83420, not to Henry's right-hand item slot.
static bool RangedBoundWeaponIdentityD72(const xcXR34::Owner& o,std::uint32_t id,void* entity,void*& attachment) noexcept {
    attachment=nullptr;void* model=nullptr;void* player=nullptr;const char* name=nullptr;
    return id&&entity&&Read(reinterpret_cast<unsigned char*>(o.source.player)+0x1B0,model)&&model&&
        Read(static_cast<unsigned char*>(model)+0x20,player)&&reinterpret_cast<std::uintptr_t>(player)==o.source.player&&
        Read(reinterpret_cast<void*>(WhBase()+0x40AC2A8+8*o.source.primary),name)&&name&&
        (attachment=ManualFindHandAttachment19c4r4(model,name))&&ManualBoundEntity19c4r4(attachment,id,entity);
}
static bool RangedBoundToolIdentityD72(const xcXR34::Owner& o,std::uint32_t id,void* entity,int& side,void*& attachment) noexcept {
    if(o.source.classId!=15)return RangedBoundHandIdentityXR34(o,id,entity,side,attachment);
    side=-1;attachment=nullptr;void* pickable=nullptr;void* weapon=nullptr;void* weaponEntity=nullptr;unsigned weaponId=0;
    void* manager=nullptr;void* character=nullptr;void* expectedCharacter=nullptr;unsigned char mounted=0;
    const auto b=WhBase();std::uintptr_t vft=0,getCharacter=0;unsigned char actual[16]{};
    constexpr unsigned char entry[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0x49,0x38,0x48,0x8B,0xDA,0x33,0xD2,0x48};
    if(!retail_build112::Ready(b)||!Read(reinterpret_cast<unsigned char*>(o.source.equipment)+0x40,weapon)||
       reinterpret_cast<std::uintptr_t>(weapon)!=o.source.weaponItem||!RangedItemEntityXR34(weapon,weaponId,weaponEntity)||
       !Read(static_cast<unsigned char*>(weapon)+0x88,pickable)||!pickable||
       !Read(reinterpret_cast<unsigned char*>(o.main)+0xA6,mounted)||mounted!=1||
       !Read(reinterpret_cast<void*>(b+0x2A89B1C),actual)||std::memcmp(actual,entry,sizeof(entry))||
       !Read(weaponEntity,vft)||!Read(reinterpret_cast<void*>(vft+0x328),getCharacter)||getCharacter!=b+0x6411AC)return false;
    __try {
        attachment=reinterpret_cast<void*(__fastcall*)(void*,const char*)>(b+0x2A89B1C)(pickable,"lever");
        expectedCharacter=reinterpret_cast<void*(__fastcall*)(void*,int)>(getCharacter)(weaponEntity,0);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(!attachment||!ManualBoundEntity19c4r4(attachment,id,entity)||!PhysVft6698(expectedCharacter,0x40602F8)||
       !Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||character!=expectedCharacter)return false;
    g_ranged_mechanism_character_d78.store(reinterpret_cast<std::uintptr_t>(character),std::memory_order_release);
    side=1-o.source.primary;return true;
}
static bool RangedBoundHandXR34(const xcXR34::Owner& o,std::uint32_t id,void* entity,int& side,void*& attachment) noexcept {
    return RangedBoundHandIdentityXR34(o,id,entity,side,attachment)&&RangedOwnerXR34(o);
}
// Native creation and attachment completion are separate steps. Observe the
// SAME action/id/entity while native finishes binding; never bless a replacement
// or turn a detached, previously complete bolt back into factory-pending state.
struct RangedBoltJoinR95 {xcXR34::Owner owner{};unsigned id=0;void* entity=nullptr;std::uint64_t started=0;bool bound=false;};
static RangedBoltJoinR95 g_ranged_bolt_join_r95;
static bool RangedReadBoltBindingR95(const xcXR34::Owner& o,unsigned id,void*& entity,int& side,
    void*& attachment,bool& pending) noexcept {
    entity=nullptr;side=-1;attachment=nullptr;pending=false;
    const auto now=std::uint64_t(GetTickCount64());
    if(!id||o.source.primary<0||o.source.primary>1||!ReloadNativeOwnerXR34(o,true))return false;
    void* model=nullptr;void* player=nullptr;void* object=nullptr;const char* name=nullptr;
    unsigned parentId=0,slot=0,otherSlot=0;const int working=1-o.source.primary;
    if(!Read(reinterpret_cast<unsigned char*>(o.parent)+0x88,parentId)||parentId!=id||
       !(entity=EntityById6694(id))||!PhysVft6698(entity,0x4072C78)||
       !Read(reinterpret_cast<unsigned char*>(o.source.player)+0x1B0,model)||!model||
       !Read(static_cast<unsigned char*>(model)+0x20,player)||reinterpret_cast<std::uintptr_t>(player)!=o.source.player||
       !Read(static_cast<unsigned char*>(model)+0x2C+0x24*working,slot)||(slot&&slot!=id)||
       !Read(static_cast<unsigned char*>(model)+0x2C+0x24*o.source.primary,otherSlot)||otherSlot==id||
       !Read(reinterpret_cast<void*>(WhBase()+0x40AC298+8*working),name)||!name)return false;
    void* found=ManualFindHandAttachment19c4r4(model,name);
    if(found&&(!PhysVft6698(found,0x3A657C0)||!Read(static_cast<unsigned char*>(found)+0x20,object)))return false;
    if(object){unsigned attachedId=0;
        if(!PhysVft6698(object,0x3A68F68)||!Read(static_cast<unsigned char*>(object)+8,attachedId)||attachedId!=id)return false;
    }
    const bool complete=slot==id&&object;
    if(complete&&!RangedBoundHandIdentityXR34(o,id,entity,side,attachment))return false;
    std::unique_lock<std::recursive_mutex> lock(g_ranged_pose_mutex_xr34,std::try_to_lock);if(!lock.owns_lock())return false;
    auto& join=g_ranged_bolt_join_r95;
    if(!xcXR34::sameOwner(join.owner,o))join={o,id,entity,now,false};
    else if(join.id!=id||join.entity!=entity)return false;
    if((!complete&&join.bound)||!join.started||now<join.started||(!join.bound&&now-join.started>80))return false;
    if(!ReloadNativeOwnerXR34(o,true)||EntityById6694(id)!=entity||
       !Read(reinterpret_cast<unsigned char*>(o.parent)+0x88,parentId)||parentId!=id)return false;
    if(complete){join.bound=true;return true;}
    unsigned slotAfter=0,otherAfter=0;void* modelAfter=nullptr;void* objectAfter=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(o.source.player)+0x1B0,modelAfter)||modelAfter!=model||
       !Read(static_cast<unsigned char*>(model)+0x2C+0x24*working,slotAfter)||slotAfter!=slot||
       !Read(static_cast<unsigned char*>(model)+0x2C+0x24*o.source.primary,otherAfter)||otherAfter!=otherSlot||
       ManualFindHandAttachment19c4r4(model,name)!=found||
       (found&&(!Read(static_cast<unsigned char*>(found)+0x20,objectAfter)||objectAfter!=object)))return false;
    pending=true;side=working;attachment=nullptr;return true;
}
// Native item/binding observation is independent of any prior tracked wrist or
// general physical frame. Explicit absence before Spawn/Ammo is not a grip.
static bool RangedNativeBindingXR34(const xcXR34::Owner& o,xcXR34::Spatial& result) noexcept {
    result={};if(!ReloadNativeOwnerXR34(o,true))return false;
    void* tool=nullptr;void* entity=nullptr;void* attachment=nullptr;int side=-1;unsigned id=0;
    if(!Read(reinterpret_cast<unsigned char*>(o.main)+0x80,tool)||!Read(reinterpret_cast<unsigned char*>(o.parent)+0x88,id))return false;
    if(tool){unsigned toolId=0;void* pickable=nullptr;
        if(!PhysVft6698(tool,0x3A2DA88)||!Read(static_cast<unsigned char*>(tool)+0x30,result.toolKey)||
           PhysEmpty6698(result.toolKey)||!Read(static_cast<unsigned char*>(tool)+0x88,pickable))return false;
        result.toolItem=reinterpret_cast<std::uintptr_t>(tool);
        if(!pickable){
            // Positive native factory-pending state. Keep current wrists alive
            // while the real tool's async entity is being created; no binding or
            // show credit is granted until the complete identity appears.
            result.toolPendingD72=true;
        }else{
            if(!RangedItemEntityXR34(tool,toolId,entity)||!RangedBoundToolIdentityD72(o,toolId,entity,side,attachment))return false;
            result.toolId=toolId;result.toolEntity=reinterpret_cast<std::uintptr_t>(entity);
            result.toolAttachment=reinterpret_cast<std::uintptr_t>(attachment);result.toolBound=true;result.toolWeaponBoundD72=o.source.classId==15;
            bool hidden=true;if(EntityHiddenFlag59c14(entity,hidden))result.toolVisible=!hidden;
        }
    }
    if(id){bool pending=false;
        if(!RangedReadBoltBindingR95(o,id,entity,side,attachment,pending))return false;
        result.boltId=id;result.boltKey=o.source.ammoKey;result.boltEntity=reinterpret_cast<std::uintptr_t>(entity);
        result.boltAttachment=reinterpret_cast<std::uintptr_t>(attachment);result.nativeSide=side;
        result.handBoltPendingR95=pending;result.handBoltBound=!pending;
    }
    result.serial=o.serial;result.qualified=ReloadNativeOwnerXR34(o,true);return result.qualified;
}
static bool RangedDefaultsXR34(void* entity,int kind,pm::Vec3& rail,pm::Vec3& rest,
    QuatT28* arrowPoseD78=nullptr,int* arrowJointD78=nullptr,void** weaponCharacterD78=nullptr,int* stringJointD78=nullptr) noexcept {
    void* character=nullptr;void* skeleton=nullptr;
    std::uintptr_t vf=0,target=0;
    // Exact retail CEntity+328 ->6411AC, also called with slot0 by the native
    // tool MasterSlave path. Verify the receiver/target BEFORE making the call.
    if(!PhysVft6698(entity,0x4072C78)||!Read(entity,vf)||
       !Read(reinterpret_cast<void*>(vf+0x328),target)||target!=WhBase()+0x6411AC)return false;
    __try {character=reinterpret_cast<void*(__fastcall*)(void*,int)>(target)(entity,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(!PhysVft6698(character,0x40602F8)||!Read(static_cast<unsigned char*>(character)+0xA70,skeleton)||
       !PhysVft6698(skeleton,0x406B930))return false;
    std::uintptr_t poseVft=0,defaultVft=0,absGetter=0;
    if(!Read(skeleton,defaultVft)||!Read(reinterpret_cast<void*>(defaultVft+0x40),target)||target!=WhBase()+0x53D91C||
       !Read(static_cast<unsigned char*>(skeleton)+0x18,poseVft)||poseVft!=WhBase()+0x3A71510||
       !Read(reinterpret_cast<void*>(poseVft+0x50),absGetter)||absGetter!=WhBase()+0x53D830)return false;
    int arrow=-1,string=-1;const char* name=kind==10?"Crossbow_light_string":kind==14?"Crossbow_medium_string":"Crossbow_heavy_string";
    if(!ResolveJointByNameExactV6545(skeleton,"Crossbow_arrow",arrow)||arrow<0||
       !ResolveJointByNameExactV6545(skeleton,name,string)||string<0)return false;
    QuatT28 a{},s{};
    __try {const auto fn=reinterpret_cast<const QuatT28*(__fastcall*)(void*,short)>(WhBase()+0x53D91C);
        const auto* pa=fn(skeleton,short(arrow));const auto* ps=fn(skeleton,short(string));
        if(!pa||!ps||!Read(pa,a)||!Read(ps,s))return false;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(!ValidPose6689(a)||!ValidPose6689(s))return false;
    const auto at=FromT(a),st=FromT(s);const double expected=kind==10?.231734782457:kind==14?.269499987364:.312299996614;
    // Exact CHR800 CRC-validated native defaults, not generic weapon-size guesses.
    if(Len(Sub(at,{0,.137999981642f,.059619996697f}))>.0001f||
       Len(Sub(st,{0,float(expected),.059749998152f}))>.0001f)return false;
    if(arrowPoseD78)*arrowPoseD78=a;if(arrowJointD78)*arrowJointD78=arrow;if(weaponCharacterD78)*weaponCharacterD78=character;
    if(stringJointD78)*stringJointD78=string;
    g_ranged_mechanism_character_d78.store(reinterpret_cast<std::uintptr_t>(character),std::memory_order_release);
    rail={at.x,at.y,at.z};rest={st.x,st.y,st.z};return true;
}
static pm::Vec3 RangedPointMatrixXR34(const Matrix34V6550& m,pm::Vec3 p) noexcept {
    return {m.m[0]*p.x+m.m[1]*p.y+m.m[2]*p.z+m.m[3],m.m[4]*p.x+m.m[5]*p.y+m.m[6]*p.z+m.m[7],m.m[8]*p.x+m.m[9]*p.y+m.m[10]*p.z+m.m[11]};
}
static bool RangedPlacementStillCurrentXR34(const RangedPlacedXR34& p) noexcept {
    Matrix34V6550 current{};
    if(!p.valid||!p.entity||!p.id||EntityById6694(p.id)!=p.entity||!pm::finite(p.localGrip)||!pm::finite(p.worldGrip)||
       !GetEntityWorldTmV6552(p.entity,p.id,current)||
       pm::length(RangedPointMatrixXR34(current,p.localGrip)-p.worldGrip)>.004)return false;
    // Rotation about slt_0 leaves its point unchanged. An alignment receipt
    // must also prove that the achieved entity basis has not been replaced.
    if(p.helperFrameD78||p.poseBasisD78)for(int i:{0,1,2,4,5,6,8,9,10})
        if(!std::isfinite(current.m[i])||std::abs(current.m[i]-p.achievedD78.m[i])>1e-4)return false;
    return true;
}
static bool RangedSpatialXR34(const xcXR34::Owner& o,const xcXR34::Source& source,xcXR34::Spatial& result) noexcept {
    result={};ManualItems596 items{};
    // R108: use the caller's accepted immutable packet. A second mailbox read
    // can select N+1 after Client::tick accepted N and falsely revoke the hand
    // owner as GEOMETRY_UNAVAILABLE. Live ownership and cancellation still
    // revalidate; a newer publication supplies no pose/gesture credit here.
    if(!RangedOwnerXR34(o)||source.cancellationR93!=xcXR34::sourceCancellationR93.load(std::memory_order_acquire)||
       !xcXR34::fresh(source,GetTickCount64())||!xcXR34::sameSource(source,o.source)||
       !ManualItemsRead596(reinterpret_cast<void*>(o.source.player),items)||items.weaponKey!=o.source.key)return false;
    std::uint32_t weaponId=0;void* weaponEntity=nullptr;Matrix34V6550 weaponMatrix{};pm::Vec3 rail{},rest{};
    QuatT28 railPoseD78{};int railJointD78=-1;void* weaponCharacterD78=nullptr;
    if(!RangedItemEntityXR34(items.weapon,weaponId,weaponEntity)||!GetEntityWorldTmV6552(weaponEntity,weaponId,weaponMatrix)||
       !RangedDefaultsXR34(weaponEntity,source.classId,rail,rest,&railPoseD78,&railJointD78,&weaponCharacterD78)||source.world[7]<=0)return false;
    const auto inverseWorld=ph::inverse(PhysWorld6698(source.world));
    const auto inverseWeapon=ph::inverse(PhysPose6698(QuatT28{{source.weaponModel.q[0],source.weaponModel.q[1],source.weaponModel.q[2],source.weaponModel.q[3]},
        {source.weaponModel.p[0],source.weaponModel.p[1],source.weaponModel.p[2]}}));
    const auto local=[&](pm::Vec3 p){auto v=ph::point(inverseWorld,p)*(1./source.world[7]);return ph::point(inverseWeapon,v);};
    const auto railWorld=RangedPointMatrixXR34(weaponMatrix,rail),restWorld=RangedPointMatrixXR34(weaponMatrix,rest);
    const auto r=local(railWorld),s=local(restWorld),delta=r-s;const double distance=pm::length(delta);
    if(!std::isfinite(distance)||distance<.03||distance>.5)return false;
    if(!RangedNativeBindingXR34(o,result))return false;
    result.serial=o.serial;result.sample=source.sample;result.tick=source.tick;
    result.rail={r.x,r.y,r.z};result.stringRest={s.x,s.y,s.z};result.axis={delta.x/distance,delta.y/distance,delta.z/distance};result.travel=distance;
    unsigned char cocked=0;if(!Read(static_cast<unsigned char*>(items.component)+0xD8,cocked)||cocked>1)return false;
    result.nativeCocked=cocked!=0;result.nativeLoaded=items.loaded&&items.nockedKey==items.ammoKey;
    result.nockBound=result.nativeLoaded&&ManualNockAttached19c4r4(items);
    RangedPlacedXR34 placed{};
    if(g_ranged_placed_xr34.read(placed)&&placed.valid&&xcXR34::sameOwner(placed.owner,o)&&
       placed.sample<=source.sample&&source.tick>=placed.tick&&source.tick-placed.tick<=80&&RangedPlacementStillCurrentXR34(placed)){
        const auto grip=local(placed.worldGrip);result.grip={grip.x,grip.y,grip.z};
        result.toolPoseReady=!placed.bolt&&placed.id==result.toolId&&result.toolBound&&
            reinterpret_cast<std::uintptr_t>(placed.toolItem)==result.toolItem&&placed.toolKey==result.toolKey;
        result.boltPoseReady=placed.bolt&&placed.id==result.boltId&&result.handBoltBound;
        if(result.toolPoseReady||result.boltPoseReady)result.placementTickR94=placed.tick;
        result.handBoltPlaced=result.boltPoseReady&&pm::length(placed.worldGrip-railWorld)<=.08;
        if(xcXR34::handDrivenD81(source)&&result.boltPoseReady)
            RangedBoltAlignmentD78(o,items,weaponCharacterD78,railJointD78,railPoseD78,weaponMatrix,placed,result);
    }
    if(source.classId==10){result.toolPoseReady=true;result.grip=result.stringRest;}
    // D74: Instant controls native clock transport, not the existing class15
    // VR crank gesture's direction/travel. Keep its opt-in railpush setting in
    // both modes. This ergonomic approximation is not native crank-motion proof.
    if(source.classId==15){
        // D66C heavy crank contract. Joints (RangedDefaultsXR34, CHR800 defaults): "Crossbow_heavy_string"
        // (0,.3123,.05975) is the string rest = where the crank engages the string = the grasp point;
        // "Crossbow_arrow" (0,.138,.0596) is the rail/nock end. The class 14 pull axis is string -> arrow
        // (backwards, 13.15 cm); the crank is pushed the other way, string -> stirrup, by VrCrankTravelD66C.
        // toolPoseReady stays what the placement receipt says: the crank must sit in the VR hand first.
        result.grip=result.stringRest;
        result.axis={-result.axis.x,-result.axis.y,-result.axis.z};
        result.travel=double(kcd2vr::ranged_options_d66c::crankTravelSetting(kcd2vr::ranged_options_d66c::crankTravel.load(std::memory_order_acquire)));
        result.crankQualified=xcXR34::finite(result.axis)&&xcXR34::finite(result.grip)&&xcXR34::crankTravelValidD66C(result.travel);
        result.crankProgress=0;
    }
    RangedMechanismSpatialD78(o,source,result,result.toolPoseReady?&placed:nullptr);
    result.qualified=RangedOwnerXR34(o)&&
        source.cancellationR93==xcXR34::sourceCancellationR93.load(std::memory_order_acquire)&&
        xcXR34::fresh(source,GetTickCount64());
    return result.qualified;
}
// Fresh raw acquisition intent is independent of an auxiliary pose join.
// It grants no clock/event/stroke permission; Client consumes it only after
// its ordinary completed-source and native ownership gates are satisfied.
static bool RangedQuiverInputR95(const xcXR34::Owner& o,xcXR34::QuiverInputR95& out) noexcept {
    out={};TrackingSnapshot tracking{};
    if(o.source.primary!=1||!ReadTrackingSnapshot(tracking))return false;
    const auto now=std::uint64_t(GetTickCount64());
    if(!tracking.allowed||!tracking.sequence||
       !tracking.tick||now<tracking.tick||now-tracking.tick>80||tracking.source_epoch!=o.source.tracking||
       !tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held||
       !ReloadNativeOwnerXR34(o,true)||!kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(o.source))return false;
    xcXR34::Source region{};ChestFlagsD66C(region,tracking);
    if(!region.quiverKnownD72||!std::isfinite(tracking.right_grip))return false;
    out={tracking.sequence,tracking.tick,double(tracking.right_grip),region.handInQuiverD72[0]};
    return ReloadNativeOwnerXR34(o,true)&&kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(o.source);
}
// Release is safety/input retirement, not new pose or gesture authority. Read
// the current tracked family, then its age clock; never restamp the retained
// reload packet or turn a held edge into acquisition during an auxiliary gap.
static bool RangedGapReleaseR95(const xcXR34::Source& previous,bool& released) noexcept {
    TrackingSnapshot tracking{};released=false;
    if(!ReadTrackingSnapshot(tracking))return false;
    const auto now=std::uint64_t(GetTickCount64());
    if(previous.primary<0||previous.primary>1||!tracking.allowed||!tracking.sequence||
       tracking.sequence<previous.sample||tracking.source_epoch!=previous.tracking||
       !tracking.tick||now<tracking.tick||now-tracking.tick>80||
       !tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held)return false;
    const float grip=previous.primary==1?tracking.right_grip:tracking.left_grip;
    if(!std::isfinite(grip)||grip<0||grip>1)return false;
    released=grip<.25f;return true;
}
static void RangedPublishXR34() noexcept {
    xcXR34::RefreshRegistration();
    RangedReloadEquipmentXR34();
    xcXR34::SetSpatialReader(&RangedSpatialXR34);xcXR34::quiverReaderR95.store(&RangedQuiverInputR95,std::memory_order_release);
    const auto before=ReadCompletedPublicationT23();
    xcXR34::Source source{};const bool read=RangedSourceXR34(source);
    const auto after=ReadCompletedPublicationT23();
    if(!read){
        xcXR34::Status state{};xcXR34::Source previous{};CommittedFrame6695 body{};bool released=false;
        const auto now=std::uint64_t(GetTickCount64());
        const bool publicationPending=(before&1)||(after&1)||before!=after;
        // A positively completed ordinary body can lack only its auxiliary
        // reload socket. Its wrists are never borrowed as input for the reload.
        const bool auxiliaryMissing=ReadCommittedPacket6695(body)&&!body.reloadSourceXR34.allowed;
        if((publicationPending||auxiliaryMissing)&&CrossbowReloadEnabledD65C()&&
           xcXR34::ReadStatus(state)&&state.clockOwned&&(state.sourceEstablished||xcXR34::completedNativeLease.load(std::memory_order_acquire)==state.owner.serial)&&
           state.phase!=xcXR34::Phase::Retiring&&xcXR34::ReadSource(previous)&&
           xcXR34::fresh(previous,now)&&previous.completedLease==state.owner.serial&&
           xcXR34::sameSource(previous,state.owner.source)&&ReloadNativeOwnerXR34(state.owner,true)&&
           kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(previous)&&RangedGapReleaseR95(previous,released)&&
           (publicationPending||(body.stamp.actor==previous.character&&body.stamp.epoch==previous.body&&
               body.doorTrackingEpoch66==previous.tracking&&body.valid[0]&&body.valid[1]))){
            // A separate non-authorizing state instructs the sole native
            // executor to pause its exact clock. No timestamp, sample, body
            // serial or edge is refreshed; repeated gaps retain this same age.
            if(!previous.auxiliaryUnavailableR95||(released&&!previous.auxiliaryReleasedR95)){
                previous.auxiliaryUnavailableR95=true;previous.auxiliaryReleasedR95|=released;
                xcXR34::PublishSource(previous);
            }
            return;
        }
    }
    xcXR34::PublishSource(source); // confirmed loss still cancels immediately
}
// The existing CEntityAttachment callback and coherent pose transaction call
// these two contributions. They never manufacture a tool/bolt or bypass native
// lifetime/attachment removal. Target mailboxes require a COMPLETED body stamp.
// Native relative-slot animation changes a descriptor, not the held item.
// Keep one grip calibration for the exact action/entity/socket identity.
static bool RangedSameGripBindingXR34(const RangedSocketXR34& a,const RangedSocketXR34& b) noexcept {
    return xcXR34::sameOwner(a.owner,b.owner)&&a.self==b.self&&a.attachment==b.attachment&&
        a.entity==b.entity&&a.toolItem==b.toolItem&&a.toolKey==b.toolKey&&a.id==b.id&&a.side==b.side&&a.joint==b.joint&&a.character==b.character&&
        a.bolt==b.bolt&&(!a.bolt||!std::memcmp(&a.helper,&b.helper,sizeof(a.helper)));
}
static bool RangedSameSocketXR34(const RangedSocketXR34& a,const RangedSocketXR34& b) noexcept {
    return RangedSameGripBindingXR34(a,b)&&!std::memcmp(&a.relative,&b.relative,sizeof(a.relative));
}
// Read only the same native entity/slot already used by the attachment writer.
// Called before the coherent body commit as well, so a freshly spawned bolt
// does not need a native visible frame merely to announce its descriptor.
static bool RangedReadHandSocketXR34(const xcXR34::Owner& owner,unsigned id,void* entity,void* tool,
    bool bolt,RangedSocketXR34& socket) noexcept {
    socket={};void* attachment=nullptr;int side=-1;
    if(!id||!entity||!RangedBoundHandXR34(owner,id,entity,side,attachment))return false;
    socket.owner=owner;socket.id=id;socket.entity=entity;socket.attachment=attachment;
    socket.side=side;socket.bolt=bolt;socket.toolItem=tool;
    if(tool&&(!Read(static_cast<unsigned char*>(tool)+0x30,socket.toolKey)||PhysEmpty6698(socket.toolKey)))return false;
    void* manager=nullptr;unsigned objectId=0;std::int16_t joint=-1;
    if(!Read(static_cast<unsigned char*>(attachment)+0x20,socket.self)||!PhysVft6698(socket.self,0x3A68F68)||
       !Read(static_cast<unsigned char*>(socket.self)+8,objectId)||objectId!=id||
       !Read(static_cast<unsigned char*>(attachment)+0x28,manager)||manager!=g_henry_char_attachment_manager.load()||
       !Read(static_cast<unsigned char*>(manager)+0x70,socket.character)||
       reinterpret_cast<std::uintptr_t>(socket.character)!=owner.source.character||
       !Read(static_cast<unsigned char*>(attachment)+0x78,joint)||joint<0||
       !Read(static_cast<unsigned char*>(attachment)+0x9C,socket.relative)||!ValidPose6689(socket.relative))return false;
    socket.joint=joint;
    return !bolt||ManualArrowHelper64(entity,socket.helper);
}
// Consumes an attachment callback without writing only while its exact last
// achieved destination and native binding remain present. The original pose
// age is retained. This does not create a placement/event/gesture receipt.
static bool RangedRetainHandPlacementR95(const RangedSocketXR34& socket) noexcept {
    RangedPlacedXR34 previous{};RangedSocketXR34 current{};
    const auto now=std::uint64_t(GetTickCount64());
    if(!g_ranged_placed_xr34.read(previous)||!previous.valid||!previous.bodyPublicationR95||
       !previous.tick||now<previous.tick||now-previous.tick>80||
       !xcXR34::sameOwner(previous.owner,socket.owner)||
       !(xcXR34::handDrivenD81(socket.owner.source)?RangedSameGripBindingXR34(previous.socketR95,socket):
            RangedSameSocketXR34(previous.socketR95,socket))||
       !RangedPlacementStillCurrentXR34(previous)||!RangedOwnerXR34(socket.owner)||
       !RangedReadHandSocketXR34(socket.owner,socket.id,socket.entity,socket.toolItem,socket.bolt,current)||
       !RangedSameSocketXR34(socket,current))return false;
    return ReloadNativeOwnerXR34(socket.owner,true)&&
        kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(socket.owner.source)&&RangedOwnerXR34(socket.owner);
}
static bool RangedRetainPrimaryPlacementR95(const xcXR34::Owner& owner,void* self,void* attachment,
    unsigned id,void* entity) noexcept {
    RangedPlacedXR34 previous{};void* bound=nullptr;unsigned objectId=0;
    if(!ReloadNativeOwnerXR34(owner,true)||!kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(owner.source)||
       !g_ranged_primary_placed_r95.read(previous)||!previous.valid||!previous.bodyPublicationR95)return false;
    // Primary attachment callbacks do not share the hand/tool placement lock.
    // Validate this copied receipt against time sampled after its publication.
    const auto now=std::uint64_t(GetTickCount64());
    return previous.tick&&now>=previous.tick&&now-previous.tick<=80&&
        xcXR34::sameOwner(previous.owner,owner)&&previous.entity==entity&&previous.id==id&&
        previous.socketR95.self==self&&previous.socketR95.attachment==attachment&&
        previous.toolKey==owner.source.key&&previous.toolItem==reinterpret_cast<void*>(owner.source.weaponItem)&&
        RangedPlacementStillCurrentXR34(previous)&&Read(static_cast<unsigned char*>(self)+8,objectId)&&objectId==id&&
        RangedBoundWeaponIdentityD72(owner,id,entity,bound)&&bound==attachment&&RangedOwnerXR34(owner);
}
static bool RangedRetainHeavyPlacementR95(const xcXR34::Status& state,void* self,void* attachment,
    unsigned id,void* tool,void* entity) noexcept {
    RangedPlacedXR34 previous{};xcXR34::Status current{};void* bound=nullptr;void* object=nullptr;void* nativeTool=nullptr;int side=-1;std::int16_t joint=-1;
    std::uint64_t key=0;const auto now=std::uint64_t(GetTickCount64());
    return g_committed_pose_valid6689.load(std::memory_order_acquire)&&
        xcXR34::ReadStatus(current)&&current.clockOwned&&current.placeTool&&current.phase!=xcXR34::Phase::Retiring&&
        xcXR34::sameOwner(current.owner,state.owner)&&current.toolMountedD78==state.toolMountedD78&&
        ReloadNativeOwnerXR34(state.owner,true)&&kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(state.owner.source)&&
        g_ranged_placed_xr34.read(previous)&&previous.valid&&previous.bodyPublicationR95&&
        previous.tick&&now>=previous.tick&&now-previous.tick<=80&&
        xcXR34::sameOwner(previous.owner,state.owner)&&previous.entity==entity&&previous.id==id&&previous.toolItem==tool&&
        previous.socketR95.self==self&&previous.socketR95.attachment==attachment&&
        previous.mountedD78==state.toolMountedD78&&
        Read(reinterpret_cast<unsigned char*>(state.owner.main)+0x80,nativeTool)&&nativeTool==tool&&
        Read(static_cast<unsigned char*>(tool)+0x30,key)&&key==previous.toolKey&&
        Read(static_cast<unsigned char*>(attachment)+0x20,object)&&object==self&&
        Read(static_cast<unsigned char*>(attachment)+0x78,joint)&&joint==previous.socketR95.joint&&
        RangedBoundToolIdentityD72(state.owner,id,entity,side,bound)&&bound==attachment&&
        RangedPlacementStillCurrentXR34(previous)&&RangedOwnerXR34(state.owner);
}
static ph::Pose RangedWristPoseD72(const xcXR34::Wrist& w) noexcept {
    return {{w.q[0],w.q[1],w.q[2],w.q[3]},{w.p[0],w.p[1],w.p[2]}};
}
// The already calibrated same-item primary grip keeps the real crossbow in
// the tracked left hand for the entire exact reload action.
static bool CrossbowWeaponAttachmentD72(const xcXR34::Status& state,void* self,void* attachment,unsigned id) noexcept {
    if(!xcXR34::handDrivenD81(state.owner.source))return false;
    void* weaponEntity=nullptr;void* bound=nullptr;unsigned weaponId=0;
    if(!RangedItemEntityXR34(reinterpret_cast<void*>(state.owner.source.weaponItem),weaponId,weaponEntity)||id!=weaponId||
       !RangedBoundWeaponIdentityD72(state.owner,id,weaponEntity,bound)||bound!=attachment)return false;
    const auto withdraw=[]() noexcept {(void)g_ranged_primary_placed_r95.publish({});return false;};
    const auto retain=[&]() noexcept {
        if(RangedRetainPrimaryPlacementR95(state.owner,self,attachment,id,weaponEntity))return true;
        return withdraw();
    };
    CommittedFrame6695 body{};ModelWorld6692 world{};float natural[8]{};
    // R96 single writer: while THIS exact reload owner holds the crossbow, a
    // body packet without its reload payload still has the tracked primary
    // wrist. Place the weapon from that wrist with the owner's own calibrated
    // grip (the same formula as the full path below) instead of handing the
    // frame to the native attachment: native/VR alternation was the flicker.
    // Presentation only: no receipt, no progress, no clock or event change.
    const auto hold=[&]() noexcept {
        ModelWorld6692 holdWorld{};float holdNatural[8]{};const int primary=state.owner.source.primary;
        if(primary<0||primary>1||!body.valid[primary]||body.stamp.actor!=state.owner.source.character||
           body.stamp.epoch!=state.owner.source.body||body.doorTrackingEpoch66!=state.owner.source.tracking||
           !g_committed_pose_valid6689.load(std::memory_order_acquire)||!xcXR34::wristValidD78(state.owner.source.primaryGripD72)||
           !PhysAttachmentWorld164(attachment,state.owner.source.character,holdWorld)||!ReadAttachmentQs6689(attachment,holdNatural))return withdraw();
        const auto holdModel=ph::compose(PhysPose6698(body.allowed[primary]),ph::inverse(RangedWristPoseD72(state.owner.source.primaryGripD72)));
        const auto holdWanted=PhysCurrentWorld164(holdModel,holdWorld);
        if(!ph::finite(holdWanted)||!RangedOwnerXR34(state.owner))return withdraw();
        float holdQs[8]{float(holdWanted.q.x),float(holdWanted.q.y),float(holdWanted.q.z),float(holdWanted.q.w),
            float(holdWanted.p.x),float(holdWanted.p.y),float(holdWanted.p.z),holdNatural[7]};
        // No receipt: holding earns no placement/progress credit (as withdraw).
        (void)g_ranged_primary_placed_r95.publish({});
        bool holdChild=false;(void)PlaceBothConsumers6689(self,weaponEntity,holdQs,holdChild);
        return true; // consumed: no native frame in between
    };
    if(!ReadCommittedPacket6695(body))return (ReadCompletedPublicationT23()&1)?retain():withdraw();
    const auto& source=body.reloadSourceXR34;
    if(!source.allowed){
        if(body.stamp.actor==state.owner.source.character&&body.stamp.epoch==state.owner.source.body&&
           body.doorTrackingEpoch66==state.owner.source.tracking&&body.valid[0]&&body.valid[1]){
            if(RangedRetainPrimaryPlacementR95(state.owner,self,attachment,id,weaponEntity))return true;
            return hold();}
        return withdraw();
    }
    if(source.auxiliaryUnavailableR95||!xcXR34::sameSource(source,state.owner.source)||
       !xcXR34::fresh(source,GetTickCount64())||!body.publicationR24||(body.publicationR24&1)||
       source.bodyPublicationR95!=body.publicationR24||body.stamp.sample!=source.sample||body.stamp.tick!=source.tick||
       body.stamp.actor!=source.character||body.stamp.epoch!=source.body||!body.valid[source.primary])return hold();
    if(!ReloadPoseWriterCurrentXR34(state.owner))return retain();
    if(!PhysAttachmentWorld164(attachment,source.character,world)||!ReadAttachmentQs6689(attachment,natural))return withdraw();
    const auto model=ph::compose(PhysPose6698(body.allowed[source.primary]),ph::inverse(RangedWristPoseD72(state.owner.source.primaryGripD72)));
    const auto wanted=PhysCurrentWorld164(model,world);
    if(!ph::finite(wanted)||!RangedOwnerXR34(state.owner))return withdraw();
    if(body.publicationR24!=ReadCompletedPublicationT23())return retain();
    if(!g_committed_pose_valid6689.load(std::memory_order_acquire)||source.tracking!=tracking59c6::epoch.load(std::memory_order_acquire))return withdraw();
    (void)g_ranged_primary_placed_r95.publish({});
    float qs[8]{float(wanted.q.x),float(wanted.q.y),float(wanted.q.z),float(wanted.q.w),float(wanted.p.x),float(wanted.p.y),float(wanted.p.z),natural[7]};
    bool child=false,checked=false;Matrix34V6550 achievedMatrix{};
    const bool achieved=PlaceBothConsumers6689(self,weaponEntity,qs,child)&&PlacementReadback6689(self,weaponEntity,id,qs,checked)&&
        GetEntityWorldTmV6552(weaponEntity,id,achievedMatrix);
    const bool currentDrawPlacementD75=body.publicationR24==ReadCompletedPublicationT23()&&
        g_committed_pose_valid6689.load(std::memory_order_acquire)&&source.tracking==tracking59c6::epoch.load(std::memory_order_acquire)&&
        RangedBoundWeaponIdentityD72(state.owner,id,weaponEntity,bound)&&bound==attachment;
    if(achieved&&currentDrawPlacementD75&&RangedOwnerXR34(state.owner)){
        RangedPlacedXR34 receipt{};receipt.owner=state.owner;receipt.entity=weaponEntity;receipt.id=id;
        receipt.toolItem=reinterpret_cast<void*>(source.weaponItem);receipt.toolKey=source.key;
        receipt.sample=source.sample;receipt.tick=source.tick;receipt.bodyPublicationR95=body.publicationR24;
        receipt.socketR95.owner=state.owner;receipt.socketR95.self=self;receipt.socketR95.attachment=attachment;
        receipt.worldGrip=RangedPointMatrixXR34(achievedMatrix,{});receipt.achievedD78=achievedMatrix;
        receipt.poseBasisD78=true;receipt.valid=pm::finite(receipt.worldGrip);
        (void)g_ranged_primary_placed_r95.publish(receipt);
        PhysTryLock6698 visibilityLock;if(visibilityLock.owned){
            const rdrawD72::Identity binding{reinterpret_cast<std::uintptr_t>(weaponEntity),reinterpret_cast<std::uintptr_t>(self),
                reinterpret_cast<std::uintptr_t>(attachment),source.key,source.generation,source.body,source.request,id,source.primary};
            RangedDrawPlacedD72(binding,true,true);
        }
    }
    return true; // consume partial writes too; no second native placement
}
static bool CrossbowHeavyToolAttachmentD72(const xcXR34::Status& state,void* self,void* attachment,unsigned id,void* tool,void* entity) noexcept {
    if(!xcXR34::handDrivenD81(state.owner.source)||state.owner.source.classId!=15||!state.placeTool)return false;
    void* bound=nullptr;int side=-1;
    if(!RangedBoundToolIdentityD72(state.owner,id,entity,side,bound)||bound!=attachment)return false;
    std::unique_lock<std::recursive_mutex> lock(g_ranged_pose_mutex_xr34,std::try_to_lock);if(!lock.owns_lock())return true;
    const auto withdraw=[]() noexcept {(void)g_ranged_placed_xr34.publish({});return false;};
    const auto retain=[&]() noexcept {
        if(RangedRetainHeavyPlacementR95(state,self,attachment,id,tool,entity))return true;
        return withdraw();
    };
    xcXR34::Passenger passenger{};CommittedFrame6695 body{};
    // R96 single writer: before it is mounted, the SAME captured lever stays at
    // the tracked right wrist of the current body when the reload payload is
    // missing (native/VR alternation was the flicker). Mounted lever motion
    // keeps its exact mechanism checks. Presentation only, no receipt.
    const auto hold=[&]() noexcept {
        const auto& c=g_ranged_heavy_grip_d72;void* holdManager=nullptr;void* holdCharacter=nullptr;
        ModelWorld6692 holdBody{};float holdNatural[8]{};
        // R97: hold only the SAME calibrated native item: key, socket joint and
        // the action's current tool (main+80) as the capture; a replaced one
        // goes through the full path (new calibration) or back to native.
        // R97: and only while the CURRENT status still asks us to place this
        // unmounted tool (same checks as R96R1 retention: placeTool, clock, owner).
        std::uint64_t holdKey=0;std::int16_t holdJoint=-1;void* holdTool=nullptr;void* holdSelf=nullptr;xcXR34::Status holdNow{};
        if(!xcXR34::ReadStatus(holdNow)||!holdNow.clockOwned||!holdNow.placeTool||holdNow.phase==xcXR34::Phase::Retiring||
           !xcXR34::sameOwner(holdNow.owner,state.owner)||holdNow.toolMountedD78||!Read(static_cast<unsigned char*>(tool)+0x30,holdKey)||holdKey!=c.key||
           !Read(static_cast<unsigned char*>(attachment)+0x78,holdJoint)||holdJoint!=c.joint||
           !Read(static_cast<unsigned char*>(attachment)+0x20,holdSelf)||holdSelf!=self||
           !Read(reinterpret_cast<unsigned char*>(state.owner.main)+0x80,holdTool)||holdTool!=tool||
           tracking59c6::epoch.load(std::memory_order_acquire)!=state.owner.source.tracking)return withdraw();
        if(state.toolMountedD78||!c.ready||!xcXR34::sameOwner(c.owner,state.owner)||c.id!=id||c.toolItem!=tool||c.entity!=entity||
           c.self!=self||c.attachment!=reinterpret_cast<std::uintptr_t>(attachment)||side<0||side>1||!body.valid[side]||
           body.stamp.actor!=state.owner.source.character||body.stamp.epoch!=state.owner.source.body||
           body.doorTrackingEpoch66!=state.owner.source.tracking||!g_committed_pose_valid6689.load(std::memory_order_acquire)||
           !Read(static_cast<unsigned char*>(attachment)+0x28,holdManager)||!holdManager||
           !Read(static_cast<unsigned char*>(holdManager)+0x70,holdCharacter)||holdCharacter!=c.character||
           !ReadModelWorld6692(reinterpret_cast<void*>(state.owner.source.character),holdBody)||!ReadAttachmentQs6689(attachment,holdNatural))return withdraw();
        const auto holdWanted=PhysCurrentWorld164(ph::compose(PhysPose6698(body.allowed[side]),ph::inverse(c.grip)),holdBody);
        if(!ph::finite(holdWanted)||!RangedOwnerXR34(state.owner))return withdraw();
        float holdQs[8]{float(holdWanted.q.x),float(holdWanted.q.y),float(holdWanted.q.z),float(holdWanted.q.w),
            float(holdWanted.p.x),float(holdWanted.p.y),float(holdWanted.p.z),holdNatural[7]};
        (void)g_ranged_placed_xr34.publish({}); // no receipt/credit while held
        bool holdChild=false;(void)PlaceBothConsumers6689(self,entity,holdQs,holdChild);
        return true;
    };
    // R97 merge: R96R1's 80 ms retention of the unchanged achieved lever first
    // (no write, no renewed receipt), then R96's hold at the tracked wrist
    // (unmounted only, no receipt). One order for every gap, never native in between.
    const auto retainOrHold=[&]() noexcept {
        if(RangedRetainHeavyPlacementR95(state,self,attachment,id,tool,entity))return true;
        return hold();
    };
    if(!ReadCommittedPacket6695(body))return (ReadCompletedPublicationT23()&1)?retain():withdraw();
    const auto& source=body.reloadSourceXR34;
    if(!source.allowed){
        if(body.stamp.actor==state.owner.source.character&&body.stamp.epoch==state.owner.source.body&&
           body.doorTrackingEpoch66==state.owner.source.tracking&&body.valid[0]&&body.valid[1])return retainOrHold();
        return withdraw();
    }
    if(source.auxiliaryUnavailableR95||!xcXR34::sameSource(source,state.owner.source)||!xcXR34::fresh(source,GetTickCount64())||
       !body.publicationR24||(body.publicationR24&1)||source.bodyPublicationR95!=body.publicationR24||
       body.stamp.sample!=source.sample||body.stamp.tick!=source.tick||body.stamp.actor!=source.character||
       body.stamp.epoch!=source.body||!body.valid[side])return hold();
    // Native pre-update may temporarily withdraw the passenger before its
    // successor is published. Keep only the already achieved exact binding;
    // its original deadline and current native lifetime still apply.
    if(!ReadReloadPassengerXR34(passenger,source.character,source.body,GetTickCount64())||!xcXR34::sameOwner(passenger.owner,state.owner)||
       !ReloadPoseWriterCurrentXR34(state.owner,passenger.publication))return retainOrHold();
    void* manager=nullptr;void* character=nullptr;ModelWorld6692 parentWorld{},bodyWorld{};float natural[8]{};
    if(!Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||!PhysVft6698(character,0x40602F8)||
       !ReadModelWorld6692(character,parentWorld)||!ReadModelWorld6692(reinterpret_cast<void*>(source.character),bodyWorld)||
       !ReadAttachmentQs6689(attachment,natural)||parentWorld.scale<=0)return withdraw();
    void* socketSelf=nullptr;std::int16_t socketJoint=-1;QuatT28 socketRelative{};
    if(!Read(static_cast<unsigned char*>(attachment)+0x20,socketSelf)||socketSelf!=self||
       !Read(static_cast<unsigned char*>(attachment)+0x78,socketJoint)||socketJoint<0||
       !Read(static_cast<unsigned char*>(attachment)+0x9C,socketRelative)||!ValidPose6689(socketRelative))return withdraw();
    RangedMechanismD78 mechanism{};
    const bool measured=source.intentKnownD78;
    if(measured&&(!RangedReadMechanismD78(state.owner,source,passenger.binding,mechanism)||
       !xcXR34::sameMechanismClockD78(mechanism.clock,passenger.clock)))return retainOrHold();
    std::uint64_t toolKey=0;if(!Read(static_cast<unsigned char*>(tool)+0x30,toolKey)||PhysEmpty6698(toolKey))return withdraw();
    auto& capture=g_ranged_heavy_grip_d72;
    if(!capture.ready||!xcXR34::sameOwner(capture.owner,state.owner)||capture.id!=id||capture.key!=toolKey||
       capture.attachment!=reinterpret_cast<std::uintptr_t>(attachment)||capture.toolItem!=tool||
       capture.entity!=entity||capture.self!=self||capture.character!=character||capture.joint!=socketJoint||
       (measured&&capture.skeleton!=mechanism.skeleton)){
        // The item-to-hand grip survives native animation of this SAME socket.
        // A replaced item/entity/attachment/joint still needs a new spawn calibration.
        capture={};
        // Calibrate against the game's actual pre-Spawn lever socket and its
        // original wrist, with the tracked weapon's parent transform removed.
        // Never infer a grab point from an invisible prop following the hand.
        const float edge=std::max(0.f,passenger.clip.spawn);
        if(passenger.clock.normalized<edge-2e-4f||passenger.clock.normalized>edge+2e-4f||
           (measured&&(source.nativePoseClockD78!=passenger.clock.entry||
            source.nativePoseNormalizedD78!=passenger.clock.normalized||source.nativePoseActiveTimeD78!=passenger.clock.activeTime)))return withdraw();
        ph::Pose local{};
        if(measured)local=PhysPose6698(mechanism.toolModel);
        else{const auto parent=PhysCurrentWorld164(ph::Pose{},parentWorld);
            local=ph::compose(ph::inverse(parent),PhysWorld6698(natural));local.p=local.p*(1./parentWorld.scale);}
        const auto nativeTool=ph::compose(RangedWristPoseD72(source.nativeWeaponD72),local);
        const auto grip=ph::compose(ph::inverse(nativeTool),RangedWristPoseD72(source.nativeWrist[side]));
        if(!ph::finite(grip)||pm::length(grip.p)>.8)return withdraw();
        capture={state.owner,id,grip,true,toolKey,reinterpret_cast<std::uintptr_t>(attachment),socketRelative,socketJoint,
            tool,entity,self,character,mechanism.skeleton};
    }
    // Exactly one writer: hand transport before mount, untouched native
    // weapon-socket motion after mount. Release frees the wrist, not the tool.
    const auto model=state.toolMountedD78?
        ph::compose(RangedWristPoseD72(source.weaponModel),PhysPose6698(mechanism.toolModel)):
        ph::compose(PhysPose6698(body.allowed[side]),ph::inverse(capture.grip));
    const auto currentHeavyR93R2=[&]() noexcept {
        xcXR34::Status current{};xcXR34::Spatial binding{};void* object=nullptr;
        std::int16_t joint=-1;QuatT28 relative{};RangedMechanismD78 currentMechanism{};
        if(!body.publicationR24||(body.publicationR24&1)||body.publicationR24!=ReadCompletedPublicationT23()||
           !g_committed_pose_valid6689.load(std::memory_order_acquire)||
           source.tracking!=tracking59c6::epoch.load(std::memory_order_acquire)||
           !xcXR34::fresh(source,GetTickCount64())||
           !xcXR34::ReadStatus(current)||!current.clockOwned||!current.placeTool||!xcXR34::sameOwner(current.owner,state.owner)||
           current.toolMountedD78!=state.toolMountedD78||!RangedNativeBindingXR34(state.owner,binding)||
           !binding.toolBound||binding.toolItem!=reinterpret_cast<std::uintptr_t>(tool)||binding.toolKey!=toolKey||
           binding.toolId!=id||binding.toolEntity!=reinterpret_cast<std::uintptr_t>(entity)||
           binding.toolAttachment!=reinterpret_cast<std::uintptr_t>(attachment)||
           !Read(static_cast<unsigned char*>(attachment)+0x20,object)||object!=self||
           !Read(static_cast<unsigned char*>(attachment)+0x78,joint)||joint!=socketJoint||
           !Read(static_cast<unsigned char*>(attachment)+0x9C,relative)||
           std::memcmp(&relative,&socketRelative,sizeof(relative)))return false;
        // The saved grip is stable identity; today's descriptor and measured
        // native pose must still remain coherent across this actual write.
        return !measured||(RangedReadMechanismD78(state.owner,source,binding,currentMechanism)&&
            xcXR34::sameMechanismClockD78(currentMechanism.clock,passenger.clock)&&
            currentMechanism.character==mechanism.character&&currentMechanism.skeleton==mechanism.skeleton&&
            currentMechanism.joint==mechanism.joint&&
            !std::memcmp(&currentMechanism.relative,&mechanism.relative,sizeof(mechanism.relative))&&
            !std::memcmp(&currentMechanism.toolModel,&mechanism.toolModel,sizeof(mechanism.toolModel)));
    };
    const auto wanted=PhysCurrentWorld164(model,bodyWorld);
    if((state.toolMountedD78&&!measured)||!ph::finite(wanted)||!RangedOwnerXR34(state.owner))return withdraw();
    if(!currentHeavyR93R2()||!ReloadPoseWriterCurrentXR34(state.owner,passenger.publication))
        return body.publicationR24!=ReadCompletedPublicationT23()?retain():withdraw();
    float qs[8]{float(wanted.q.x),float(wanted.q.y),float(wanted.q.z),float(wanted.q.w),float(wanted.p.x),float(wanted.p.y),float(wanted.p.z),natural[7]};
    RangedPlacedXR34 receipt{};receipt.owner=state.owner;receipt.entity=entity;receipt.toolItem=tool;receipt.id=id;
    receipt.sample=source.sample;receipt.tick=source.tick;receipt.bodyPublicationR95=body.publicationR24;
    receipt.socketR95.owner=state.owner;receipt.socketR95.self=self;receipt.socketR95.attachment=attachment;
    receipt.socketR95.joint=socketJoint;
    if(!Read(static_cast<unsigned char*>(tool)+0x30,receipt.toolKey))return withdraw();
    float sx=0,sy=0,sz=0;if(!Read(static_cast<unsigned char*>(self)+0xC,sx)||!Read(static_cast<unsigned char*>(self)+0x10,sy)||
       !Read(static_cast<unsigned char*>(self)+0x14,sz)||std::abs(sx*natural[7])<1e-4||std::abs(sy*natural[7])<1e-4||std::abs(sz*natural[7])<1e-4)return withdraw();
    const pm::Vec3 localGrip{capture.grip.p.x*bodyWorld.scale/(sx*natural[7]),capture.grip.p.y*bodyWorld.scale/(sy*natural[7]),capture.grip.p.z*bodyWorld.scale/(sz*natural[7])};
    (void)g_ranged_placed_xr34.publish({});
    bool child=false,checked=false;Matrix34V6550 achieved{};
    if(PlaceBothConsumers6689(self,entity,qs,child)&&PlacementReadback6689(self,entity,id,qs,checked)&&
       GetEntityWorldTmV6552(entity,id,achieved)&&currentHeavyR93R2()&&RangedOwnerXR34(state.owner)&&ReloadPoseWriterCurrentXR34(state.owner,passenger.publication)){
        receipt.localGrip=localGrip;receipt.worldGrip=RangedPointMatrixXR34(achieved,localGrip);
        receipt.valid=pm::finite(receipt.worldGrip);
        receipt.poseBasisD78=measured&&receipt.valid;receipt.achievedD78=achieved;
        if(state.toolMountedD78){RangedMechanismD78 after{};
            receipt.valid=receipt.valid&&RangedReadMechanismD78(state.owner,source,passenger.binding,after)&&
                xcXR34::sameMechanismClockD78(after.clock,passenger.clock)&&
                !std::memcmp(&after.toolModel,&mechanism.toolModel,sizeof(after.toolModel));
            receipt.mountedD78=receipt.valid;receipt.mechanismClockD78=mechanism.clock;receipt.mechanismPoseD78=mechanism.toolModel;}}
    (void)g_ranged_placed_xr34.publish(receipt);return true;
}
static bool CrossbowAttachmentXR34(void* self,void* attachment) noexcept {
    xcXR34::Status state{};
    if(!xcXR34::ReadStatus(state)||!state.clockOwned||state.phase==xcXR34::Phase::Retiring||
       !PhysVft6698(self,0x3A68F68)||!RangedOwnerXR34(state.owner))return false;
    const auto passengerPublication=xcXR34::passengerPublication.load(std::memory_order_acquire);
    std::uint32_t id=0,bolt=0,toolId=0;void* entity=nullptr;void* tool=nullptr;void* toolEntity=nullptr;void* bound=nullptr;int side=-1;
    if(!Read(static_cast<unsigned char*>(self)+8,id)||!id||!Read(reinterpret_cast<unsigned char*>(state.owner.parent)+0x88,bolt))return false;
    if(CrossbowWeaponAttachmentD72(state,self,attachment,id))return true;
    if(id==bolt){if(!state.placeBolt)return false;entity=EntityById6694(id);}
    else if(Read(reinterpret_cast<unsigned char*>(state.owner.main)+0x80,tool)&&tool&&
        RangedItemEntityXR34(tool,toolId,toolEntity)&&toolId==id){if(!state.placeTool)return false;entity=toolEntity;}
    else return false;
    if(tool&&state.owner.source.classId==15&&xcXR34::handDrivenD81(state.owner.source))
        return CrossbowHeavyToolAttachmentD72(state,self,attachment,id,tool,entity);
    if(!RangedBoundHandXR34(state.owner,id,entity,side,bound)||bound!=attachment)return false;
    std::unique_lock<std::recursive_mutex> lock(g_ranged_pose_mutex_xr34,std::try_to_lock);
    if(!lock.owns_lock())return true; // this exact binding already has a writer in flight
    const auto withdraw=[]() noexcept {(void)g_ranged_placed_xr34.publish({});return false;};
    RangedSocketXR34 socket{};
    if(!RangedReadHandSocketXR34(state.owner,id,entity,tool,id==bolt,socket)||
       socket.self!=self||socket.attachment!=attachment)return withdraw();
    (void)g_ranged_socket_xr34.publish(socket);
    const auto retain=[&]() noexcept {
        if(RangedRetainHandPlacementR95(socket))return true;
        return withdraw();
    };
    CommittedFrame6695 body{};
    // R96 single writer: the SAME tool/bolt binding whose hand grip was already
    // captured (g_ranged_grip_xr34, hand-driven reloads) stays at the tracked
    // wrist of the current committed body when that body has no reload target.
    // The previous code returned to the native attachment there, and the item
    // jumped between the VR hand and the paused native animation hand every
    // few frames (the reported flicker). Presentation only: no receipt.
    const auto hold=[&]() noexcept {
        const auto& captured=g_ranged_grip_xr34;
        if(!xcXR34::handDrivenD81(state.owner.source)||!captured.valid||!RangedSameGripBindingXR34(captured.socket,socket)||
           socket.side<0||socket.side>1||!body.valid[socket.side]||body.stamp.actor!=state.owner.source.character||
           body.stamp.epoch!=state.owner.source.body||body.doorTrackingEpoch66!=state.owner.source.tracking||
           !g_committed_pose_valid6689.load(std::memory_order_acquire))return withdraw();
        ModelWorld6692 holdWorld{};float holdNatural[8]{};float hx=0,hy=0,hz=0;
        if(!PhysAttachmentWorld164(attachment,body.stamp.actor,holdWorld)||!ReadAttachmentQs6689(attachment,holdNatural)||
           !Read(static_cast<unsigned char*>(self)+0xC,hx)||!Read(static_cast<unsigned char*>(self)+0x10,hy)||!Read(static_cast<unsigned char*>(self)+0x14,hz)||
           !std::isfinite(hx)||!std::isfinite(hy)||!std::isfinite(hz)||std::abs(hx*holdNatural[7])<1e-4||std::abs(hy*holdNatural[7])<1e-4||std::abs(hz*holdNatural[7])<1e-4)return withdraw();
        const auto wristPose=PhysPose6698(body.allowed[socket.side]);
        auto holdWanted=PhysCurrentWorld164(ph::compose(wristPose,ph::inverse(PhysPose6698(captured.grip))),holdWorld);
        if(socket.bolt){const auto h=FromT(socket.helper);const auto wristWorld=PhysCurrentWorld164({{},wristPose.p},holdWorld);
            holdWanted.p=wristWorld.p-ph::point({holdWanted.q,{}},{h.x*hx*holdNatural[7],h.y*hy*holdNatural[7],h.z*hz*holdNatural[7]});}
        if(!ph::finite(holdWanted)||!RangedOwnerXR34(state.owner))return withdraw();
        float holdQs[8]{float(holdWanted.q.x),float(holdWanted.q.y),float(holdWanted.q.z),float(holdWanted.q.w),
            float(holdWanted.p.x),float(holdWanted.p.y),float(holdWanted.p.z),holdNatural[7]};
        (void)g_ranged_placed_xr34.publish({}); // no receipt/credit while held
        bool holdChild=false;(void)PlaceBothConsumers6689(self,entity,holdQs,holdChild);
        return true;
    };
    if(!ReadCommittedPacket6695(body)){
        // No new destination is read while the paired body is in flight. A
        // stale/invalid completed body is not publication grace.
        if(ReadCompletedPublicationT23()&1)return retain();
        return withdraw();
    }
    const auto& target=body.reloadTargetXR34;
    const auto& source=body.reloadSourceXR34;
    if(!target.valid||!source.allowed){
        // A current ordinary body may lack an auxiliary socket/target only.
        // Keep an already achieved, unchanged item briefly (never renew the
        // receipt); R96: after that, hold it at the tracked wrist.
        if(body.stamp.actor==state.owner.source.character&&body.stamp.epoch==state.owner.source.body&&
           body.doorTrackingEpoch66==state.owner.source.tracking&&body.valid[0]&&body.valid[1]){
            if(RangedRetainHandPlacementR95(socket))return true;
            return hold();}
        return withdraw();
    }
    if(!body.publicationR24||(body.publicationR24&1)||target.publicationR95!=body.publicationR24||
       source.bodyPublicationR95!=body.publicationR24||source.auxiliaryUnavailableR95||
       !xcXR34::sameSource(source,state.owner.source)||!xcXR34::fresh(source,GetTickCount64())||source.sample!=body.stamp.sample||source.tick!=body.stamp.tick||
       !(xcXR34::handDrivenD81(state.owner.source)?RangedSameGripBindingXR34(target.socket,socket):RangedSameSocketXR34(target.socket,socket))||
       body.stamp.sample!=target.stamp.sample||body.stamp.tick!=target.stamp.tick||
       body.stamp.epoch!=target.stamp.epoch||body.stamp.actor!=target.stamp.actor)return hold();
    if(!ReloadPoseWriterCurrentXR34(state.owner,passengerPublication))return retain();
    // A positive new target is now selected. Real geometry/readback failures
    // withdraw its old receipt; only a publication handoff may retain it.
    ModelWorld6692 world{};float natural[8]{};
    if(!PhysAttachmentWorld164(attachment,body.stamp.actor,world)||!ReadAttachmentQs6689(attachment,natural))return withdraw();
    auto wanted=PhysCurrentWorld164(PhysPose6698(target.model),world);
    const auto wrist=PhysCurrentWorld164({{},{target.wrist.x,target.wrist.y,target.wrist.z}},world);
    float sx=0,sy=0,sz=0;
    if(!Read(static_cast<unsigned char*>(self)+0xC,sx)||!Read(static_cast<unsigned char*>(self)+0x10,sy)||
       !Read(static_cast<unsigned char*>(self)+0x14,sz)||!std::isfinite(sx)||!std::isfinite(sy)||!std::isfinite(sz)||
       std::abs(sx*natural[7])<1e-4||std::abs(sy*natural[7])<1e-4||std::abs(sz*natural[7])<1e-4)return withdraw();
    pm::Vec3 localGrip{};
    if(socket.bolt){const auto h=FromT(socket.helper);localGrip={h.x,h.y,h.z};
        wanted.p=wrist.p-ph::point({wanted.q,{}},{h.x*sx*natural[7],h.y*sy*natural[7],h.z*sz*natural[7]});}
    else{const auto h=FromT(target.grip);localGrip={h.x*world.scale/(sx*natural[7]),h.y*world.scale/(sy*natural[7]),h.z*world.scale/(sz*natural[7])};}
    const auto currentSocketR93=[&]() noexcept {
        RangedSocketXR34 current{};
        return body.publicationR24==ReadCompletedPublicationT23()&&
            g_committed_pose_valid6689.load(std::memory_order_acquire)&&
            RangedReadHandSocketXR34(state.owner,id,entity,tool,id==bolt,current)&&RangedSameSocketXR34(socket,current);
    };
    if(!ph::finite(wanted)||!RangedOwnerXR34(state.owner))return withdraw();
    if(!currentSocketR93()||!ReloadPoseWriterCurrentXR34(state.owner,passengerPublication)){
        if(body.publicationR24!=ReadCompletedPublicationT23())return retain();
        return withdraw();
    }
    (void)g_ranged_placed_xr34.publish({});
    float qs[8]{float(wanted.q.x),float(wanted.q.y),float(wanted.q.z),float(wanted.q.w),
        float(wanted.p.x),float(wanted.p.y),float(wanted.p.z),natural[7]};
    RangedPlacedXR34 receipt{};receipt.owner=state.owner;receipt.id=id;receipt.entity=entity;receipt.sample=body.stamp.sample;
    receipt.tick=body.stamp.tick;receipt.bolt=socket.bolt;receipt.toolItem=tool;receipt.socketR95=socket;
    receipt.bodyPublicationR95=body.publicationR24;
    if(tool&&!Read(static_cast<unsigned char*>(tool)+0x30,receipt.toolKey))return false;
    bool child=false,checked=false;Matrix34V6550 achieved{};
    // From the first attempted destination write this callback is consumed,
    // including failure. An original/native write must not follow a partial one.
    if(PlaceBothConsumers6689(self,entity,qs,child)&&PlacementReadback6689(self,entity,id,qs,checked)&&
       GetEntityWorldTmV6552(entity,id,achieved)&&currentSocketR93()&&
       (RangedOwnerXR34(state.owner)&&ReloadPoseWriterCurrentXR34(state.owner,passengerPublication))){
        receipt.localGrip=localGrip;receipt.worldGrip=RangedPointMatrixXR34(achieved,localGrip);receipt.valid=pm::finite(receipt.worldGrip);
        if(xcXR34::handDrivenD81(state.owner.source)){receipt.poseBasisD78=true;receipt.achievedD78=achieved;
            if(socket.bolt){receipt.helperFrameD78=true;receipt.helperD78=socket.helper;}}}
    if(receipt.valid&&socket.bolt&&side==0){
        // The native ammo attachment and entity have independent hide bits.
        // Reveal only the same right-hand bolt AFTER its real placement was
        // read back. Recheck on later writes because native refresh may hide it.
        ManualBow596 visible{};visible.player=reinterpret_cast<void*>(state.owner.source.player);
        visible.entity=entity;visible.hand=id;visible.side=side;visible.epoch=state.owner.source.body;visible.request=state.owner.source.request;
        receipt.valid=currentSocketR93()&&ReloadPoseWriterCurrentXR34(state.owner,passengerPublication)&&
            ManualArrowVisible42(visible)&&currentSocketR93()&&RangedOwnerXR34(state.owner)&&
            ReloadPoseWriterCurrentXR34(state.owner,passengerPublication);
    }
    (void)g_ranged_placed_xr34.publish(receipt);return true;
}
static void BuildCrossbowPosesXR34(CommittedFrame6695& packet,const QuatT28* original,std::size_t count,
    const QuatT28* reference=nullptr,int rightHand=-1,int leftHand=-1) noexcept {
    packet.reloadTargetXR34={};
    std::unique_lock<std::recursive_mutex> lock(g_ranged_pose_mutex_xr34,std::try_to_lock);if(!lock.owns_lock())return;
    RangedSocketXR34 socket{};RangedTargetXR34 result{};xcXR34::Status state{};xcXR34::Spatial binding{};
    // Discover the actual native-created item at this body transaction. This
    // removes the callback -> next-body -> callback one-frame handover delay.
    bool haveSocket=false;
    if(xcXR34::ReadStatus(state)&&state.clockOwned&&RangedOwnerXR34(state.owner)&&
       ReloadPoseWriterCurrentXR34(state.owner)&&RangedNativeBindingXR34(state.owner,binding)){
        if(state.placeBolt&&binding.handBoltBound)
            haveSocket=RangedReadHandSocketXR34(state.owner,binding.boltId,
                reinterpret_cast<void*>(binding.boltEntity),nullptr,true,socket);
        else if(state.placeTool&&binding.toolBound&&!binding.toolWeaponBoundD72)
            haveSocket=RangedReadHandSocketXR34(state.owner,binding.toolId,
                reinterpret_cast<void*>(binding.toolEntity),reinterpret_cast<void*>(binding.toolItem),false,socket);
    }
    if(!haveSocket||socket.side<0||socket.side>1||!packet.valid[socket.side]||
       packet.stamp.epoch!=socket.owner.source.body||packet.stamp.actor!=reinterpret_cast<std::uintptr_t>(socket.character)||
       socket.joint<0||std::size_t(socket.joint)>=count){
        packet.reloadTargetXR34=result;return;}
    (void)g_ranged_socket_xr34.publish(socket);
    const auto passengerPublication=xcXR34::passengerPublication.load(std::memory_order_acquire);
    void* currentTool=nullptr;std::uint64_t currentToolKey=0;std::uint32_t currentBolt=0;
    if(socket.bolt){if(!Read(reinterpret_cast<unsigned char*>(socket.owner.parent)+0x88,currentBolt)||currentBolt!=socket.id)return;}
    else if(!Read(reinterpret_cast<unsigned char*>(socket.owner.main)+0x80,currentTool)||currentTool!=socket.toolItem||
       !Read(static_cast<unsigned char*>(currentTool)+0x30,currentToolKey)||currentToolKey!=socket.toolKey)return;
    QuatT28 relative{};std::int16_t joint=-1;
    if(!Read(static_cast<unsigned char*>(socket.attachment)+0x9C,relative)||
       std::memcmp(&relative,&socket.relative,sizeof(relative))||!Read(static_cast<unsigned char*>(socket.attachment)+0x78,joint)||joint!=socket.joint)return;
    const auto nativeEntity=AbsoluteFromRelative(original[socket.joint],socket.relative);
    const auto grip=RelativeFromAbsolute(packet.native[socket.side],nativeEntity);
    if(!ValidPose6689(nativeEntity)||!ValidPose6689(grip)||Len(FromT(grip))>1.5f)return;
    result.socket=socket;result.stamp=packet.stamp;
    result.grip=RelativeFromAbsolute(nativeEntity,packet.native[socket.side]);
    if(xcXR34::handDrivenD81(socket.owner.source)){const auto& previous=g_ranged_grip_xr34;
        // The native reload animation may turn its slot while we hold its
        // clock. Capture this exact tool/bolt grip once; only the real tracked
        // wrist rotates the same bound item thereafter, as in manual bow hold.
        if(previous.valid&&RangedSameGripBindingXR34(previous.socket,socket))
            result.grip=previous.grip;
        else if(!socket.bolt){
            // R98 xbow: the spanner/lever item exists (hidden) from the action
            // start, but the game holds it only from SpawnLever on. The first
            // bound body came at clock 0.0000 (D77 00:33:54.450, tick 507774046)
            // before our clock reached SpawnLever 0.2057 (tick 507774078), so the
            // grip came from the idle hand slot: lever turned in the VR hand.
            // Calibrate like the cranequin writer, at the native spawn edge:
            // the client leaves Setup (clock driven to SpawnLever) for Span only
            // after that edge was reached (D77: phase 2 at clock 0.0000/0.2056,
            // phase 3 from tick 507774109). Before that: no target, no capture.
            // R103 crank: R98 waited for Span here, but the client only leaves
            // Setup for Span AFTER this placement exists (toolPoseReady ->
            // toolTakenD72 -> clock to SpawnLever -> visible -> Span). No target
            // in Setup = no receipt = 500 ms setup join ran out = GEOMETRY_UNAVAILABLE
            // NO_FIRST_HELD_ITEM_PLACEMENT and the game cocked alone with the
            // lever in its own hand (his R101 run, KCD2VR.log 09:47:04.221).
            // Setup holds the clock just before SpawnLever (spawn-1e-4): that
            // pose is the game's own hold at the spawn edge (same moment the
            // cranequin writer calibrates). Capture there, on the 2nd body in a
            // row at the edge (the 1st one may still be posed at the old clock
            // when the clock jumped there in one update). Before the edge
            // (clock 0, idle hand slot) still no target: the lever is hidden.
            bool edgeR103=state.phase==xcXR34::Phase::Span||state.phase==xcXR34::Phase::Tail;
            float clockR103=-1,spawnR103=-1;
            if(!edgeR103&&state.phase==xcXR34::Phase::Setup){
                xcXR34::Passenger p{};
                if(ReadReloadPassengerXR34(p,packet.stamp.actor,packet.stamp.epoch,GetTickCount64())&&
                   xcXR34::sameOwner(p.owner,socket.owner)&&p.clip.cocking&&p.clock.qualified&&
                   std::isfinite(p.clock.normalized)&&std::isfinite(p.clip.spawn)){
                    clockR103=p.clock.normalized;spawnR103=std::max(0.f,p.clip.spawn);
                    if(clockR103>=spawnR103-2e-4f){
                        static RangedSocketXR34 seenR103{};static std::uint64_t seenSampleR103=0;
                        edgeR103=seenSampleR103&&seenSampleR103<packet.stamp.sample&&RangedSameGripBindingXR34(seenR103,socket);
                        if(!edgeR103){seenR103=socket;seenSampleR103=packet.stamp.sample;}
                    }
                }
            }
            if(!edgeR103){packet.reloadTargetXR34={};return;}
            // R105: the medium spanner is bound to the native hand slot. Its
            // animated slot can turn/translate relative to that wrist even at
            // the spawn edge; freezing that pose freezes the clip's offset as
            // the held grip. Use the same validated anatomy reference basis as
            // manual arrow hold, with THIS item's native relative descriptor.
            // No guessed axis turn, mirrored hand, asset offset or clock write.
            // A late reference retries on the next eligible body transaction;
            // an existing exact-item calibration above needs no new reference.
            const auto animatedGripR105=result.grip;
            if(socket.owner.source.classId==14){
                const int hand=socket.side?leftHand:rightHand;
                if(!reference||hand<0||std::size_t(hand)>=count||
                   !ValidPose6689(reference[hand])||!ValidPose6689(reference[socket.joint]))return;
                const auto referenceEntity=AbsoluteFromRelative(reference[socket.joint],socket.relative);
                const auto referenceGrip=RelativeFromAbsolute(referenceEntity,reference[hand]);
                if(!ValidPose6689(referenceEntity)||!ValidPose6689(referenceGrip)||Len(FromT(referenceGrip))>1.5f)return;
                result.grip=referenceGrip;
            }
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,GetTickCount64())){
                char line[640]{};std::snprintf(line,sizeof(line),
                    "R105 XBOW_TOOL_GRIP class=%d phase=%u clock=%.4f spawn=%.4f handInToolQ=%.5f,%.5f,%.5f,%.5f handInToolT=%.4f,%.4f,%.4f joint=%d basis=%s animatedQ=%.5f,%.5f,%.5f,%.5f animatedT=%.4f,%.4f,%.4f handJoint=%d serial=%llu entity=%u",
                    socket.owner.source.classId,unsigned(state.phase),clockR103,spawnR103,
                    result.grip.q[0],result.grip.q[1],result.grip.q[2],result.grip.q[3],result.grip.t[0],result.grip.t[1],result.grip.t[2],socket.joint,
                    socket.owner.source.classId==14?"NATIVE_REFERENCE_HAND_SLOT":"NATIVE_ANIMATED_HAND_SLOT",
                    animatedGripR105.q[0],animatedGripR105.q[1],animatedGripR105.q[2],animatedGripR105.q[3],
                    animatedGripR105.t[0],animatedGripR105.t[1],animatedGripR105.t[2],socket.side?leftHand:rightHand,
                    (unsigned long long)socket.owner.serial,socket.id);
                diagnostic_trace::WriteFor(diagnostics::capture168::Domain::Reload,line);}
        }
        const auto model=ph::compose(PhysPose6698(packet.allowed[socket.side]),ph::inverse(PhysPose6698(result.grip)));
        result.model={{float(model.q.x),float(model.q.y),float(model.q.z),float(model.q.w)},
            {float(model.p.x),float(model.p.y),float(model.p.z)}};
    }else result.model=AbsoluteFromRelative(packet.allowed[socket.side],grip);
    result.wrist=FromT(packet.allowed[socket.side]);result.valid=ValidPose6689(result.model)&&ValidPose6689(result.grip);
    if(!ReloadPoseWriterCurrentXR34(socket.owner,passengerPublication))result={};
    if(result.valid&&xcXR34::handDrivenD81(socket.owner.source))
        g_ranged_grip_xr34={socket,result.grip,true};
    // R95: target, source, raw goals and solved wrists become visible only
    // when their containing body transaction commits. No auxiliary mailbox
    // can expose target N+1 while body N is still the completed publication.
    packet.reloadTargetXR34=result;
}
