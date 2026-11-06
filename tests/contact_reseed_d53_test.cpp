// Run the exact extracted contact adapter with synthetic native ray geometry.
// Last displayed wrist is history for sweep origin, never proof of clearance.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
int main(){
    Init();const HandContact6695 empty{};std::uint64_t previous=0;HandContact6695 c=empty;V3 point{},offsets[5]{};
    const auto ch=reinterpret_cast<void*>(character);
    const auto unavailable=[&](std::uint64_t tick,std::uint64_t sample,V3 displayed){
        {ContactPass6695 pass(true,tick,previous,sample,7);g_contact_budget6695.remaining=0;
            Check(!ContactHand6695(c,ch,1,{0,0,0},displayed,point,offsets)&&!c.have&&!c.fresh169r1,
                  "starved query keeps no measured contact authority");
            ContactCommit6695(c,ch,displayed,tick);}
        Check(c.presentationTickD53==tick&&Len(Sub(c.presentationWorldD53,displayed))<1e-6f&&
              !c.lastCompleteTickH18&&!c.planeCount,"staged final wrist records presentation only, no false query receipt");
    };
    unavailable(1000,1,{.532f,0,0});mockMode=4;wallPoint={.20f,0,0};wallNormal={-1,0,0};
    {ContactPass6695 pass(true,1011,previous,2,7);
        const bool ok=ContactHand6695(c,ch,1,{0,0,0},{.53325f,0,0},point,offsets);
        Check(ok&&c.fresh169r1&&!c.blocked&&Len(Sub(point,{.53325f,0,0}))<1e-6f,
              "1.25mm real motion never sweeps unrelated authored wrist across old33cm obstacle gap");
        Check(!mockOrigins55.empty()&&std::abs(mockOrigins55.front().x-.532f)<1e-6f,
              "actual native ray starts at prior accepted displayed wrist");
        ContactCommit6695(c,ch,point,1011);}
    wallPoint={.55f,0,0};
    {ContactPass6695 pass(true,1022,previous,3,7);
        Check(ContactHand6695(c,ch,1,{0,0,0},{.62f,0,0},point,offsets)&&c.blocked&&point.x<=.54201f,
              "new obstacle on real controller trajectory still produces measured contact");}
    // New body / old seed cannot borrow the prior display's world position.
    for(unsigned variant=0;variant<4;++variant){Init();previous=0;c=empty;unavailable(1000,1,{.532f,0,0});
        mockMode=4;wallPoint={.20f,0,0};wallNormal={-1,0,0};
        if(variant==3)c.character=reinterpret_cast<void*>(character+0x100);
        ContactPass6695 pass(true,variant==1?1081:1011,previous,2,variant==2?8:7);
        Check(ContactHand6695(c,ch,variant?1:2,{0,0,0},{.53325f,0,0},point,offsets)&&c.blocked&&point.x<=.19201f,
              "different body/character, expired display or new tracking origin preserves native initial-seed route");}
    Init();previous=0;c=empty;unavailable(1000,1,{.532f,0,0});
    const float moved[]{0,0,0,1,.1f,0,0,1};Store(character+0xA08,moved);
    mockMode=4;wallPoint={.60f,0,0};wallNormal={-1,0,0};
    {ContactPass6695 pass(true,1011,previous,2,7);
        Check(ContactHand6695(c,ch,1,{0,0,0},{.53325f,0,0},point,offsets)&&c.blocked&&point.x<=.49201f,
              "previous displayed world point is not retransformed by newer root travel");}
    std::printf("contact_reseed_d53=%d/%d; native/headset=NOT_EXECUTED\n",assertions-failures,assertions);return failures?1:0;
}
