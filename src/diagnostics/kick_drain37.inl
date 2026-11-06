// C37C1: the new procedural foot contact is observable, not permission to create a native kick.
// Native attack_special admission/effort split and held-weapon damage source remain unproven.
// Keep the one-click ledger and real contact receipt; never spend stamina or apply damage here.
namespace geom27 {
void KickDrain37(void* henryActor) noexcept {
    namespace k=kcd2vr::kick34;
    if(!k::ledger.pending())return;
    // Legacy real-hit settings cannot turn a foot contact into an unadmitted attack.
    const auto now=std::uint64_t(GetTickCount64());
    const void *entity=nullptr,*state=nullptr;
    if(!CombatActor(henryActor,entity,state,false)||!IsPlayer(entity))return;
    const auto c=k::ledger.claim(reinterpret_cast<std::uintptr_t>(henryActor),reinterpret_cast<std::uintptr_t>(state),now);
    if(!c.ready)return;
    (void)k::ledger.finish(c.binding.serial,k::Why::AdmissionUnproven);
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    char line[384]{};std::snprintf(line,sizeof(line),
        "C37C1 KICK_TX serial=%llu stage=CONTACT_OBSERVED target=%u part=%d body=%u bodyMaterial=%d ageMs=%llu nativeAdmission=UNPROVEN damage=NONE stamina=NONE",
        (unsigned long long)c.binding.serial,c.contact.entity,c.contact.part,c.contact.body,c.contact.bodyMaterial,
        (unsigned long long)(now>=c.contact.tick?now-c.contact.tick:0));
    diagnostic_trace::Write(line);
}
}
