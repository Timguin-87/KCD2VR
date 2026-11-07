static bool PhysReadItemWorld54(void* item,void*& link,std::uint32_t& id) noexcept {
    link=nullptr;id=0;
    return item&&Read(static_cast<unsigned char*>(item)+0x88,link)&&
        (!link||Read(static_cast<unsigned char*>(link)+0x30,id));
}
// R135: why the last PhysDescribeItem54 refused (game thread; read right after the call). Short reason + def vtable rva.
static const char* g_describe_refusal_r135="NONE";static std::uint32_t g_describe_vft_r135=0;static int g_describe_class_r135=-1;
// R136: the R135 "native class id" fallback is removed. vslot +0x50 is NOT a weapon class id for every definition:
// S_FoodItemClass 0x3A4DD20 returns its own food type there (R136 log 03:19: schnapps=1 accepted as a sword, bread=8,
// apple=0, cheese=9). Only weapon definitions (melee/missile/S_WeaponItemClass, or an alias of one) are weapons.
// R136: default inventory weight when a weapon definition carries none (tutorial practice sword, R136 log line 12846
// `reason=CLASS_HAND_WEIGHT` for key A9EC = census line 2792 "alias_prepadeni_longSword000630" class=4). These are the
// neutral references of weapon_dynamics177::body (ratio 1.0), so the sword feels like a standard one of its class.
static float PhysDefaultWeightR136(int classId) noexcept {
    switch(classId){case 2:return 3.2f;case 3:return 6.5f;case 4:return 3.4f;case 5:return 7.f;case 7:return 9.6f;default:return 3.f;}
}
static float g_describe_weight_r136=0;static int g_describe_flags_r136=0; // last raw weight / two|right<<8 (refusal row only)
static bool PhysDescribeItem54(std::uint64_t key,void* item,PhysEquipmentItem6698& x) noexcept {
    g_describe_refusal_r135="NONE";g_describe_vft_r135=0;g_describe_class_r135=-1;g_describe_weight_r136=0;g_describe_flags_r136=0;
    x={};if(!key||!PhysVft6698(item,0x3A2DA88)){g_describe_refusal_r135="NOT_AN_ITEM";return false;}x.item=item;
    void* rawDef=nullptr;void* def=nullptr;void* cls=nullptr;std::uint64_t again=0;
    if(!Read(static_cast<unsigned char*>(item)+0x30,again)||again!=key||
       !Read(static_cast<unsigned char*>(item)+0x48,rawDef)||!rawDef){g_describe_refusal_r135="KEY_OR_DEF";return false;}
    // R134: an alias item (quest/tutorial weapon) reads class, hands, weight and requirements from its source definition.
    def=PhysWeaponDefinitionR134(rawDef);
    {std::uintptr_t v=0;if(Read(rawDef,v)&&v>WhBase())g_describe_vft_r135=std::uint32_t(v-WhBase());}
    if(!def){g_describe_refusal_r135="DEF_NOT_WEAPON";return false;}
    {std::uintptr_t v=0;if(Read(rawDef,v)&&v>WhBase())x.defVftR135=std::uint32_t(v-WhBase());x.defSourceR135=def==rawDef?0:1;}
    if(!Read(static_cast<unsigned char*>(def)+0xE8,cls)||!cls||
       !PhysReadItemWorld54(item,x.worldLink54,x.profile.entity)){g_describe_refusal_r135="WEAPON_CLASS_ROW";return false;}
    unsigned char right=0,two=0;float weight=0;
    if(!Read(static_cast<unsigned char*>(cls)+8,x.profile.classId)||x.profile.classId<0||x.profile.classId>32){g_describe_refusal_r135="CLASS_RANGE";return false;}
    g_describe_class_r135=x.profile.classId;
    const bool twoRead=Read(static_cast<unsigned char*>(cls)+0x24,two),rightRead=Read(static_cast<unsigned char*>(cls)+0x25,right);
    g_describe_flags_r136=int(two)|(int(right)<<8);
    if(!twoRead||two>1||!rightRead||right>1){g_describe_refusal_r135="HAND_FLAGS";return false;}
    // R136: a weapon definition without a usable weight (quest/tutorial copy) is still the weapon; weight only scales handling.
    if(!Read(static_cast<unsigned char*>(def)+0x58,weight))weight=0;g_describe_weight_r136=weight;
    const bool defaultWeightR136=!std::isfinite(weight)||weight<=0||weight>100;
    if(defaultWeightR136)weight=PhysDefaultWeightR136(x.profile.classId);
    x.weightDefaultR136=defaultWeightR136;
    x.profile.key=key;x.profile.side=right?0:1;x.profile.twoHanded=two!=0;x.profile.weight=weight;
    if(PhysVft6698(def,0x3A4E390)){int sr=-1,ar=-1;
        if(Read(static_cast<unsigned char*>(def)+0x118,sr)&&sr>=0&&sr<=60)x.profile.strReq59c17=sr;
        if(Read(static_cast<unsigned char*>(def)+0x11C,ar)&&ar>=0&&ar<=60)x.profile.agiReq59c17=ar;}
    void* linkAfter=nullptr;void* defAfter=nullptr;void* clsAfter=nullptr;std::uint32_t idAfter=0;
    const bool same=Read(static_cast<unsigned char*>(item)+0x30,again)&&again==key&&
        Read(static_cast<unsigned char*>(item)+0x48,defAfter)&&defAfter==rawDef&&PhysWeaponDefinitionR134(defAfter)==def&&
        Read(static_cast<unsigned char*>(def)+0xE8,clsAfter)&&clsAfter==cls&&
        PhysReadItemWorld54(item,linkAfter,idAfter)&&linkAfter==x.worldLink54&&idAfter==x.profile.entity;
    if(!same)g_describe_refusal_r135="CHANGED_DURING_READ";
    return same;
}
static bool PhysCachedItemMatches54(const PhysEquipmentItem6698& x,std::uint64_t key) noexcept {
    std::uint64_t actual=0;void* link=nullptr;std::uint32_t id=0;
    return key&&x.profile.key==key&&PhysVft6698(x.item,0x3A2DA88)&&
        Read(static_cast<unsigned char*>(x.item)+0x30,actual)&&actual==key&&
        PhysReadItemWorld54(x.item,link,id)&&link==x.worldLink54&&id==x.profile.entity&&
        (!id||x.entity); // retry a present entity ID whose model was not ready yet
}
// Metadata-refresh-only helper retry. A model can exist while its helpers are
// still loading; caching that initial failure must not disable a valid sword.
static bool PhysReadGuardFaceH24(PhysEquipmentItem6698& x) noexcept {
    if(!x.entity||x.profile.classId!=4)return false;
    ph::Pose helper{};pm::Vec3 localNormal{1,0,0};
    if(!PhysHelper34(x.entity,"sharpening_center",helper,false)){
        // Exact retail swords_long proof: all30 available slt_0 helpers map
        // local -Y to the visual blade's thin X. No wrist/bounds axis guess.
        if(!PhysHelper34(x.entity,"slt_0",helper,false))return false;
        localNormal={0,-1,0};
    }
    const auto face=pm::unit(pm::rotate(helper.q,localNormal));
    if(!face)return false;
    x.profile.guardFaceNormalH24=*face;x.profile.guardFaceReadyH24=true;return true;
}
static void PhysRetryGuardFaceH24(PhysEquipmentItem6698& x,std::uint64_t request) noexcept {
    if(!x.held){x.guardFaceWasHeldH24=false;x.guardFaceRetryCountH24=0;return;}
    if(!x.guardFaceWasHeldH24||x.guardFaceRetryRequestH24!=request){
        x.guardFaceWasHeldH24=true;x.guardFaceRetryRequestH24=request;x.guardFaceRetryCountH24=0;
    }
    if(x.profile.guardFaceReadyH24||!x.entity||x.profile.classId!=4||x.guardFaceRetryCountH24>=4)return;
    ++x.guardFaceRetryCountH24;(void)PhysReadGuardFaceH24(x);
}
static bool PhysResolveItem6698(std::uint64_t key,PhysEquipmentItem6698& x) noexcept {
    x={};if(PhysEmpty6698(key)||!PhysDescribeItem54(key,PhysItemLookup6698(key),x))return false;
    x.entity=x.profile.entity?EntityById6694(x.profile.entity):nullptr; // C45: equipped metadata survives a despawned stowed model
    x.profile.legacyHolsters147=g_legacy_holsters147.load(std::memory_order_relaxed);
    static std::uint64_t generation=0;x.profile.generation=++generation;
    x.boundsValid=x.entity&&PhysBounds6698(x.entity,x.bounds);
    // H24: visual-mesh thin X was observed in all36 swords_long CGFs;
    // 29 contain sharpening_center; slt_0 extends verified coverage to30.
    // These are retail-family proofs, not a guarantee for modified assets.
    // A qualified helper gives its entity-space
    // blade-face normal; the round collision proxy cannot. Missing helpers
    // remain unavailable, with bounded retries after drawing a loading model.
    (void)PhysReadGuardFaceH24(x);
    if(x.entity&&x.profile.twoHanded&&x.profile.melee()&&!x.profile.bow()){ // C34b: once per item resolve, never per frame
        x.slotValid34[0]=PhysHelper34(x.entity,"slt_0",x.slot34[0],false);x.slotValid34[1]=PhysHelper34(x.entity,"slt_1",x.slot34[1],false);
        ph::Pose raw0{},raw1{};const bool r0=PhysHelper34(x.entity,"slt_0",raw0,true),r1=PhysHelper34(x.entity,"slt_1",raw1,true);
        const double slotDelta=(r0&&x.slotValid34[0])?pm::length(raw0.p-x.slot34[0].p):-1.0;(void)r1;(void)raw1;
        char sm[320]{};std::snprintf(sm,sizeof(sm),"C34 WEAPON_SLOTS key=%016llX class=%d slt_0=%d(%.3f,%.3f,%.3f) slt_1=%d(%.3f,%.3f,%.3f) entitySlotOffsetM=%.4f source=WEAPON_MODEL_HELPERS frame=ENTITY",
            (unsigned long long)key,x.profile.classId,x.slotValid34[0]?1:0,x.slot34[0].p.x,x.slot34[0].p.y,x.slot34[0].p.z,x.slotValid34[1]?1:0,x.slot34[1].p.x,x.slot34[1].p.y,x.slot34[1].p.z,slotDelta);
        diagnostic_trace::Write(sm);}
    if(x.boundsValid)for(int i=0;i<3;++i)if(!std::isfinite(x.bounds[i])||!std::isfinite(x.bounds[i+3])||x.bounds[i+3]<x.bounds[i]||x.bounds[i+3]-x.bounds[i]>8)x.boundsValid=false;
    static const char* const kDefSourceR135[3]{"WEAPON_DEF","ALIAS_SOURCE","WEAPON_DEF"};
    char message[420]{};std::snprintf(message,sizeof(message),"V66.98 ITEM key=%016llX class=%d nativeHand=%c twoHand=%d inventoryWeight=%.3f bounds=%d strReq=%d agiReq=%d worldLink=%d entity=%u metadataOnly=%d def=%X defSource=%s weightDefault=%d",(unsigned long long)key,x.profile.classId,x.profile.side?'L':'R',x.profile.twoHanded,x.profile.weight,x.boundsValid,x.profile.strReq59c17,x.profile.agiReq59c17,x.worldLink54?1:0,x.profile.entity,x.entity?0:1,x.defVftR135,kDefSourceR135[x.defSourceR135<3?x.defSourceR135:0],x.weightDefaultR136?1:0);diagnostic_trace::Write(message);
    return true;
}
static bool PhysSupplyHeldT2(const PhysEquipment6698& e,const PhysEquipmentItem6698& item,int side) noexcept {
    kcd2vr::supply_t2::Lease l{};
    return kcd2vr::action_time_owner59c11::ReadThrowingSupplyT2(l)&&l.key==item.profile.key&&l.side==side&&
        l.manager==reinterpret_cast<std::uintptr_t>(e.manager)&&l.player==reinterpret_cast<std::uintptr_t>(e.player)&&
        l.body==e.epoch&&l.request==e.request&&l.entity==item.profile.entity&&l.classId==item.profile.classId&&
        e.handKnown[side]&&e.handKey[side]==l.key;
}
static bool PhysReadEquipment6698(PhysEquipment6698& out) noexcept {
    static PhysEquipment6698 cache{};out={};const auto request=g_weapon_request_epoch6694.load();
    out.manager=g_henry_item_attach_manager.load();if(!PhysVft6698(out.manager,0x3EC9958)||!Read(static_cast<unsigned char*>(out.manager)+0x38,out.player)||!PhysVft6698(out.player,0x3AA4B98))return false;
    void* back=nullptr;if(!Read(static_cast<unsigned char*>(out.player)+0x9D0,back)||back!=out.manager)return false;
    out.request=request;out.epoch=g_pose_epoch6689.load();out.tick=GetTickCount64();
    if(!Read(static_cast<unsigned char*>(out.manager)+0x40,out.selected)||out.selected<0||out.selected>2)return false;
    for(int s=0;s<2;++s){out.handKnown[s]=NativeHandKey6697(out.manager,s,out.handKey[s]);if(!out.handKnown[s])return false;if(PhysEmpty6698(out.handKey[s]))out.handKey[s]=0;}
    auto add=[&](std::uint64_t key,int set,int slot){
        if(PhysEmpty6698(key))return;
        for(int i=0;i<out.count;++i)if(out.items[i].profile.key==key)return;
        if(out.count>=int(out.items.size()))return;
        PhysEquipmentItem6698 x{};bool found=false;
        if(cache.manager==out.manager)for(int i=0;i<cache.count;++i)if(cache.items[i].profile.key==key){
            if(PhysCachedItemMatches54(cache.items[i],key)){x=cache.items[i];found=true;}break;}
        if(!found&&!PhysResolveItem6698(key,x))return;
        x.profile.legacyHolsters147=g_legacy_holsters147.load(std::memory_order_relaxed);
        x.set=set;x.profile.holsterSetR12=set;x.slot126=slot;x.held=false;
        if(x.profile.classId==0)x.profile.side=1; // native default, independent of the scoped explicit-hand lease
        if(PhysSupplyHeldT2(out,x,0))x.profile.side=0;
        for(int side=0;side<2;++side)if(out.handKey[side]==key&&x.profile.side==side){x.held=true;out.heldIndex[side]=out.count;}
        // Called only by the existing metadata refresh (500ms or equipment
        // change), at most four held attempts per draw/request, none after success.
        PhysRetryGuardFaceH24(x,request);
        if(x.entity&&x.held&&found&&x.profile.twoHanded&&x.profile.melee()&&!x.profile.bow()&&!x.slotValid34[1]&&x.slotRetry34!=request){ // C34b: model may not have been loaded while stowed
            x.slotRetry34=request;x.slotValid34[0]=PhysHelper34(x.entity,"slt_0",x.slot34[0],false);x.slotValid34[1]=PhysHelper34(x.entity,"slt_1",x.slot34[1],false);
            if(x.slotValid34[1]){char sm[200]{};std::snprintf(sm,sizeof(sm),"C34 WEAPON_SLOTS key=%016llX retry=DRAWN slt_0=%d slt_1=1",(unsigned long long)key,x.slotValid34[0]?1:0);diagnostic_trace::Write(sm);}}
        out.items[out.count++]=x;
    };
    for(int set=0;set<3;++set)for(int slot=0;slot<2;++slot){std::uint64_t key=0;if(Read(static_cast<unsigned char*>(out.manager)+0x48+set*16+slot*8,key))add(key,set,slot);}
    std::uint64_t extra45=0;
    if(Read(static_cast<unsigned char*>(out.manager)+0x78,extra45)&&!PhysEmpty6698(extra45)){
        const int previous=out.count;add(extra45,3,2);
        if(out.count>previous&&out.items[out.count-1].profile.classId!=7){
            --out.count;for(int side=0;side<2;++side)if(out.heldIndex[side]>=out.count)out.heldIndex[side]=-1;
        } // only proven pole class
    }
    static std::uint64_t reportedR135[2]{};
    for(int side=0;side<2;++side){
        if(!out.handKey[side]){reportedR135[side]=0;continue;}
        if(out.heldIndex[side]>=0)continue;
        g_describe_refusal_r135="NOT_READ";g_describe_vft_r135=0;g_describe_class_r135=-1;g_describe_weight_r136=0;g_describe_flags_r136=0;
        add(out.handKey[side],-1,-1);
        // R135: one short line when an occupied hand stays unknown (once per key; an empty hand or another key re-arms it).
        // Not a latch: the snapshot retries this item every refresh.
        if(out.heldIndex[side]<0&&reportedR135[side]!=out.handKey[side]){reportedR135[side]=out.handKey[side];char m[240]{};
            std::snprintf(m,sizeof(m),"R135 HAND_ITEM_UNKNOWN side=%c key=%016llX reason=%s def=%X class=%d twoRight=%X weight=%.3f hand=NOT_OWNED retry=EVERY_SNAPSHOT",
                side?'L':'R',(unsigned long long)out.handKey[side],g_describe_refusal_r135,g_describe_vft_r135,g_describe_class_r135,g_describe_flags_r136,double(g_describe_weight_r136));diagnostic_trace::Write(m);}
    }
    // Unsupported occupied hands stay explicitly UNKNOWN/OCCUPIED to the solver.
    out.valid=true;cache=out;return true;
}
// V66.103: cache expensive immutable profiles, never cached live hand eligibility.
// No native item lookup, string resolution or bounds calls in this snapshot.
struct PhysLive103 {
    void* manager=nullptr;void* player=nullptr;
    std::uint64_t request=0,epoch=0,hand[2]{},slots[6]{},extra45=0;int selected=-1;
};
static bool PhysReadLive103(PhysLive103& live) noexcept {
    live={};live.request=g_weapon_request_epoch6694.load();live.epoch=g_pose_epoch6689.load();
    live.manager=g_henry_item_attach_manager.load();
    if(!PhysVft6698(live.manager,0x3EC9958)||!Read(static_cast<unsigned char*>(live.manager)+0x38,live.player)||!PhysVft6698(live.player,0x3AA4B98))return false;
    void* back=nullptr;
    if(!Read(static_cast<unsigned char*>(live.player)+0x9D0,back)||back!=live.manager||
       !Read(static_cast<unsigned char*>(live.manager)+0x40,live.selected)||live.selected<0||live.selected>2||
       !Read(static_cast<unsigned char*>(live.manager)+0x48,live.slots)||
       !Read(static_cast<unsigned char*>(live.manager)+0x78,live.extra45))return false;
    for(int side=0;side<2;++side){if(!NativeHandKey6697(live.manager,side,live.hand[side]))return false;if(PhysEmpty6698(live.hand[side]))live.hand[side]=0;}
    // If equipment changes halfway through the read, wait for the next callback.
    std::uint64_t after[6]{},extra45=0;int selected=-1;
    if(!Read(static_cast<unsigned char*>(live.manager)+0x48,after)||std::memcmp(after,live.slots,sizeof(after))||
       !Read(static_cast<unsigned char*>(live.manager)+0x40,selected)||selected!=live.selected||
       !Read(static_cast<unsigned char*>(live.manager)+0x78,extra45)||extra45!=live.extra45)return false;
    return live.manager==g_henry_item_attach_manager.load()&&live.request==g_weapon_request_epoch6694.load()&&live.epoch==g_pose_epoch6689.load();
}
static bool PhysSameLive103(const PhysLive103& a,const PhysLive103& b) noexcept {
    return a.player==b.player&&a.manager==b.manager&&a.request==b.request&&a.epoch==b.epoch&&a.selected==b.selected&&
        a.hand[0]==b.hand[0]&&a.hand[1]==b.hand[1]&&a.extra45==b.extra45&&!std::memcmp(a.slots,b.slots,sizeof(a.slots));
}
static std::uint64_t g_equipment_reads121=0,g_equipment_refresh121=0; // native-frame owner
struct PhysMetadataCache104 {PhysEquipment6698 profiles{};PhysLive103 last{};std::uint64_t metadataAt=0;};
static PhysMetadataCache104 g_phys_metadata104; // native-frame owner only, no cross-thread content reuse
static bool PhysRefreshEquipment103(PhysEquipment6698& out,std::uint64_t now) noexcept {
    auto& profiles=g_phys_metadata104.profiles;auto& last=g_phys_metadata104.last;auto& metadataAt=g_phys_metadata104.metadataAt;
    ++g_equipment_reads121;
    PhysLive103 live{};
    {PerfScope101R2 liveTiming122(6);if(!PhysReadLive103(live)){profiles={};out={};return false;}}
    const bool refresh=!profiles.valid||!PhysSameLive103(live,last)||now<metadataAt||now-metadataAt>=500;
    if(refresh){
        PerfScope101R2 metadataTiming122(7);
        ++g_equipment_refresh121;
        if(!PhysReadEquipment6698(profiles)){profiles={};out={};return false;}
        if(profiles.player!=live.player||profiles.manager!=live.manager||profiles.request!=live.request||profiles.epoch!=live.epoch||
           profiles.selected!=live.selected||profiles.handKey[0]!=live.hand[0]||profiles.handKey[1]!=live.hand[1]){profiles={};out={};return false;}
        PhysLive103 after{};
        if(!PhysReadLive103(after)||!PhysSameLive103(live,after)){profiles={};out={};return false;}
        last=live;metadataAt=now;
    }
    out=profiles;
    // This timestamp certifies the new live snapshot, not a rejuvenated stale pose.
    out.tick=now;out.handKey[0]=live.hand[0];out.handKey[1]=live.hand[1];out.handKnown[0]=out.handKnown[1]=true;
    out.heldIndex[0]=out.heldIndex[1]=-1;
    for(int i=0;i<out.count;++i){auto& item=out.items[i];
        if(item.profile.classId==0)item.profile.side=1;
        const bool supply=PhysSupplyHeldT2(out,item,0);if(supply)item.profile.side=0;
        const int side=item.profile.side;
        item.held=side>=0&&side<2&&live.hand[side]==item.profile.key;
        if(item.held)out.heldIndex[side]=i;
        item.dropUnitT1=false;
        if(item.held&&kcd2vr::weapon_release_t1::Mode()==1&&kcd2vr::weapon_release_t1::SupportedClass(item.profile.classId)){
            int amount=0;unsigned flags=0;
            item.dropUnitT1=Read(static_cast<unsigned char*>(item.item)+0x50,amount)&&amount==1&&
                Read(static_cast<unsigned char*>(item.item)+0x60,flags)&&!(flags&2u)&&((flags&1u)||supply);
        }
    }
    return true;
}
