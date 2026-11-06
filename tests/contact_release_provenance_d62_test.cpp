#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53

static void WorldD62(){
    Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockMode=4;
    mockCollider172=reinterpret_cast<void*>(0x180004000ull);
    Store(0x180004000ull,base+0x3a578c0);
    Store(base+0x3a578c0+0x50,base+0x5d6f44);Store(0x180004020ull,std::uintptr_t(0));
}
static bool EntityReleaseD63(const HandContact6695& c){
#ifdef KCD2VR_CONTACT_PROXY_D63
    return c.releaseEntitySurfaceD63;
#else
    return false;
#endif
}
int main(){
    const auto ch=reinterpret_cast<void*>(character);
    for(unsigned changed=0;changed<4;++changed){
        WorldD62();std::uint64_t previous=0;HandContact6695 c{};V3 out{},features[5]{};
        for(int i=1;i<5;++i)features[i]={0,.02f*i,0}; // nondegenerate palm plus four actual feature segments
        {ContactPass6695 pass(true,1000,previous,1,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},{.4f,0,0},out,features)&&c.releaseWorldD62,
                "exact base-physical plus successful null foreign-data read qualifies a world release source");}
        wallPoint={.255f,0,0};
        if(changed<2)Store(0x180004000ull,base+(changed?0x3a48b30:0x3a82570));
        else if(changed==2){const std::uintptr_t entity=0x180006000ull;Store(0x180004020ull,entity);
            Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));}
        else Store(0x180004020ull,std::uintptr_t(0x180009000ull)); // unknown nonnull owner after real world contact
        {ContactPass6695 pass(true,1011,previous,2,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},{.4f,0,0},out,features)&&c.blocked&&c.fresh169r1&&
                  out.x<=.24701f&&!c.releaseWorldD62,
                "new living, articulated or entity-owned hit clamps now and retires old world provenance");}
        bool qualifiedOwnedBase=c.planeCount>0;
        for(int p=0;p<c.planeCount;++p){const auto& w=c.planes[p].native172;
            qualifiedOwnedBase=qualifiedOwnedBase&&w.mask==npc172::BodiesAndWorld&&
                w.body==npc172::Body::Physical&&w.vft==0x3a578c0&&w.collider==0x180004000ull&&
                w.ownerRead&&w.owner==0x180006000ull&&w.entityId==777&&!w.foreignNullD62;}
        Check(EntityReleaseD63(c)==(changed==2)&&(!EntityReleaseD63(c)||qualifiedOwnedBase),
            "only a new current exact owned BASE witness independently grants entity return; unknown owner cannot borrow old world provenance");
        const auto accepted=out;const int calls=mockCalls;
        mockMode=0;
        {ContactPass6695 pass(true,1022,previous,3,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},{.4f,0,0},out,features)&&c.fresh169r1&&
                c.lastCompleteTickH18==1022&&!c.blocked&&!c.planeCount&&!c.releaseWorldD62,
                "a complete current clear retires every contact plane and keeps world provenance revoked");}
        bool currentOrigins=mockCalls-calls>=5&&mockCalls-calls<=18&&argumentsOk;
        if(currentOrigins){
            currentOrigins=Len(Sub(mockOrigins55[std::size_t(calls)],accepted))<.00001f;
            for(int q=1;q<5;++q)currentOrigins=currentOrigins&&
                Len(Sub(mockOrigins55[std::size_t(calls+q)],out))<.00001f;
        }
        Check(currentOrigins,
            "the clear return runs its actual palm travel and four current wrist-to-feature native queries");
        if(changed==2)Check(EntityReleaseD63(c)&&out.x>accepted.x&&Len(Sub(out,accepted))<=.01601f,
            "new owned BASE provenance returns only by the current checked bounded candidate without reviving world authority");
        else Check(!EntityReleaseD63(c)&&Len(Sub(out,{.4f,0,0}))<.00001f,
            "living, articulated and unknown-owner clear paths cannot retain old world or entity correction");
    }
    for(unsigned unavailable=0;unavailable<3;++unavailable){
        WorldD62();std::uint64_t previous=0;HandContact6695 c{};V3 out{},features[5]{};
        if(unavailable==0)memory.erase(base+0x3a578c0+0x50);
        else if(unavailable==1)memory.erase(0x180004020ull);
        else Store(0x180004020ull,std::uintptr_t(0x180009000ull));
        {ContactPass6695 pass(true,1000,previous,1,7);
            Check(ContactHand6695(c,ch,1,{0,0,0},{.4f,0,0},out,features)&&c.blocked&&!c.releaseWorldD62&&!EntityReleaseD63(c),
                "missing method, failed foreign read or unknown nonnull owner is never a world-release grant");}
    }
    WorldD62();std::uint64_t previous=0;HandContact6695 c{};V3 out{},features[5]{};features[1]={.1f,0,0};
    mockReplyAtH25=2;mockReplyModeH25=2;distance=.262f;
    {ContactPass6695 pass(true,1000,previous,1,7);HandContactPending6695 pending{};
        Check(ContactHand6695(c,ch,1,{0,0,0},{.4f,0,0},out,features,nullptr,&pending)&&pending.ready&&
              pending.queried.planeCount>0&&pending.unqualifiedHitD62&&pending.fallbackFraction==1.f,
            "plane-less hit beyond the endpoint is recorded even when legacy distance fraction clamps to one");
        Check(CompleteHandContact6695(c,pending,out)&&c.blocked&&!c.releaseWorldD62&&!EntityReleaseD63(c),
            "a later valid recovery plane cannot erase an unqualified hit from this query pass");}
    std::printf("contact_release_provenance_d62=%d/%d; native/headset=NOT_EXECUTED\n",assertions-failures,assertions);
    return failures?1:0;
}
