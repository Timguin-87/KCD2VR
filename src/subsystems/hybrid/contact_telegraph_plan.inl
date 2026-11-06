    const bool telegraph34=g_telegraph178r34.load(std::memory_order_relaxed)&&!NativeOwned178r33(now)&&!vrDirect596; // R59.6: the VR state driver already published the zone natively
    if(telegraph34){int z=g_tg_zone178r34.load(std::memory_order_relaxed);const auto tt=g_tg_tick178r34.load(std::memory_order_relaxed);
        if(p.kind==1)z=5;else if(!tt||now-tt>700||z<0)z=g_tg_written_zone178r34>=0?g_tg_written_zone178r34:I(hs,kAttackZone);
        TelegraphWrite34(hs,2,z);}
