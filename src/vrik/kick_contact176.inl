// Synchronous observation only, within the existing coherent pose query budget.
// Three foot-width rays are a probe, not a capsule or native hit certificate.
static kcd2vr::kick176::Contact ProbeKickContact176(void* character,
    kcd2vr::pose_core::Vec3 previousWorld,kcd2vr::pose_core::Vec3 currentWorld,int* bodyMaterial37=nullptr,std::uintptr_t* collider37=nullptr,float assistM72=0.f,float* assistGap72=nullptr) noexcept {
    namespace k=kcd2vr::kick176;
    k::Contact out{};ModelWorld6692 checkedWorld{};
    if(!g_contact_budget_active6695||!g_contact_pass6695||
       !k::good(previousWorld)||!k::good(currentWorld)||!ReadModelWorld6692(character,checkedWorld))return out;
    const V3 from{float(previousWorld.x),float(previousWorld.y),float(previousWorld.z)};
    const V3 to{float(currentWorld.x),float(currentWorld.y),float(currentWorld.z)};
    const V3 sweep=Sub(to,from);const float length=Len(sweep);
    if(!std::isfinite(length)||length<.0001f||length>.5f)return out;
    // Do not let F3/F11 or a legacy lab selector decide whether this F4 probe
    // sees bodies. Restore the previous per-callback query context on every exit.
    struct RestoreNpc176 {NpcQuery172 prior;~RestoreNpc176(){g_npc_query172=prior;g_kick_query40=false;}} restore{g_npc_query172};g_kick_query40=true; // C40: no impale skip for the foot
    g_npc_query172={};g_npc_query172.requested=true;
    V3 lateral=Cross(sweep,V3{0,0,1});
    lateral=Len(lateral)>.001f?Mul(Normalize(lateral),.045f):V3{.045f,0,0};
    const V3 offsets[3]{{0,0,0},lateral,Mul(lateral,-1)};
    // C72: assist only along THIS sample's own sweep (no cached direction), only with a valid sweep (checked above).
    const bool assist72=assistM72>.0f&&std::isfinite(assistM72)&&assistM72<=.30f;
    const V3 assisted72=assist72?Mul(sweep,(length+assistM72)/length):sweep;
    for(const auto offset:offsets){
        const auto hit=WorldRayDistance6692(Add(from,offset),sweep);++out.queries;
        if(hit.state==wc::Result::Clear&&assist72){ // extra length only after the exact foot sweep found nothing
            const auto far72=WorldRayDistance6692(Add(from,offset),assisted72);++out.queries;
            if(far72.state==wc::Result::Hit&&far72.plane){
                if(assistGap72)*assistGap72=Len(Sub(V3{far72.point.x,far72.point.y,far72.point.z},to));
                out.kind=k::ContactKind::Hit;out.point={far72.point.x,far72.point.y,far72.point.z};
                out.normal={far72.normal.x,far72.normal.y,far72.normal.z};
                out.entity=far72.native172.entityId;out.part=far72.native172.part;
                out.body=unsigned(far72.native172.body);out.bodyOwner=far72.native172.ownerRead;
                if(bodyMaterial37)*bodyMaterial37=far72.native172.bodyMaterial;
                if(collider37)*collider37=far72.native172.collider;
                return out;
            }
            if(far72.state!=wc::Result::Clear)return out;
            continue;
        }
        if(hit.state==wc::Result::Hit&&hit.plane){
            out.kind=k::ContactKind::Hit;out.point={hit.point.x,hit.point.y,hit.point.z};
            out.normal={hit.normal.x,hit.normal.y,hit.normal.z};
            out.entity=hit.native172.entityId;out.part=hit.native172.part;
            out.body=unsigned(hit.native172.body);out.bodyOwner=hit.native172.ownerRead;
            if(bodyMaterial37)*bodyMaterial37=hit.native172.bodyMaterial; // C37: ray_hit +0x1A idmatOrg of the struck body proxy
            if(collider37)*collider37=hit.native172.collider;
            return out;
        }
        if(hit.state!=wc::Result::Clear)return out; // unknown is never empty clearance
    }
    out.kind=k::ContactKind::Clear;return out;
}
