// Actual physical::step guard exit and command producer, not a copied recognizer.
    const auto Guard32=[](Replay28& r,bool thrust=false){
        r.in.positionGuard176=true;r.in.guardEnterMaxSpeed178r29=2.5;r.in.guardExitSpeed178r45=2.9;
        r.profiles[0].axis=thrust?Vec3{.5,.866025403784,0}:Vec3{1,0,0};
        r.profiles[0].tip=r.profiles[0].axis*.6;
        r.in.hands[0].p={-.15,.25,1.35};r.in.intentHands[0]=r.in.hands[0];r.Rest(200);r.commands.clear();
        Check(r.out.guard117,"C32 fixture enters actual settled weapon guard");
    };
    {Replay28 r(false);Guard32(r);const auto delta=Vec3{.012,.035,0};
        r.Move(delta);r.Move(delta);
        Check(!r.Count(CommandKind::Prepare)&&!r.Hits(),"C32 HYBRID postguard 10ms candidate does not advertise a recovery twitch");
        r.Move(delta*-1.0);r.Rest(80);
        Check(!r.Count(CommandKind::Prepare)&&!r.Hits(),"C32 reversed short guard recovery ends without native input or a banked attack");
    }
    for(bool direct:{false,true})for(bool thrust:{false,true}){Replay28 r(direct);Guard32(r,thrust);
        const auto start=r.in.tick;const auto delta=thrust?r.profiles[0].axis*.022:Vec3{.012,.035,0};
        for(unsigned j=0;j<24;++j)r.Move(delta);
        Check(r.Count(CommandKind::Prepare)==1&&r.Count(thrust?CommandKind::Thrust:CommandKind::Slash)==1&&r.Hits()==1,
            "C32 deliberate cut/stab from guard still prepares and commits once in HYBRID and DIRECT");
        Check(r.First(CommandKind::Prepare)<r.First(thrust?CommandKind::Thrust:CommandKind::Slash)&&
              r.First(CommandKind::Prepare)<=start+60,
            "C32 no fixed recovery wait: existing travel qualification advertises before commitment");
        if(direct)std::printf("C59 postguard thrust=%d firstPrepareMs=%llu firstCommitMs=%llu\n",thrust,
            (unsigned long long)(r.First(CommandKind::Prepare)-start),(unsigned long long)(r.First(thrust?CommandKind::Thrust:CommandKind::Slash)-start));
        if(direct)Check(thrust?r.First(CommandKind::Prepare)<=start+30:r.First(CommandKind::Prepare)==start+50,
            "C59 DIRECT guarded cut waits for actual hand/path proof; axial stab retains early onset");
    }
    {Replay28 r(false);const auto onset=r.in.tick+10;r.Move({.025,.012,0});
        Check(!r.Count(CommandKind::Prepare),"C42 one short hybrid sample cannot announce a cut");
        for(int i=0;i<4;++i)r.Move({.025,.012,0});
        Check(r.First(CommandKind::Prepare)>onset&&r.First(CommandKind::Prepare)<=onset+60,"C32 HYBRID outside guard retains original first qualified onset");
    }
    {Replay28 r(false);Guard32(r);r.Move({.012,.035,0});
        r.in.hands[0].p={-.1,.1,.5};r.in.intentHands[0]=r.in.hands[0];r.Rest(350);r.commands.clear();
        const auto onset=r.in.tick+10;for(int i=0;i<5;++i)r.Move({.012,.035,0});
        Check(r.First(CommandKind::Prepare)>=onset&&r.First(CommandKind::Prepare)<=onset+60,"C32 postguard onset probation expires; no lasting rearm/input delay");
    }
    {Replay28 r(false);Guard32(r);r.Move({.012,.035,0});++r.profiles[0].generation;
        r.in.hands[0].p={-.1,.1,.5};r.in.intentHands[0]=r.in.hands[0];r.Rest(100);r.commands.clear();
        const auto onset=r.in.tick+10;for(int i=0;i<5;++i)r.Move({.012,.035,0});
        Check(r.First(CommandKind::Prepare)>=onset&&r.First(CommandKind::Prepare)<=onset+60,"C32 item generation replacement cannot inherit old guard probation");
    }
    {Replay28 r(false);Guard32(r);r.Move({.012,.035,0});r.Move({.012,.035,0});
        for(int i=0;i<3;++i)r.Move({.012,.035,0});
        r.blocked=r.actor=true;r.Move({.012,.035,0});
        Check(r.Count(CommandKind::Prepare)==1&&r.Count(CommandKind::Slash)==1,
            "C32 qualified first actor contact from guard still bypasses free-space preparation distance");
    }
