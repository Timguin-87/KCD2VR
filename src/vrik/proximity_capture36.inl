// F3-only, bounded current-pair observation. No NPC setter/phase/action writes.
static std::atomic<float> g_proximity_radius36{.4f};
// C37: edge-only native defense state of the CURRENT pair, on the same F3 lease and 100 ms cadence. Reads only;
// no setter, no virtual call, no AI allocation (see vrik/defense_observe37.h for the retail layouts).
static bool Copy37(std::uintptr_t p,void* out,std::size_t n) noexcept {
    if(!p||!out||!n||!Safe(reinterpret_cast<const void*>(p),n))return false;
    __try{std::memcpy(out,reinterpret_cast<const void*>(p),n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void DefenseState37(const NativeOwner104& h,const NativeParryOpponent176* o,std::uint64_t now) noexcept {
    namespace d37=kcd2vr::defense37;
    static std::uint32_t lastTarget=0;static d37::Henry lastHenry{};static d37::Npc lastNpc{};static std::uint64_t session=0;
    const auto s=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    if(s!=session){session=s;lastTarget=0;lastHenry=d37::Henry();lastNpc=d37::Npc();}
    if(!o||!o->actor||!o->state||!h.state||!h.actor){
        if(lastTarget){lastTarget=0;lastHenry=d37::Henry();lastNpc=d37::Npc();diagnostic_trace::Write("C37 DEFENSE_STATE pair=LOST_OR_UNQUALIFIED writes=NONE");}return;}
    const auto base=WhBase();
    const auto henry=d37::ReadHenry(Copy37,base,reinterpret_cast<std::uintptr_t>(h.state));
    const auto npc=d37::ReadNpc(Copy37,base,reinterpret_cast<std::uintptr_t>(o->actor),reinterpret_cast<std::uintptr_t>(o->state),reinterpret_cast<std::uintptr_t>(h.actor));
    if(o->id==lastTarget&&d37::Same(henry,lastHenry)&&d37::Same(npc,lastNpc))return; // edge-only
    const bool pairChanged=o->id!=lastTarget;lastTarget=o->id;lastHenry=henry;lastNpc=npc;(void)now;
    char npcText[560]{};d37::FormatNpc(npcText,sizeof(npcText),npc);
    char line[800]{};std::snprintf(line,sizeof(line),
        "C37 DEFENSE_STATE target=%u pairChanged=%d henryPhase=%d henryReach4B0=%d henryAlmost4E0=%d henryBlockDowngrade390=%d %s source=PROXIMITY_100MS offer=NONE writes=NONE",
        o->id,pairChanged?1:0,henry.phase,henry.reach,henry.almost,henry.blockDowngrade390,npcText);
    diagnostic_trace::Write(line);
}
static void ProximityCapture36(const PhysFrame6698& f,std::uint64_t now) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)||
       !kcd2vr::motion59c4::selected.load()||!retail_build112::Ready(WhBase()))return;
    static std::uint64_t due=0;if(now<due)return;due=now+100; // <=10 current-pair observations/s only while F3 captures
    const char* why="FRAME_UNAVAILABLE";kcd2vr::response36::Near proximity36{};NativeOwner104 h{};NativeParryOpponent176 o{};
    int phase=-1,type=-99,stance=-1;bool pair37=false;float radius=g_proximity_radius36.load(std::memory_order_relaxed);
    const bool frameOk=f.input.motionOwned59c5&&f.input.allowed&&f.input.tracked[0]&&f.input.tracked[1]&&
       f.input.tick&&now>=f.input.tick&&now-f.input.tick<=80&&f.input.sample&&f.output.active[0]&&f.output.owned[0]&&
       f.profiles[0].melee()&&f.profiles[0].geometry&&f.request==g_weapon_request_epoch6694.load()&&
       f.input.bodyEpoch==g_pose_epoch6689.load()&&!animation_filter::nativeBodyOwned112.load();
    if(frameOk&&NativeResolve104(f.player,h)&&h.epoch==f.input.bodyEpoch){
        why="NO_RECIPROCAL_OPPONENT";
        if(NativeParryOpponentRead176(h,o)&&PhysVft6698(o.actor,0x3E3FAC8)){
            pair37=true;phase=o.observedPhase19c4r1;type=g_opp_type49.load();
            (void)Read(static_cast<unsigned char*>(o.state)+0xF8,stance);
            Matrix34V6550 tm{};why="TARGET_TRANSFORM_UNAVAILABLE";
            if(GetEntityWorldTmV6552(o.entity,o.id,tm)&&std::isfinite(f.world[7])&&f.world[7]>.01f){
                auto wp=f.output.weapons[0];wp.p=wp.p*f.world[7];wp=ph::compose(PhysWorld6698(f.world),wp);
                const auto& p=f.profiles[0];
                const auto a=ph::point(wp,p.grip[0].p*f.world[7]),b=ph::point(wp,p.tip*f.world[7]);
                // Explicit reference: entity origin +1.1 m up, NOT a decoded physical body.
                const pm::Vec3 up{tm.m[2],tm.m[6],tm.m[10]};
                proximity36=kcd2vr::response36::proximity(a,b,pm::Vec3{tm.m[3],tm.m[7],tm.m[11]}+up*1.1,
                    {tm.m[0],tm.m[4],tm.m[8]},{tm.m[1],tm.m[5],tm.m[9]},up,radius);
                why=!proximity36.known?"GEOMETRY_UNAVAILABLE":proximity36.inside?"NEAR_TORSO_REFERENCE":"OUTSIDE_REFERENCE_RADIUS";
            }
        }
    }
    char line[900]{};std::snprintf(line,sizeof(line),
        "C36 NPC_PROXIMITY target=%u sample=%llu radius=%.3f referenceDistance=%.3f side=%d npcStance=%d npcPhase=%d npcType=%d candidate=%d reason=%s withinRadius360=%d sector=%s lateralM=%.3f verticalM=%.3f depthM=%.3f sectorFrame=NPC_CURRENT_POSITION_NOT_ATTACK_ORIGIN geometry=ENTITY_TORSO_REFERENCE_NOT_COLLIDER speedRequired=0 guardRequest=NOT_SENT passiveBoundary=UNPROVEN writes=NONE",
        o.id,(unsigned long long)f.input.sample,double(radius),proximity36.distance,proximity36.zone,stance,phase,type,int(proximity36.inside),why,int(proximity36.withinRadius),kcd2vr::response36::sectorName36(proximity36.sector),proximity36.lateral,proximity36.vertical,proximity36.depth);
    diagnostic_trace::Write(line);
    DefenseState37(h,pair37?&o:nullptr,now);
}
