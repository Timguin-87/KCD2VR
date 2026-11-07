// Actual service body and pending-grant publication; existing native leaves remain substitutes.
#define main LegacyServiceMainD66
#include "native_ammo_service_t22_test.cpp"
#undef main
using namespace fixture;
static adm23::Grant originalD66{};
static ta20::Request SetupD66(){
    Reset();in.chestKnivesT26=true;in.rightHipPouchD66=true;in.near18[1][1]=false;Publish(0,1);
    auto& h=g_ammo_service22[0];h.owner=in;h.side=1;h.family=0;h.serial=44;h.key=0x0200000000000010ull;
    h.launcher.key=0x0200000000000020ull;h.stage=svc22::Stage::Reserved;h.started=now;
    h.throwEpoch=kcd2vr::collab_h21::throwEpoch.load();h.physicsEpoch=kcd2vr::collab_h21::epoch.load();
    h.tracking=kcd2vr::tracking59c6::epoch.load();h.pending={in,h.serial,1,0};
    h.pendingVm=context.generation;h.pendingLoad=1;h.pendingPublished=adm23::PublishPending(h.pending);assert(h.pendingPublished);
    g_ammo_context22=&context;g_ammo_input22=&in;g_ammo_current22[1]=0;
    Publish(0,1);assert(adm23::CurrentGrant(h.pending,in,now,&originalD66));g_ammo_grant22[1]=&originalD66;
    auto& r=h.preview;r.owner=h.owner;r.side=1;r.serial=h.serial;r.legacyKey=h.key;r.launcherKey=h.launcher.key;
    r.itemManager=&itemManager;r.world=&world;r.vmGeneration=context.generation;r.load=1;r.scriptSystem=context.system;r.scriptState=context.state;
    r.recordClean=r.sourceOwned=true;r.source=&stock[0];r.sourceKey=0x0200000000000050ull;
    assert(AmmoServicePublishT22(h,now));
    ta20::Request q{};q.owner=h.owner;q.side=1;q.serial=h.serial;q.legacyKey=h.key;q.launcherKey=h.launcher.key;
    assert(AmmoServiceOriginalOwnerT22(context,q.owner,1,q.serial));
    now+=81;Publish(0,1);assert(!AmmoServiceOriginalOwnerT22(context,q.owner,1,q.serial));return q;
}
int main(){
    image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5600000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(image);
    auto q=SetupD66();auto& h=g_ammo_service22[0];const auto oldSample=originalD66.frame.sample;const auto oldTick=originalD66.frame.tick;
    Check(AmmoServiceTransientOwnerT22(context,q));Check(h.preview.previewGrantRejoinedD66&&h.preview.previewGrantRejoinAttemptedD66);
    Check(g_ammo_grant22[1]==&h.retainedGrant&&g_ammo_grant22[1]->frame.sample>oldSample&&g_ammo_grant22[1]->frame.tick>oldTick);
    Check(originalD66.frame.sample==oldSample&&originalD66.frame.tick==oldTick&&h.owner.tick==oldTick);
    Check(AmmoServiceTransientOwnerT22(context,q));Check(previewCalls==0&&payCalls==0&&shootCalls==0&&counts[0]==5);
    now+=81;Publish(0,1);Check(!AmmoServiceTransientOwnerT22(context,q)); // only one rejoin
    for(int reason=0;reason<24;++reason){
        q=SetupD66();auto& r=h.preview;adm23::Grant bad{};assert(adm23::grants[1].read(bad));
        switch(reason){
        case 0:bad.frame.grip18[1]=0;bad.source.frame=bad.frame;break;
        case 1:bad.frame.trigger18[1]=1;bad.source.frame=bad.frame;break;
        case 2:bad.frame.hand[1]=10;bad.source.frame=bad.frame;break;
        case 3:bad.source.handEmpty[1]=false;break;
        case 4:bad.frame.rightHipPouchD66=false;bad.source.frame=bad.frame;break;
        case 5:++bad.tracking;bad.source.tracking=bad.tracking;break;
        case 6:++bad.pending.serial;break;
        case 7:++bad.frame.body;bad.source.frame=bad.frame;break;
        case 8:++bad.frame.request;bad.source.frame=bad.frame;break;
        case 9:bad.source.complete=false;break;
        case 10:bad.source.publication|=1;bad.publication=bad.source.publication;break;
        case 11:bad=originalD66;break;
        case 12:worldChanged=true;break;
        case 13:++context.generation;break;
        case 14:++inventory_lifecycle_t8::loadEpisode18;break;
        case 15:handAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;break;
        case 16:equipmentAdmission[1]=kcd2vr::frik_pose_probe::ThrowAuthorityT22::Denied;break;
        case 17:r.previewEntered=true;break;
        case 18:r.retireEntered=true;break;
        case 19:r.recordClean=false;break;
        case 20:bad.source=originalD66.source;break; // fresh metadata cannot renew old wrists
        case 21:st2::enabled=false;break;
        case 22:++nativeAuthorityRequest;break;
        case 23:g_context13=false;break;
        }
        assert(adm23::PublishGrant(1,bad));Check(!AmmoServiceTransientOwnerT22(context,q));
        Check(!r.previewGrantRejoinedD66&&g_ammo_grant22[1]==&originalD66&&payCalls==0&&shootCalls==0);
    }
    std::printf("{\"checks\":%u,\"passed\":true,\"actual_service_and_grant_code\":true,\"native_execution\":false}\n",checks);
    VirtualFree(image,0,MEM_RELEASE);return 0;
}
