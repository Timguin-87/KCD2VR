    bool comboEdge19=false;
    // A direct hit result is not proof that Henry's CURRENT action owns this
    // contact. Re-read the existing exact admitted-action/release reader AFTER
    // native submission; a callback may have changed state/action/target/mode.
    // Unbound fallback hits keep their native damage/reaction, without lending
    // a combo hit flag or grace to a different ordinary action.
    const auto comboAfter19=kcd2vr::combo_lineage19::valid(comboDirectOwner19)?
        ComboOwner19(p,henryActor,hs,target,p.action,std::uint64_t(GetTickCount64())):kcd2vr::combo_lineage19::Owner{};
    const bool comboOwnedH14=kcd2vr::combo_lineage19::valid(comboDirectOwner19)&&
        kcd2vr::combo_lineage19::valid(comboAfter19)&&kcd2vr::combo_lineage19::sameLineage(comboDirectOwner19,comboAfter19)&&
        comboDirectOwner19.action==comboAfter19.action&&comboDirectOwner19.release==comboAfter19.release&&
        comboDirectOwner19.key.gesture==comboAfter19.key.gesture;
    // Existing admitted F4 press-1/3 flag bridge only; never a recipe advance.
    if(ok&&apply&&syncDirect59c10r9&&g_combo_flag59c17.load(std::memory_order_relaxed)&&hs&&target==Rd<const void*>(hs,0xCF8,nullptr)&&
       comboOwnedH14&&p.lease47c2.serial&&g_combo_flag_serial59c17.load(std::memory_order_relaxed)!=p.lease47c2.serial&&vrstate596::Verified()){
        const int phase17=I(hs,kActionPhaseProp39);
        const auto before480=B(hs,0x480);
        if((phase17==4||phase17==8||phase17==16)&&vrstate596::SetBool(hs,0x480,true)){
            g_combo_flag_serial59c17.store(p.lease47c2.serial,std::memory_order_relaxed);g_combo_flags59c17.fetch_add(1,std::memory_order_relaxed);
            const bool edge=before480==0;comboEdge19=edge;
            if(edge){g_combo_edges19.fetch_add(1,std::memory_order_relaxed);}
            else if(before480==1)g_combo_same_value19.fetch_add(1,std::memory_order_relaxed);
            else g_combo_unknown_edge19.fetch_add(1,std::memory_order_relaxed);
            if(Allow(4,12)){char line[260]{};std::snprintf(line,sizeof(line),"C10R19C1 COMBO_FLAG serial=%u gesture=%llu before=%u after=1 changedEdge=%d nativeAdvance=UNOBSERVED sameValueReadback=NOT_AN_ADVANCE",
                p.serial,(unsigned long long)p.gesture59c3,unsigned(before480),edge?1:before480==1?0:-1);diagnostic_trace::Write(line);}}}
