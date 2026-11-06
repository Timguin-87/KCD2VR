    const bool nativeFingerCapture=g_finger_capture66100.load(std::memory_order_acquire);
    FingerCaptureAndLoad66100(s,ts,nativeFingerCapture);
    // A saved native pair has priority over automatic CAF defaults. Capture itself
    // observes native fingers without depending on curve residency or completion.
    const bool nativeFingerProfile=g_capture_runtime66100.owner==s.owner&&g_capture_runtime66100.applied!=0;
    if(ts.allowed&&ts.head_valid&&!nativeFingerCapture&&!nativeFingerProfile){
        AcquireCurveReferences6694(s,anim,ts.tick,false);
    }
    else for(int side=0;side<2;++side){if(!s.open[side].ready)s.open[side].have=false;if(!s.closed[side].ready)s.closed[side].have=false;}
    if(tx.fingerTrialRequestedH7)LoadCapturedPunchH45(s);
    FingerDiagnostics105(s,rung,ts);
    if(rung==1) {
        if(!tx.traceH29)return RejectCoherent6693(tx,__LINE__);
        // A finite raw reference export is useful even when a CAF is not resident.
        // Four input categories per hand, at most once per category/rig; no pose
        // is silently promoted to an authored endpoint by these button values.
        static void* refOwner=nullptr;static unsigned emitted[2]{},lastKind[2]{99,99};static std::uint64_t stable[2]{};
        if(refOwner!=owner){refOwner=owner;emitted[0]=emitted[1]=0;lastKind[0]=lastKind[1]=99;}
        for(int side=0;side<2;++side){const float g=side?ts.left_grip:ts.right_grip,t=side?ts.left_trigger:ts.right_trigger;
            const unsigned kind=(g>.8f?1u:0u)|(t>.8f?2u:0u);
            if(lastKind[side]!=kind){lastKind[side]=kind;stable[side]=ts.tick;}
            if(ts.tick<stable[side]||ts.tick-stable[side]<350||(emitted[side]&(1u<<kind)))continue;
            emitted[side]|=1u<<kind;
            for(int i=0;i<s.fingerCount[side];++i){const int j=s.fingerIds[side][i],parent=s.parent[j];
                const auto q=RelativeFromAbsolute(s.original[parent],s.original[j]);char m[768]{};
                std::snprintf(m,sizeof(m),"V66.94-FINGER RAW_REFERENCE side=%c category=%u sample=%llu index=%d count=%d joint=%d parent=%d grip=%.3f trigger=%.3f q=(%.9g,%.9g,%.9g,%.9g) offset=(%.9g,%.9g,%.9g) provenance=NATIVE_EVALUATED_NOT_AUTHORED_ENDPOINT",
                    side?'L':'R',kind,(unsigned long long)ts.sequence,i,s.fingerCount[side],j,parent,g,t,q.q[0],q.q[1],q.q[2],q.q[3],q.t[0],q.t[1],q.t[2]);diagnostic_trace::Write(m);}
        }
        if(ts.tick-s.lastLog>=10000){s.lastLog=ts.tick;
            char m[384]{};std::snprintf(m,sizeof(m),"V66.89-FINGER-REF R={joints:%d open:%d fist:%d} L={joints:%d open:%d fist:%d} mapping=SHARED-PRODUCTION parentLocal=1 writes=NONE",s.fingerCount[0],s.open[0].ready,s.closed[0].ready,s.fingerCount[1],s.open[1].ready,s.closed[1].ready);diagnostic_trace::Write(m);}
        return RejectCoherent6693(tx,__LINE__);
    }
