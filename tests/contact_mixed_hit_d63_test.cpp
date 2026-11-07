// Executes the production CompleteHandContact6695 and ProjectPalmPlanes6697
// through the existing generated native-query adapter. The pending result is a
// controlled current-query completion, not a claimed native/headset replay.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
int main(){
    for(bool entity:{false,true})for(float fraction:{.1f,.25f,.5f,.75f}){
        Init();g_npc_query172.requested=true;g_contact_query_tick_h18=1011;
        HandContact6695 c{};c.have=c.rawFeatureReady55=true;c.lastCompleteTickH18=1000;c.featureCount=5;
        c.worldAllowed={0,0,0};c.releaseRawWorldD45={.1f,0,0};c.releaseCorrectionWorldD45={-.1f,0,0};
        c.releaseActiveD45=true;c.releaseWorldD62=!entity;c.releaseTickD45=1000;
        for(auto& point:c.rawFeatureWorld55)point=c.releaseRawWorldD45;
        HandContactPending6695 pending{};pending.before=c;pending.queried=c;
        pending.ready=true;pending.worldScale=1;pending.worldRotation={0,0,0,1};
        pending.desired=pending.rawDesiredD45=pending.requested={.1f,.1f,0};
        pending.skin=.008f;pending.fallbackFraction=fraction;pending.unqualifiedHitD62=true;
        pending.queried.blocked=true;pending.queried.planeCount=1;
        auto& plane=pending.queried.planes[0];plane.feature=0;plane.point={.008f,0,0};plane.normal={-1,0,0};
        auto& w=plane.native172;w.mask=npc172::BodiesAndWorld;w.body=npc172::Body::Physical;
        w.vft=0x3a578c0;w.collider=0x1000;w.foreignNullD62=!entity;
        if(entity){w.ownerRead=true;w.owner=0x2000;w.entityId=7;}
        for(int i=1;i<5;++i){pending.offsets[i]={0,0,.02f*i};
            pending.before.offset[i]=pending.offsets[i];pending.queried.offset[i]=pending.offsets[i];}
        V3 out{};const bool complete=CompleteHandContact6695(c,pending,out);
        Check(complete&&std::abs(out.x)<.00001f&&std::abs(out.y-.1f*fraction)<.00001f,
            "current plane-less hit limit survives a simultaneous qualified plane projection");
        Check(c.blocked&&!c.releaseWorldD62&&!c.releaseEntitySurfaceD63&&c.fresh169r1,
            "mixed hit remains blocked and cannot grant a world/entity release provenance");
        Check(mockCalls==0,"this completed same-pass result needs no extra recovery query");
    }
    std::printf("D63 mixed-current-contact %d/%d; actual completion/projection; pending query input=SIMULATED\n",assertions-failures,assertions);
    return failures?1:0;
}
