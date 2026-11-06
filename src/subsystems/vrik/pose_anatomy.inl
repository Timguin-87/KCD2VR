static bool ReadNativeBuffer6689(void* pose,PoseGetFn get,std::uint32_t count,QuatT28* out) noexcept {
    if(!pose||!get||!out||count>kPoseCapacity6689)return false;
    const QuatT28* p=nullptr;
    __try { p=get(pose,0); if(!p||!Safe(p,count*sizeof(QuatT28)))return false;
        std::memcpy(out,p,count*sizeof(QuatT28)); }
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return true;
}
static bool Compose6689(CoherentState6689& s) noexcept {
#ifdef KCD2VR_POSE_COST_D60
    KCD2VR_POSE_COST_D60(ComposeDirty);
#endif
    // Parent-first dirty propagation. V66.137 may defer descendants that are
    // never read again during the solve; essential137 is ancestor-closed, so an
    // essential joint can never depend on a deferred parent.
    for(std::uint32_t k=0;k<s.count;++k) {
        const int j=s.order[k],p=s.parent[j];
        const bool changed=s.dirtyAll||s.dirty[j]||(p>=0&&s.dirty[p]);
        s.dirty[j]=changed;
        if(!changed)continue;
        if(s.deferPassengers137&&!s.essential137[j]){s.pending137[j]=true;continue;}
        s.work[j]=p>=0?AbsoluteFromRelative(s.work[p],s.local[j]):s.local[j];
        ++s.composedJoints;if(!ValidPose6689(s.work[j]))return false;
    }
    s.dirty.fill(false);s.dirtyAll=false;return true;
}
static bool ComposeFull137(CoherentState6689& s) noexcept {
#ifdef KCD2VR_POSE_COST_D60
    KCD2VR_POSE_COST_D60(ComposeFull);
#endif
    // Settle every deferred passenger from the final parent transforms. A local
    // 'refreshed' chain guarantees descendants of a pending helper are updated
    // even if they were not individually marked in the last partial pass.
    std::array<bool,kPoseCapacity6689> refreshed{};
    for(std::uint32_t k=0;k<s.count;++k){
        const int j=s.order[k],p=s.parent[j];
        const bool changed=s.dirtyAll||s.dirty[j]||s.pending137[j]||(p>=0&&refreshed[p]);
        refreshed[j]=changed;if(!changed)continue;
        s.work[j]=p>=0?AbsoluteFromRelative(s.work[p],s.local[j]):s.local[j];
        ++s.composedJoints;if(!ValidPose6689(s.work[j]))return false;
    }
    s.pending137.fill(false);s.dirty.fill(false);s.dirtyAll=false;s.deferPassengers137=false;
    return true;
}
static bool Descendant6689(const CoherentState6689& s,int id,int ancestor) noexcept {
    for(std::uint32_t d=0;d<s.count&&id>=0;++d) {
        if(id==ancestor)return true; id=s.parent[id];
    }
    return false;
}
// V66.128 (a): find thigh/knee/ankle per side from the validated rest skeleton.
// Pelvis descendants outside the spine subtree, split by the rest body-right axis.
// Ankle = farthest-from-hip chain joint that is not a toe (>=4 cm above the lowest
// joint); knee = chain joint between hip and ankle maximizing the shorter segment.
// Fails closed (follow simply stays off) when the rig does not look like two legs.
static void DiscoverLegs128(CoherentState6689& s,const RigMap& rig,V3 bodyUp,V3 bodyRight) noexcept {
    s.legsReady128=false;s.leg[0]={};s.leg[1]={};s.pelvisTree128.fill(false);
    for(std::uint32_t j=0;j<s.count;++j)s.pelvisTree128[j]=Descendant6689(s,int(j),rig.pelvis);
    const V3 pelvis=FromT(s.rest[rig.pelvis]);
    for(int side=0;side<2;++side){
        int lowest=-1;float lowestUp=1e9f;
        for(std::uint32_t j=0;j<s.count;++j){
            if(!s.pelvisTree128[j]||s.torso[j]||int(j)==rig.pelvis)continue;
            const V3 rel=Sub(FromT(s.rest[j]),pelvis);
            if(Dot(rel,bodyRight)*(side?-1.f:1.f)<.03f)continue;
            const float up=Dot(rel,bodyUp);if(up<lowestUp){lowestUp=up;lowest=int(j);}
        }
        if(lowest<0||lowestUp>-.55f)continue; // a leg reaches at least 55 cm below the pelvis
        std::array<int,24> chain{};int n=0;
        for(int j=lowest;j>=0&&j!=rig.pelvis&&n<int(chain.size());j=s.parent[j])chain[n++]=j;
        if(n<3||n>=int(chain.size()))continue;
        std::reverse(chain.begin(),chain.begin()+n); // chain[0] = first joint below the pelvis
        int hip=-1;for(int k=0;k<n;++k)if(Len(Sub(FromT(s.rest[chain[k]]),pelvis))>.04f){hip=k;break;}
        if(hip<0)continue;
        const V3 H=FromT(s.rest[chain[hip]]);int ankle=-1;float best=0;
        for(int k=hip+2;k<n;++k){
            const V3 p=FromT(s.rest[chain[k]]);
            if(Dot(Sub(p,pelvis),bodyUp)<lowestUp+.04f)continue; // toe / toe nub
            const float d=Len(Sub(p,H));if(d>best){best=d;ankle=k;}
        }
        if(ankle<0)continue;
        const V3 A=FromT(s.rest[chain[ankle]]);int knee=-1;float kneeScore=0;
        for(int k=hip+1;k<ankle;++k){const V3 p=FromT(s.rest[chain[k]]);
            const float sc=std::min(Len(Sub(p,H)),Len(Sub(A,p)));if(sc>kneeScore){kneeScore=sc;knee=k;}}
        if(knee<0)continue;
        auto& L=s.leg[side];L.hip=chain[hip];L.knee=chain[knee];L.ankle=chain[ankle];
        L.thigh=Len(Sub(FromT(s.rest[L.knee]),H));L.shin=Len(Sub(A,FromT(s.rest[L.knee])));
        L.ok=L.thigh>.25&&L.thigh<.70&&L.shin>.25&&L.shin<.70;
    }
    s.legsReady128=s.leg[0].ok&&s.leg[1].ok&&s.leg[0].hip!=s.leg[1].hip;
    if(!kcd2vr::diagnostics::vrik_h29::selected()||!kcd2vr::diagnostics::vrik_h29::active(GetTickCount64()))return;
    char m[320]{};std::snprintf(m,sizeof(m),"V66.128-LEGS %s R(hip/knee/ankle)=%d/%d/%d len=%.3f/%.3f L=%d/%d/%d len=%.3f/%.3f source=REST_TOPOLOGY",
        s.legsReady128?"READY":"UNAVAILABLE_FOLLOW_FAILS_CLOSED",s.leg[0].hip,s.leg[0].knee,s.leg[0].ankle,s.leg[0].thigh,s.leg[0].shin,
        s.leg[1].hip,s.leg[1].knee,s.leg[1].ankle,s.leg[1].thigh,s.leg[1].shin);diagnostic_trace::Write(m);
}
static void BuildEssential137(CoherentState6689& s,const RigMap& rig) noexcept {
    s.essential137.fill(false);s.essentialCount137=0;
    auto addChain=[&](int id){
        for(std::uint32_t guard=0;guard<s.count&&id>=0;++guard){
            if(id>=int(s.count))break;
            if(!s.essential137[id]){s.essential137[id]=true;++s.essentialCount137;}
            id=s.parent[id];
        }
    };
    // Everything read again before the final settle pass, plus ancestors.
    const int core[]{rig.pelvis,rig.spine,rig.spine1,rig.spine2,rig.spine3,rig.neck,rig.head,
                     rig.clav_r,rig.upper_r,rig.fore_r,rig.hand_r,
                     rig.clav_l,rig.upper_l,rig.fore_l,rig.hand_l};
    for(int id:core)if(id>=0)addChain(id);
    for(int side=0;side<2;++side){
        for(int i=0;i<s.fingerCount[side];++i)addChain(s.fingerIds[side][i]);
        if(s.leg[side].ok){addChain(s.leg[side].hip);addChain(s.leg[side].knee);addChain(s.leg[side].ankle);}
    }
    if(!kcd2vr::diagnostics::vrik_h29::selected()||!kcd2vr::diagnostics::vrik_h29::active(GetTickCount64()))return;
    char m[256]{};std::snprintf(m,sizeof(m),
        "V66.137 COMPOSE_SET essential=%u of %u joints policy=ANCESTOR_CLOSED_PASSENGER_DEFER",
        unsigned(s.essentialCount137),unsigned(s.count));diagnostic_trace::Write(m);
}
static bool InitCoherent6689(CoherentState6689& s,void* owner,void* character,void* pose,
                             PoseGetFn get,std::uint32_t count,const RigMap& rig) noexcept {
    if(s.ready&&s.owner==owner&&s.character==character&&s.count==count)return true;
    ++s.initRuns137;
    s.graspMappingGeneration55=g_grasp_mapping_generation55.fetch_add(1,std::memory_order_relaxed)+1;
    s.graspFingers54[0]={};s.graspFingers54[1]={}; // cached payload cannot cross a local skeleton/map lifetime
    s.ready=false;s.owner=owner;s.character=character;s.count=count;s.rig=rig;
    s.stowedJointsD63.fill(-1);
    for(std::size_t i=0;i<std::size(kStowedSlotsD63);++i){
        int joint=-1;
        if(ResolveJointByNameExactV6545(owner,kStowedSlotsD63[i].bone,joint)&&joint>=0&&joint<int(count))
            s.stowedJointsD63[i]=joint;
    }
    s.rightHandSlotR26=-1;
    (void)ResolveJointByNameExactV6545(owner,"RightHandSlot",s.rightHandSlotR26);
    if(s.rightHandSlotR26<0||s.rightHandSlotR26>=int(count))s.rightHandSlotR26=-1;
    s.pronation[0]={};s.pronation[1]={};s.objectCandidate[0]={};s.objectCandidate[1]={};s.dirtyAll=true;
    s.pending137.fill(false);s.essential137.fill(false);s.deferPassengers137=false;s.essentialCount137=0;
    s.elbow[0]={};s.elbow[1]={};s.chestAssist[0]={};s.chestAssist[1]={};s.clavicleAssist[0]={};s.clavicleAssist[1]={};
    s.shoulder[0]={};s.shoulder[1]={};
    // D47: every temporal arm/body lane shares this skeleton lifetime. A new
    // native owner cannot inherit shoulder or native-handover memory merely
    // because its body epoch and selected experiment happen to match.
    s.shoulderFitD45[0]={};s.shoulderFitD45[1]={};s.elbowPolicyD45[0]=s.elbowPolicyD45[1]=0;
    s.handoverW178r14[0]=s.handoverW178r14[1]=0;s.naturalTick178r14[0]=s.naturalTick178r14[1]=0;
    s.archer169r2={};s.handPairD39={};s.stationBodyH41={};s.bodyFollowR38=kcd2vr::body_follow_r38::State{};
    s.ordinaryReference54={};s.acquisitionReference54={};s.acquisitionSerial54=s.acquisitionAction54=0;
    s.kick176={}; // Never carry a procedural leg across a changed character/rig.
    s.trackedFrame51={};
    s.handSmoothH50[0]={};s.handSmoothH50[1]={};s.handFreeH50[0]=s.handFreeH50[1]=false;
    s.lastContactPass=s.contactControl=0;s.contact[0]={};s.contact[1]={};
    s.allowedReady[0]=s.allowedReady[1]=false;s.collisionEpoch=0;s.stableWindow=false;s.stableSince=0;
    s.handBlocked[0]=s.handBlocked[1]=s.armBlocked[0]=s.armBlocked[1]=s.armSaturated[0]=s.armSaturated[1]=false;
    s.lastInvariantLog=s.lastHierarchyLog=s.lastHmdInvariantLog=0;s.lastArmCollisionTick[0]=s.lastArmCollisionTick[1]=0;
    if(count<1||count>kPoseCapacity6689)return false;
    s.torso.fill(false);s.armR.fill(false);s.armL.fill(false);s.isFinger.fill(false);s.trunk.fill(false);
    std::array<unsigned short,kPoseCapacity6689> depth{};
    for(std::uint32_t j=0;j<count;++j) {
        if(!ReadParentDirect(owner,int(j),s.parent[j])||s.parent[j]>=int(count)||s.parent[j]==int(j)||
           !ReadCompanion(owner,int(j),s.rest[j])||!ValidPose6689(s.rest[j]))return false;
        s.order[j]=int(j);
    }
    for(std::uint32_t j=0;j<count;++j) {
        int p=s.parent[j];std::uint32_t n=0;
        while(p>=0&&n<count){p=s.parent[p];++n;}
        if(n>=count)return false;depth[j]=static_cast<unsigned short>(n);
        s.restLocal[j]=s.parent[j]>=0?RelativeFromAbsolute(s.rest[s.parent[j]],s.rest[j]):s.rest[j];
        s.torso[j]=Descendant6689(s,int(j),rig.spine);
        s.armR[j]=Descendant6689(s,int(j),rig.upper_r);
        s.armL[j]=Descendant6689(s,int(j),rig.upper_l);
    }
    std::sort(s.order.begin(),s.order.begin()+count,[&](int a,int b){return depth[a]<depth[b]||(depth[a]==depth[b]&&a<b);});
    if(!s.torso[rig.head]||!s.torso[rig.upper_r]||!s.torso[rig.upper_l])return false;
    // Derive body axes from neutral anatomy. Chest BONE-local axes are not
    // assumed to be Cry world X/Y/Z; preserve this through chest reconstruction.
    const V3 bodyUp=Normalize(Sub(FromT(s.rest[rig.spine3]),FromT(s.rest[rig.pelvis])));
    const V3 bodyRight=Normalize(Sub(FromT(s.rest[rig.upper_r]),FromT(s.rest[rig.upper_l])));
    const V3 bodyForward=Normalize(Cross(bodyUp,bodyRight));
    if(Len(bodyUp)<.5f||Len(bodyRight)<.5f||Len(bodyForward)<.5f)return false;
    s.chestRightLocal=QRotateV(QConj(QFromQuatT(s.rest[rig.spine3])),bodyRight);
    s.chestUpLocal=QRotateV(QConj(QFromQuatT(s.rest[rig.spine3])),bodyUp);
    for(int side=0;side<2;++side) {
        const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
        if(s.parent[f]!=u||s.parent[h]!=f)return false;
        s.upper[side]=Len(FromT(s.restLocal[f]));s.fore[side]=Len(FromT(s.restLocal[h]));
        if(s.upper[side]<0.15||s.upper[side]>0.5||s.fore[side]<0.15||s.fore[side]>0.5)return false;
        V3 axis=Normalize(Sub(FromT(s.rest[h]),FromT(s.rest[u])));
        V3 bend=Sub(Sub(FromT(s.rest[f]),FromT(s.rest[u])),Mul(axis,Dot(Sub(FromT(s.rest[f]),FromT(s.rest[u])),axis)));
        // W1: relaxed-arm pole, elbow mostly back (was .35 out/.25 back -> sideways elbow on a hanging arm).
        const float outW1=kcd2vr::elbow_pole_w1::clampOut(kcd2vr::elbow_pole_w1::outward.load(std::memory_order_relaxed));
        const float backW1=kcd2vr::elbow_pole_w1::clampBack(kcd2vr::elbow_pole_w1::backward.load(std::memory_order_relaxed));
        const V3 preference=Normalize(Add(Add(Mul(bodyRight,side?-outW1:outW1),Mul(bodyForward,-backW1)),Mul(bodyUp,-1.f)));
        // Only neutral reference anatomy seeds the plane. Near-straight bind
        // anatomy uses body-derived outward/down, never live attack frames.
        if(Len(bend)<0.005f)bend=preference;
        const V3 signedRestBend=bend;
        if(Dot(bend,preference)<0)bend=Mul(bend,-1.f);
        const Q4 inverseChest=QConj(QFromQuatT(s.rest[rig.spine3]));
        s.poleLocal[side]=QRotateV(inverseChest,Normalize(bend));
        s.sideDownLocal[side]=QRotateV(inverseChest,preference);
        const V3 plane=Normalize(Cross(axis,Normalize(signedRestBend)));
        if(Len(plane)<.5f)return false;
        s.hingeNormalLocal[side]=QRotateV(QConj(QFromQuatT(s.rest[u])),plane);
        s.objectFingerKey[side]=s.objectFingerGeneration[side]=0;
        s.lastBend[side]=Normalize(bend);s.open[side]={};s.closed[side]={};s.nativeCurveClosedH7[side]={};s.fingerTrialRecipeH7[side]=false;s.fingerCount[side]=0;s.anatomyHand[side]=false;
        s.fingerMap[side]=GetCachedFingerChainsV6546(pose,get,owner,count,rig,side!=0,s.fingers[side]);
        if(s.fingerMap[side]) {
            for(const auto& chain:s.fingers[side])for(int k=0;k<chain.count;++k) {
                if(s.fingerCount[side]>=32)return false;
                s.fingerIds[side][s.fingerCount[side]++]=chain.id[k];s.isFinger[chain.id[k]]=true;
            }
            const auto& chains=s.fingers[side];V3 center{};
            for(int k=1;k<5;++k)center=Add(center,FromT(s.rest[chains[k].id[0]]));center=Mul(center,0.25f);
            const V3 distal=Normalize(Sub(center,FromT(s.rest[h])));
            const V3 indexSide=Normalize(Sub(FromT(s.rest[chains[1].id[0]]),FromT(s.rest[chains[4].id[0]])));
            const V3 normal=Normalize(Cross(distal,indexSide)),lateral=Normalize(Cross(normal,distal));
            if(Len(normal)>.5f&&Len(distal)>.5f&&Len(lateral)>.5f) {
                s.boneToAnatomy[side]=QMul(QConj(QFromQuatT(s.rest[h])),QFromBasis(distal,lateral,normal));s.anatomyHand[side]=true;
            }
        }
        // R90 (R89 elbow option): the displayed hand's own axes in hand-bone space (rest pose). wrist->fingers continues the
        // forearm; the thumb root gives the thumb side. no finger map = no thumb part, the other parts still work.
        {const Q4 invHandR89=QConj(QFromQuatT(s.rest[h]));
         const V3 restForeR89=Sub(FromT(s.rest[h]),FromT(s.rest[f]));
         s.handForwardLocalR89[side]=Len(restForeR89)>.01f?QRotateV(invHandR89,Normalize(restForeR89)):V3{};
         s.thumbLocalR89[side]=V3{};
         if(s.fingerMap[side]&&Len(s.handForwardLocalR89[side])>.5f){
            const V3 t=QRotateV(invHandR89,Sub(FromT(s.rest[s.fingers[side][0].id[0]]),FromT(s.rest[h])));
            const V3 fw=s.handForwardLocalR89[side];const V3 p=Sub(t,Mul(fw,Dot(t,fw)));
            if(Len(p)>.002f)s.thumbLocalR89[side]=Normalize(p);}}
        // W1: the controller->hand conversion belongs to Henry's hand bone, not to one joint numbering. when this
        // skeleton's fingers can't be mapped, reuse the last good conversion if the hand bone sits the same way on
        // its forearm (within 10 deg); never fall back silently to the raw controller rotation.
        {static bool lastGoodW1[2]{};static Q4 lastConvW1[2]{},lastHandLocalW1[2]{};
         const Q4 handLocalW1=QFromQuatT(s.restLocal[h]);
         if(s.anatomyHand[side]){lastGoodW1[side]=true;lastConvW1[side]=s.boneToAnatomy[side];lastHandLocalW1[side]=handLocalW1;}
         else if(lastGoodW1[side]){
            const float d=std::fabs(handLocalW1.x*lastHandLocalW1[side].x+handLocalW1.y*lastHandLocalW1[side].y+handLocalW1.z*lastHandLocalW1[side].z+handLocalW1.w*lastHandLocalW1[side].w);
            const bool same=d>0.9962f; // cos(10 deg / 2)
            if(same){s.boneToAnatomy[side]=lastConvW1[side];s.anatomyHand[side]=true;}
            if(kcd2vr::diagnostics::vrik_h29::selected()&&kcd2vr::diagnostics::vrik_h29::active(GetTickCount64())){
            char m[256]{};std::snprintf(m,sizeof(m),"W1 HAND_ANATOMY side=%c fingers=%d conversion=%s handBoneMatch=%.4f",side?'L':'R',s.fingerMap[side]?1:0,same?"LAST_GOOD_SAME_HAND_BONE":"RAW_CONTROLLER",double(d));diagnostic_trace::Write(m);}
         }
         if(!s.anatomyHand[side])(void)kcd2vr::visible_failure::publish("VRIK: hand bone not mapped - wrist follows the controller raw",static_cast<std::uint64_t>(GetTickCount64()));}
    }
    for(int k=rig.head;k>=0;k=s.parent[k]){s.trunk[k]=true;if(k==rig.pelvis)break;}
    BuildStowedReferenceR112(s,rig);
    DiscoverLegs128(s,rig,bodyUp,bodyRight);s.bodyFollowR38=kcd2vr::body_follow_r38::State{};
    BuildEssential137(s,rig);
    s.ready=true;g_body_cal_ready.store(true,std::memory_order_release);
    if(!kcd2vr::diagnostics::vrik_h29::selected()||!kcd2vr::diagnostics::vrik_h29::active(GetTickCount64()))return true;
    char m[576]{};std::snprintf(m,sizeof(m),"V66.89-ANATOMY READY character=%p joints=%u fixedArmsR=%.4f/%.4f fixedArmsL=%.4f/%.4f fingersR/L=%d/%d source=VALIDATED-COMPANION modelSpace=1 nativeLegs=1 initRuns137=%llu",character,count,s.upper[0],s.fore[0],s.upper[1],s.fore[1],s.fingerCount[0],s.fingerCount[1],(unsigned long long)s.initRuns137);diagnostic_trace::Write(m);
    // One compact sibling map per validated Henry skeleton. This is deliberately
    // not a whole-rig dump: it records only direct children at ownership boundaries.
    auto emitChildren=[&](const char* label,int parentId){
        char ids[176]{};std::size_t pos=0;int nchild=0;
        for(std::uint32_t j=0;j<count;++j)if(s.parent[j]==parentId){++nchild;if(pos+16<sizeof(ids))pos+=std::snprintf(ids+pos,sizeof(ids)-pos,"%s%d",pos?",":"",int(j));}
        char h[384]{};std::snprintf(h,sizeof(h),"V66.92-HIERARCHY MAP parent=%s:%d children=%d ids=[%s] torsoOwnedParent=%d",label,parentId,nchild,ids,s.torso[parentId]?1:0);diagnostic_trace::Write(h);
    };
    int torsoCount=0;for(std::uint32_t j=0;j<count;++j)if(s.torso[j])++torsoCount;
    {char h[320]{};std::snprintf(h,sizeof(h),"V66.92-HIERARCHY SUMMARY joints=%u torsoDescendants=%d rootSpine=%d head=%d neck=%d clavR/L=%d/%d upperR/L=%d/%d",count,torsoCount,rig.spine,rig.head,rig.neck,rig.clav_r,rig.clav_l,rig.upper_r,rig.upper_l);diagnostic_trace::Write(h);}
    emitChildren("HEAD",rig.head);emitChildren("NECK",rig.neck);emitChildren("CLAV_R",rig.clav_r);emitChildren("CLAV_L",rig.clav_l);emitChildren("UPPER_R",rig.upper_r);emitChildren("UPPER_L",rig.upper_l);
    return true;
}
