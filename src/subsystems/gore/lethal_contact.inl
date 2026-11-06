// C2: phase-1 diagnostics only. The native post-hit phase is an observation,
// not an ordinal eligibility gate. Candidate does not authorize head removal.
namespace decap59c7e {
inline void Evaluate(const Pending& p,const kcd2vr::contact58c1::Receipt& r,int before,int after) noexcept {
    if(r.death==1&&r.accepted&&r.applied&&r.target)kcd2vr::gore32::NoteDead(r.target,r.targetIdentity,std::uint64_t(GetTickCount64())); // C43T death proof
    const auto& cutD56=p.metadata59c7g.goreCutD56;
    const bool exactCutD56=kcd2vr::gore_cut_d56::owned(cutD56,p.gesture59c3,p.key,p.generation,p.epoch,p.request,p.sample,p.tick);
    const auto cutWhyD56=exactCutD56?kcd2vr::gore_cut_d56::qualify(cutD56,kcd2vr::gore32::decapMinPower.load()):kcd2vr::gore_cut_d56::Why::Identity;
    if(r.death==1&&r.accepted&&r.applied)kcd2vr::gore32::noteLethalGestureD56({r.target,r.targetIdentity,p.gesture59c3,p.key,p.generation,p.epoch,p.request});
    impulse596::NoteNativeDeath56(r,p.epoch,p.recovery55c1,std::uint64_t(GetTickCount64())); // C56: exact native lethal receipt, separate lifetime-scoped F9 proof
    // C32: a strong lethal cut on a body goes to the decapitation check (neck test in the victim's model space,
    // attachment hiding and blood happen on the next native frame in frik_pose_probe). Armour is ignored (Miki).
    if(kcd2vr::gore32::decap.load(std::memory_order_relaxed)&&kcd2vr::strike_feature_d41::severWound(r)&&r.death==1&&
       cutWhyD56==kcd2vr::gore_cut_d56::Why::Ok){
        kcd2vr::gore32::CutCandidate c{};c.serial=r.serial;c.tick=r.tick?r.tick:std::uint64_t(GetTickCount64());c.target=r.target;c.identity=r.targetIdentity;
        for(int i=0;i<3;++i)c.point[i]=r.point[i];c.power=float(cutD56.power);c.cutD56=cutD56;c.twoHands=r.twoHands;
        c.bodyMaterial=r.bodyMaterial;c.corpse=false;c.nativeLethal42=true;c.bodyEpoch42=p.epoch;c.request42=p.request;c.part44=r.nativePart>=0?r.nativePart:r.rawPart;c.rawPart47=r.rawPart;(void)kcd2vr::gore32::lethalCut.publish(c); // C41: head/neck surface id travels along; C44: struck part id (receipt only)
    }else if(kcd2vr::gore32::decap.load(std::memory_order_relaxed)&&r.death==1){
        // C39: every KILLING hit that was not sent to the head/neck test says why
        const char* why=cutWhyD56!=kcd2vr::gore_cut_d56::Why::Ok?kcd2vr::gore_cut_d56::name(cutWhyD56):r.kind!=0?"NOT_A_CUT":!kcd2vr::strike_feature_d41::canCut(r.feature,r.weaponClass)?"NO_PROVEN_CUTTING_FEATURE":
            !kcd2vr::strike_feature_d41::severWound(r)?"NO_CONFIRMED_CUT_WOUND":!r.bodyContact?"NO_BODY_CONTACT":(r.blocked||r.perfect||r.parry)?"BLOCKED_OR_PARRIED":
            !(r.accepted&&r.applied&&r.fields)?"HIT_NOT_APPLIED":"NOT_ELIGIBLE";
        char m[320]{};std::snprintf(m,sizeof(m),"C39 DECAP skipped serial=%llu target=%u reason=%s power=%.2f need=%.2f kind=%d bodyMaterial=%d net=%.3f fast=%.3f transverse=%.2f",(unsigned long long)r.serial,r.target,why,
            cutD56.power,std::max(.90,double(kcd2vr::gore32::decapMinPower.load(std::memory_order_relaxed))),r.kind,r.bodyMaterial,
            cutD56.netTravel,cutD56.fastTravel,cutD56.transverseSpeed);diagnostic_trace::Write(m);
    }
    // R94: living-limb contacts never publish a severing candidate. Native death is required.
    // Keep accepted native damage untouched; only the optional presentation candidate is suppressed.
    else if(kcd2vr::gore32::decap.load(std::memory_order_relaxed)&&kcd2vr::gorepak50::dismember.load(std::memory_order_relaxed)&&
        kcd2vr::strike_feature_d41::severWound(r)&&r.death==0&&cutWhyD56==kcd2vr::gore_cut_d56::Why::Ok&&
        kcd2vr::gore32::LimbFromMaterial60(r.bodyMaterial)!=kcd2vr::gorepak50::Cut::None){
        char m[200]{};std::snprintf(m,sizeof(m),"R94 LIMB skipped serial=%llu target=%u reason=NOT_A_KILL power=%.2f bodyMaterial=%d",
            (unsigned long long)r.serial,r.target,cutD56.power,r.bodyMaterial);diagnostic_trace::Write(m);
    }
    const bool decap=g_enabled.load(std::memory_order_relaxed);
    const bool capture=Active()||kcd2vr::contact58c1::experiment.load(std::memory_order_relaxed);
    if(!decap&&!capture)return;
    namespace f=kcd2vr::finisher_observation_c2;
    const f::Input input{r.accepted,r.applied,r.fields,r.bodyContact,r.continuation,r.death,r.kind,
        r.bodyMaterial,p.surface,r.blocked,r.perfect,r.parry,p.energy,g_min_energy.load(std::memory_order_relaxed),before,after};
    const auto result=f::evaluate(input);
    const auto thrust=kcd2vr::contact58c1::lethalThrustReasonC2(r,r.tick,p.key,p.generation,p.epoch,p.request);
    if(decap)(result.candidate?g_candidates:g_refused).fetch_add(1,std::memory_order_relaxed);
    char line[960]{};std::snprintf(line,sizeof(line),
        "C10R18BC2 FINISHER_RECEIPT serial=%llu attempt=%llu target=%u route=%s accepted=%d applied=%d fields=%d continued=%d death=%d kind=%d body=%d bodyMaterialCandidate=%d submittedBodyMaterial=%d surfaceCandidate=%d energy=%.3f defense=%d/%d/%d phaseBefore=%d phaseAfter=%d decapCandidate=%d failures=0x%X impaleReceipt=%s materialDomain=UNCONFIRMED severBoundary=UNPROVEN phase=LOG_ONLY writes=NONE",
        (unsigned long long)r.serial,(unsigned long long)r.attempt,r.target,r.direct?"DIRECT":"LIVE",
        r.accepted,r.applied,r.fields,r.continuation,r.death,r.kind,r.bodyContact,r.bodyMaterial,p.bodyMaterial,p.surface,
        double(p.energy),r.blocked,r.perfect,r.parry,before,after,result.candidate,result.failures,kcd2vr::contact58c1::name(thrust));
    diagnostic_trace::Write(line);
}
} // namespace decap59c7e
