// Existing checked native candidate and head/chat projection only; no hook,
// scan, extra candidate enumeration or modified native result.
static void ObserveQuickTalkCandidateD78(const Candidate57c1& native,bool inside,std::uint64_t now) noexcept {
 namespace q=kcd2vr::quick_talk_target_d78;
 if(!q::Observing(now)||!native.id||(std::strcmp(native.cls,"NPC")&&std::strcmp(native.cls,"Dog")))return;
 if(!inside){q::Candidate prior{};if(q::candidate.read(prior)&&prior.entity==native.id)(void)q::candidate.publish({});return;}
 const auto generation=DoorAcquisitionGeneration65();std::uintptr_t player=0,again=0;
 if(!generation||!q::Fresh(native.tick,now)||!SelectionVft68(native.player,0x3AA4B98)||
    !SelectionRead68(native.player+0x38,player)||!SelectionVft68(player,0x4072C78)||
    !SelectionRead68(native.player+0x38,again)||again!=player||
    !SelectionVft68(native.player,0x3AA4B98)||!DoorAcquisitionCurrent65(generation)||!q::Observing(now))return;
 (void)q::candidate.publish({player,now,generation,native.id});
}
