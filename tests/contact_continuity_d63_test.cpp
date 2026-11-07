// New live-log cases through the actual production hand/native-query adapter.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
int main(){
    for(const auto vft:{0x3a578c0ull})for(bool entityOwned:{false,true}){
        Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;
        const std::uintptr_t body=0x180004000ull,entity=0x180006000ull;
        mockCollider172=reinterpret_cast<void*>(body);Store(body,base+vft);
        Store(base+vft+0x50,base+0x5d6f44);Store(body+0x20,entityOwned?entity:std::uintptr_t(0));
        if(entityOwned){Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(952226));}
        std::uint64_t previous=0;HandContact6695 c{};V3 out{},features[5]{};
        const auto ch=reinterpret_cast<void*>(character);const V3 raw{.4f,0,0};mockMode=4;
        {ContactPass6695 pass(true,1000,previous,1,7);Check(ContactHand6695(c,ch,1,{0,0,0},raw,out,features)&&c.blocked,"current measured contact establishes the correction");}
        auto last=out;mockMode=0;
        for(unsigned sample=2;sample<20;++sample){
            // Actual Windows pattern: multiple distinct XR families share a tick.
            const auto tick=1000+16*((sample-2)/2);
            {ContactPass6695 pass(true,tick,previous,sample,7);Check(ContactHand6695(c,ch,1,{0,0,0},raw,out,features)&&c.fresh169r1&&!c.planeCount&&!c.blocked,"every return candidate gets current native clearance; no old plane survives");}
            Check(Len(Sub(out,last))<=.0161f,"clock quantization and entity ownership cannot snap the whole contact correction");
            if(sample%2==1)Check(Len(Sub(out,last))<.0001f,"same-tick new XR family spends no second spring step");
            last=out;
        }
        mockMode=0;
        {ContactPass6695 pass(true,1300,previous,20,7);Check(ContactHand6695(c,ch,1,{0,0,0},{-.1f,0,0},out,features)&&Len(Sub(out,{-.1f,0,0}))<.0001f,"deliberate withdrawal and expired correction retain ordinary tracking");}
        // A moving current surface must still push out its own new location.
        Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockCollider172=reinterpret_cast<void*>(body);
        Store(body,base+vft);Store(base+vft+0x50,base+0x5d6f44);Store(body+0x20,entityOwned?entity:std::uintptr_t(0));
        if(entityOwned){Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(952226));}
        previous=0;ResetHandContact6695(c);mockMode=4;
        {ContactPass6695 pass(true,1000,previous,1,7);ContactHand6695(c,ch,1,{0,0,0},raw,out,features);}
        wallPoint.x=.255f;
        {ContactPass6695 pass(true,1016,previous,2,7);Check(ContactHand6695(c,ch,1,{0,0,0},raw,out,features)&&c.blocked&&out.x<=.2471f,"new current surface intersecting the release path remains authoritative");}
    }
    // Exact owner/body class from the bench contact at log line632336. The
    // current native leaf is a modeled plane: rawF7 motion/ticks are real,
    // full scene geometry was not logged and is NOT claimed to be replayed.
    for(bool seven:{false,true})for(float tangent:{0.f,.00246f}){
        Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockMode=4;
        const std::uintptr_t body=0x180004000ull,entity=0x180006000ull;
        mockCollider172=reinterpret_cast<void*>(body);Store(body,base+0x3a578c0);
        Store(base+0x3a578c0+0x50,base+0x5d6f44);Store(body+0x20,entity);
        Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(952226));
        V3 features[6]{};for(auto& f:features)f={.14f,0,0};features[0]={.04f,0,0};
        FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};
        if(seven)for(int f=1;f<5;++f){geometry.curve[f].count=3;
            geometry.curve[f].offset[0]={.07,0,0};geometry.curve[f].offset[1]={.07,0,0};geometry.input[f]=1;}
        HandContact6695 c{};V3 raw{.14f,0,0},out{};std::uint64_t previous=0;
        wallPoint={.145f,0,0};
        {ContactPass6695 pass(true,1000,previous,238014,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&c.blocked&&c.fresh169r1,
                "qualified BASE finger contact starts from the actual native query adapter");}
        auto accepted=out;const auto beforeCalls=mockCalls;wallPoint.x=.136f;raw.y+=tangent;
        {ContactPass6695 pass(true,1011,previous,238018,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&c.blocked&&c.fresh169r1&&
                out.x+.14f<=wallPoint.x-.0079f&&Len(Sub(out,accepted))<.0101f,
                "a9mm BASE surface overtaking a fingertip cannot disappear when the palm remains clear");}
        Check(mockCalls-beforeCalls<=14,"lost-feature requalification shares the original bounded native query budget");
        auto current=out;const auto calls=mockCalls;
        {ContactPass6695 pass(true,1012,previous,238018,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,&geometry)&&mockCalls==calls&&Len(Sub(out,current))<.0001f,
                "repeated family keeps its current BASE result without a second contact integration");}
        {ContactPass6695 pass(true,1022,previous,238022,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},{-.2f,0,0},out,features,&geometry)&&!c.blocked&&std::abs(out.x+.2f)<.0001f,
                "current clear withdrawal retires BASE geometry immediately");}
    }
    // A measured palm stop changes the actual wrist for later finger rays.
    // Fit at that wrist, not at the controller request behind the wall. The
    // latter falsely reports every curl impossible and pulls the arm back by
    // a full finger length instead of giving that finger natural resistance.
    {
        Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockMode=4;
        const std::uintptr_t body=0x180004000ull,entity=0x180006000ull;
        mockCollider172=reinterpret_cast<void*>(body);Store(body,base+0x3a578c0);
        Store(base+0x3a578c0+0x50,base+0x5d6f44);Store(body+0x20,entity);
        Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(952204));
        V3 features[6]{};for(auto& f:features)f={.05f,0,0};features[1]={.15f,0,0};
        FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};
        auto& curve=geometry.curve[0];curve.count=3;
        for(int k=0;k<3;++k)curve.offset[k]={.05,0,0};
        curve.closed[0]=curve.closed[1]={0,0,std::sqrt(.5),std::sqrt(.5)};
        geometry.enabled[0]=true;geometry.input[0]=0;wallPoint={.2f,0,0};
        HandContact6695 c{};V3 out{};std::uint64_t previous=0;
        {ContactPass6695 pass(true,1000,previous,258166,3);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{0,0,0},{.3f,0,0},out,features,&geometry)&&
                c.fresh169r1&&c.fingerContact[0]&&out.x>.1419f&&out.x<.1421f,
                "current palm wrist permits finger curl without a100mm extra arm correction");}
        auto fit=kcd2vr::pose_core::finger_clearance103(curve,c.curl[0],{0,0,0,1},{out.x,out.y,out.z},{{.2,0,0},{-1,0,0}},.008);
        Check(fit.valid&&fit.fits&&mockCalls<=12,
                "the actually applied curled phalanges respect the current native plane and bounded work");
    }
    // Actual production projection, current exact feature planes. A one-ULP
    // plane displacement must cause that penetration's correction, not reuse
    // the full old controller penetration as new sideways travel.
    for(float origin:{0.f,3208.f}){
        Init();HandContact6695 c{};c.have=c.rawFeatureReady55=true;c.lastCompleteTickH18=1000;
        c.featureCount=5;c.worldAllowed={origin,.05f,0};c.releaseRawWorldD45={origin+.15f,.15f,0};
        for(auto& f:c.rawFeatureWorld55)f=c.releaseRawWorldD45;
        c.planeCount=1;c.planes[0].feature=0;c.planes[0].normal={-1,0,0};
        // At X3208 a single float ULP is244um; it dwarfs the old1um feasibility test.
        const float shifted=origin?std::nextafter(origin,-INFINITY):-.00002f;
        c.planes[0].point={shifted+.008f,0,0};
        V3 offsets[7]{},raw{origin+.15f,.151f,0},out{};std::uint64_t previous=0;
        {ContactPass6695 pass(true,1011,previous,2,3);
#ifdef KCD2VR_CONTACT_PROXY_D63
            out=ProjectPalmPlanes6697(c,raw,offsets,.008f,&raw);
#else
            out=ProjectPalmPlanes6697(c,raw,offsets,.008f);
#endif
        }
        Check(Len(Sub(out,c.worldAllowed))<.0015f&&out.y>.0509f&&out.y<.0511f,
            "current-plane reproject uses only real raw displacement after a tiny feasibility change");
        Check(Dot(c.planes[0].normal,Sub(out,c.planes[0].point))>=.0078f,
            "contact continuity never smooths through the current native plane");
    }
    std::printf("D63 contact continuity %d/%d; native geometry=SIMULATED\n",assertions-failures,assertions);return failures?1:0;
}
