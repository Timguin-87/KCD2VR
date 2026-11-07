// Actual combo display branch SHA256 22ae6db622ff668fdc0b01131f8764fde9a35269b93aaabd677159d070db2d22
static void ComboGoldD74(std::uint64_t now){
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,now)){
        char text[96]{};std::snprintf(text,sizeof(text),"combo %s %u/%u: %s%s",pick.recipe->id,pick.step+1,pick.required,
            cz::zoneText(pick.zone),cz::kindText(pick.recipe->steps[pick.step]));
        (void)kcd2vr::visible_failure::publishDiagnosticD74(text,now,diagnostics::capture168::Domain::Bug);
    }
}
// Actual holster display branch SHA256 8c36e047b23c9d83b6646283a4468df830f560eb92022e4fbb6eb0126074e332
static void HolsterGoldD74(bool reachedZoneR9,std::uint64_t now){
        if(reachedZoneR9&&kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Bug,now)&&hd::visible_due(diag,now)){
            char t[kcd2vr::visible_failure::kMaxText+1]{};hd::visible_text(a,t,sizeof(t));
            namespace cap=kcd2vr::diagnostics::capture168;
            (void)kcd2vr::visible_failure::publishDiagnosticD74(t,now,cap::Domain::Bug);
        }
}
// Actual F5 producer gate + display publication SHA256 e23a071ed26c75a28eecb34e97a51c5cadcb87a0e30de79133f80e1df56799f1
static void CraftGoldD74(bool visible,std::uint64_t now){
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
        if(visible&&r.firstMissing!=ca::Stage::Count)(void)visible_failure::publishDiagnosticD74(why,now,diagnostics::capture168::Domain::Bug);
}
