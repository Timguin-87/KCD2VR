struct NativeWeaponAnatomySlotV6567 {
    std::uint64_t w0{},w1{};
    int side{-1};
    int anchor{-1}; // 0=ITEM_L, 1=ITEM_R; native item-operation side is kept separately
    std::uint64_t samples{};
    double rpx{},rpy{},rpz{},lpx{},lpy{},lpz{};
    double r2{},l2{};
    bool haveR{},haveL{};
};
constexpr int kNativeWeaponAnatomySlotsV6567=16;
NativeWeaponAnatomySlotV6567 g_native_weapon_anatomy_v6567[kNativeWeaponAnatomySlotsV6567]{};
std::atomic_flag g_native_weapon_anatomy_lock_v6567=ATOMIC_FLAG_INIT;

static NativeWeaponAnatomySlotV6567* AnatomySlotV6567(std::uint64_t w0,std::uint64_t w1,int side,int anchor) noexcept {
    NativeWeaponAnatomySlotV6567* empty=nullptr;
    for(auto& a:g_native_weapon_anatomy_v6567){
        if(a.w0==w0&&a.w1==w1&&a.side==side&&a.anchor==anchor) return &a;
        if(!empty&&a.samples==0) empty=&a;
    }
    if(empty){empty->w0=w0;empty->w1=w1;empty->side=side;empty->anchor=anchor;return empty;}
    return &g_native_weapon_anatomy_v6567[(w0^w1^(std::uint64_t)(side+1)^(std::uint64_t)(anchor+3))%kNativeWeaponAnatomySlotsV6567];
}


static void SampleNativeWeaponAnatomyV6567(void* pose,PoseGetFn b1fn,const RigMap& rig) noexcept {
    if(g_f11_theory_phase.load(std::memory_order_acquire)!=3||!pose||!b1fn) return;
    if(!diagnostics::reload_capture_d84::requested())return;
    static std::atomic<std::uint64_t> last{0};
    const auto now=(std::uint64_t)GetTickCount64();const auto captureD84=diagnostics::reload_capture_d84::select(now);
    if(!captureD84.session)return;
    auto prev=last.load(std::memory_order_relaxed);
    if(now-prev<100ull||!last.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) return; // bounded 10 Hz
    QuatT28 lh{},rh{},li{},ri{};
    const bool ol=ReadQuat(b1fn,pose,rig.hand_l,lh),orr=ReadQuat(b1fn,pose,rig.hand_r,rh);
    const bool oli=rig.item_l>=0&&ReadQuat(b1fn,pose,rig.item_l,li),ori=rig.item_r>=0&&ReadQuat(b1fn,pose,rig.item_r,ri);
    if(!ol&&!orr) return;
    while(g_native_weapon_anatomy_lock_v6567.test_and_set(std::memory_order_acquire)) return;
    for(int side=0;side<2;++side){
        const auto tick=g_native_side_item_tick[side].load(std::memory_order_acquire);
        if(!tick||now<tick||now-tick>5000ull) continue;
        const auto w0=g_native_side_wuid0[side].load(std::memory_order_acquire),w1=g_native_side_wuid1[side].load(std::memory_order_acquire);
        if(!(w0||w1)) continue;
        for(int anchor=0;anchor<2;++anchor){
            const bool itemOk=anchor==0?oli:ori; const QuatT28& item=anchor==0?li:ri;
            if(!itemOk) continue;
            auto* a=AnatomySlotV6567(w0,w1,side,anchor); if(!a) continue;
            a->samples++;
            char rbuf[128]="NA",lbuf[128]="NA";
            if(orr){const auto rr=RelativeFromAbsolute(item,rh);const double x=rr.t[0],y=rr.t[1],z=rr.t[2];a->rpx+=x;a->rpy+=y;a->rpz+=z;a->r2+=x*x+y*y+z*z;a->haveR=true;std::snprintf(rbuf,sizeof(rbuf),"(%+.4f,%+.4f,%+.4f)",x,y,z);}
            if(ol){const auto lr=RelativeFromAbsolute(item,lh);const double x=lr.t[0],y=lr.t[1],z=lr.t[2];a->lpx+=x;a->lpy+=y;a->lpz+=z;a->l2+=x*x+y*y+z*z;a->haveL=true;std::snprintf(lbuf,sizeof(lbuf),"(%+.4f,%+.4f,%+.4f)",x,y,z);}
            if((a->samples%10ull)==1ull){
                char m[960]{};std::snprintf(m,sizeof(m),
                    "V65.67-F11-WEAPON-ANATOMY nativeSide=%d anchor=%s item=%p wuid={%016llX %016llX} sample=%llu nativeHandInAnchor={R:%s L:%s} itemJoint=%d handJoints={R:%d L:%d} mode=READ_ONLY_10HZ use=idle_guard_swing_aim",
                    side,anchor==0?"ITEM_L":"ITEM_R",g_native_side_item[side].load(std::memory_order_acquire),(unsigned long long)w0,(unsigned long long)w1,(unsigned long long)a->samples,rbuf,lbuf,
                    anchor==0?rig.item_l:rig.item_r,rig.hand_r,rig.hand_l);diagnostic_trace::WriteReceiptFor(captureD84.domain,captureD84.session,m);
            }
        }
    }
    g_native_weapon_anatomy_lock_v6567.clear(std::memory_order_release);
}


static void SampleWeaponUpstreamV6568(void* pose,PoseGetFn b1fn,const RigMap& rig) noexcept {
    const int mode=g_weapon_upstream_diag_mode.load(std::memory_order_acquire);
    if(mode<=0||!pose||!b1fn)return;
    const auto captureD84=diagnostics::capture_ticket_d84::select<diagnostics::capture168::Domain::Physics>([]{return GetTickCount64();});
    if(!captureD84)return;
    const auto now=captureD84.now;
    static std::atomic<std::uint64_t> last{0}; auto prev=last.load(std::memory_order_relaxed);
    if(now-prev<100ull||!last.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) return;
    QuatT28 lh{},rh{},li{},ri{}; const bool hl=ReadQuat(b1fn,pose,rig.hand_l,lh),hr=ReadQuat(b1fn,pose,rig.hand_r,rh);
    const bool il=rig.item_l>=0&&ReadQuat(b1fn,pose,rig.item_l,li),ir=rig.item_r>=0&&ReadQuat(b1fn,pose,rig.item_r,ri);
    void* item=g_last_concrete_item_object.load(std::memory_order_acquire); void* owner=g_last_item_attach_owner.load(std::memory_order_acquire); const int side=g_last_item_attach_side.load(std::memory_order_acquire);
    void* ent=g_last_wuid_entity_iface.load(std::memory_order_acquire); const std::uint32_t eid=g_last_wuid_entity_id.load(std::memory_order_acquire);
    Matrix34V6550 wt{},lt{}; const bool wtm=(mode>=4&&ent&&eid)?GetEntityWorldTmV6552(ent,eid,wt):false; const bool ltm=(mode>=4&&ent&&eid)?GetEntityLocalTmV6552(ent,eid,lt):false;
    auto xyz=[](bool ok,const QuatT28&q,char*o,std::size_t n){if(ok)std::snprintf(o,n,"(%+.3f,%+.3f,%+.3f)",double(q.t[0]),double(q.t[1]),double(q.t[2]));else std::snprintf(o,n,"NA");};
    char a[64]{},b[64]{},c[64]{},d[64]{};xyz(hr,rh,a,sizeof(a));xyz(hl,lh,b,sizeof(b));xyz(ir,ri,c,sizeof(c));xyz(il,li,d,sizeof(d));
    char m[1200]{}; std::snprintf(m,sizeof(m),
        "V65.68-F9-UPSTREAM-SAMPLE session=%llu rung=%d HAND={R:%s L:%s} ITEM={R:%s L:%s} C_Item=%p attachOwner=%p side=%d resolvedEntity=%p id=%u world=%s(%+.3f,%+.3f,%+.3f) local=%s(%+.3f,%+.3f,%+.3f) manager=%p input=NATIVE writes=NONE",
        (unsigned long long)captureD84.session,mode,a,b,c,d,mode>=3?item:nullptr,mode>=5?owner:nullptr,side,mode>=4?ent:nullptr,mode>=4?eid:0,wtm?"OK":"NA",wtm?double(wt.m[3]):0.0,wtm?double(wt.m[7]):0.0,wtm?double(wt.m[11]):0.0,ltm?"OK":"NA",ltm?double(lt.m[3]):0.0,ltm?double(lt.m[7]):0.0,ltm?double(lt.m[11]):0.0,mode>=5?g_henry_item_attach_manager.load(std::memory_order_acquire):nullptr);
    diagnostic_trace::WriteReceiptFor(captureD84.domain,captureD84.session,m);
}

static void SampleFocusedF11Recorder(void* pose,PoseGetFn b1fn,const RigMap& rig) noexcept {
    if(g_f11_theory_phase.load(std::memory_order_acquire)<=0||!pose||!b1fn) return;
    if(!diagnostics::reload_capture_d84::requested())return;
    static std::atomic<std::uint64_t> last{0};
    const auto now=static_cast<std::uint64_t>(GetTickCount64()); const auto captureD84=diagnostics::reload_capture_d84::select(now);
    if(!captureD84.session)return;
    auto prev=last.load(std::memory_order_relaxed);
    if(now-prev<400ull||!last.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) return;
    QuatT28 h{},lh{},rh{},li{},ri{},pel{},sp3{};
    const bool oh=ReadQuat(b1fn,pose,rig.head,h);
    const bool ol=ReadQuat(b1fn,pose,rig.hand_l,lh);
    const bool orr=ReadQuat(b1fn,pose,rig.hand_r,rh);
    const bool oli=rig.item_l>=0&&ReadQuat(b1fn,pose,rig.item_l,li);
    const bool ori=rig.item_r>=0&&ReadQuat(b1fn,pose,rig.item_r,ri);
    const bool op=rig.pelvis>=0&&ReadQuat(b1fn,pose,rig.pelvis,pel);
    const bool os=rig.spine3>=0&&ReadQuat(b1fn,pose,rig.spine3,sp3);
    TrackingSnapshot ts{}; const bool tr=ReadTrackingSnapshot(ts);
    auto pstr=[](bool ok,const QuatT28& q,char* dst,std::size_t cap){
        if(ok) std::snprintf(dst,cap,"(%+.3f,%+.3f,%+.3f)",double(q.t[0]),double(q.t[1]),double(q.t[2]));
        else std::snprintf(dst,cap,"NA");
    };
    char sh[64]{},sl[64]{},sr[64]{},sil[64]{},sir[64]{},spel[64]{},ssp[64]{};
    pstr(oh,h,sh,sizeof(sh));pstr(ol,lh,sl,sizeof(sl));pstr(orr,rh,sr,sizeof(sr));pstr(oli,li,sil,sizeof(sil));pstr(ori,ri,sir,sizeof(sir));pstr(op,pel,spel,sizeof(spel));pstr(os,sp3,ssp,sizeof(ssp));
    char m[1280]{}; std::snprintf(m,sizeof(m),
        "V65.47-F11-POSE recorder=%s session=%llu captureMask=0x%X t=%llu XR={H:%d(%+.3f,%+.3f,%+.3f) L:%d(%+.3f,%+.3f,%+.3f) R:%d(%+.3f,%+.3f,%+.3f)} NATIVE={pel:%s chest:%s head:%s handL:%s handR:%s ITEM_L:%s ITEM_R:%s} gripIds={slt0:%d slt1:%d RightWeaponSlot:%d LeftWeaponSlot:%d} solverC/E=%d/%d setIK=%llu ctx={dlg:%d cut:%d menu:%d scripted:%d}",
        diagnostics::capture168::name(captureD84.domain),(unsigned long long)captureD84.session,captureD84.activeMask,(unsigned long long)now,
        (tr&&ts.head_valid)?1:0,double(ts.head.x),double(ts.head.y),double(ts.head.z),
        (tr&&ts.left_valid)?1:0,double(ts.left.x),double(ts.left.y),double(ts.left.z),
        (tr&&ts.right_valid)?1:0,double(ts.right.x),double(ts.right.y),double(ts.right.z),
        spel,ssp,sh,sl,sr,sil,sir,
        g_slt0_joint.load(std::memory_order_acquire),g_slt1_joint.load(std::memory_order_acquire),
        g_weapon_slot_r.load(std::memory_order_acquire),g_weapon_slot_l.load(std::memory_order_acquire),
        g_solver_joint_c.load(),g_solver_joint_e.load(),
        (unsigned long long)g_setik_calls.load(),g_ctx_dialogue.load()?1:0,g_ctx_cutscene.load()?1:0,g_ctx_menu.load()?1:0,g_ctx_scripted.load()?1:0);
    diagnostic_trace::WriteReceiptFor(captureD84.domain,captureD84.session,m);
}


