// Independent raw-controller packet. Published on Henry's natural skeleton callback
// even when F10 is OFF or F12=2 deliberately suspends skeletal writes.
struct WeaponTracking6694 {
    void* character=nullptr;void* skeleton=nullptr;std::uint64_t epoch=0,tick=0,sample=0;
    QuatT28 nativeHand{},requestedHand{};float world[8]{};bool valid=false;
};
static std::atomic_flag g_weapon_tracking_lock6694=ATOMIC_FLAG_INIT;
static WeaponTracking6694 g_weapon_tracking6694; // legacy fixture/history only
static kcd2vr::core::SnapshotMailbox<WeaponTracking6694> g_weapon_frames6695;
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1};
static std::atomic<bool> g_weapon_fault6694{false};
static std::atomic<std::uint64_t> g_weapon_applied6694{0};
static std::atomic<int> g_weapon_reason6694{0};
static const char* WeaponReason6694(int r) noexcept {
    switch(r){case 1:return "WAIT_HELD_ITEM";case 2:return "WAIT_NATIVE_LINK_ID";case 3:return "WAIT_MATCHING_ATTACHMENT";
    case 4:return "WAIT_TRACKING_FRAME";case 5:return "NATIVE_REFERENCE_SUSPENDED";case 6:return "PLACEMENT_FAULT_LATCHED";
    case 7:return "CONTROLLER_PLACEMENT_ACTIVE";case 8:return "NATIVE_PLACEMENT_RESTORED";case 9:return "WAIT_COMMITTED_HAND_NO_RAW_SWITCH";default:return "OFF";}
}
// V66.97: native hand membership, not merely an item/entity still being alive.
// Retail +9D34E8 reads manager+38 -> player+990 -> interface+258.
// Retail +8D0758 writes interface[8+side*8]; +8D06F0 side=2 clears
// the matching hand via that writer. No gameplay getter is called here.
static bool NativeHandKey6697(void* manager,int side,std::uint64_t& key) noexcept {
    key=0;if(!manager||side<0||side>1)return false;
    const auto base=WhBase();void* player=nullptr;void* slots=nullptr;void* back=nullptr;
    std::uintptr_t vf=0;
    if(!base||!Read(manager,vf)||vf!=base+0x3EC9958ull||
       !Read(static_cast<unsigned char*>(manager)+0x38,player)||!player||
       !Read(player,vf)||vf!=base+0x3AA4B98ull||
       !Read(static_cast<unsigned char*>(player)+0x9D0,back)||back!=manager||
       !Read(static_cast<unsigned char*>(player)+0x990,slots)||!slots||
       !Read(static_cast<unsigned char*>(slots)+0x260+side*8,key))return false;
    // Verify the small chain after the read; changing equipment ownership is
    // not permission to keep a previously cached hand transform.
    void* again=nullptr;std::uint64_t keyAgain=0;
    return Read(static_cast<unsigned char*>(manager)+0x38,again)&&again==player&&
        Read(static_cast<unsigned char*>(player)+0x990,again)&&again==slots&&
        Read(static_cast<unsigned char*>(slots)+0x260+side*8,keyAgain)&&keyAgain==key;
}
// Cheap exclusion filter only. A match must subsequently pass the complete
// NativeHeldItemId6694 owner/role/coherence validation; this grants no writes.
static bool NativeCandidateItemId6699(int side,std::uint32_t& id) noexcept {
    id=0;if(side<0||side>1)return false;
    void* item=g_native_side_item[side].load(std::memory_order_acquire);
    void* link=nullptr;
    if(!item||g_native_side_item_state[side].load()!=1||
       !Read(static_cast<unsigned char*>(item)+0x88,link)||!link||
       !Read(static_cast<unsigned char*>(link)+0x30,id)||!id)return false;
    return item==g_native_side_item[side].load(std::memory_order_acquire);
}
static bool NativeHeldItemId6694(int side,std::uint32_t& id) noexcept {
    id=0;if(side<0||side>1)return false;
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);
    void* item=g_native_side_item[side].load(std::memory_order_acquire);
    if(!manager||!item||g_native_side_owner[side].load()!=manager||g_native_side_item_state[side].load()!=1)return false;
    std::uintptr_t vf=0;std::uint64_t key=0;void* link=nullptr;
    if(!Read(item,vf)||vf!=WhBase()+0x3A2DA88ull||!Read(static_cast<unsigned char*>(item)+0x30,key)||
       !key||key!=g_native_side_wuid0[side].load())return false;
    // +82D014 native getter: this is an ID record, NOT a CEntity pointer.
    if(!Read(static_cast<unsigned char*>(item)+0x88,link)||!link||!Read(static_cast<unsigned char*>(link)+0x30,id)||!id)return false;
    std::uint64_t handKey=0;
    if(!NativeHandKey6697(manager,side,handKey)||handKey!=key){id=0;return false;}
    return manager==g_henry_item_attach_manager.load(std::memory_order_acquire)&&
        item==g_native_side_item[side].load(std::memory_order_acquire)&&key==g_native_side_wuid0[side].load();
}
static void* CallEntityLookup6694(std::uintptr_t fn,void* sys,std::uint32_t id) noexcept {
    __try {return reinterpret_cast<void*(__fastcall*)(void*,std::uint32_t)>(fn)(sys,id);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_weapon_fault6694.store(true,std::memory_order_release);return nullptr;}
}
static void* EntityById6694(std::uint32_t id) noexcept {
    if(!id||g_weapon_fault6694.load())return nullptr;void* sys=nullptr;std::uintptr_t vf=0,fn=0;
    if(!Read(reinterpret_cast<void*>(WhBase()+0x492D8A0ull),sys)||!sys||!Read(sys,vf)||vf!=WhBase()+0x4077048ull||
       !Read(reinterpret_cast<void*>(vf+0x70),fn)||fn!=WhBase()+0x4B4F98ull)return nullptr;
    void* ent=CallEntityLookup6694(fn,sys,id);
    std::uint32_t actual=0;std::uintptr_t ev=0;
    return ent&&Read(ent,ev)&&ev==WhBase()+kCEntityVftRva&&Read(static_cast<unsigned char*>(ent)+0x0C,actual)&&actual==id?ent:nullptr;
}
static Q4 AnatomicalGrip6694(Q4 frame,Q4 xr,Q4 boneToAnatomy) noexcept {
    const Q4 c=QMul(frame,XrToCryQ(xr));
    const V3 distal=QRotateV(c,XrToCryV({0,-1,0})),side=QRotateV(c,XrToCryV({0,0,-1}));
    return QNormalize(QMul(QFromBasis(distal,side,Normalize(Cross(distal,side))),QConj(boneToAnatomy)));
}
static void PublishWeaponTracking6694(void* character,void* skeleton,void* pose,PoseGetFn getter,
                                    const RigMap& rig,const TrackingSnapshot& ts) noexcept {
    if(!character||!skeleton||!ts.allowed||!ts.head_valid||!ts.right_valid||!ts.tick)return;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());if(now<ts.tick||now-ts.tick>100)return;
    WeaponTracking6694 packet{};QuatT28 head{},hand{};
    kcd2vr::native_roomscale_d76::Origin roomOriginD76{};
    if(!kcd2vr::native_roomscale_d76::ReadCharacterOrigin(reinterpret_cast<std::uintptr_t>(character),roomOriginD76))return;
    if(!ReadQuat(getter,pose,rig.head,head)||!ReadQuat(getter,pose,rig.hand_r,hand)||!Read(static_cast<unsigned char*>(character)+0xA08,packet.world))return;
    double norm=0;for(int i=0;i<8;++i)if(!std::isfinite(packet.world[i]))return;
    for(int i=0;i<4;++i)norm+=double(packet.world[i])*packet.world[i];
    if(norm<.8||norm>1.2||packet.world[7]<.1f||packet.world[7]>10)return;
    // Only a handful of immutable anatomy reads; this does NOT run an arm solve.
    static void* cachedSkeleton=nullptr;static Q4 boneToAnatomy{};
    if(cachedSkeleton!=skeleton){
        QuatT28 h{},p[4]{};const int fingers[4]{415,414,413,412};
        if(!ReadCompanion(skeleton,rig.hand_r,h))return;
        V3 center{};for(int i=0;i<4;++i){if(!ReadCompanion(skeleton,fingers[i],p[i]))return;center=Add(center,FromT(p[i]));}
        center=Mul(center,.25f);const V3 d=Normalize(Sub(center,FromT(h))),side=Normalize(Sub(FromT(p[0]),FromT(p[3])));
        const V3 n=Normalize(Cross(d,side)),l=Normalize(Cross(n,d));if(Len(n)<.5f)return;
        boneToAnatomy=QMul(QConj(QFromQuatT(h)),QFromBasis(d,l,n));cachedSkeleton=skeleton;
    }
    Q4 frame{};V3 baseHead{},trackingHead{};
    if(g_cal_state.load(std::memory_order_acquire)==2){V3 unused{};(void)ControllerHeadAnchoredArmGoal(ts,head,false,unused);
        frame=QNormalize({g_cal_frame_qx.load(),g_cal_frame_qy.load(),g_cal_frame_qz.load(),g_cal_frame_qw.load()});
        baseHead={g_cal_native_head_x.load(),g_cal_native_head_y.load(),g_cal_native_head_z.load()};
        trackingHead={g_cal_head_x.load(),g_cal_head_y.load(),g_cal_head_z.load()};
    }else{
        // Weapon-only mode does not demand a T-pose. Establish a fixed head-level
        // origin/yaw once per actor/request. Current HMD pitch/roll is never reused.
        static void* originChar=nullptr;static std::uint64_t originEpoch=0;
        static V3 nh{},th{};static Q4 fq{};const auto e=g_weapon_request_epoch6694.load();
        if(originChar!=character||originEpoch!=e){
            const V3 forward=QRotateV(XrToCryQ(ts.hq),{0,1,0});
            const float angle=std::atan2(-forward.x,forward.y);fq={0,0,std::sin(-angle*.5f),std::cos(-angle*.5f)};
            nh=FromT(head);th=Sub(ts.head,{roomOriginD76.consumedTracking.x,roomOriginD76.consumedTracking.y,roomOriginD76.consumedTracking.z});originChar=character;originEpoch=e;
        }frame=fq;baseHead=nh;trackingHead=th;
    }
    if(kcd2vr::roomscale_d76::recenterPending.load(std::memory_order_acquire)||
       (g_cal_state.load()==2&&roomOriginD76.owner.epoch&&roomOriginD76.owner.epoch!=g_cal_room_origin_d76.load(std::memory_order_acquire)))return;
    trackingHead=Add(trackingHead,{roomOriginD76.consumedTracking.x,roomOriginD76.consumedTracking.y,roomOriginD76.consumedTracking.z});
    packet.nativeHand=hand;packet.requestedHand=hand;
    ToT(packet.requestedHand,Add(baseHead,QRotateV(frame,XrToCryV(Sub(ts.right,trackingHead)))));
    QToQuatT(packet.requestedHand,AnatomicalGrip6694(frame,ts.rq,boneToAnatomy));
    packet.character=character;packet.skeleton=skeleton;packet.tick=ts.tick;packet.sample=ts.sequence;
    packet.epoch=g_pose_epoch6689.load();packet.valid=true;
    if(!kcd2vr::native_roomscale_d76::SameSerial(kcd2vr::native_roomscale_d76::Surface::Character,
        reinterpret_cast<std::uintptr_t>(character),roomOriginD76.serial))return;
    (void)g_weapon_frames6695.publish(packet);
}
static bool ReadWeaponTracking6694(WeaponTracking6694& out) noexcept {
    if(!g_weapon_frames6695.read(out)||!g_gameplay_allowed.load()||!g_head_valid.load()||!g_right_valid.load())return false;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    return out.valid&&out.sample&&out.tick&&now>=out.tick&&now-out.tick<=100&&out.epoch==g_pose_epoch6689.load();
}
static QuatT28 HandWorld6694(const QuatT28& model,const float* world) noexcept {
    const Q4 q=QNormalize({world[0],world[1],world[2],world[3]});QuatT28 result=model;
    QToQuatT(result,QMul(q,QFromQuatT(model)));
    ToT(result,Add({world[4],world[5],world[6]},QRotateV(q,Mul(FromT(model),world[7]))));return result;
}
