    if(vrDirect596&&!contactContext5912.setStrength(strength)){
        char m[230]{};std::snprintf(m,sizeof(m),"R59.16C10C1 VR_HIT_SKIPPED serial=%u reason=CONTACT_STRENGTH_NOT_OWNED applied=0 claim=RELEASE_ON_EXIT",p.serial);
        diagnostic_trace::Write(m);return;
    }
