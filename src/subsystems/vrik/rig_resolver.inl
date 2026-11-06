bool ResolveNamedGripCandidatesV6545(void* owner,std::uint32_t count,const RigMap& rig,bool forceLog=false) noexcept {
    if(!owner||count==0) return false;
    // Name topology is invariant for one validated CDefaultSkeleton. Do the guarded
    // retail lookups once per owner, then reuse the IDs. This keeps F10/F11 out of
    // the skinning hot path after the first successful (or failed) probe.
    const void* cachedOwner=g_named_probe_owner.load(std::memory_order_acquire);
    const int cachedAttempts=g_named_api_attempts.load(std::memory_order_acquire);
    if(!forceLog&&cachedOwner==owner&&cachedAttempts>0)
        return g_named_joint_api_valid.load(std::memory_order_acquire);

    g_named_api_attempts.fetch_add(1,std::memory_order_acq_rel);
    int hd=-1,hr=-1,hl=-1,s0=-1,s1=-1,wr=-1,wl=-1,rhs=-1,lhs=-1;
    if(!ResolveJointByNameExactV6545(owner,"head",hd)||hd!=rig.head) {
        g_named_joint_api_valid.store(false,std::memory_order_release);
        void* old=g_named_probe_owner.exchange(owner,std::memory_order_acq_rel);
        if(forceLog||old!=owner){
            char m[384]{};std::snprintf(m,sizeof(m),
                "V65.46-NAME-GRIP fail-closed owner=%p headLookup=%d semanticHead=%d exactRva=0x78BC4C",
                owner,hd,rig.head);diagnostic_trace::Write(m);
        }
        return false;
    }
    (void)ResolveJointByNameExactV6545(owner,"hand_right",hr);
    (void)ResolveJointByNameExactV6545(owner,"hand_left",hl);
    (void)ResolveJointByNameExactV6545(owner,"slt_0",s0);
    (void)ResolveJointByNameExactV6545(owner,"slt_1",s1);
    (void)ResolveJointByNameExactV6545(owner,"RightWeaponSlot",wr);
    (void)ResolveJointByNameExactV6545(owner,"LeftWeaponSlot",wl);
    (void)ResolveJointByNameExactV6545(owner,"RightHandSlot",rhs);
    (void)ResolveJointByNameExactV6545(owner,"LeftHandSlot",lhs);
    auto valid=[&](int id){return id>=0&&static_cast<std::uint32_t>(id)<count;};
    if(!valid(hr))hr=-1;if(!valid(hl))hl=-1;if(!valid(s0))s0=-1;if(!valid(s1))s1=-1;if(!valid(wr))wr=-1;if(!valid(wl))wl=-1;if(!valid(rhs))rhs=-1;if(!valid(lhs))lhs=-1;
    g_named_joint_api_valid.store(true,std::memory_order_release);
    g_named_hand_r.store(hr);g_named_hand_l.store(hl);g_slt0_joint.store(s0);g_slt1_joint.store(s1);
    g_weapon_slot_r.store(wr);g_weapon_slot_l.store(wl);
    int p0=-2,p1=-2,pr=-2,pl=-2;
    if(s0>=0)(void)ReadParentDirect(owner,s0,p0); if(s1>=0)(void)ReadParentDirect(owner,s1,p1);
    if(wr>=0)(void)ReadParentDirect(owner,wr,pr); if(wl>=0)(void)ReadParentDirect(owner,wl,pl);
    g_weapon_slot_parent_r.store(pr);g_weapon_slot_parent_l.store(pl);
    void* old=g_named_probe_owner.exchange(owner,std::memory_order_acq_rel);
    if(forceLog||old!=owner){
        char m[704]{};std::snprintf(m,sizeof(m),
            "V65.80-NAME-GRIP owner=%p head=%d/%d hand_right=%d semanticR=%d hand_left=%d semanticL=%d slt_0=%d(p=%d) slt_1=%d(p=%d) RightWeaponSlot=%d(p=%d) LeftWeaponSlot=%d(p=%d) RightHandSlot=%d LeftHandSlot=%d exactRva=0x78BC4C cached=1",
            owner,hd,rig.head,hr,rig.hand_r,hl,rig.hand_l,s0,p0,s1,p1,wr,pr,wl,pl,rhs,lhs);diagnostic_trace::Write(m);
    }
    return true;
}

bool ReadSemanticJoint(void* owner,int boneId,std::uint32_t count,int& out) noexcept {
    out=-1; if(!owner||boneId<0||boneId>64) return false; std::uint16_t v=0xFFFF;
    const auto* p=static_cast<unsigned char*>(owner)+kSemanticJointTableOffset+static_cast<std::ptrdiff_t>(boneId*2);
    if(!Read(p,v)||v==0xFFFFu||v>=count) return false; out=static_cast<int>(v); return true;
}
bool ResolveRig(void* owner,std::uint32_t count,RigMap& r) noexcept {
    if(!ReadSemanticJoint(owner,BONE_PELVIS,count,r.pelvis)||!ReadSemanticJoint(owner,BONE_SPINE,count,r.spine)||
       !ReadSemanticJoint(owner,BONE_SPINE1,count,r.spine1)||!ReadSemanticJoint(owner,BONE_SPINE2,count,r.spine2)||
       !ReadSemanticJoint(owner,BONE_SPINE3,count,r.spine3)||!ReadSemanticJoint(owner,BONE_NECK,count,r.neck)||!ReadSemanticJoint(owner,BONE_HEAD,count,r.head)||
       !ReadSemanticJoint(owner,BONE_CLAVICLE_L,count,r.clav_l)||!ReadSemanticJoint(owner,BONE_UPPERARM_L,count,r.upper_l)||
       !ReadSemanticJoint(owner,BONE_FOREARM_L,count,r.fore_l)||!ReadSemanticJoint(owner,BONE_ELBOW_L,count,r.elbow_l)||!ReadSemanticJoint(owner,BONE_HAND_L,count,r.hand_l)||
       !ReadSemanticJoint(owner,BONE_CLAVICLE_R,count,r.clav_r)||!ReadSemanticJoint(owner,BONE_UPPERARM_R,count,r.upper_r)||
       !ReadSemanticJoint(owner,BONE_FOREARM_R,count,r.fore_r)||!ReadSemanticJoint(owner,BONE_ELBOW_R,count,r.elbow_r)||!ReadSemanticJoint(owner,BONE_HAND_R,count,r.hand_r)) return false;
    const int v[]{r.pelvis,r.spine,r.spine1,r.spine2,r.spine3,r.neck,r.head,r.clav_l,r.upper_l,r.fore_l,r.elbow_l,r.hand_l,r.clav_r,r.upper_r,r.fore_r,r.elbow_r,r.hand_r};
    for(std::size_t i=0;i<17;++i){ if(v[i]<0||static_cast<std::uint32_t>(v[i])>=count) return false; for(std::size_t j=0;j<i;++j) if(v[i]==v[j]) return false; }
    // Diagnostic extension only: failure to resolve ITEM_L/R must never invalidate the proven arm rig.
    (void)ReadSemanticJoint(owner,BONE_ITEM_L,count,r.item_l);
    (void)ReadSemanticJoint(owner,BONE_ITEM_R,count,r.item_r);
    return true;
}
bool ReadParentDirect(void* owner,int joint,int& parent) noexcept {
    parent=-2; if(!owner||joint<0) return false; void* table=nullptr;
    if(!Read(static_cast<unsigned char*>(owner)+0x08,table)||!table) return false;
    std::int16_t p=-2;
    if(!Read(static_cast<unsigned char*>(table)+0x16+static_cast<std::ptrdiff_t>(joint)*0x30,p)) return false;
    parent=static_cast<int>(p); return parent>=-1;
}
bool ValidateRawPoseApi(void* pose,PoseGetFn& b0,PoseGetFn& b1) noexcept {
    b0=nullptr;b1=nullptr; if(!pose) return false; std::uintptr_t vt=0,f0=0,f1=0;
    if(!Read(pose,vt)||!vt||!Read(reinterpret_cast<void*>(vt+0x10),f0)||!Read(reinterpret_cast<void*>(vt+0x50),f1)) return false;
    const auto base=WhBase(); if(!SupportedWhBuild(base)||f0!=base+kPoseGetterBuffer0Rva||f1!=base+kPoseGetterBuffer1Rva) return false;
    b0=reinterpret_cast<PoseGetFn>(f0); b1=reinterpret_cast<PoseGetFn>(f1); return true;
}
// Retired unused implementation archived: research_archive/retired_pose_implementations

bool ValidateBuffer1Setter(void* pose,PoseSetFn& out) noexcept {
    out=nullptr; if(!pose) return false; std::uintptr_t vt=0,fn=0;
    if(!Read(pose,vt)||!vt||!Read(reinterpret_cast<void*>(vt+0x70),fn)||!fn) return false;
    const auto base=WhBase(); if(!SupportedWhBuild(base)||fn!=base+kPoseSetterBuffer1Rva) return false;
    out=reinterpret_cast<PoseSetFn>(fn); return true;
}
bool ReadQuat(PoseGetFn fn,void* pose,int joint,QuatT28& out) noexcept {
    const QuatT28* p=nullptr; __try { p=fn(pose,joint); if(p&&Safe(p,sizeof(out))) out=*p; } __except(EXCEPTION_EXECUTE_HANDLER){p=nullptr;}
    if(!p) return false; for(float v:out.q) if(!std::isfinite(v)) return false; for(float v:out.t) if(!std::isfinite(v)) return false; return true;
}
bool WriteQuat(PoseSetFn fn,void* pose,int joint,const QuatT28& in) noexcept {
    if(!fn||!pose||joint<0) return false;
    __try { fn(pose,joint,&in); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool ReadCompanion(void* owner,int joint,QuatT28& out) noexcept {
    const auto base=WhBase(); if(!SupportedWhBuild(base)||!owner||joint<0) return false;
    auto sel=reinterpret_cast<PoseBufferSelectorFn>(base+kPoseBufferSelectorRva); void* arr=nullptr;
    __try { arr=sel(static_cast<unsigned char*>(owner)+0x20,1); } __except(EXCEPTION_EXECUTE_HANDLER){arr=nullptr;}
    if(!arr) return false; return Read(static_cast<unsigned char*>(arr)+static_cast<std::ptrdiff_t>(joint)*0x1C,out);
}
Q4 ReadQ(const float* v,bool wxyz) noexcept { return wxyz?Q4{v[1],v[2],v[3],v[0]}:Q4{v[0],v[1],v[2],v[3]}; }
V3 DecodeDqTranslation(const Dq32& d,bool wxyz,bool conjFirst) noexcept {
    Q4 qr=ReadQ(d.a,wxyz), qd=ReadQ(d.b,wxyz); const float n=qr.x*qr.x+qr.y*qr.y+qr.z*qr.z+qr.w*qr.w;
    if(!std::isfinite(n)||n<1.0e-8f) return {};
    const Q4 x=conjFirst?QMul(QConj(qr),qd):QMul(qd,QConj(qr));
    const float k=2.0f/n; return {x.x*k,x.y*k,x.z*k};
}

// V65.63 F12 final-owner bridge. KCD2's skinning output is a dual quaternion
// array. For the retail pose path used here the real/dual quaternions are stored
// xyzw, matching QuatT28.  Left-multiplying a rigid delta at the finished skinning
// boundary moves the ITEM_L/ITEM_R attachment/socket branch without guessing an
// IEntity transform ABI. This is deliberately side/socket based, never item-name based.
// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations

