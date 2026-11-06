// D84: shared copied-data diagnostics use one finite original-session ticket.
static auto HandlingCaptureD84() noexcept {
    using D=diagnostics::capture168::Domain;
    return diagnostics::capture_ticket_d84::select<D::Physics,D::Vrik,D::Combat,D::Interaction,D::Reload>([]{return GetTickCount64();});
}
static void PhysFillProfile6698(const PhysEquipmentItem6698& item,const PhysGrip6698& grip,ph::Profile& p) noexcept {
    p=item.profile;p.nativeWeapon=grip.nativeWeapon;p.nativeSeed=true;p.primary=grip.ready[p.side];p.support=grip.ready[1-p.side];
    p.grip[0]=grip.hand[0];p.grip[1]=grip.hand[1];p.geometry=item.boundsValid;
    double lo[3]{},hi[3]{};for(int k=0;k<3;++k){lo[k]=item.bounds[k]*grip.geometryScale[k];hi[k]=item.bounds[k+3]*grip.geometryScale[k];}
    if(p.guardFaceReadyH24){
        // Normals transform by inverse scale; pose rotation is applied later
        // to this same entity-space profile by natural_guard117.
        const auto n=p.guardFaceNormalH24;
        const double sx=grip.geometryScale[0],sy=grip.geometryScale[1],sz=grip.geometryScale[2];
        p.guardFaceReadyH24=std::isfinite(sx)&&std::isfinite(sy)&&std::isfinite(sz)&&sx>0&&sy>0&&sz>0;
        if(p.guardFaceReadyH24){const auto face=pm::unit(pm::Vec3{n.x/sx,n.y/sy,n.z/sz});
            p.guardFaceReadyH24=bool(face);if(face)p.guardFaceNormalH24=*face;}
    }
    p.diagnosticBoundsMin59c6={lo[0],lo[1],lo[2]};p.diagnosticBoundsMax59c6={hi[0],hi[1],hi[2]};
    p.diagnosticBounds59c6=item.boundsValid&&pm::finite(p.diagnosticBoundsMin59c6)&&pm::finite(p.diagnosticBoundsMax59c6)&&
        lo[0]<=hi[0]&&lo[1]<=hi[1]&&lo[2]<=hi[2];
    int axis=0;for(int k=1;k<3;++k)if(hi[k]-lo[k]>hi[axis]-lo[axis])axis=k;
    p.extent=hi[axis]-lo[axis];pm::Vec3 a{(lo[0]+hi[0])*.5,(lo[1]+hi[1])*.5,(lo[2]+hi[2])*.5},b=a;
    if(axis==0){a.x=lo[0];b.x=hi[0];}else if(axis==1){a.y=lo[1];b.y=hi[1];}else{a.z=lo[2];b.z=hi[2];}
    p.tip=pm::length(a-p.grip[p.side].p)>pm::length(b-p.grip[p.side].p)?a:b;
    {   // Capture-only, copied data: no native lookup or formatting while off.
        static kcd2vr::handling59c10r2::GripTraceGate logGate;
        const auto capture=HandlingCaptureD84();
        const unsigned readiness=(p.primary?1u:0u)|(p.support?2u:0u);
        if(capture&&logGate.due(p.side,p.key,readiness,capture.now)){char m[700]{};const auto& b=grip.captureBinding;
            std::snprintf(m,sizeof(m),"R59.16C7E GRIP_SOCKET key=%016llX generation=%llu class=%d side=%d twoHanded=%d primary=%d support=%d hand0=(%.3f,%.3f,%.3f) hand1=(%.3f,%.3f,%.3f) tip=(%.3f,%.3f,%.3f) ready=%d/%d stable=%u/%u binding{valid=%d character=%p joint=%d relativeQ=(%.4f,%.4f,%.4f,%.4f) relativeT=(%.4f,%.4f,%.4f) scale=(%.4f,%.4f,%.4f)} geometryScale=(%.4f,%.4f,%.4f)",
                (unsigned long long)p.key,(unsigned long long)p.generation,p.classId,p.side,p.twoHanded?1:0,p.primary?1:0,p.support?1:0,p.grip[0].p.x,p.grip[0].p.y,p.grip[0].p.z,p.grip[1].p.x,p.grip[1].p.y,p.grip[1].p.z,p.tip.x,p.tip.y,p.tip.z,
                grip.ready[0]?1:0,grip.ready[1]?1:0,grip.stable[0],grip.stable[1],int(b.valid),b.character,b.joint,
                b.relative.q[0],b.relative.q[1],b.relative.q[2],b.relative.q[3],b.relative.t[0],b.relative.t[1],b.relative.t[2],b.scale[0],b.scale[1],b.scale[2],
                grip.geometryScale[0],grip.geometryScale[1],grip.geometryScale[2]);diagnostic_trace::WriteReceiptFor(capture.domain,capture.session,m);}
    }
    auto direction=pm::unit(p.tip-p.grip[p.side].p);if(direction)p.axis=*direction;else p.geometry=false;
    // C34b/C37: the weapon model's hand helpers. Proven: the attach routine 0x1085234 places the item so that slt_0 sits on
    // RightHandSlot (logged binding relativeQ = slt_0 rotation, relativeT = 0), and slt_0 * inverse(RightHandSlot) reproduces
    // the captured native right wrist within 5 mm in three runs. NOT proven: slt_1 is the left hand's ATTACH helper (table
    // {"slt_0","slt_1"} by hand), not the animation's two-hand target - the animation drives LeftWeapon_IKTarget under
    // RightHandSlot, and the captured native left wrist on Henry's sword is 9-14 cm toward the pommel from slt_1. So a
    // captured native left frame always wins; slt_1 only replaces the synthetic fallback, and only after the measured right
    // relation confirms slt_0 (fail-closed, no bind fallback), the two slots are at least a hand apart (Henry's own sword has
    // 5 cm: rejected) and slt_1 lies on the weapon. Helper positions are model metres; the native QuatT is orthonormalised,
    // so they scale with geometryScale exactly like the bounds and stay in the rigid weapon frame of the hand relations.
    p.supportSource37=p.support?1:0;
    double slotErr37=-1,slotDeg37=-1,slotSpan37=-1,slotVsNative37=-1;
    if(g_support_slot34.load(std::memory_order_relaxed)&&p.primary&&p.side==0&&p.twoHanded&&(p.melee()||p.classId==10||p.classId==13||p.classId==14||p.classId==15)&&!p.bow()&&!p.shield176()&&
       item.slotValid34[0]&&item.slotValid34[1]){ // slt_0 = right hand, slt_1 = left hand (the game's table); right-primary weapons only
        auto s0=item.slot34[0],s1=item.slot34[1]; // model-local -> the same scaled weapon space as the hand frames and bounds
        s0.p={s0.p.x*grip.geometryScale[0],s0.p.y*grip.geometryScale[1],s0.p.z*grip.geometryScale[2]};
        s1.p={s1.p.x*grip.geometryScale[0],s1.p.y*grip.geometryScale[1],s1.p.z*grip.geometryScale[2]};
        const auto rightOff=ph::compose(ph::inverse(p.grip[p.side]),s0); // right wrist -> RightHandSlot, measured
        const pm::Vec3 bind{-.0661,.0239,.0161}; // male.chr RightHandSlot in RightHand, identity rotation
        slotErr37=ph::finite(rightOff)?pm::length(rightOff.p-bind):1e3;slotDeg37=ph::finite(rightOff)?m34angle(rightOff.q)*57.29578:1e3;slotSpan37=pm::length(s1.p-s0.p);
        const bool measured=ph::finite(rightOff)&&slotErr37<.03&&m34angle(rightOff.q)<.35; // within 3 cm / 20 deg of the skeleton
        const bool span=slotSpan37>=.08; // two palms cannot be closer than a hand (male.chr index-pinky knuckles 6.3 cm)
        const bool onWeapon=item.boundsValid&&s1.p.x>=lo[0]-.01&&s1.p.x<=hi[0]+.01&&s1.p.y>=lo[1]-.01&&s1.p.y<=hi[1]+.01&&s1.p.z>=lo[2]-.01&&s1.p.z<=hi[2]+.01;
        ph::Pose leftOff{};leftOff.q=rightOff.q;leftOff.p=rightOff.p*-1.0; // LeftHandSlot = the skeleton's point mirror of the MEASURED right offset
        const auto slotLeft=ph::compose(s1,ph::inverse(leftOff));
        if(p.support)slotVsNative37=pm::length(slotLeft.p-p.grip[1-p.side].p);
        p.slotCheck37=!measured?2:!span?3:!onWeapon?4:p.support?5:1;
        if(p.slotCheck37==1&&ph::finite(slotLeft)){ // fail-closed: otherwise the existing native / synthetic path is untouched
            p.grip[1-p.side]=slotLeft;p.support=true;p.supportPositionOnly174r2=false;p.supportSlot34=true;p.supportSource37=2;}
        else if(p.slotCheck37==1)p.slotCheck37=2;
    }
    // V66.109: a two-handed melee weapon may have no captured native off-hand
    // helper in the current animation (latest runtime repeatedly had R ready / L
    // missing).  Synthesize only a conservative hilt-local support point close to
    // the proven primary grip, inside this item's real scaled bounds. Never use
    // extent as a pole/blade sliding range and never synthesize for bows.
    if(!p.support&&p.primary&&p.twoHanded&&p.melee()&&p.geometry&&direction){
        const auto primary=p.grip[p.side].p;
        const double userOffset=double(p.classId==7?g_support_offset_pole178r9.load(std::memory_order_relaxed):g_support_offset_sword178r9.load(std::memory_order_relaxed));
        const double supportDistance=kcd2vr::handling59c10r2::supportDistance(p.classId,g_support_synthetic178r9.load(std::memory_order_relaxed),userOffset,p.extent);
        auto inside=[&](pm::Vec3 v){return v.x>=lo[0]-.005&&v.x<=hi[0]+.005&&v.y>=lo[1]-.005&&v.y<=hi[1]+.005&&v.z>=lo[2]-.005&&v.z<=hi[2]+.005;};
        pm::Vec3 candidate=primary+(*direction)*supportDistance;
        // C4R2: the actual native left wrist learned in this run is behind the
        // primary wrist, toward the pommel. The old startup fallback put it on
        // the blade and copied the RIGHT wrist's rotation as if it were an
        // authored left-hand frame. Keep the configured bounded spacing, but
        // synthesize position only on the hilt; acquisition captures the left
        // controller orientation once. A learned native socket above is untouched.
        if(kcd2vr::handling59c10r2::swordFallback(p.classId,g_two_hand_v120_178r27.load(std::memory_order_relaxed))){
            const auto hilt=kcd2vr::support_grip166::Fallback(primary,p.tip,lo,hi,std::clamp(supportDistance,.06,.14));
            candidate=hilt?*hilt:pm::Vec3{1e6,1e6,1e6};
            if(hilt){const int other=1-p.side;p.grip[other]=p.grip[p.side];p.grip[other].p=*hilt;
                p.support=true;p.supportPositionOnly174r2=true;p.supportV120_178r27=true;}
        }else if(p.classId==4){
            const auto hilt=kcd2vr::support_grip166::Fallback(primary,p.tip,lo,hi,supportDistance);
            if(hilt)candidate=*hilt;else candidate={1e6,1e6,1e6};
        }else if(!inside(candidate))candidate=primary-(*direction)*supportDistance;
        if(!p.support&&inside(candidate)&&pm::length(candidate-primary)>=.06&&pm::length(candidate-primary)<=.60){
            const int other=1-p.side;p.grip[other]=p.grip[p.side];p.grip[other].p=candidate;p.support=true;
            p.supportPositionOnly174r2=true;
        }
    }
    if(p.support&&!p.supportSource37)p.supportSource37=3;
    {   // C37: one line when THIS item's support source or slot check changes (capture only; nothing formatted otherwise)
        static std::uint64_t lastKey37=0,lastGen37=0,lastSessionD84=0;static unsigned lastCode37=~0u,lastDomainD84=~0u;
        const unsigned code=p.supportSource37|(unsigned(p.slotCheck37)<<4);
        const auto capture=HandlingCaptureD84();
        if(capture&&(p.key!=lastKey37||p.generation!=lastGen37||code!=lastCode37||
           capture.session!=lastSessionD84||unsigned(capture.domain)!=lastDomainD84)&&p.primary&&p.twoHanded&&p.melee()){
            lastKey37=p.key;lastGen37=p.generation;lastCode37=code;lastSessionD84=capture.session;lastDomainD84=unsigned(capture.domain);
            static const char* const kSource[]={"NONE","NATIVE_ANIMATION_FRAME","WEAPON_SLOT_SLT1","SYNTHETIC_FALLBACK"};
            static const char* const kCheck[]={"NOT_EVALUATED","MEASURED_USED","RIGHT_WRIST_MISMATCH","SLOTS_CLOSER_THAN_HAND","SLT1_OUTSIDE_BOUNDS","MEASURED_NATIVE_WINS"};
            char m[360]{};std::snprintf(m,sizeof(m),"C37 SUPPORT_SOURCE key=%016llX generation=%llu class=%d source=%s slot=%s slotOn=%d rightErrCm=%.1f rightErrDeg=%.1f slotSpanCm=%.1f slotVsNativeCm=%.1f positionOnly=%d",
                (unsigned long long)p.key,(unsigned long long)p.generation,p.classId,kSource[p.supportSource37&3],kCheck[p.slotCheck37<6?p.slotCheck37:0],g_support_slot34.load(std::memory_order_relaxed)?1:0,
                slotErr37*100,slotDeg37,slotSpan37*100,slotVsNative37*100,p.supportPositionOnly174r2?1:0);diagnostic_trace::WriteReceiptFor(capture.domain,capture.session,m);}
    }
    if(g_support_hilt_region178r26.load(std::memory_order_relaxed)&&!p.supportV120_178r27&&p.classId==4&&p.twoHanded&&p.primary&&p.support&&p.geometry) // V66.178R26: off = fixed socket (V164)
        p.supportRegion169r2=kcd2vr::hilt169r2::qualify(p.grip[p.side].p,p.tip,p.grip[1-p.side].p,lo,hi);
    if(p.bow()&&p.support){
        auto draw=p.grip[0].p-p.grip[1].p;
        // The two native wrists may be at different heights. Their diagonal
        // must not become a draw-upwards axis. Remove the bow's long-axis part.
        if(direction)draw=draw-(*direction)*pm::dot(draw,*direction);
        auto normalized=pm::unit(draw);if(normalized)p.axis=*normalized;else p.support=false;
    }
}
static void PhysInputWorld6698(ph::Input& input,void* character) noexcept {
    ModelWorld6692 w{};if(!ReadModelWorld6692(character,w)){input.allowed=false;return;}
    input.world={{w.q.x,w.q.y,w.q.z,w.q.w},{w.t.x,w.t.y,w.t.z}};input.worldScale=w.scale;input.haveWorld=true;
}
static bool PhysPrepare6698(ph::Input& in,std::array<ph::Profile,2>& profiles,PhysEquipment6698& equipment,std::array<PhysCorrelation126,2>* correlation=nullptr) noexcept {
    if(!g_phys_armed6698.load()||g_phys_fault6698.load()||g_weapon_control_mode.load()<4||diagnostics::lab::NativeReference()||!g_phys_equipment6698.read(equipment)||!equipment.valid||
       equipment.request!=g_weapon_request_epoch6694.load()||equipment.epoch!=g_pose_epoch6689.load()||std::uint64_t(GetTickCount64())<equipment.tick||std::uint64_t(GetTickCount64())-equipment.tick>100){g_physical_gates101R2[0].fetch_add(1,std::memory_order_relaxed);return false;}
    PhysGripBank6698 grips{};g_phys_grips6698.read(grips);
    for(int side=0;side<2;++side){profiles[side]={};profiles[side].side=side;
        in.drawMotionD63[side]={};in.drawHeldD63[side]=false;
        kcd2vr::draw_motion_d63::Ticket drawTicket{};
        if(kcd2vr::draw_signal_d63::latest[side].read(drawTicket)&&drawTicket.body==equipment.epoch&&drawTicket.request==equipment.request)
            in.drawMotionD63[side]=drawTicket; // observation only before held ACK
        if(!equipment.handKnown[side]||equipment.handKey[side])profiles[side].key=~std::uint64_t(0);
        if(equipment.heldIndex[side]<0)continue;
        auto& item=equipment.items[equipment.heldIndex[side]];
        for(const auto& g:grips.grip)if(g.key==item.profile.key&&g.generation==item.profile.generation){PhysFillProfile6698(item,g,profiles[side]);if(correlation)(*correlation)[side]={g.nativeStamp,g.nativeSocket126};break;}
        // Draw return is not the ACK. Arm this observed input-only ticket only after
        // the fresh native snapshot positively owns this exact held binding.
        if(equipment.handKnown[side]&&item.held&&equipment.handKey[side]==item.profile.key&&
           item.profile.key&&item.profile.generation&&item.profile.side==side&&profiles[side].primary){
            const auto& ticket=in.drawMotionD63[side];
            in.drawHeldD63[side]=ticket.serial&&ticket.key==item.profile.key&&ticket.generation==item.profile.generation;
        }
    }
    BowGeometry166 bowGeometry{};
    if(g_bow_geometry166.read(bowGeometry))PhysApplyBowGeometry166(bowGeometry,equipment,profiles[1],std::uint64_t(GetTickCount64()));
    g_physical_gates101R2[1].fetch_add(1,std::memory_order_relaxed);
    // Only the selected motion backend changes handling; native comparison and
    // bow behavior keep their prior profile. Snapshot age is the ORIGINAL read.
    in.attributes={};
    static thread_local kcd2vr::live5912::Snapshot live{};
    kcd2vr::live5912::Snapshot observation{};
    // R59.16C10R14: live strength/agility/stamina reach the weapon in EVERY F4 rung (the weight/fatigue feel needs
    // them); before, rung 2 and rung 4 always drove the weapon with neutral stats.
    const bool wantLive59c14=kcd2vr::motion59c4::selected.load()||g_live_attributes_all_rungs59c14.load(std::memory_order_relaxed);
    if(!wantLive59c14)live={};
    if(wantLive59c14&&
       kcd2vr::live5912::readBounded(live,kcd2vr::live5912::latest.read(observation),observation,
          reinterpret_cast<std::uintptr_t>(equipment.player),reinterpret_cast<std::uintptr_t>(g_last_player_entity.load()),
          GetTickCount64(),kcd2vr::contact176::recoveryEpoch55c1.load())){
        in.attributes.strength=live.strength;in.attributes.agility=live.agility;
        in.attributes.stamina=live.stamina;in.attributes.live=true;
        in.attributes.strengthEffective=live.strengthEffective;in.attributes.agilityEffective=live.agilityEffective; // R59.16C10R17
        for(unsigned i=0;i<5;++i)in.attributes.skills26[i]=live.skills26[i]; // C26c weapon skills
    }
    {   // R59.16C7B: one essential line when Henry becomes exhausted / recovers (weapon handling x5, attacks and blocks unaffected).
        static bool exhausted=false;const double sc=kcd2vr::physical::exhaustionScale59c7b(in.attributes);
        kcd2vr::kick26::exhaustedSeenCLE.store(in.attributes.live&&sc>=4.9?std::uint64_t(GetTickCount64()):0,std::memory_order_relaxed); // CL-E2
        const bool becameExhausted=!exhausted&&sc>=4.9;
        const bool recovered=exhausted&&in.attributes.live&&sc<=1.0;
        if(becameExhausted||recovered){
            exhausted=becameExhausted;kcd2vr::kick26::henryExhaustedCLE.store(exhausted,std::memory_order_relaxed);
            // Gameplay state above is independent of recording.
            using D=diagnostics::capture168::Domain;
            const auto capture=diagnostics::capture_ticket_d84::select<D::Physics,D::Combat>([]{return GetTickCount64();});
            if(capture){char m[240]{};std::snprintf(m,sizeof(m),"R59.16C1 SWING_STAMINA_EXHAUSTION %s stamina=%.3f exhaustionRequest=x%.1f effect=BOUNDED_DRIVE admission=UNCHANGED staticGuard=SUPPORTED",
                exhausted?"ON":"OFF",in.attributes.stamina,exhausted?sc:1.0);diagnostic_trace::WriteReceiptFor(capture.domain,capture.session,m);}
        }
    }
    in.epoch=g_weapon_request_epoch6694.load(); // explicit mode/lifecycle generation, not frame count
    return true;
}
