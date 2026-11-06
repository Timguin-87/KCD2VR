// Retired unused implementation archived: research_archive/retired_pose_implementations

static void InitFocusedRelativeBaselineLocked() noexcept {
    if(g_joint_record_count<=420) return;
    for(int e=0;e<kFocusEdgeCount;++e){
        const int c=g_focus_child[e],p=g_focus_parent[e];
        if(c<0||p<0||static_cast<std::uint32_t>(c)>=g_joint_record_count||static_cast<std::uint32_t>(p)>=g_joint_record_count) return;
        g_focus_base_dist[e]=Len(Sub(FromT(g_joint_base[c]),FromT(g_joint_base[p])));
        g_focus_base_ang[e]=QuatAngleDeg(g_joint_base[c],g_joint_base[p]);
    }
    g_focus_baseline_valid=true;
}
static void UpdateFocusedRelativeEvidenceLocked() noexcept {
    if(!g_focus_baseline_valid) return;
    for(int e=0;e<kFocusEdgeCount;++e){
        const int c=g_focus_child[e],p=g_focus_parent[e];
        const float d=Len(Sub(FromT(g_joint_prev[c]),FromT(g_joint_prev[p])));
        const float a=QuatAngleDeg(g_joint_prev[c],g_joint_prev[p]);
        g_focus_max_dist_dev[e]=std::max(g_focus_max_dist_dev[e],std::fabs(d-g_focus_base_dist[e]));
        g_focus_max_ang_dev[e]=std::max(g_focus_max_ang_dev[e],std::fabs(a-g_focus_base_ang[e]));
    }
}
static void LogFocusedRelativeSampleLocked(std::uint64_t sample) noexcept {
    if(!g_focus_baseline_valid) return;
    char m[768]{}; std::size_t used=0;
    int n=std::snprintf(m,sizeof(m),"V64.22-F11-RHAND sample=%llu scenario=%s",(unsigned long long)sample,F11ScenarioNameLocal(g_f11_theory_phase.load(std::memory_order_acquire)));
    if(n>0) used=static_cast<std::size_t>(n);
    for(int e=0;e<kFocusEdgeCount && used<sizeof(m)-110;++e){
        const int c=g_focus_child[e],p=g_focus_parent[e];
        const float d=Len(Sub(FromT(g_joint_prev[c]),FromT(g_joint_prev[p])));
        const float a=QuatAngleDeg(g_joint_prev[c],g_joint_prev[p]);
        const int w=std::snprintf(m+used,sizeof(m)-used," %d<-%d{d:%.4f dd:%.4f a:%.1f da:%.1f}",c,p,double(d),double(std::fabs(d-g_focus_base_dist[e])),double(a),double(std::fabs(a-g_focus_base_ang[e])));
        if(w>0) used+=static_cast<std::size_t>(w);
    }
    diagnostic_trace::Write(m);
}
// Retired unused implementation archived: research_archive/retired_pose_implementations

static void SampleJointRecorder(void* owner,void* pose,PoseGetFn b1fn,std::uint32_t count) noexcept {
    // V65.67 rung 3 is deliberately bounded: do not walk the whole skeleton.
    // The anatomy sampler reads only native hands + ITEM_L/ITEM_R at 10 Hz.
    if(g_f11_theory_phase.load(std::memory_order_acquire)==3) return;
    if(!g_joint_recording.load(std::memory_order_acquire)||!owner||!pose||!b1fn||count<8||count>kJointRecordCap)return;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());auto prevTick=g_joint_record_last_ms.load(std::memory_order_relaxed);
    if(now-prevTick<100ull||!g_joint_record_last_ms.compare_exchange_strong(prevTick,now,std::memory_order_acq_rel))return;
    if(g_joint_record_lock.test_and_set(std::memory_order_acquire))return;
    bool ok=true;QuatT28 cur{};
    if(!g_joint_have_base){
        g_joint_record_count=count;
        for(std::uint32_t j=0;j<count;++j){if(!ReadQuat(b1fn,pose,static_cast<int>(j),cur)){ok=false;break;}g_joint_base[j]=cur;g_joint_prev[j]=cur;int p=-2;(void)ReadParentDirect(owner,static_cast<int>(j),p);g_joint_parent[j]=p;}
        if(ok){g_joint_have_base=true;InitFocusedRelativeBaselineLocked();char m[320]{};std::snprintf(m,sizeof(m),"V64.22-F11 BASELINE scenario=%s joints=%u pose=%p skeleton=%p sampling=10Hz READ-ONLY",F11ScenarioNameLocal(g_f11_theory_phase.load(std::memory_order_acquire)),count,pose,owner);diagnostic_trace::Write(m);}
    } else if(count==g_joint_record_count){
        float framePos=0.0f,frameRot=0.0f;int framePosJ=-1,frameRotJ=-1;
        for(std::uint32_t j=0;j<count;++j){
            if(!ReadQuat(b1fn,pose,static_cast<int>(j),cur)){ok=false;break;}
            const float bp=Len(Sub(FromT(cur),FromT(g_joint_base[j]))),br=QuatAngleDeg(cur,g_joint_base[j]);
            const float sp=Len(Sub(FromT(cur),FromT(g_joint_prev[j]))),sr=QuatAngleDeg(cur,g_joint_prev[j]);
            if(bp>g_joint_max_pos[j])g_joint_max_pos[j]=bp;if(br>g_joint_max_rot[j])g_joint_max_rot[j]=br;
            if(sp>framePos){framePos=sp;framePosJ=static_cast<int>(j);}if(sr>frameRot){frameRot=sr;frameRotJ=static_cast<int>(j);}g_joint_prev[j]=cur;
        }
        if(ok){
            UpdateFocusedRelativeEvidenceLocked();
            const auto sample=g_joint_record_samples.fetch_add(1,std::memory_order_relaxed)+1;
            if((sample%10u)==0u){
                char m[640]{};std::snprintf(m,sizeof(m),"V64.22-F11 LIVE scenario=%s sample=%llu joints=%u topStepPos=%d/%.4fm topStepRot=%d/%.2fdeg solverC/E=%d/%d setIK=%llu weaponRecent=%d ctx={dlg:%d cut:%d menu:%d reading:%d agencyKnown:%d agency:%d scripted:%d}",F11ScenarioNameLocal(g_f11_theory_phase.load(std::memory_order_acquire)),(unsigned long long)sample,count,framePosJ,framePos,frameRotJ,frameRot,g_solver_joint_c.load(),g_solver_joint_e.load(),(unsigned long long)g_setik_calls.load(),(now-g_last_setik_tick.load())<=1500ull?1:0,g_ctx_dialogue.load()?1:0,g_ctx_cutscene.load()?1:0,g_ctx_menu.load()?1:0,g_ctx_reading.load()?1:0,g_ctx_agency_known.load()?1:0,g_ctx_agency_count.load(),g_ctx_scripted.load()?1:0);diagnostic_trace::Write(m);
                LogFocusedRelativeSampleLocked(sample);
            }
        }
    }
    g_joint_record_lock.clear(std::memory_order_release);
}


