// Generated verbatim from production; do not edit.
static bool NativeHeldBodyT4(void* entity,void*& proxy,void*& body,int& id,bool suspended) noexcept {
    if(!PhysVft6698(entity,0x4072C78))return false;
    std::uintptr_t fn=0;
    if(!Read(reinterpret_cast<void*>(WhBase()+0x4072C78+0x250),fn)||fn!=WhBase()+0x5D70C4)return false;
    proxy=NativeHeldProxyT4(entity);unsigned flags=0;std::uint32_t packed=0;void* foreign=nullptr;short foreignType=0;
    if(!PhysVft6698(proxy,0x3AC3478)||!Read(static_cast<unsigned char*>(proxy)+0x28,flags)||
       (suspended&&!(flags&0x40))||!Read(static_cast<unsigned char*>(proxy)+0x38,body)||!body||
       !Read(static_cast<unsigned char*>(body)+0x44,packed)||
       !Read(static_cast<unsigned char*>(body)+0x20,foreign)||foreign!=entity||
       !Read(static_cast<unsigned char*>(body)+0x28,foreignType)||foreignType!=2)return false;
    // 52950C is signed 23-bit, not an unsigned mask.
    id=static_cast<std::int32_t>(packed<<9)>>9;
    return id>=0;
}
static const char* NativeHeldProfileReason62(const NativeHeldProfile62& p) noexcept {
    if(!(p.readMask&1))return "REQUEST_UNREADABLE";
    if(!(p.readMask&2))return "APPLIED_UNREADABLE";
    if(!(p.readMask&4))return "DIRTY_UNREADABLE";
    if(p.requested!=p.applied)return "TRANSITION_PENDING";
    // C63: native Attach can leave a clean, disabled profile0. This only
    // admits further observation: the exact retained suspended proxy/body and
    // registry/simClass/saved-state proof below and in the physics phase still
    // gate every owned collider. A profile0 without a body remains refused.
    if(p.applied!=0&&p.applied!=2&&p.applied!=4)return "STABLE_PROFILE_UNSUPPORTED";
    return p.dirty?"PROFILE_DIRTY":nullptr;
}
static void NativeHeldCaptureT4(const PhysEquipment6698& e,std::uint64_t now) noexcept {
    if(!heldT4::Enabled()){g_native_held_profile_receipt62={};g_native_held_reason_t4.store(HeldPhysicsSourceT4::Disabled,std::memory_order_release);NativeHeldInvalidateT4();return;}
    (void)heldT4::SetSourcePredicate(NativeHeldSourceCurrentT4);
    NativeHeldSourceT4 s{};auto& in=s.input;
    const auto refuse=[](HeldPhysicsSourceT4 reason){g_native_held_reason_t4.store(reason,std::memory_order_release);NativeHeldInvalidateT4();};
    if(g_native_held_equipment_pending_h36.load(std::memory_order_acquire))return refuse(HeldPhysicsSourceT4::Context);
    if(!retail_build112::Ready(WhBase()))return refuse(HeldPhysicsSourceT4::NoEquipment);
    if(!e.valid||!e.handKnown[0]||!e.handKnown[1])return refuse(HeldPhysicsSourceT4::NoEquipment);
    if(e.handKey[1])return refuse(HeldPhysicsSourceT4::LeftOccupied);
    if(e.heldIndex[0]<0||e.heldIndex[0]>=e.count)return refuse(HeldPhysicsSourceT4::NoHeldSword);
    const auto& item=e.items[e.heldIndex[0]];const auto& p=item.profile;
    if(!item.held||!p.key||e.handKey[0]!=p.key||p.side!=0||!p.generation||
       !(p.classId==1||p.classId==2||p.classId==4||p.classId==16)||!item.boundsValid||
       !PhysVft6698(e.player,0x3AA4B98)||!PhysVft6698(item.item,0x3A2DA88)||
       !PhysVft6698(item.entity,0x4072C78)||EntityById6694(p.entity)!=item.entity)return refuse(HeldPhysicsSourceT4::NoHeldSword);
    void* link=nullptr;void* linkedEntity=nullptr;void* backing=nullptr;std::uint32_t id=0;
    if(!Read(static_cast<unsigned char*>(item.item)+0x88,link)||link!=item.worldLink54||!PhysVft6698(link,0x3EC85E0)||
       !Read(static_cast<unsigned char*>(link)+0x38,linkedEntity)||linkedEntity!=item.entity||
       !Read(static_cast<unsigned char*>(link)+0x58,backing)||backing!=item.item||
       !Read(static_cast<unsigned char*>(link)+0x30,id)||id!=p.entity)return refuse(HeldPhysicsSourceT4::ItemLink);
    NativeHeldProfile62 profile{};
    if(Read(static_cast<unsigned char*>(link)+0x78,profile.requested))profile.readMask|=1;
    if(Read(static_cast<unsigned char*>(link)+0x7C,profile.applied))profile.readMask|=2;
    if(Read(static_cast<unsigned char*>(link)+0x75,profile.dirty))profile.readMask|=4;
    if(const auto* why=NativeHeldProfileReason62(profile)){
        // No profile/native write or expanded acceptance. First exact failure
        // and changed values only, <=1/s and32/session, F9 selected only.
        if(g_native_held_profile_receipt62.take(p.key,profile,now)){char m[320]{};
            std::snprintf(m,sizeof(m),"T4 F9 profileRefusal=%s key=%016llX entity=%u requested=%u applied=%u dirty=%u readMask=%u nativeProfile=UNCHANGED",
                why,static_cast<unsigned long long>(p.key),p.entity,profile.requested,profile.applied,unsigned(profile.dirty),unsigned(profile.readMask));
            diagnostic_trace::Write(m);}
        return refuse(HeldPhysicsSourceT4::NativeProfile);
    }
    s.requested=profile.requested;s.applied=profile.applied;
    void* model=nullptr;void* modelOwner=nullptr;void* attachment=nullptr;void* object=nullptr;void* manager=nullptr;void* character=nullptr;
    float scale[3]{};NativeHeldSlotF2 slot{};
    auto readSlot=[](std::uintptr_t at,auto& value) noexcept {return Read(reinterpret_cast<void*>(at),value);};
    if(!Read(static_cast<unsigned char*>(e.player)+0x1B0,model)||!model||
       !Read(static_cast<unsigned char*>(model)+0x20,modelOwner)||modelOwner!=e.player||
       !ReadNativeHeldSlotF2(WhBase(),reinterpret_cast<std::uintptr_t>(model),p.entity,readSlot,slot))return refuse(HeldPhysicsSourceT4::Attachment);
    attachment=NativeHeldAttachmentFindT4(model,reinterpret_cast<const char*>(slot.name));
    if(!PhysVft6698(attachment,0x3A657C0)||!Read(static_cast<unsigned char*>(attachment)+0x20,object)||!PhysVft6698(object,0x3A68F68)||
       !Read(static_cast<unsigned char*>(object)+8,id)||id!=p.entity||
       !Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||!PhysVft6698(character,0x40602F8))return refuse(HeldPhysicsSourceT4::Attachment);
    for(int j=0;j<3;++j)if(!Read(static_cast<unsigned char*>(object)+0xC+4*j,scale[j])||!std::isfinite(scale[j])||scale[j]<.1f||scale[j]>10)return refuse(HeldPhysicsSourceT4::Attachment);
    if(std::abs(scale[0]-scale[1])>1e-5||std::abs(scale[0]-scale[2])>1e-5)return refuse(HeldPhysicsSourceT4::Attachment);
    // GetAttachment traverses native owner/character interfaces. Its callbacks
    // cannot leave us publishing a former model slot as a current binding.
    void* modelAfter=nullptr;void* modelOwnerAfter=nullptr;NativeHeldSlotF2 slotAfter{};
    if(!Read(static_cast<unsigned char*>(e.player)+0x1B0,modelAfter)||modelAfter!=model||
       !Read(static_cast<unsigned char*>(model)+0x20,modelOwnerAfter)||modelOwnerAfter!=e.player||
       !ReadNativeHeldSlotF2(WhBase(),reinterpret_cast<std::uintptr_t>(model),p.entity,readSlot,slotAfter)||
       !(slot==slotAfter))return refuse(HeldPhysicsSourceT4::Attachment);
    s.nativeSlot=slot.index;
    void* proxy=nullptr;void* body=nullptr;int bodyId=-1;
    if(!NativeHeldBodyT4(item.entity,proxy,body,bodyId,true))return refuse(HeldPhysicsSourceT4::NativeProxySuspension);
    void* playerEntity=nullptr;void* playerProxy=nullptr;void* playerBody=nullptr;int playerId=-1;
    if(!Read(static_cast<unsigned char*>(e.player)+0x38,playerEntity)||
       !NativeHeldBodyT4(playerEntity,playerProxy,playerBody,playerId,false))return refuse(HeldPhysicsSourceT4::HenryProxy);
    Matrix34V6550 tm{};if(!GetEntityWorldTmV6552(item.entity,p.entity,tm))return refuse(HeldPhysicsSourceT4::NativeWorldPose);
    kcd2vr::grasp13::Pose world{};if(!kcd2vr::grasp13::fromMatrix(tm.m,world))return refuse(HeldPhysicsSourceT4::NativeWorldPose);
    s.attachmentScale=world.scale/scale[0];
    for(int j=0;j<3;++j)s.authoredObjectScaleH5[j]=scale[j]; // exact authored fields, not matrix norms
    if(!std::isfinite(s.attachmentScale)||s.attachmentScale<.1f||s.attachmentScale>10)return refuse(HeldPhysicsSourceT4::NativeWorldPose);
    in.owner={p.key,p.generation,e.request,e.epoch,reinterpret_cast<std::uintptr_t>(e.player),reinterpret_cast<std::uintptr_t>(e.manager),
        reinterpret_cast<std::uintptr_t>(character),reinterpret_cast<std::uintptr_t>(item.entity),reinterpret_cast<std::uintptr_t>(attachment),reinterpret_cast<std::uintptr_t>(object),p.entity,0};
    in.weaponWorld={{world.q[0],world.q[1],world.q[2],world.q[3]},{world.p[0],world.p[1],world.p[2]}};
    in.targetWorld=in.weaponWorld;in.scale=world.scale;
    in.boundsMin={item.bounds[0],item.bounds[1],item.bounds[2]};in.boundsMax={item.bounds[3],item.bounds[4],item.bounds[5]};
    // Inventory kilograms are a bounded DESIGN mass estimate, not a native
    // GetStatus mass receipt. No fabricated claim about the suspended static.
    in.mass=std::clamp(p.weight,.2,12.);in.classId=p.classId;
    in.nativeBody=reinterpret_cast<std::uintptr_t>(body);in.nativeProxy=reinterpret_cast<std::uintptr_t>(proxy);in.nativeBodyId=bodyId;
    in.playerBody=reinterpret_cast<std::uintptr_t>(playerBody);in.playerEntityPointer=reinterpret_cast<std::uintptr_t>(playerEntity);in.playerBodyId=playerId;
    kcd2vr::deepseek_h29::StampWeaponNpcContactControlH5(in);
    in.nativeColliderExcluded=true;in.emptyLeft=true;s.item=reinterpret_cast<std::uintptr_t>(item.item);s.link=reinterpret_cast<std::uintptr_t>(link);s.tick=now;s.leftNativeMembershipH36=e.leftNativeMembershipH36;s.valid=true;
    NativeHeldSourceT4 before{};const bool sameBindingH5=g_native_held_valid_t4.load(std::memory_order_acquire)&&g_native_held_source_t4.read(before)&&before.valid&&
        before.input.owner==in.owner&&before.input.nativeBody==in.nativeBody&&before.input.nativeProxy==in.nativeProxy&&before.input.nativeBodyId==in.nativeBodyId&&
        before.input.playerBody==in.playerBody&&before.input.playerBodyId==in.playerBodyId&&before.item==s.item&&before.link==s.link&&before.applied==s.applied&&
        before.authoredObjectScaleH5[0]==s.authoredObjectScaleH5[0]&&before.authoredObjectScaleH5[1]==s.authoredObjectScaleH5[1]&&
        before.authoredObjectScaleH5[2]==s.authoredObjectScaleH5[2]&&before.nativeSlot==s.nativeSlot&&before.input.mass==in.mass&&
        pm::length(before.input.boundsMin-in.boundsMin)<1e-7&&pm::length(before.input.boundsMax-in.boundsMax)<1e-7;
    // Float Matrix34 rotation roundoff must not create a new collider owner.
    // Anchor to the retained canonical geometry, never the previous noisy norm.
    // Authored object scale is compared EXACTLY above; real source changes still
    // revoke. No pose/source clock is reused and the existing80ms limit remains.
    const auto roundoffH5=[](double anchor,double measured) noexcept {
        return std::isfinite(anchor)&&std::isfinite(measured)&&anchor>0&&measured>0&&
            std::abs(anchor-measured)<=anchor*(2.0/8388608.0); // two float epsilons
    };
    if(sameBindingH5&&before.tick&&now>=before.tick&&now-before.tick<=80&&
       roundoffH5(before.input.scale,in.scale)&&roundoffH5(before.attachmentScale,s.attachmentScale)){
        in.scale=before.input.scale;s.attachmentScale=before.attachmentScale;
    }
    const bool same=sameBindingH5&&before.input.scale==in.scale&&before.attachmentScale==s.attachmentScale;
    if(!same)g_native_held_lease_t4.fetch_add(1,std::memory_order_acq_rel);
    in.colliderLease=g_native_held_lease_t4.load(std::memory_order_acquire);
    in.trackingEpoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
    in.cancelGeneration=g_pose_policy_generation57.load(std::memory_order_acquire);
    if(!g_native_held_source_t4.publish(s))return refuse(HeldPhysicsSourceT4::NoEquipment);
    g_native_held_valid_t4.store(true,std::memory_order_release);
    // Edge-only successful binding, bounded per session; no loading-time poll.
    static std::uint64_t bindingKeyF2=0;static unsigned bindingSlotF2=6,linesF2=0;
    if(linesF2<16&&(bindingKeyF2!=p.key||bindingSlotF2!=s.nativeSlot)){
        bindingKeyF2=p.key;bindingSlotF2=s.nativeSlot;++linesF2;char m[240]{};
        std::snprintf(m,sizeof(m),"F2 F9 NATIVE_ATTACHMENT key=%016llX entity=%u slot=%u binding=CURRENT_NATIVE_SLOT colliderAdmission=SEPARATE",
            static_cast<unsigned long long>(p.key),p.entity,s.nativeSlot);diagnostic_trace::Write(m);}
    g_native_held_reason_t4.store(!LegacyPhysBodiesRetiredT4()?HeldPhysicsSourceT4::LegacyRetiring:
        g_right_grip.load(std::memory_order_acquire)<.25f?HeldPhysicsSourceT4::AwaitGrip:
        NativeHeldSourceCurrentT4(s.input)?HeldPhysicsSourceT4::Ready:HeldPhysicsSourceT4::Context,std::memory_order_release);
}
