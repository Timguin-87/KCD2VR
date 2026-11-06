    { // C23b OUTCOME_CHAIN receipt: this direct submission's own gesture; apply==0 stays an evaluation (skipped).
        kcd2vr::outcome23::Hit h23{};h23.ok=ok;h23.apply=apply;h23.flagEdge=comboEdge19;
        if(ok&&apply&&directResult59c7g.fields){h23.parry=directResult59c7g.parry;h23.blocked=directResult59c7g.blocked;h23.perfect=directResult59c7g.perfect;
            h23.combo=directResult59c7g.combo;h23.comboEnd=directResult59c7g.comboEnd;h23.master=directResult59c7g.master;h23.death=directResult59c7g.death;
            h23.ko=Rd<std::uint8_t>(hitInfo,0x41,0);h23.target=directResult59c7g.target;h23.reaction=reaction23;
            h23.deathKind=reaction23>=0?Rd<std::int32_t>(reinterpret_cast<const void*>(g_base+0x5320D24ull),0,-1):-1;}
        const auto now23=std::uint64_t(GetTickCount64());
        kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.hit(p.gesture59c3,h23,now23,kcd2vr::outcome23::counters);});
    }
