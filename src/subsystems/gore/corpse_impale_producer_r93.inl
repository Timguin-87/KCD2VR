// R97: signed depth of the weapon TIP under the surface a body impact record touched (same physics step: the record and
// the stopped weapon pose live in the same published frame). Blade-contact impale proof (strike_feature_d41::canPierce).
static double ImpactTipDepthR97(const PhysFrame6698& f,int side,const ph::Vec3& point,const ph::Vec3& normal) noexcept {
    if(side<0||side>1||!f.input.haveWorld||!std::isfinite(f.world[7])||!(f.world[7]>1e-6)||!ph::finite(f.output.weapons[side]))return std::nan("");
    const auto tip=ph::point(PhysWorld6698(f.world),ph::point(f.output.weapons[side],f.profiles[side].tip)*double(f.world[7]));
    const double t[3]{tip.x,tip.y,tip.z},c[3]{point.x,point.y,point.z},n[3]{normal.x,normal.y,normal.z};
    return kcd2vr::strike_feature_d41::tipDepth96(t,c,n);
}
// Existing N2 producer, independent of unrelated combat configuration.
// Native death, completed contact, exact ownership and physical feature gates are unchanged.
static void CorpseImpaleProducerR93() noexcept {
    // N2 (Miki: impalements): a THRUST of the owned weapon into an articulated body that the GAME killed in this lifetime
    // (gore32::KnownDead from the native lethal receipt, same entity identity) becomes a corpse-impale candidate for the
    // weapon side (same hang path as a killing stab). Own completed contact record only; no native hit is requested.
    {static kcd2vr::impact59c6::Seen lastStabR15[2]{};static std::uint64_t sayR15=0;PhysFrame6698 sf{};namespace icr=kcd2vr::impale_corpse_r15;
     if(kcd2vr::gore32::impaleHang.load(std::memory_order_relaxed)&&g_phys_frame6698.read(sf))for(int s=0;s<2;++s){
        const auto& r=sf.output.impactRecord178r30[s];
        if(!r.valid||!r.entityId||!r.collider||r.kind!=1||!lastStabR15[s].take(r))continue;
        const auto now=std::uint64_t(GetTickCount64());if(now<r.tick)continue;
        void* entity=FlinchEntityById178r11(r.entityId);const auto ident=reinterpret_cast<std::uintptr_t>(entity);
        if(!entity||entity==g_last_player_entity.load(std::memory_order_acquire))continue;
        const bool owned=r.epoch==g_pose_epoch6689.load()&&r.request==g_weapon_request_epoch6694.load()&&
            sf.profiles[s].key==r.key&&sf.profiles[s].generation==r.generation&&sf.output.active[s]&&sf.output.owned[s];
        const bool articulated=r.body==int(kcd2vr::npc_contact172::Body::Articulated);
        const bool dead=kcd2vr::gore32::KnownDead(r.entityId,ident,now);
        const double pt[3]{r.point.x,r.point.y,r.point.z};
        const double need=double(kcd2vr::impale40::minEnergy.load(std::memory_order_relaxed));
        const double depthR97=ImpactTipDepthR97(sf,s,r.point,r.normal); // R97: a BLADE touch needs the tip under his surface
        const double axial155=icr::axialCos(r.goreCutD56.totalSpeed,r.goreCutD56.transverseSpeed); // R155: the point must travel along the blade (a chop with the point is not a stab)
        const auto why=icr::qualifyPhysicalD41(true,r.kind,r.feature59c7g,sf.profiles[s].classId,articulated,dead,r.speed,need,now-r.tick,owned,pt,depthR97,axial155);
        if(why==icr::Why::Ok){
            icr::Candidate c{};c.serial=(0x15000000ull<<8)+now*2+std::uint64_t(s);c.tick=r.tick;c.key=r.key;c.generation=r.generation;c.epoch=r.epoch;c.request=r.request;
            c.target=r.entityId;c.identity=ident;c.collider=r.collider;c.part=r.part;c.bodyMaterial=r.bodyMaterial;c.side=s;
            c.feature=r.feature59c7g;c.weaponClass=sf.profiles[s].classId;c.tipDepth96=depthR97;
            c.speed=r.speed;c.power=icr::power(r.speed);for(int i=0;i<3;++i)c.point[i]=pt[i];
            c.normal[0]=r.normal.x;c.normal[1]=r.normal.y;c.normal[2]=r.normal.z;c.valid=true;
            (void)icr::latest.publish(c);
        }
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char m[300]{};std::snprintf(m,sizeof(m),
            "N2 IMPALE_CORPSE_STAB side=%d target=%u verdict=%s speed=%.2f power=%.2f need=%.2f articulated=%d deadByGame=%d owned=%d part=%d feature=%d tipDepthCm=%.1f axialCos=%.2f",
            s,r.entityId,icr::name(why),r.speed,icr::power(r.speed),need,articulated?1:0,dead?1:0,owned?1:0,r.part,r.feature59c7g,std::isfinite(depthR97)?depthR97*100.0:-1.0,std::isfinite(axial155)?axial155:-1.0);diagnostic_trace::Write(m);}
        if(why==icr::Why::TooWeak&&articulated&&dead&&(now<sayR15||now-sayR15>2000)){sayR15=now;
            char t[140]{};std::snprintf(t,sizeof(t),"impale: stab the body harder (power %.2f, need %.2f)",icr::power(r.speed),need);
            frik_pose_probe::PublishVisibleFailure(t);}
     }}
}
