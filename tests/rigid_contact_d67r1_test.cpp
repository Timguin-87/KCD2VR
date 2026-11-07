// Production native-query adapter + projection, synthetic exact entity/body
// memory and surfaces. Full scene geometry was not recorded in D66; this is a
// reproduced policy counterexample, not a headset or native physics replay.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#ifndef KCD2VR_ADAPTER_FIXTURE
#define KCD2VR_ADAPTER_FIXTURE "world_adapter_test.cpp"
#endif
#include KCD2VR_ADAPTER_FIXTURE
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
static constexpr std::uintptr_t bodyR=0x180004000ull,entityR=0x180006000ull,proxyR=0x180008000ull,
    headR=0x180009000ull,nodeR=0x18000A000ull,partsR=0x18000B000ull,geometryR=0x18000C000ull;
static void RigidMemory(){
    mockRetailLeaseD69=true;
    g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockMode=4;mockCollider172=reinterpret_cast<void*>(bodyR);
    Store(bodyR,base+0x3a48dc0);Store(base+0x3a48dc0+0x50,base+0x5d6f44);Store(bodyR+0x20,entityR);
    Store(bodyR+0x28,std::int16_t(2));Store(bodyR+0x44,std::uint32_t(123));Store(entityR,base+0x4072c78);
    Store(entityR+0xC,std::uint32_t(983380));Store(entityR+0xa8,headR);Store(headR+8,nodeR);
    Store(headR+0x19,static_cast<unsigned char>(1));Store(nodeR+0x19,static_cast<unsigned char>(0));
    Store(nodeR+0x20,1u);Store(nodeR+0x28,proxyR);Store(nodeR,headR);Store(nodeR+0x10,headR);
    Store(proxyR,base+0x3ac3478);Store(proxyR+0x38,bodyR);Store(bodyR+0x118,partsR);Store(bodyR+0x120,1);
    Store(partsR+0x40,geometryR);Store(partsR+0x48,0);
    Store(partsR,std::array<float,8>{0,0,0,0,0,0,1,1});
    Store(entityR+0x58,std::array<float,12>{1,0,0,0,0,1,0,0,0,0,1,0});
    Store(base+0x4072c78+0x100,base+0x1a74140);
    Store(base+0x1a74140,std::array<unsigned char,5>{0x48,0x8d,0x41,0x58,0xc3});
    Store(base+0x3d57c6,std::array<unsigned char,6>{0x3b,0x8b,0x20,0x01,0,0});
    Store(base+0x3d57f0,std::array<unsigned char,7>{0x49,0x69,0xd2,0x88,0,0,0});
}
static npc172::Witness RigidWitness(){npc172::Witness w{};w.mask=npc172::BodiesAndWorld;w.body=npc172::Body::Rigid;
    w.vft=0x3a48dc0;w.collider=bodyR;w.ownerRead=true;w.owner=entityR;w.entityId=983380;w.part=0;return w;}
int main(){
    for(float origin:{0.f,3208.f}){
        Init();RigidMemory();HandContact6695 c{};c.have=c.rawFeatureReady55=true;c.lastCompleteTickH18=1000;
        c.featureCount=5;c.worldAllowed={origin,.05f,0};c.releaseRawWorldD45={origin+.15f,.15f,0};
        for(auto& f:c.rawFeatureWorld55)f=c.releaseRawWorldD45;
        c.planeCount=1;c.blocked=true;c.planes[0].feature=0;
        const float shifted=origin?std::nextafter(origin,-INFINITY):-.00002f;
        wallPoint={shifted+.008f,0,0};wallNormal={-1,0,0};V3 offsets[7]{},raw{origin+.15f,.151f,0},out{};std::uint64_t previous=0;
        {ContactPass6695 pass(true,1011,previous,2,3);
#ifdef KCD2VR_RIGID_CURRENT_D67R1
            HandRigidContactScopeD67R1 handQuery;
#endif
            g_npc_query172.requested=true;
            const auto h=WorldRayDistance6692({origin-.5f,0,0},{1,0,0});
            Check(h.state==wc::Result::Hit&&h.plane&&h.native172.body==npc172::Body::Rigid,
                  "counterexample obtains a current exact rigid witness from production adapter");
            c.planes[0]={h.point,h.normal,0,h.witness169r1,h.native172};
            out=ProjectPalmPlanes6697(c,raw,offsets,.008f,&raw);
            Check(Len(Sub(out,c.worldAllowed))<.0015f&&out.y>.0509f&&out.y<.0511f,
                  "D67 regression:1mm current raw motion cannot spend100mm old rigid penetration");
            Check(Dot(c.planes[0].normal,Sub(out,c.planes[0].point))>=.0078f,
                  "rigid locality remains outside the measured surface at existing skin");
            std::printf("COUNTEREXAMPLE origin=%.0f rawDeltaMm=%.6f acceptedStepMm=%.6f\n",origin,
                1000*Len(Sub(raw,c.releaseRawWorldD45)),1000*Len(Sub(out,c.worldAllowed)));
#ifdef KCD2VR_RIGID_CURRENT_D67R1
            Check(CurrentRigidPlaneD67R1(h.native172),"exact active owner body part and stable transform qualify current plane");
            auto stale=h.native172;--stale.rigidCurrentSampleD67R1;
            Check(!CurrentRigidPlaneD67R1(stale),"old sample cannot turn a stored rigid plane into current geometry");
            stale=h.native172;--stale.rigidCurrentTickD67R1;
            Check(!CurrentRigidPlaneD67R1(stale),"different tick does not qualify as the active query pass");
#endif
            // A NEW measured plane has moved9mm into the previous accepted
            // wrist. Its full current penetration must still be pushed out.
            wallPoint={origin+.001f,0,0};wallNormal={1,0,0};
            const auto moved=WorldRayDistance6692({origin+.5f,0,0},{-1,0,0});
            c.planes[0]={moved.point,moved.normal,0,moved.witness169r1,moved.native172};
            out=ProjectPalmPlanes6697(c,raw,offsets,.008f,&raw);
            Check(std::abs(out.x-(origin+.009f))<.0003f&&out.y<.052f,
                  "new moving rigid plane pushes full9mm normal penetration while preserving local tangent intent");
        }
    }
#ifdef KCD2VR_RIGID_CURRENT_D67R1
    const auto rd=[](std::uintptr_t a,auto& v){return Read(reinterpret_cast<const void*>(a),v);};
    for(int bad=0;bad<13;++bad){Init();RigidMemory();auto w=RigidWitness();int index=0;
        switch(bad){
        case 0:Store(bodyR+0x28,std::int16_t(1));break;
        case 1:Store(proxyR+0x38,bodyR+8);break;
        case 2:Store(entityR+0xC,std::uint32_t(983381));break;
        case 3:Store(partsR+0x48,7);break;
        case 4:index=1;break;
        case 5:Store(partsR+0x40,std::uintptr_t(0));break;
        case 6:Store(partsR,std::array<float,8>{0,0,0,0,0,0,0,1});break;
        case 7:Store(entityR+0x58,std::array<float,12>{});break;
        case 8:Store(bodyR+0x44,std::uint32_t(0x7fffff));break;
        case 9:w.body=npc172::Body::Articulated;break;
        case 10:Store(bodyR+0x120,65);break;
        case 11:Store(base+0x1a74140,std::array<unsigned char,5>{});break;
        case 12:Store(bodyR+0x20,entityR+8);break;}
        Check(!npc172::qualifyRigidSurfaceD67R1(w,base,index,3,1022,npc172::rigidSurfaceContractD67R1(base,rd),rd)&&!npc172::rigidSurfaceReceiptD67R1(w),
              "unknown wrong owner body part shape transform or lifetime cannot enable rigid locality");
    }
    for(int change=0;change<4;++change){Init();RigidMemory();auto w=RigidWitness();unsigned seen=0;
        const auto changing=[&](std::uintptr_t a,auto& v){
            if(a==entityR+0x58&&++seen==2){
                if(change==0)Store(entityR+0x58,std::array<float,12>{1,0,0,.001f,0,1,0,0,0,0,1,0});
                if(change==1)Store(partsR,std::array<float,8>{.001f,0,0,0,0,0,1,1});
                if(change==2)Store(proxyR+0x38,bodyR+8);
                if(change==3)Store(partsR+0x40,geometryR+8);
            }return rd(a,v);};
        // The identity/part rejoin runs after each pose observation; a changed
        // second observation must not publish the first observation's receipt.
        Check(!npc172::qualifyRigidSurfaceD67R1(w,base,0,3,1022,npc172::rigidSurfaceContractD67R1(base,rd),changing),
              "source changing during current transform observation never publishes a coherence receipt");
    }
    {Init();RigidMemory();auto w=RigidWitness();unsigned pinReads=0,currentReads=0;
        const auto pins=[&](std::uintptr_t a,auto& v){++pinReads;return rd(a,v);};
        const auto current=[&](std::uintptr_t a,auto& v){++currentReads;return rd(a,v);};
        const bool contract=npc172::rigidSurfaceContractD67R1(base,pins);
        Check(npc172::qualifyRigidSurfaceD67R1(w,base,0,3,1022,contract,current),"instrumented exact one-node owner is qualified");
        std::printf("READ_COUNTS oneTimeContract=%u currentQualification=%u nativeQueriesAdded=0\n",pinReads,currentReads);
        Check(currentReads<=48&&pinReads<=8,"one-node current source qualification has bounded read work; image pins cache separately");}
    {Init();RigidMemory();mockRetailLeaseD69=false;
        Check(!RigidContactContractD67R1(base)&&g_rigid_contact_contract_d67r1.load()==0,
              "D69 pending exact-image lease cannot poison the immutable pin cache");
        mockRetailLeaseD69=true;
        Check(RigidContactContractD67R1(base)&&g_rigid_contact_contract_d67r1.load()==1,
              "D69 ready full-image lease admits exact contract after pending state");
        mockRetailLeaseD69=false;
        Check(!RigidContactContractD67R1(base)&&g_rigid_contact_contract_d67r1.load()==1,
              "D69 revoked image lease overrides earlier positive pin cache");}
    // Failed extra qualification keeps the native collision, not pass-through.
    Init();RigidMemory();Store(bodyR+0x28,std::int16_t(1));std::uint64_t prior=0;
    {ContactPass6695 pass(true,1011,prior,2,3);HandRigidContactScopeD67R1 handQuery;g_npc_query172.requested=true;
        const auto h=WorldRayDistance6692({0,0,0},{.5f,0,0});
        Check(h.state==wc::Result::Hit&&h.plane&&!CurrentRigidPlaneD67R1(h.native172),
              "unqualified rigid still collides; missing locality proof is never clearance");}
    Init();RigidMemory();prior=0;
    {ContactPass6695 pass(true,1011,prior,2,3);g_npc_query172.requested=true;
        const auto h=WorldRayDistance6692({0,0,0},{.5f,0,0});
        Check(h.state==wc::Result::Hit&&h.plane&&!CurrentRigidPlaneD67R1(h.native172)&&g_rigid_contact_contract_d67r1.load()==0,
              "ordinary weapon/body rays do not pay hand-only qualification or image pin reads");}
#endif
    // Integration with the real feature query/recovery pipeline. New planes,
    // not old rigid-plane transport, must catch a moving surface overtaking a tip.
    Init();RigidMemory();V3 features[6]{};for(auto& f:features)f={.14f,0,0};features[0]={.04f,0,0};
    FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};HandContact6695 c{};V3 out{},raw{.14f,0,0};std::uint64_t previous=0;
    wallPoint={.145f,0,0};
    {ContactPass6695 pass(true,1000,previous,238014,3);
        Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&c.blocked&&c.fresh169r1,
              "qualified rigid finger contact starts from actual query owner and native adapter");}
    const auto accepted=out;const auto beforeCalls=mockCalls;wallPoint.x=.136f;raw.y+=.00246f;
    Store(entityR+0x58,std::array<float,12>{1,0,0,-.009f,0,1,0,0,0,0,1,0});
    {ContactPass6695 pass(true,1011,previous,238018,3);
        Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&c.blocked&&c.fresh169r1&&
              out.x+.14f<=wallPoint.x-.0079f&&Len(Sub(out,accepted))<.0101f,
              "moving9mm rigid surface cannot lose the fingertip when palm remains clear");}
    Check(mockCalls-beforeCalls<=14,"rigid requalification preserves shared native ray budget");
    const auto admitted=out;const auto beforeRepeat=mockCalls;
    {ContactPass6695 pass(true,1012,previous,238018,3);
        Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&
              mockCalls==beforeRepeat&&Len(Sub(out,admitted))<.0001f,
              "repeated callback neither resamples rigid geometry nor transports old planes");}
    {ContactPass6695 pass(true,1022,previous,238022,3);
        Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},{-.2f,0,0},out,features,&geometry)&&!c.blocked&&std::abs(out.x+.2f)<.0001f,
              "clear deliberate withdrawal retires rigid contact immediately");}
    std::printf("D67R1 rigid contact %d/%d; geometry=SIMULATED native=MOCKED\n",assertions-failures,assertions);return failures?1:0;
}
