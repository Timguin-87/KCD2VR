// Actual current native-query adapter/projection; geometry/native memory are
// explicit synthetic leaves. This is not a replay of a dog's unrecorded mesh.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
static constexpr std::uintptr_t bodyD71=0x180004000ull,entityD71=0x180006000ull;
static void BodyD71(std::uintptr_t vf,bool gate=true){
    mockRetailLeaseD69=true;kcd2vr::contact_handover_d71::SetContactContinuity(gate);
    g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockMode=4;
    mockCollider172=reinterpret_cast<void*>(bodyD71);mockPart172=7;
    Store(bodyD71,base+vf);Store(base+vf+0x50,base+0x5d6f44);Store(bodyD71+0x20,entityD71);
    Store(entityD71,base+0x4072c78);Store(entityD71+0xC,std::uint32_t(45099));
}
int main(){
    for(auto vf:{0x3a82570ull,0x3a48b30ull})for(float origin:{0.f,3208.f})for(bool enabled:{false,true}){
        Init();BodyD71(vf,enabled);HandContact6695 c{};c.have=c.rawFeatureReady55=true;c.lastCompleteTickH18=1000;
        c.featureCount=5;c.worldAllowed={origin,.05f,0};c.releaseRawWorldD45={origin+.15f,.15f,0};
        for(auto& f:c.rawFeatureWorld55)f=c.releaseRawWorldD45;
        c.planeCount=1;c.blocked=true;c.planes[0].feature=0;
        const float shifted=origin?std::nextafter(origin,-INFINITY):-.00002f;
        wallPoint={shifted+.008f,0,0};wallNormal={-1,0,0};
        V3 offsets[7]{},raw{origin+.15f,.151f,0},out{};std::uint64_t previous=0;
        {ContactPass6695 pass(true,1011,previous,2,3);HandRigidContactScopeD67R1 handQuery;
            g_npc_query172.requested=true;
            auto h=WorldRayDistance6692({origin-.5f,0,0},{1,0,0});
            Check(h.state==wc::Result::Hit&&h.plane&&npc172::bodySurfaceIdentityD71(h.native172),
                  "current exact living/articulated native hit supplies body/part identity");
            Check(CurrentBodyPlaneD71(h.native172)==enabled,"OFF remains original body policy; ON admits only current front face");
            c.planes[0]={h.point,h.normal,0,h.witness169r1,h.native172};
            out=ProjectPalmPlanes6697(c,raw,offsets,.008f,&raw);
            Check(enabled?(Len(Sub(out,c.worldAllowed))<.0015f&&std::abs(out.y-.051f)<.0001f):out.y>.14f,
                  "same1mm motion: gated local projection avoids spending100mm stored body penetration; OFF reproduces baseline");
            Check(Dot(c.planes[0].normal,Sub(out,c.planes[0].point))>=.0078f,"current surface remains authoritative at existing positive clearance");
            std::printf("BODY_COUNTEREXAMPLE vf=%llx gate=%d origin=%.0f inputMm=%.4f acceptedMm=%.4f\n",
                static_cast<unsigned long long>(vf),enabled,origin,1000*Len(Sub(raw,c.releaseRawWorldD45)),1000*Len(Sub(out,c.worldAllowed)));
            if(enabled){
                auto stale=h.native172;--stale.bodyCurrentSampleD71;
                Check(!CurrentBodyPlaneD71(stale),"stale sample is never current body geometry");
                stale=h.native172;--stale.bodyCurrentTickD71;
                Check(!CurrentBodyPlaneD71(stale),"stale tick is never current body geometry");
                wallPoint={origin+.001f,0,0};wallNormal={1,0,0};
                const auto moved=WorldRayDistance6692({origin+.5f,0,0},{-1,0,0});
                c.planes[0]={moved.point,moved.normal,0,moved.witness169r1,moved.native172};
                out=ProjectPalmPlanes6697(c,raw,offsets,.008f,&raw);
                Check(std::abs(out.x-(origin+.009f))<.0003f&&out.y<.052f,
                      "current moving surface pushes full9mm normal penetration without tangential jump or extra filter");
            }
        }
    }
    // Native normal points out of the solid. The presentation normal gets
    // flipped for an inside-origin/back-face ray; that flip must NOT authorize
    // the new locality policy. It remains a hit, never fabricated clearance.
    for(int bad=0;bad<8;++bad){
        Init();BodyD71(0x3a48b30);std::uint64_t previous=0;
        if(bad==1)Store(bodyD71+0x20,std::uintptr_t(0));
        if(bad==2)mockPart172=-1;
        if(bad==3)Store(entityD71+0xC,std::uint32_t(0));
        if(bad==4)mockRetailLeaseD69=false;
        if(bad==7)diagnostics::lab::Set(diagnostics::lab::Key::F3,1);
        if(bad==5)Store(bodyD71,base+0x3a48dc0); // rigid needs its own stronger receipt
        {ContactPass6695 pass(true,1011,previous,2,3);HandRigidContactScopeD67R1 handQuery;
            g_npc_query172.requested=true;
            if(bad==6)kcd2vr::contact_handover_d71::SetContactContinuity(false);
            const auto h=bad==0?WorldRayDistance6692({.5f,0,0},{-.5f,0,0}):WorldRayDistance6692({0,0,0},{.5f,0,0});
            Check(h.state==wc::Result::Hit&&h.plane&&!CurrentBodyPlaneD71(h.native172),
                  "backface or missing exact identity/image/gate rejects locality while retaining collision");
            if(bad==0)Check(h.normal.x>0&&h.nativeNormalD63.x<0,"fixture proves outward witness uses unflipped native normal");
        }
    }
    // A finger can be overtaken without the palm touching. The OLD feature
    // nominates a NEW query only; current owner/part/geometry decide the stop.
    for(auto vf:{0x3a82570ull,0x3a48b30ull}){
        Init();BodyD71(vf);V3 features[6]{};for(auto& f:features)f={.14f,0,0};features[0]={.04f,0,0};
        FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};HandContact6695 c{};
        V3 out{},raw{.14f,0,0};std::uint64_t previous=0;wallPoint={.145f,0,0};
        {ContactPass6695 pass(true,1000,previous,238014,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&c.blocked&&c.fresh169r1,
                  "exact body fingertip establishes current contact through production adapter");}
        auto accepted=out;int beforeCalls=mockCalls;wallPoint.x=.136f;raw.y+=.00246f;mockPart172=8;
        {ContactPass6695 pass(true,1011,previous,238018,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&c.blocked&&c.fresh169r1&&
                std::abs(out.x-accepted.x)<.0001f&&std::abs(out.y-raw.y)<.0001f,
                  "new body part stops further entry without borrowing old-part displacement or moving wrist");
            Check(c.planeCount>0&&c.planes[0].native172.part==8,"new part is read from fresh query, not inherited from prior part");}
        Check(mockCalls-beforeCalls<=14,"part requalification remains inside existing bounded shared ray budget");
        accepted=out;beforeCalls=mockCalls;
        {ContactPass6695 pass(true,1012,previous,238018,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&
                mockCalls==beforeCalls&&Len(Sub(out,accepted))<.0001f,"repeat callback neither re-integrates nor moves old planes");}
        {ContactPass6695 pass(true,1022,previous,238022,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},{-.2f,0,0},out,features,&geometry)&&!c.blocked&&std::abs(out.x+.2f)<.0001f,
                  "deliberate withdrawal follows controller immediately without added hand lag");}
        mockMode=0;
        {ContactPass6695 pass(true,1033,previous,238026,3);
            const V3 free{-1.f,.6f,.2f};Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},free,out,features,&geometry)&&
                out.x==free.x&&out.y==free.y&&out.z==free.z,"free air remains exact current controller pose even for a fast move");}
    }
    // Exercise the changed-part FRESH PALM branch itself, not just the lost
    // fingertip recovery. Start with baseline contact then enable the trial;
    // no inherited body release receipt can change this candidate's first ray.
    for(int variant=0;variant<3;++variant){
        Init();BodyD71(0x3a48b30,false);V3 features[5]{};
        for(auto& f:features)f={.14f,0,0};features[0]={.04f,0,0};
        HandContact6695 c{};V3 out{};std::uint64_t previous=0;wallPoint={.148f,0,0};
        {ContactPass6695 pass(true,1000,previous,1,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},{.2f,0,0},out,features)&&c.blocked,
                  "fresh-palm fixture begins with ordinary contact on old part");}
        kcd2vr::contact_handover_d71::SetContactContinuity(variant!=0);
        mockPart172=8;wallPoint.x=.08f;const auto first=mockOrigins55.size();
        if(variant==2)Store(entityD71+0xC,std::uint32_t(45103));
        {ContactPass6695 pass(true,1011,previous,2,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},{.201f,.001f,0},out,features),
                  "changed-part fixture completes current contact transaction");}
        Check(mockOrigins55.size()>first+1,"fixture includes the mandatory palm and first finger ray");
        Check(variant==1?c.bodyPartRequeryMaskD71!=0:c.bodyPartRequeryMaskD71==0,
              "copied trial diagnostic records only current qualified changed-part requery");
        const auto origin=mockOrigins55[first+1];
        Check(variant==1?origin.x<.04f:origin.x>.13f,
              "only gated fresh-front-face SAME body changed part redirects fingertip from outside wrist; OFF/changed identity retains baseline");
    }
    // Disabling the trial must also disable its saved release candidate on the
    // very next complete pass. No mode-dependent positional filter lingers.
    for(bool keepEnabled:{false,true}){
        Init();BodyD71(0x3a82570);HandContact6695 c{};V3 out{},features[5]{};std::uint64_t previous=0;
        {ContactPass6695 pass(true,1000,previous,1,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{0,0,0},{.4f,0,0},out,features)&&
                  c.releaseBodySurfaceD71,"current qualified body contact records separate gated release receipt");}
        const auto accepted=out;mockMode=0;kcd2vr::contact_handover_d71::SetContactContinuity(keepEnabled);
        {ContactPass6695 pass(true,1011,previous,2,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{0,0,0},{.4f,0,0},out,features)&&!c.blocked,
                  "toggle case re-queries all required geometry and obtains current clearance");}
        Check(keepEnabled?Len(Sub(out,accepted))<=.0161f:out.x==.4f,
              "ON bounds only cleared contact spring-back; OFF immediately restores original unfiltered return");
        if(!keepEnabled)Check(!c.releaseBodySurfaceD71,"OFF clear endpoint retires trial release history");
    }
    // D72: this synthetic captured curve has two feasible curl branches on
    // opposite sides of a measured surface. Continuing contact follows the
    // physically connected branch instead of teleporting through the solid.
    {namespace fp=kcd2vr::pose_core;fp::FingerCurve103 curve{};curve.count=3;
        curve.offset[0]={.02,0,0};curve.offset[1]={.08,0,0};curve.offset[2]={.02,0,0};
        curve.open[0]=fp::from_rotation_vector({0,0,-1.5707963267948966});
        curve.closed[0]=fp::from_rotation_vector({0,0,1.5707963267948966});
        fp::FingerSurface103 plane{{.06,0,0},{-1,0,0}};
        auto continued=fp::fit_finger_surfaces_d72(curve,1.,0.,{0,0,0,1},{},&plane,1,.008);
        Check(continued.valid&&continued.fits&&continued.curl>0&&continued.curl<.2,
              "same-current-surface curl stops at the first physical boundary rather than crossing an infeasible interval");
        auto replacement=fp::fit_finger_surfaces_d72(curve,1.,-1.,{0,0,0,1},{},&plane,1,.008);
        Check(replacement.fits&&replacement.curl==1.,"a new surface never inherits the previous contact branch");
        plane.point.x=1.;auto clear=fp::fit_finger_surfaces_d72(curve,1.,0.,{0,0,0,1},{},&plane,1,.008);
        Check(clear.fits&&clear.curl==1.,"a completely feasible current path accepts controller curl immediately without filtering");
        HandContact6695 before{};before.fingerContact[1]=true;before.lastCompleteTickH18=1000;before.planeCount=1;
        before.planes[0].feature=2;auto& witness=before.planes[0].native172;
        witness.collider=bodyD71;witness.part=3;witness.owner=entityD71;witness.entityId=42;
        auto now=before.planes[0];g_contact_query_tick_h18=1011;
        Check(SameFingerSurfaceD72(before,now,1)&&!SameFingerSurfaceD72(before,now,0),
              "continuation is qualified per digit by an exact current body and part hit");
        ++now.native172.entityId;Check(!SameFingerSurfaceD72(before,now,1),"entity replacement revokes prior curl seed");
        now=before.planes[0];++now.native172.part;Check(!SameFingerSurfaceD72(before,now,1),"changed part must establish its own feasible branch");
        now=before.planes[0];g_contact_query_tick_h18=1081;Check(!SameFingerSurfaceD72(before,now,1),"expired contact has no retained curl authority");
    }
    std::printf("D71 body contact %d/%d; actual adapter/projection, geometry=SIMULATED native=MOCKED\n",assertions-failures,assertions);
    return failures?1:0;
}
