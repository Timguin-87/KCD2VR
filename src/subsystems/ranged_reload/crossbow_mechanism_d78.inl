// Read-only native weapon-pose witness. Called at the existing skinning input
// boundary, BEFORE hierarchy/model writes. Entity PRS is never a pose source.
// The only live pose API is the same exact fingerprint-validated CPoseData
// buffer1 getter used by the bow and smithing adapters.
static bool RangedHelperBasisD78(const Matrix34V6550& entity,const QuatT28& helper,xcXR34::Vec (&axes)[3]) noexcept {
    if(!ValidPose6689(helper))return false;
    xcXR34::Vec basis[3]{};
    for(int i=0;i<3;++i){basis[i]={entity.m[i],entity.m[4+i],entity.m[8+i]};const double n=xcXR34::length(basis[i]);
        if(!std::isfinite(n)||n<1e-4)return false;basis[i]=xcXR34::scale(basis[i],1./n);}
    const float inverse[4]{-helper.q[0],-helper.q[1],-helper.q[2],helper.q[3]};
    for(int i=0;i<3;++i){xcXR34::Vec unit{};if(i==0)unit.x=1;else if(i==1)unit.y=1;else unit.z=1;
        const auto v=xcXR34::rotateInverse(inverse,unit);
        axes[i]={basis[0].x*v.x+basis[1].x*v.y+basis[2].x*v.z,
            basis[0].y*v.x+basis[1].y*v.y+basis[2].y*v.z,basis[0].z*v.x+basis[1].z*v.y+basis[2].z*v.z};}
    bool ignored=false;return xcXR34::boltFrameAlignedD78(axes,axes,ignored);
}
static void RangedBoltAlignmentD78(const xcXR34::Owner& o,const ManualItems596& items,void* weaponCharacter,
    int arrowJoint,const QuatT28& rail,const Matrix34V6550& weapon,const RangedPlacedXR34& placed,xcXR34::Spatial& result) noexcept {
    // A socket retained from an actual native nock proves the helper convention.
    // Before that evidence exists, keep the former distance-only placement:
    // an unobserved mesh axis must not be promoted to a mandatory aiming rule.
    if(!PhysSocketContract101(WhBase())||!placed.helperFrameD78||!placed.bolt||!items.visual||!weaponCharacter||arrowJoint<0)return;
    void* attachment=ManualFindNockAttachment19c4r4(items.visual);void* manager=nullptr;void* character=nullptr;
    std::int16_t joint=-1;unsigned char deferred=1;QuatT28 relative{},after{};
    if(!PhysVft6698(attachment,0x3A657C0)||!Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||character!=weaponCharacter||
       !Read(static_cast<unsigned char*>(attachment)+0x78,joint)||joint!=arrowJoint||
       !Read(static_cast<unsigned char*>(attachment)+0x7A,deferred)||deferred||
       !Read(static_cast<unsigned char*>(attachment)+0x9C,relative)||!ValidPose6689(relative))return;
    const auto identity=AbsoluteFromRelative(relative,placed.helperD78);
    if(!ValidPose6689(identity)||Len(FromT(identity))>.001||std::abs(identity.q[3])<.999999f)return;
    xcXR34::Vec actual[3]{},target[3]{};bool aligned=false;
    if(!RangedHelperBasisD78(placed.achievedD78,placed.helperD78,actual)||!RangedHelperBasisD78(weapon,rail,target)||
       !xcXR34::boltFrameAlignedD78(actual,target,aligned)||
       !Read(static_cast<unsigned char*>(attachment)+0x9C,after)||std::memcmp(&relative,&after,sizeof(relative))||
       !RangedPlacementStillCurrentXR34(placed)||!RangedOwnerXR34(o))return;
    result.boltAlignmentKnownD78=true;result.boltAlignedD78=aligned;
    result.handBoltPlaced=result.handBoltPlaced&&aligned;
}
// Same default-skeleton joint names, exact CHR positions, entity GetCharacter
// target and Henry weapon binding already qualified by XR34; no new ABI.
static bool RangedWeaponPoseIdentityD78(const xcXR34::Owner& o,void*& character,void*& skeleton,int& stringJoint) noexcept {
    character=skeleton=nullptr;stringJoint=-1;void* entity=nullptr;void* attachment=nullptr;unsigned id=0;
    pm::Vec3 rail{},rest{};
    return ReloadNativeOwnerXR34(o,true)&&RangedItemEntityXR34(reinterpret_cast<void*>(o.source.weaponItem),id,entity)&&
        RangedBoundWeaponIdentityD72(o,id,entity,attachment)&&
        RangedDefaultsXR34(entity,o.source.classId,rail,rest,nullptr,nullptr,&character,&stringJoint)&&
        Read(static_cast<unsigned char*>(character)+0xA70,skeleton)&&PhysVft6698(skeleton,0x406B930);
}
static bool RangedReadStringD78(const xcXR34::Owner& o,const xcXR34::Source& source,RangedMechanismD78& out) noexcept {
    out={};RangedMechanismD78 r{};const auto now=std::uint64_t(GetTickCount64());
    if(!xcXR34::handDrivenD81(source)||!source.intentKnownD78||!xcXR34::fresh(source,now)||
       !g_ranged_mechanism_d78.read(r)||!r.valid||!r.stringValid||!xcXR34::sameOwner(r.owner,o)||
       !r.observed||now<r.observed||now-r.observed>80||!r.sample||r.sample>source.sample||
       !r.tick||now<r.tick||now-r.tick>80||
       source.tick<r.tick||source.tick-r.tick>80||!ValidPose6689(r.stringModel))return false;
    void* character=nullptr;void* skeleton=nullptr;int joint=-1;
    if(!RangedWeaponPoseIdentityD78(o,character,skeleton,joint)||character!=r.character||skeleton!=r.skeleton||
       joint!=r.stringJoint||r.pose!=static_cast<unsigned char*>(character)+0x8E0)return false;
    out=r;return true;
}
static bool RangedMechanismDescriptorD78(const xcXR34::Owner& o,const xcXR34::Spatial& b,
    void*& character,void*& skeleton,std::int16_t& joint,QuatT28& relative) noexcept {
    character=skeleton=nullptr;joint=-1;relative={};
    void* attachment=nullptr;void* manager=nullptr;int side=-1;unsigned char deferred=1;
    if(!PhysSocketContract101(WhBase())||!b.toolBound||!b.toolWeaponBoundD72||!b.toolId||!b.toolEntity||!b.toolAttachment||
       !RangedBoundToolIdentityD72(o,b.toolId,reinterpret_cast<void*>(b.toolEntity),side,attachment)||
       reinterpret_cast<std::uintptr_t>(attachment)!=b.toolAttachment||side!=1-o.source.primary||
       !Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||!PhysVft6698(character,0x40602F8)||
       !Read(static_cast<unsigned char*>(character)+0xA70,skeleton)||!PhysVft6698(skeleton,0x406B930)||
       !Read(static_cast<unsigned char*>(attachment)+0x78,joint)||joint<0||
       !Read(static_cast<unsigned char*>(attachment)+0x7A,deferred)||deferred||
       !Read(static_cast<unsigned char*>(attachment)+0x9C,relative)||!ValidPose6689(relative))return false;
    return true;
}
static bool RangedReadMechanismD78(const xcXR34::Owner& o,const xcXR34::Source& source,
    const xcXR34::Spatial& binding,RangedMechanismD78& out) noexcept {
    out={};RangedMechanismD78 r{};const auto now=std::uint64_t(GetTickCount64());
    if(!xcXR34::handDrivenD81(source)||!source.intentKnownD78||source.classId!=15||!xcXR34::fresh(source,now)||
       !g_ranged_mechanism_d78.read(r)||!r.valid||!xcXR34::sameOwner(r.owner,o)||
       !r.observed||now<r.observed||now-r.observed>80||!r.sample||r.sample>source.sample||
       !r.tick||now<r.tick||now-r.tick>80||
       source.tick<r.tick||source.tick-r.tick>80||r.toolItem!=binding.toolItem||r.toolKey!=binding.toolKey||
       r.toolId!=binding.toolId||r.toolEntity!=binding.toolEntity||r.attachment!=binding.toolAttachment)return false;
    void* character=nullptr;void* skeleton=nullptr;std::int16_t joint=-1;QuatT28 relative{};
    if(!RangedMechanismDescriptorD78(o,binding,character,skeleton,joint,relative)||
       character!=r.character||skeleton!=r.skeleton||joint!=r.joint||
       r.pose!=static_cast<unsigned char*>(character)+0x8E0||
       std::memcmp(&relative,&r.relative,sizeof(relative))||!ValidPose6689(r.toolModel))return false;
    out=r;return true;
}
static void RangedMechanismSpatialD78(const xcXR34::Owner& o,const xcXR34::Source& source,
    xcXR34::Spatial& result,const RangedPlacedXR34* placed) noexcept {
    result.toolMechanismD72=false;result.toolMountedPoseD78=false;
    result.mechanismClockD78={};result.mechanismTickD78=0;
    result.stringMechanismD78=false;result.stringClockD78={};result.stringTickD78=0;
    if(!xcXR34::handDrivenD81(source)||!source.intentKnownD78)return;
    std::unique_lock<std::recursive_mutex> lock(g_ranged_pose_mutex_xr34,std::try_to_lock);if(!lock.owns_lock())return;
    RangedMechanismD78 r{};
    if(source.classId!=15){
        if(RangedReadStringD78(o,source,r)){
            result.stringCurrentD78={r.stringModel.t[0],r.stringModel.t[1],r.stringModel.t[2]};
            result.stringClockD78=r.clock;result.stringTickD78=r.observed;result.stringMechanismD78=true;}
        return;
    }
    const auto& capture=g_ranged_heavy_grip_d72;void* object=nullptr;
    if(!capture.ready||!xcXR34::sameOwner(capture.owner,o)||capture.id!=result.toolId||
       capture.key!=result.toolKey||capture.attachment!=result.toolAttachment||
       capture.toolItem!=reinterpret_cast<void*>(result.toolItem)||capture.entity!=reinterpret_cast<void*>(result.toolEntity)||
       !RangedReadMechanismD78(o,source,result,r)||capture.joint!=r.joint||
       capture.character!=r.character||capture.skeleton!=r.skeleton||
       !Read(reinterpret_cast<unsigned char*>(result.toolAttachment)+0x20,object)||object!=capture.self)return;
    const auto wrist=ph::compose(PhysPose6698(r.toolModel),capture.grip);
    if(!ph::finite(wrist))return;
    result.toolWristD78={{float(wrist.q.x),float(wrist.q.y),float(wrist.q.z),float(wrist.q.w)},
        {float(wrist.p.x),float(wrist.p.y),float(wrist.p.z)}};
    result.toolMountD72=result.stringRest;result.toolHandleD72={wrist.p.x,wrist.p.y,wrist.p.z};
    result.mechanismClockD78=r.clock;result.mechanismTickD78=r.observed;result.toolMechanismD72=true;
    // A free-hand receipt cannot acknowledge the change of writer at mount.
    // Nor can yesterday's mounted entity acknowledge a new native lever pose.
    result.toolMountedPoseD78=placed&&placed->valid&&placed->mountedD78&&
        xcXR34::sameMechanismClockD78(placed->mechanismClockD78,r.clock)&&
        !std::memcmp(&placed->mechanismPoseD78,&r.toolModel,sizeof(r.toolModel));
}
static void CrossbowMechanismSkinningD78(void* pose,void* owner,void* charPlus8) noexcept {
    const auto candidate=g_ranged_mechanism_character_d78.load(std::memory_order_acquire);
    if(!candidate||reinterpret_cast<std::uintptr_t>(charPlus8)!=candidate+8||
       !xcXR34::activeNativeLease.load(std::memory_order_acquire))return;
    xcXR34::Status state{};
    if(!xcXR34::ReadStatus(state)||!state.clockOwned||!state.placeTool||
       !xcXR34::handDrivenD81(state.owner.source)||!state.owner.source.intentKnownD78)return;
    std::unique_lock<std::recursive_mutex> lock(g_ranged_pose_mutex_xr34,std::try_to_lock);if(!lock.owns_lock())return;
    // This is a weapon-pose observation, not permission to write Henry's body.
    // A transient passenger publication miss does not invalidate an already
    // observed paused pose. Its original source/observation age still expires
    // at 80 ms; every consumer also matches the exact native clock and owner.
    xcXR34::Source source{};xcXR34::Passenger passenger{};xcXR34::Spatial binding{};
    const auto now=std::uint64_t(GetTickCount64());
    const auto note=[&](const char* why,bool withdraw=true) noexcept {
        if(withdraw)(void)g_ranged_mechanism_d78.publish({});
        kcd2vr::reload_diagnostics_d48::recordMechanismD86(state,why,now,source.publication,passenger.frame.publication,passenger.publication);
    };
    if(!xcXR34::ReadMechanismWitnessD86(passenger,now)){note("NO_NATIVE_POSE_WITNESS",false);return;}
    // The witness already joins the fresh current source, this original frame,
    // the active reload and its evaluated native clock. Requiring a second
    // completed-Henry publication here creates a cycle: the controller waits
    // for this string pose while Henry's next body waits for the controller.
    source=passenger.frame;
    if(!xcXR34::fresh(source,now)||!xcXR34::sameSource(source,state.owner.source)){note("WITNESS_SOURCE");return;}
    // R98: a string (class 14) witness is not withdrawn when only the clock
    // moved on: every consumer still checks its owner, its 80 ms age and its
    // clock (exact for stroke credit, one-frame lag only for the hand pin).
    if(!ReloadMechanismCurrentD86(passenger,now)){note("NATIVE_CLOCK_OR_OWNER",source.classId==15);return;}
    if(!xcXR34::sameOwner(passenger.owner,state.owner)){note("OWNER_CHANGED");return;}
    if(!RangedNativeBindingXR34(state.owner,binding)){note("NATIVE_BINDING");return;}
    void* character=nullptr;void* skeleton=nullptr;void* poseOwner=nullptr;void* anim=nullptr;int stringJoint=-1;
    if(!RangedWeaponPoseIdentityD78(state.owner,character,skeleton,stringJoint)||
       reinterpret_cast<std::uintptr_t>(character)!=candidate||owner!=skeleton||
       pose!=static_cast<unsigned char*>(character)+0x8E0||
       !ReadValidatedCharInstanceFromSkinning(charPlus8,character,anim)||reinterpret_cast<std::uintptr_t>(character)!=candidate||
       !ReadValidatedDefaultSkeletonFromPose(pose,poseOwner)||poseOwner!=skeleton){note("WEAPON_POSE_IDENTITY");return;}
    std::int16_t joint=-1;QuatT28 relative{};const bool heavy=source.classId==15;
    if(heavy){void* toolCharacter=nullptr;void* toolSkeleton=nullptr;
        if(!RangedMechanismDescriptorD78(state.owner,binding,toolCharacter,toolSkeleton,joint,relative)||
           toolCharacter!=character||toolSkeleton!=skeleton){note("TOOL_BINDING");return;}}
    std::uint32_t count=0;PoseGetFn reference=nullptr,current=nullptr;QuatT28 string{},native{},again{};
    if(!ReadJointCount(skeleton,count)||stringJoint<0||std::uint32_t(stringJoint)>=count||
       (heavy&&std::uint32_t(joint)>=count)||!ValidateRawPoseApi(pose,reference,current)||!current||
       !ReadQuat(current,pose,stringJoint,string)||!ValidPose6689(string)||
       !ReadQuat(current,pose,stringJoint,again)||std::memcmp(&string,&again,sizeof(string))){note("STRING_POSE");return;}
    QuatT28 tool{};
    if(heavy){
        if(!ReadQuat(current,pose,joint,native)||!ValidPose6689(native)||
           !ReadQuat(current,pose,joint,again)||std::memcmp(&native,&again,sizeof(native))){note("TOOL_POSE");return;}
        tool=AbsoluteFromRelative(native,relative);if(!ValidPose6689(tool)){note("TOOL_POSE");return;}
        void* c2=nullptr;void* s2=nullptr;std::int16_t j2=-1;QuatT28 r2{};
        if(!RangedMechanismDescriptorD78(state.owner,binding,c2,s2,j2,r2)||c2!=character||s2!=skeleton||j2!=joint||
           std::memcmp(&relative,&r2,sizeof(relative))){note("TOOL_BINDING_CHANGED");return;}
    }
    void* c2=nullptr;void* s2=nullptr;int string2=-1;
    if(!RangedWeaponPoseIdentityD78(state.owner,c2,s2,string2)||c2!=character||s2!=skeleton||string2!=stringJoint||
       !ReloadMechanismCurrentD86(passenger,now)){note("FINAL_CLOCK_OR_OWNER");return;}
    RangedMechanismD78 next{};next.owner=state.owner;next.clock=passenger.clock;
    next.sample=source.sample;next.tick=source.tick;next.observed=now;
    next.character=character;next.skeleton=skeleton;next.pose=pose;next.joint=joint;next.relative=relative;next.toolModel=tool;
    next.stringJoint=stringJoint;next.stringModel=string;next.stringValid=true;
    next.toolItem=binding.toolItem;next.toolKey=binding.toolKey;next.toolId=binding.toolId;
    next.toolEntity=binding.toolEntity;next.attachment=binding.toolAttachment;next.valid=true;
    const bool published=g_ranged_mechanism_d78.publish(next);
    note(published?"PUBLISHED":"PUBLICATION_BUSY",false);
}
