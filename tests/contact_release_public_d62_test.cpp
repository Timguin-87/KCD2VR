// Actual production query adapter, synthetic native ray results only.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
#include "../src/vrik/user_modes175.h"

int main(){
    for(int mode:{4,5}){
        Init();g_census_mode=mode;mockBodies172=true;mockPrepareNpcH9=true;
        mockCollider172=reinterpret_cast<void*>(0x180004000ull);
        Store(0x180004000ull,base+0x3a578c0);
        Store(base+0x3a578c0+0x50,base+0x5d6f44);Store(0x180004020ull,std::uintptr_t(0));
        std::uint64_t previous=0;HandContact6695 c{};V3 out{},features[5]{};
        const auto ch=reinterpret_cast<void*>(character);
        const V3 raw{.4f,0,0};mockMode=4;
        {ContactPass6695 pass(true,1000,previous,1,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},raw,out,features)&&c.blocked&&c.fresh169r1,
                "public hand mode establishes current wall contact");}
        const auto accepted=out;mockMode=0;
        {ContactPass6695 pass(true,1011,previous,2,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},raw,out,features)&&c.fresh169r1&&!c.blocked&&!c.planeCount,
                "complete current clear query retires contact and every plane");}
        Check(Len(Sub(out,accepted))<=.01601f&&out.x>accepted.x,
            "stationary raw input cannot snap 158mm when the last surface disappears");
        const auto first=out;const auto calls=mockCalls;
        {ContactPass6695 pass(true,1012,previous,2,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},raw,out,features)&&Len(Sub(out,first))<.00001f&&mockCalls==calls,
                "same family neither advances release nor spends additional query credits");}
        for(unsigned sample=3;sample<16;++sample){const auto before=out;
            {ContactPass6695 pass(true,1000+11*(sample-1),previous,sample,7);
                Check(ContactHand6695(c,ch,1,{0,0,0},raw,out,features)&&c.fresh169r1&&!c.planeCount&&!c.blocked,
                    "continued release is requalified by a complete current clear query");}
            Check(Len(Sub(out,before))<=.01601f,"cleared surface returns without a topology jump");}
        Check(Len(Sub(out,raw))<.00001f&&!c.releaseActiveD45,"bounded return finishes at the exact raw request");
        // Ordinary motion never acquires this presentation correction.
        {ContactPass6695 pass(true,1200,previous,16,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},{.8f,.2f,0},out,features)&&Len(Sub(out,{.8f,.2f,0}))<.00001f,
                "unconstrained tracking remains immediate without a previous measured correction");}
        // A newly hit wall is still resolved by the current measured plane.
        mockMode=4;wallPoint={.9f,0,0};
        {ContactPass6695 pass(true,1211,previous,17,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},{1.1f,.2f,0},out,features)&&c.blocked&&out.x<=.89201f,
                "new live wall retains its current collision authority");}
        mockMode=0;
        {ContactPass6695 pass(true,1222,previous,18,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},{.6f,.2f,0},out,features)&&Len(Sub(out,{.6f,.2f,0}))<.00001f,
                "deliberate withdrawal is immediate after the current sweep is clear");}
    }
    // A selected body-query mode does not turn living/articulated/rigid/unknown
    // contacts into world planes. Exact owned BASE surfaces use the D63 bounded
    // current-query proxy too, without becoming static. Each case uses the native reader.
    for(const auto vft:{0x3a82570ull,0x3a48b30ull,0x3a48dc0ull,0x3000000ull,0x3a578c0ull}){
        Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;
        const std::uintptr_t body=0x180004000ull,entity=0x180006000ull;
        mockCollider172=reinterpret_cast<void*>(body);Store(body,base+vft);
        if(vft==0x3a578c0){
            Store(base+vft+0x50,base+0x5d6f44);Store(body+0x20,entity);
            Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));
        }
        std::uint64_t previous=0;HandContact6695 c{};V3 out{},features[5]{};
        mockMode=4;
        {ContactPass6695 pass(true,1000,previous,1,7);
            Check(ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.4f,0,0},out,features)&&c.blocked,
                "non-world fixture establishes actual qualified native contact");}
        const auto accepted=out;mockMode=0;
        {ContactPass6695 pass(true,1011,previous,2,7);
            const bool ok=ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.4f,0,0},out,features);
            Check(ok&&(vft==0x3a578c0?Len(Sub(out,accepted))<=.01601f:Len(Sub(out,{.4f,0,0}))<.00001f),
                "only qualified owned BASE surfaces join bounded return; moving and unknown classes keep existing coverage");}
    }
    std::printf("contact_release_public_d62=%d/%d; native/headset=NOT_EXECUTED\n",assertions-failures,assertions);
    return failures?1:0;
}
