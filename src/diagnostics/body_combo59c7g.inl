// Included inside geom27 after Pending. Native game thread only. No native
// writes, shared recipe edits, damage calls, per-frame scans or log dependency.
static kcd2vr::contact58c1::Receipt PublishContact59c7g(const Pending& p,const InjectResult35& result,std::uint64_t now,
 const void* targetEntity,int origin,bool direct,bool applied,bool continued,int phaseBefore=-1,int phaseAfter=-1,const void* henryStateD85=nullptr) noexcept {
 namespace bc=kcd2vr::bodycombo59c7g;
 const auto r=kcd2vr::contact59c7g::makeReceipt(p,result,now,targetEntity?EntityId(targetEntity):0,
     reinterpret_cast<std::uintptr_t>(targetEntity),origin,direct,applied,continued);
 (void)kcd2vr::contact58c1::latest.publish(r);
 if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
  const char* verdict=!r.fields?"NATIVE_RESULT_FIELDS_UNAVAILABLE":!r.accepted?"NATIVE_REFUSED":!r.applied?"NATIVE_NOT_APPLIED":r.master>0?"NATIVE_MASTERSTRIKE":r.perfect>0?"NATIVE_PERFECT_BLOCK":r.parry>0?"NATIVE_PARRY":r.blocked>0?"NATIVE_BLOCK":r.damage>0?"NATIVE_HEALTH_DAMAGE":r.stamina>0?"NATIVE_STAMINA_ONLY":"NATIVE_ZERO_HEALTH_AND_STAMINA";
  char line[1200]{};std::snprintf(line,sizeof(line),"H29 STRIKE_RESULT tick=%llu gesture=%llu sample=%llu serial=%u target=%u kind=%s originZone=%d executedZone=%d rawPart=%d nativePart=%d originalBodyMaterial=%d resolvedMaterial=%d materialSource=%s npcPhaseBefore=%d npcPhaseAfter=%d route=%s fields=%d accepted=%d applied=%d healthDamage=%.5f staminaDamage=%.5f block=%d perfect=%d parry=%d master=%d death=%d reactionDispatch=%d combo=%d comboEnd=%d verdict=%s join=SAME_STACK_ORIGINAL_CONTACT renderedReaction=NOT_INFERRED",
   (unsigned long long)now,(unsigned long long)p.gesture59c3,(unsigned long long)p.sample,p.serial,p.entityId,p.kind?"STAB":"CUT",p.metadata59c7g.originZone47,origin,r.rawPart,r.nativePart,r.bodyMaterial,r.material,r.bodyMaterial>=0?"ORIGINAL_COLLISION_WITNESS":"NATIVE_ZONE_FALLBACK_OR_UNKNOWN",phaseBefore,phaseAfter,direct?"DIRECT_NATIVE_CONTINUATION":"HYBRID_NATIVE_LIVE",r.fields,r.accepted,r.applied,double(r.damage),double(r.stamina),r.blocked,r.perfect,r.parry,r.master,r.death,r.dispatch,r.combo,r.comboEnd,verdict);diagnostic_trace::Write(line);
  if(henryStateD85){ // D85 zone: origin / sent / committed / executed for the same strike (F3 only)
   namespace zd=::kcd2vr::zone_d85;zd::Snapshot s{I(henryStateD85,zd::kRequestedZone),I(henryStateD85,zd::kCommittedZone),
    I(henryStateD85,zd::kRowAttackZone),I(henryStateD85,zd::kRowInputClass)};
   char zl[400]{};zd::formatRow(zl,sizeof(zl),now,p.gesture59c3,direct?"DIRECT_NATIVE_CONTINUATION":"HYBRID_NATIVE_LIVE",
    p.metadata59c7g.originZone47,zd::sentFor(p.gesture59c3),s);diagnostic_trace::Write(zl);}
 }

#include "../subsystems/gore/completed_receipt_observer.inl" // M2_OWNED_SOURCE
#include "../subsystems/masterstrike_combos/body_recipe_observer.inl" // M2_OWNED_SOURCE
 return r;
}
