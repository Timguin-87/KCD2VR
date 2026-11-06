// V66.128 (c) bow/arrow observation + (d) string and arrow follow the draw hand.
// Included in frik_pose_probe's private namespace after coherent_pose.inl.
//
// Static facts (retail WHGame.dll 0x6A350E20, wh_bow_database.adb):
//  * The arrow hangs on an attachment named "arrow" on the BOW's skeleton
//    (C_MissileWeaponItem +0x2A8AD10), offset by the inverse of the arrow's slt_0.
//  * The bow skeleton is posed by bow_pull_back through LayerManualUpdate driven
//    by ChargeFraction. String + arrow therefore follow native charge TIME.
//
// (d) is visual only: we learn the bow's own rest pose and full-charge pose from
// native frames, then while drawing replay ONLY the joints that differ between
// them at a fraction measured from the tracked right hand along the learned draw
// axis. Native ChargeFraction, timing, stamina, ammunition and the shot are never
// written. Any write fault disables (d) for the session; observation continues.
constexpr std::uint32_t kBowJoints128=96;
struct BowRig128 {
    void* character=nullptr;void* owner=nullptr;std::uint32_t count=0;
    std::uint64_t itemKey59c10r2=0,itemGeneration59c10r2=0;
    std::array<int,kBowJoints128> parent{},order{};
    std::array<QuatT28,kBowJoints128> restModel{},fullModel{},restLocal{},fullLocal{},lastWritten{};
    std::array<bool,kBowJoints128> moving{},affected{};
    bool haveRest=false,haveFull=false,analysed=false,wroteLast=false,namesLogged=false;
    int movingCount=0,stringJoint=-1,namedString166=-1,namedArrow166=-1;V3 stringRest{},axis{};float drawLen=0;
    std::uint64_t idleSince=0,lastLog=0,writes=0,learnRest=0,learnFull=0;
    bool drawReceipt48=false;unsigned receiptRank48=0;
    V3 partialAxis48{};unsigned partialStable48=0;std::uint64_t partialTick48=0;bool partialReady48=false;
    kcd2vr::bow_grip178r36c1::Latch grip36c1{};
    QuatT28 arrowInString36c1{};
};
static BowRig128 g_bow_rig128;
static std::atomic_flag g_bow_lock128=ATOMIC_FLAG_INIT;

static bool BowCandidate128(void* charPlus8) noexcept {
    void* bow=g_bow_char128.load(std::memory_order_acquire);
    if(!bow||!charPlus8||reinterpret_cast<std::uintptr_t>(charPlus8)!=reinterpret_cast<std::uintptr_t>(bow)+8u)return false;
    const auto now=std::uint64_t(GetTickCount64()),t=g_bow_char_tick128.load(std::memory_order_acquire);
    return t&&now>=t&&now-t<=1000;
}
static bool BowDrawing128(std::uint64_t now,float& charge) noexcept {
    const auto t=g_bow_charge_tick128.load(std::memory_order_acquire);charge=g_bow_charge128.load(std::memory_order_relaxed);
    return t&&now>=t&&now-t<=150;
}
static void BowComposeOrder128(BowRig128& r) noexcept {
    std::array<unsigned short,kBowJoints128> depth{};
    for(std::uint32_t j=0;j<r.count;++j){int p=r.parent[j];unsigned short n=0;while(p>=0&&n<r.count){p=r.parent[p];++n;}depth[j]=n;r.order[j]=int(j);}
    std::sort(r.order.begin(),r.order.begin()+r.count,[&](int a,int b){return depth[a]<depth[b]||(depth[a]==depth[b]&&a<b);});
}
static void BowLocals128(const BowRig128& r,const QuatT28* model,QuatT28* local) noexcept {
    for(std::uint32_t j=0;j<r.count;++j){const int p=r.parent[j];local[j]=p>=0?RelativeFromAbsolute(model[p],model[j]):model[j];}
}
static void BowAnalyse128(BowRig128& r) noexcept {
    BowLocals128(r,r.restModel.data(),r.restLocal.data());BowLocals128(r,r.fullModel.data(),r.fullLocal.data());
    r.movingCount=0;r.stringJoint=-1;float best=0;r.moving.fill(false);r.affected.fill(false);
    for(std::uint32_t j=0;j<r.count;++j){
        const double a=pc::angle(PQ(QFromQuatT(r.restLocal[j])),PQ(QFromQuatT(r.fullLocal[j])));
        const float dt=Len(Sub(FromT(r.restLocal[j]),FromT(r.fullLocal[j])));
        r.moving[j]=(std::isfinite(a)&&a>.0175)||dt>.004f;if(r.moving[j])++r.movingCount;
        const float disp=Len(Sub(FromT(r.restModel[j]),FromT(r.fullModel[j])));if(disp>best){best=disp;r.stringJoint=int(j);}
    }
    for(std::uint32_t k=0;k<r.count;++k){const int j=r.order[k],p=r.parent[j];r.affected[j]=r.moving[j]||(p>=0&&r.affected[p]);}
    // The asset defines Bow_CenterString and Bow_Arrow. Prefer the real string
    // joint over an arbitrary equally-moving attachment/helper.
    if(r.namedString166>=0&&r.namedString166<int(r.count)){
        r.stringJoint=r.namedString166;best=Len(Sub(FromT(r.restModel[r.stringJoint]),FromT(r.fullModel[r.stringJoint])));
    }
    r.drawLen=best;r.analysed=r.stringJoint>=0&&best>.12f&&best<.95f&&r.movingCount>0;
    if(r.analysed){r.stringRest=FromT(r.restModel[r.stringJoint]);r.axis=Normalize(Sub(FromT(r.fullModel[r.stringJoint]),r.stringRest));}
    char m[768]{};int pos=std::snprintf(m,sizeof(m),"V66.128-BOW_RIG LEARNED joints=%u moving=%d string=%d drawLenM=%.3f axisBow=(%.3f,%.3f,%.3f) follow=%s list=",
        r.count,r.movingCount,r.stringJoint,r.drawLen,r.axis.x,r.axis.y,r.axis.z,r.analysed?"ENABLED":"REJECTED");
    for(std::uint32_t j=0,n=0;j<r.count&&n<14&&pos>0&&pos<int(sizeof(m))-48;++j)if(r.moving[j]){++n;
        pos+=std::snprintf(m+pos,sizeof(m)-pos,"%u(p%d,d=%.3f) ",j,r.parent[j],Len(Sub(FromT(r.restModel[j]),FromT(r.fullModel[j]))));}
    diagnostic_trace::Write(m);
}
static void BowNames128(BowRig128& r) noexcept {
    if(r.namesLogged)return;r.namesLogged=true;
    static const char* names[]{"Bow_CenterString","Bow_Arrow","Bow_TopString","Bow_BottomString","string","String","bowstring","bow_string","string_mid","string_center","arrow","Arrow","arrow_bone","nock","slt_0","slt_1","root","Root","Bip01","bow"};
    char m[640]{};int pos=std::snprintf(m,sizeof(m),"V66.128-BOW_NAMES owner=%p joints=%u ",r.owner,r.count);
    for(const char* n:names){int id=-1;(void)ResolveJointByNameExactV6545(r.owner,n,id);
        if(!std::strcmp(n,"Bow_CenterString"))r.namedString166=id;
        if(!std::strcmp(n,"Bow_Arrow"))r.namedArrow166=id;
        if(pos>0&&pos<int(sizeof(m))-32)pos+=std::snprintf(m+pos,sizeof(m)-pos,"%s=%d ",n,id);}
    diagnostic_trace::Write(m);
}
static void BowLearnNativeSample175(BowRig128& r,
        const std::array<QuatT28,kBowJoints128>& cur,std::uint64_t now,bool drawing,float charge) noexcept {
    // R44C2: keep the measured rest/full pair immutable for this exact rig.
    // Updating restModel after analysing restLocal/fullLocal mixed two geometries.
    if(r.analysed)return;
    // Identical callbacks after our write remain our output, even across idle or
    // skipped frames. Only a genuinely different native buffer (or a new rig
    // lifetime) clears this provenance latch; elapsed time grants no permission.
    const bool nativeFresh=!r.wroteLast||std::memcmp(cur.data(),r.lastWritten.data(),r.count*sizeof(QuatT28))!=0;
    if(!nativeFresh)return;
    r.wroteLast=false;
    // C48: learn a direction from native partial travel BEFORE the first full
    // shot. Three distinct native ticks must agree; rest and current belong to
    // this rig. No draw length/full pose is invented from a partial sample.
    if(drawing&&!r.partialReady48&&r.haveRest&&charge>=.15f&&r.namedString166>=0&&r.namedString166<int(r.count)&&now>r.partialTick48){
        const V3 travel=Sub(FromT(cur[r.namedString166]),FromT(r.restModel[r.namedString166]));
        const float distance=Len(travel);
        if(distance>=.04f&&distance<=.95f){
            const V3 axis=Normalize(travel);
            if(r.partialStable48&&Dot(axis,r.partialAxis48)>.9968f)++r.partialStable48;else r.partialStable48=1;
            r.partialAxis48=axis;r.partialTick48=now;
            if(r.partialStable48>=3)r.partialReady48=true;
        }
    }
    if(!drawing){if(!r.idleSince)r.idleSince=now;
        if(now>=r.idleSince&&now-r.idleSince>=300){r.restModel=cur;if(!r.haveRest){r.haveRest=true;diagnostic_trace::Write("V66.128-BOW_RIG REST captured (bow idle >=300 ms)");}++r.learnRest;}}
    else r.idleSince=0;
    if(drawing&&charge>=.97f){const bool first=!r.haveFull;r.fullModel=cur;r.haveFull=true;++r.learnFull;
        if(r.haveRest&&(first||!r.analysed))BowAnalyse128(r);}
}
// Historical R36 helper retained for its comparison fixture. R44C2 production
// uses BowDrawHierarchy44c2 below; this helper must not independently displace
// the centre/arrow subtree while the other string branches use a different pose.
static bool BowGripHierarchy36c1(const BowRig128& r,bool acquire,ph::Pose target,
        QuatT28& arrowInString,std::array<QuatT28,kBowJoints128>& local,
        std::array<QuatT28,kBowJoints128>& model,std::array<bool,kBowJoints128>& mask) noexcept {
    const int string=r.namedString166,arrow=r.namedArrow166;
    if(!r.count||r.count>kBowJoints128||string<0||string>=int(r.count)||
       arrow<0||arrow>=int(r.count)||arrow==string||!ph::finite(target))return false;
    std::array<bool,kBowJoints128> child{};
    for(unsigned k=0;k<r.count;++k){
        const int j=r.order[k];if(j<0||j>=int(r.count))return false;
        const int p=r.parent[j];if(p>=int(r.count)||p==j)return false;
        child[j]=j==string||(p>=0&&child[p]);
    }
    if(!child[arrow])return false;
    if(acquire)arrowInString=RelativeFromAbsolute(model[string],model[arrow]);
    if(!ValidPose6689(arrowInString))return false;
    QuatT28 desired{};QToQuatT(desired,QP(target.q));ToT(desired,VP(target.p));
    const QuatT28 desiredArrow=AbsoluteFromRelative(desired,arrowInString);
    for(unsigned k=0;k<r.count;++k){
        const int j=r.order[k],p=r.parent[j];
        if(j==string||j==arrow){const auto& goal=j==string?desired:desiredArrow;
            local[j]=p>=0?RelativeFromAbsolute(model[p],goal):goal;}
        model[j]=p>=0?AbsoluteFromRelative(model[p],local[j]):local[j];
        if(!ValidPose6689(model[j]))return false;
        if(child[j])mask[j]=true;
    }
    return true;
}
// All authored moving joints are composed together before any write. No
// controller/wrist transform can displace a lone string end or arrow socket.
static bool BowDrawHierarchy44c2(const BowRig128& r,
        const std::array<QuatT28,kBowJoints128>& cur,float fraction,
        std::array<QuatT28,kBowJoints128>& local,
        std::array<QuatT28,kBowJoints128>& model,
        std::array<bool,kBowJoints128>& mask) noexcept {
    if(!r.analysed||!r.count||r.count>kBowJoints128||!std::isfinite(fraction)||fraction<0||fraction>1)return false;
    BowLocals128(r,cur.data(),local.data());
    for(std::uint32_t j=0;j<r.count;++j)if(r.affected[j]){
        const auto q=pc::slerp(PQ(QFromQuatT(r.restLocal[j])),PQ(QFromQuatT(r.fullLocal[j])),fraction);if(!q)return false;
        QToQuatT(local[j],QP(*q));
        ToT(local[j],Add(FromT(r.restLocal[j]),Mul(Sub(FromT(r.fullLocal[j]),FromT(r.restLocal[j])),fraction)));
    }
    for(std::uint32_t k=0;k<r.count;++k){const int j=r.order[k],p=r.parent[j];
        model[j]=p>=0?AbsoluteFromRelative(model[p],local[j]):local[j];
        if(!ValidPose6689(model[j]))return false;
    }
    mask=r.affected;return true;
}
// C48 first-draw preview: interpolate the ENTIRE currently authored chain
// back toward its own rest. Never extrapolate beyond an observed native pose.
// This makes the first pull use the same whole-rig owner as later full samples.
static bool BowDrawPartial48(const BowRig128& r,
        const std::array<QuatT28,kBowJoints128>& cur,double targetDraw,
        std::array<QuatT28,kBowJoints128>& local,
        std::array<QuatT28,kBowJoints128>& model,
        std::array<bool,kBowJoints128>& mask) noexcept {
    const int string=r.namedString166;
    if(!r.haveRest||r.analysed||!r.count||r.count>kBowJoints128||string<0||string>=int(r.count)||
       !std::isfinite(targetDraw)||targetDraw<0||targetDraw>.95)return false;
    const double observed=Len(Sub(FromT(cur[string]),FromT(r.restModel[string])));
    if(!std::isfinite(observed)||observed<.004||observed>.95)return false;
    const double mix=std::clamp(targetDraw/observed,0.,1.);
    std::array<QuatT28,kBowJoints128> rest{},native{};
    BowLocals128(r,r.restModel.data(),rest.data());BowLocals128(r,cur.data(),native.data());mask.fill(false);
    for(std::uint32_t k=0;k<r.count;++k){
        const int j=r.order[k],parent=r.parent[j];
        if(j<0||j>=int(r.count)||parent>=int(r.count)||parent==j)return false;
        const auto q=pc::slerp(PQ(QFromQuatT(rest[j])),PQ(QFromQuatT(native[j])),mix);if(!q)return false;
        QToQuatT(local[j],QP(*q));ToT(local[j],Add(FromT(rest[j]),Mul(Sub(FromT(native[j]),FromT(rest[j])),float(mix))));
        model[j]=parent>=0?AbsoluteFromRelative(model[parent],local[j]):local[j];
        if(!ValidPose6689(model[j]))return false;
        mask[j]=(parent>=0&&mask[parent])||Len(Sub(FromT(rest[j]),FromT(native[j])))>.00001f||
            pc::angle(PQ(QFromQuatT(rest[j])),PQ(QFromQuatT(native[j])))>.00001;
    }
    return true;
}
static void BowSkinning128(void* pose,void* owner,void* charPlus8) noexcept {
    if(g_bow_lock128.test_and_set(std::memory_order_acquire))return;
    struct Unlock{~Unlock(){g_bow_lock128.clear(std::memory_order_release);}} unlock;
    // Any rejected skinning pass ends the visual lease. Never carry a stale
    // attachment through menus, tracking loss, a new bow or a new draw.
    bool gripWritten36c1=false;
    struct GripExit36c1 {bool& written;~GripExit36c1(){if(!written)g_bow_rig128.grip36c1={};}} gripExit36c1{gripWritten36c1};
    void* ci=nullptr;void* anim=nullptr;void* skel=nullptr;
    if(!ReadValidatedCharInstanceFromSkinning(charPlus8,ci,anim)||ci!=g_bow_char128.load()||
       !ReadValidatedDefaultSkeletonFromPose(pose,skel)||skel!=owner)return;
    auto& r=g_bow_rig128;const auto now=std::uint64_t(GetTickCount64());
    bowgeom59::Placement placement59{};
    const bool placed59=g_bow_placement59c10r2.read(placement59)&&bowgeom59::fresh(placement59,now)&&
        placement59.identity.character==ci&&placement59.identity.pose==pose&&placement59.identity.owner==owner;
    const bool itemChanged59=placed59&&(r.itemKey59c10r2!=placement59.identity.key||
        r.itemGeneration59c10r2!=placement59.identity.generation);
    if(r.character!=ci||r.owner!=owner||itemChanged59){
        r=BowRig128{};std::uint32_t count=0;
        if(placed59){r.itemKey59c10r2=placement59.identity.key;r.itemGeneration59c10r2=placement59.identity.generation;}
        if(!ReadJointCount(owner,count)||count<2||count>kBowJoints128){
            char m[160]{};std::snprintf(m,sizeof(m),"V66.128-BOW_RIG UNSUPPORTED joints=%u (max %u) observation-only",count,kBowJoints128);diagnostic_trace::Write(m);
            r.character=ci;r.owner=owner;r.count=0;return;}
        for(std::uint32_t j=0;j<count;++j)if(!ReadParentDirect(owner,int(j),r.parent[j])||r.parent[j]>=int(count)||r.parent[j]==int(j))return;
        r.character=ci;r.owner=owner;r.count=count;BowComposeOrder128(r);
        char m[192]{};std::snprintf(m,sizeof(m),"V66.128-BOW_RIG IDENTIFIED bowChar=%p skeleton=%p joints=%u",ci,owner,count);diagnostic_trace::Write(m);
        BowNames128(r);
    }
    if(!r.count)return;
    PoseGetFn b0=nullptr,b1=nullptr;std::array<QuatT28,kBowJoints128> cur{};
    if(!ValidateRawPoseApi(pose,b0,b1)||!ReadNativeBuffer6689(pose,b1,r.count,cur.data()))return;
    for(std::uint32_t j=0;j<r.count;++j)if(!ValidPose6689(cur[j]))return;
    float charge=0.f;const bool drawing=BowDrawing128(now,charge);
    BowLearnNativeSample175(r,cur,now,drawing,charge);
    // Admit copied geometry only when the current native bow character and the
    // allowed physical weapon agree in WORLD space. This checks the model origin
    // and rotation before sharing a skeleton-space nock with the gesture policy.
    if(placed59&&r.haveRest&&r.namedString166>=0&&r.namedString166<int(r.count)){
        PhysFrame6698 frame{};PhysEquipment6698 equipment{};ModelWorld6692 nativeWorld{};BowCharacterIdentity166 identity{};
        const auto complete=g_phys_ready_sample6698.load(std::memory_order_acquire);
        if(g_bow_identity166.read(identity)&&identity.character==ci&&identity.tick&&now>=identity.tick&&now-identity.tick<=80&&
           complete&&g_phys_frame6698.read(frame)&&frame.input.sample==complete&&
           complete==g_phys_ready_sample6698.load(std::memory_order_acquire)&&
           g_phys_equipment6698.read(equipment)&&equipment.valid&&frame.profiles[1].bow()&&
           frame.request==equipment.request&&frame.input.bodyEpoch==equipment.epoch&&
           now>=frame.input.tick&&now-frame.input.tick<=80&&now>=equipment.tick&&now-equipment.tick<=100&&
           equipment.heldIndex[1]>=0&&equipment.heldIndex[1]<equipment.count&&frame.output.active[1]&&
           equipment.items[equipment.heldIndex[1]].entity==identity.entity&&equipment.items[equipment.heldIndex[1]].held&&
           equipment.items[equipment.heldIndex[1]].profile.key==frame.profiles[1].key&&
           equipment.items[equipment.heldIndex[1]].profile.generation==frame.profiles[1].generation&&
           ReadModelWorld6692(ci,nativeWorld)&&frame.world[7]>.1f&&frame.world[7]<10.f){
            const auto expected=ph::compose(PhysWorld6698(frame.world),ph::Pose{frame.output.weapons[1].q,frame.output.weapons[1].p*frame.world[7]});
            const ph::Pose actual{PQ(nativeWorld.q),PV(nativeWorld.t)};
            const double positionError=pm::length(expected.p-actual.p),angleError=pm::angle(expected.q,actual.q);
            const double scale=nativeWorld.scale/frame.world[7];
            BowGeometry166 geometry{};geometry.nock=PV(FromT(r.restModel[r.namedString166]))*scale;
            std::uintptr_t poseType59=0;
            const bool sameLifetime59=placement59.identity.key==frame.profiles[1].key&&
                placement59.identity.generation==frame.profiles[1].generation&&
                placement59.identity.request==frame.request&&placement59.identity.epoch==frame.input.bodyEpoch&&
                placement59.identity.entity==identity.entity&&Read(pose,poseType59)&&
                poseType59==placement59.identity.poseType&&std::abs(placement59.identity.scale-scale)<=1e-5;
            const bool measured=r.analysed&&r.namedString166==r.stringJoint;
            const bool structural=ph::structural_bow_axis175(frame.profiles[1],geometry.nock,geometry.axis);
            const bool partial48=!measured&&r.partialReady48;
            const bool geometryReady=measured||partial48||structural;
            if(positionError<.03&&angleError<.08&&scale>.1&&scale<10&&geometryReady&&sameLifetime59){
                static_cast<bowgeom59::Identity&>(geometry)=placement59.identity;
                geometry.tick=now;geometry.lifetime=placement59.lifetime;
                geometry.restStructural175=!measured;
                if(measured){geometry.axis=PV(r.axis);geometry.drawLength=r.drawLen*scale;}
                else if(partial48){geometry.axis=PV(r.partialAxis48);geometry.partialAxis48=true;}
                g_bow_geometry166.publish(geometry);
            }
            static std::uint64_t next166=0;if(now>=next166){next166=now+1000;char line[360]{};
                std::snprintf(line,sizeof(line),"V66.175 BOW_GEOMETRY key=%016llX stringJoint=%d arrowJoint=%d nockLocal=(%.3f,%.3f,%.3f) scale=%.3f worldErrorM=%.4f angleRad=%.4f admitted=%d source=%s",
                    (unsigned long long)frame.profiles[1].key,r.namedString166,r.namedArrow166,geometry.nock.x,geometry.nock.y,geometry.nock.z,scale,positionError,angleError,
                    positionError<.03&&angleError<.08&&scale>.1&&scale<10&&geometryReady&&sameLifetime59,measured?"MEASURED_FULL_DRAW":partial48?"MEASURED_PARTIAL_AXIS":"NAMED_REST_STRING");diagnostic_trace::Write(line);}
        }
    }
    if(!drawing){r.drawReceipt48=false;return;}
    if(!g_bow_string_follow128.load(std::memory_order_acquire)||g_bow_fault128.load())return;
    // Read draw and wrists as ONE completed publication. A concurrently newer
    // physical mailbox cannot withdraw an otherwise current visual draw.
    CommittedFrame6695 body36c1{};BowCharacterIdentity166 identity36c1{};
    PhysEquipment6698 equipment36c1{};ModelWorld6692 bowWorld36c1{};
    if(!ReadCommittedPacket6695(body36c1)||!body36c1.valid[0]||!body36c1.valid[1]||
       !TrackingHandUsable111(0)||!TrackingHandUsable111(1))return;
    const auto& drawR93=body36c1.bowDrawR93;
    const bool current36c1=kcd2vr::bow_draw_publication_r93::current(drawR93,body36c1.stamp,
            g_weapon_request_epoch6694.load(std::memory_order_acquire),now)&&
        g_bow_identity166.read(identity36c1)&&identity36c1.character==ci&&identity36c1.tick&&
        now>=identity36c1.tick&&now-identity36c1.tick<=80&&
        g_phys_equipment6698.read(equipment36c1)&&equipment36c1.valid&&
        equipment36c1.request==drawR93.request&&equipment36c1.epoch==body36c1.stamp.epoch&&
        now>=equipment36c1.tick&&now-equipment36c1.tick<=100&&
        equipment36c1.heldIndex[1]>=0&&equipment36c1.heldIndex[1]<equipment36c1.count&&
        equipment36c1.items[equipment36c1.heldIndex[1]].held&&
        equipment36c1.items[equipment36c1.heldIndex[1]].entity==identity36c1.entity&&
        equipment36c1.items[equipment36c1.heldIndex[1]].profile.key==drawR93.key&&
        equipment36c1.items[equipment36c1.heldIndex[1]].profile.generation==drawR93.generation&&
        ReadModelWorld6692(ci,bowWorld36c1);
    if(!current36c1)return;
    const ph::Pose actualBow36c1{PQ(bowWorld36c1.q),PV(bowWorld36c1.t)};
    const auto expected36c1=ph::compose(PhysWorld6698(body36c1.world),
        ph::Pose{drawR93.weaponRotation,drawR93.weaponPosition*body36c1.world[7]});
    if(pm::length(expected36c1.p-actualBow36c1.p)>=.03||pm::angle(expected36c1.q,actualBow36c1.q)>=.08)return;
    ph::Pose handLocal36c1{};
    if(!kcd2vr::bow_grip178r36c1::hand_in_bow(
        {PQ(QFromQuatT(body36c1.allowed[0])),PV(FromT(body36c1.allowed[0]))},
        PhysWorld6698(body36c1.world),body36c1.world[7],actualBow36c1,bowWorld36c1.scale,handLocal36c1))return;
    const V3 h=VP(handLocal36c1.p);
    const V3 v=Sub(h,r.stringRest);const float along=Dot(v,r.axis);const float lateral=Len(Sub(v,Mul(r.axis,along)));
    const bool physical=drawR93.physical;
    if(physical&&!drawR93.held){r.drawReceipt48=false;return;}
    // R44C2: ONE authored draw fraction owns limbs, both string ends, centre
    // and the nocked arrow. The R9 lateral glue plus R36 wrist translation moved
    // only the centre/arrow subtree, leaving the other string joints behind.
    // The wrist includes its acquisition offset; it is not the string's nock.
    // Right Grip still rotates the whole bow in physical_weapon.h. Until a
    // measured pair exists, native charge animation supplies the whole rig.
    if(!r.analysed&&!physical)return;
    const float fraction=physical?float(drawR93.fraction):
        std::clamp((along-std::clamp(g_bow_finger_bias128.load(),0.f,.15f))/r.drawLen,0.f,1.f);
    if(!std::isfinite(fraction)||fraction<0||fraction>1||(!physical&&lateral>.25f))return;
    std::array<QuatT28,kBowJoints128> local{},model{};
    std::array<bool,kBowJoints128> writeMask36c1{};
    const bool partialPreview48=!r.analysed;
    if(partialPreview48){
        if(!BowDrawPartial48(r,cur,double(fraction)*drawR93.drawLength,local,model,writeMask36c1))return;
    }else if(!BowDrawHierarchy44c2(r,cur,fraction,local,model,writeMask36c1))return;
    PoseSetFn setter=nullptr;if(!ValidateBuffer1Setter(pose,setter))return;
    bool ok=true;for(std::uint32_t j=0;j<r.count&&ok;++j)if(writeMask36c1[j])ok=WriteQuat(setter,pose,int(j),model[j]);
    if(!ok){
        for(std::uint32_t j=0;j<r.count;++j)if(writeMask36c1[j])(void)WriteQuat(setter,pose,int(j),cur[j]);
        g_bow_fault128.store(true);diagnostic_trace::Write("V66.128-BOW_STRING FAULT write failed; native bow pose restored; string follow disabled for this session");return;}
    for(std::uint32_t j=0;j<r.count;++j)r.lastWritten[j]=writeMask36c1[j]?model[j]:cur[j];
    r.grip36c1={}; // no independent nock-to-wrist lease
    r.wroteLast=true;++r.writes;
    if(r.namedString166>=0&&r.namedString166<int(r.count)&&
       (!r.drawReceipt48||r.receiptRank48!=drawR93.geometryRank)&&
       diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now)){
        r.drawReceipt48=true;r.receiptRank48=drawR93.geometryRank;
        const V3 string=FromT(model[r.namedString166]);char line[420]{};
        std::snprintf(line,sizeof(line),"V66.128-BOW_STRING C48_ALIGNMENT sample=%llu key=%016llX geometryRank=%u firstPreview=%d visualFraction=%.3f nativeCharge=%.3f wristBow=(%.3f,%.3f,%.3f) stringBow=(%.3f,%.3f,%.3f) wristStringGapM=%.4f wristNotFingerTip=1 cameraWrite=NONE",
            (unsigned long long)body36c1.stamp.sample,(unsigned long long)drawR93.key,drawR93.geometryRank,int(partialPreview48),fraction,double(charge),h.x,h.y,h.z,string.x,string.y,string.z,Len(Sub(h,string)));
        diagnostic_trace::Write(line);
    }

    if(now-r.lastLog>=1000){r.lastLog=now;char m[384]{};
        std::snprintf(m,sizeof(m),"V66.128-BOW_STRING FOLLOW visualFraction=%.3f nativeCharge=%.3f alongM=%.3f lateralM=%.3f drawLenM=%.3f fingerBias=%.3f writes=%llu learnRest/Full=%llu/%llu nativeChargeWritten=NO poseOwner=AUTHORED_DRAW_CHAIN rightGrip=%d arrowJoint=%d firstDrawPreview=%d",
            fraction,double(charge),along,lateral,r.drawLen,double(g_bow_finger_bias128.load()),(unsigned long long)r.writes,(unsigned long long)r.learnRest,(unsigned long long)r.learnFull,int(drawR93.rightGrip>=.25),r.namedArrow166,int(partialPreview48));
        diagnostic_trace::Write(m);}
}
