static void FingerDiagnostics105(const CoherentState6689& s,int rung,const TrackingSnapshot& ts) noexcept {
    if(!kcd2vr::diagnostics::vrik_h29::selected()||!kcd2vr::diagnostics::vrik_h29::active(GetTickCount64()))return;
    static std::uint64_t lastDiag=0;static void* dumpedOwner=nullptr;static std::uint64_t dumpedApplied=~0ull;static bool iniAsked=false;
    if(rung!=1)iniAsked=false;
    if(rung==1&&!iniAsked){iniAsked=true;g_finger_ini_diag105.store(true,std::memory_order_release);}
    const char* source=g_capture_runtime66100.applied?"SAVED_INI_PROFILE":(s.open[0].ready&&s.closed[0].ready?"NATIVE_CURVE+BIND_OPEN":"NONE(native animation)");
    if(rung==1&&ts.tick-lastDiag>=4000){
        lastDiag=ts.tick;
        for(int side=0;side<2;++side){
            if(!s.fingerMap[side]||!s.open[side].ready||!s.closed[side].ready){
                char m[256]{};std::snprintf(m,sizeof(m),"V66.105-FINGER-DIAG side=%c endpoints=NOT_READY open=%s fist=%s fingerMap=%d",side?'L':'R',s.open[side].issue,s.closed[side].issue,s.fingerMap[side]?1:0);
                diagnostic_trace::Write(m);continue;}
            char m[900]{};int w=std::snprintf(m,sizeof(m),"V66.105-FINGER-DIAG side=%c source=%s bindBlend=%.2f/%.2f(thumb) degreesFromFist[native|bind|final]:",side?'L':'R',source,
                double(g_open_bind105.load()),double(g_open_bind_thumb105.load()));
            static const char* names[5]{"thumb","index","middle","ring","pinky"};
            int n=0;
            for(int f=0;f<5;++f){double a=0,b=0,c=0;const auto& chain=s.fingers[side][f];
                for(int k=0;k<chain.count;++k,++n){const int id=chain.id[k];
                    const auto open=PQ(QFromQuatT(s.open[side].local[n])),closed=PQ(QFromQuatT(s.closed[side].local[n])),bind=PQ(QFromQuatT(s.restLocal[id]));
                    const auto fin=RelaxedFinger101(open,closed,f,k,chain.count,g_capture_runtime66100.applied?nullptr:&bind);
                    a+=pc::angle(open,closed);b+=pc::angle(bind,closed);c+=pc::angle(fin,closed);}
                if(w>0&&w<int(sizeof(m))-60)w+=std::snprintf(m+w,sizeof(m)-w," %s=%.0f|%.0f|%.0f",names[f],a*57.29578,b*57.29578,c*57.29578);}
            diagnostic_trace::Write(m);
        }
        char m[256]{};std::snprintf(m,sizeof(m),"V66.105-FINGER-DIAG rigHash=%016llX savedApplied=%llu profileStatus=%d rung=1 writes=NONE",
            (unsigned long long)g_capture_runtime66100.rig,(unsigned long long)g_capture_runtime66100.applied,g_finger_profile_status101R2.load());
        diagnostic_trace::Write(m);
    }
    if(rung>=2&&(dumpedOwner!=s.owner||dumpedApplied!=g_capture_runtime66100.applied)&&s.open[0].ready&&s.closed[0].ready&&s.open[1].ready&&s.closed[1].ready&&s.fingerMap[0]&&s.fingerMap[1]){
        dumpedOwner=s.owner;dumpedApplied=g_capture_runtime66100.applied;
        char h[256]{};std::snprintf(h,sizeof(h),"V66.105-FINGER-HARDCODE BEGIN rigHash=%016llX source=%s format=side,index,digit,joint,id,parent,openQ(xyzw),fistQ(xyzw)",(unsigned long long)g_capture_runtime66100.rig,source);
        diagnostic_trace::Write(h);
        for(int side=0;side<2;++side){int n=0;
            for(int f=0;f<5;++f){const auto& chain=s.fingers[side][f];
                for(int k=0;k<chain.count;++k,++n){const int id=chain.id[k];
                    const auto open=PQ(QFromQuatT(s.open[side].local[n])),closed=PQ(QFromQuatT(s.closed[side].local[n])),bind=PQ(QFromQuatT(s.restLocal[id]));
                    const auto fin=RelaxedFinger101(open,closed,f,k,chain.count,g_capture_runtime66100.applied?nullptr:&bind);
                    char m[320]{};std::snprintf(m,sizeof(m),"V66.105-FINGER-HARDCODE %c,%d,%d,%d,%d,%d,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f",
                        side?'L':'R',n,f,k,id,s.parent[id],fin.x,fin.y,fin.z,fin.w,closed.x,closed.y,closed.z,closed.w);
                    diagnostic_trace::Write(m);}}}
        diagnostic_trace::Write("V66.105-FINGER-HARDCODE END");
    }
}
// V66.128 (a) helpers. Model-space position of one joint with its parent fixed.
static void SetWorldPosition128(CoherentState6689& s,int id,V3 p) noexcept {
    s.dirty[id]=true;const int par=s.parent[id];
    if(par<0){ToT(s.local[id],p);return;}
    ToT(s.local[id],QRotateV(QConj(QFromQuatT(s.work[par])),Sub(p,FromT(s.work[par]))));
}
// Two-bone IK with the rest thigh/shin lengths (never stretched). The knee bends in
// the plane of the supplied pole; the foot keeps the supplied world rotation.
static bool SolveLeg128(CoherentState6689& s,const CoherentState6689::Leg128& L,V3 target,V3 pole,Q4 footWorld,float& reachError) noexcept {
    const V3 hip=FromT(s.work[L.hip]);
    const V3 d=Sub(target,hip);const float dist=Len(d);if(!(dist>1e-3f))return false;
    const V3 dir=Mul(d,1.f/dist);
    const float lo=float(std::fabs(L.thigh-L.shin))+.01f,hi=float(L.thigh+L.shin)-.002f;
    const float c=std::clamp(dist,lo,hi);reachError=dist-c;target=Add(hip,Mul(dir,c));
    const float cosA=std::clamp(float((L.thigh*L.thigh+double(c)*c-L.shin*L.shin)/(2.0*L.thigh*c)),-1.f,1.f);
    const float sinA=std::sqrt(std::max(0.f,1.f-cosA*cosA));
    V3 bend=Sub(pole,Mul(dir,Dot(pole,dir)));if(Len(bend)<1e-3f)return false;bend=Normalize(bend);
    const V3 kneeWanted=Add(hip,Add(Mul(dir,float(L.thigh)*cosA),Mul(bend,float(L.thigh)*sinA)));
    SetWorldRotation6689(s,L.hip,QMul(DeltaRotation6689(Sub(FromT(s.work[L.knee]),hip),Sub(kneeWanted,hip)),QFromQuatT(s.work[L.hip])));
    if(!Compose6689(s))return false;
    const V3 knee=FromT(s.work[L.knee]);
    SetWorldRotation6689(s,L.knee,QMul(DeltaRotation6689(Sub(FromT(s.work[L.ankle]),knee),Sub(target,knee)),QFromQuatT(s.work[L.knee])));
    if(!Compose6689(s))return false;
    SetWorldRotation6689(s,L.ankle,footWorld);return Compose6689(s);
}
