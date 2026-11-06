// Full production impale coordinator. The runner extracts the existing native
// fixture prefix and adds only a deterministic clock/publication interleaving.
// Native body status, identity and actions remain explicit fixture leaves.
#include "impale_native_eligibility_r93_fixture.h"
#include "impale_coordinator_r96r1_fixture.inc"
#include <cstring>
#include <limits>

static unsigned checksR96R1=0;
static void requireR96R1(bool ok,const char* why) {
    ++checksR96R1;
    if(!ok){std::printf("FAIL_IMPALE_NATIVE_R96R1 %s\n",why);std::exit(1);}
}
static void globalsR96R1() {
    g_weapon_request_epoch6694=3;g_pose_epoch6689=2;g_pose_policy_generation57=2;
    kcd2vr::tracking59c6::epoch=1;kcd2vr::contact176::recoveryEpoch55c1=0;
    g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=false;
    animation_filter::nativeBodyOwned112=false;kcd2vr::gore32::impaleHang=true;
    kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
    ownerH18=driveH18=equippedH18=true;reenterH18=bloodReenterH21=false;
    statusAdvanceH18=rootAdvanceH18=revokePhysicsAfterStatusH21=revokeImpaleAfterStatusR93=referenceAfterStatusR93=0;
    clockReadHookR96R1=nullptr;kcd2vr::collab_h21::setPhysicsMode(0);
}
static void seedR96R1(std::uint64_t serial,std::uint64_t when) {
    globalsR96R1();request(serial,when);
    requireR96R1(!pin(),"native lethal request starts without a pin");
}
static unsigned requestsR96R1(std::uint64_t serial) {
    const auto needle="stage=REQUESTED serial="+std::to_string(serial)+" ";unsigned count=0;
    for(const auto& line:diagnostic_trace::lines)count+=line.find(needle)!=std::string::npos;
    return count;
}
struct EffectsR96R1 {
    unsigned roots=rootsH18,contacts=newContactsH18,status=statusH18,oldStatus=oldStatusH18,drives=drivesH18;
    bool unchanged()const{return roots==rootsH18&&contacts==newContactsH18&&status==statusH18&&oldStatus==oldStatusH18&&drives==drivesH18;}
};
static void publishGapR96R1(std::uint64_t when) {
    nowH18=when;frameH18.input.tick=when;++frameH18.input.sample;
    // The producer publishes its coherent packet before its separate ready store.
    requireR96R1(g_phys_frame6698.publish(frameH18),"fixture publishes next coherent packet");
    requireR96R1(g_phys_ready_sample6698.load()!=0&&g_phys_ready_sample6698.load()!=frameH18.input.sample,
        "fixture creates a nonzero ready/publication mismatch");
    ImpalePartTick59c7g();
}
static void publicationR96R1() {
    seedR96R1(960101,1000);const auto original=frameH18.output.impaleHit47;const EffectsR96R1 before;
    for(auto when:{1010u,1020u,1030u}) {
        publishGapR96R1(when);ImpaleSkip40 skip{};
        requireR96R1(!pin()&&g_impale_skip40.read(skip)&&!skip.body&&!skip.entity&&before.unchanged(),
            "publication gap grants no body query, drive, pin or victim exclusion");
        requireR96R1(requestsR96R1(original.serial)==1,"publication gap cannot admit or replay a consumed native hit");
    }
    contact();tick(1060);
    requireR96R1(pin()&&drivesH18>before.drives,"same pending request resumes after publication mismatch");
    requireR96R1(requestsR96R1(original.serial)==1&&
        !std::memcmp(&original,&frameH18.output.impaleHit47,sizeof(original)),
        "recovery retains the original immutable hit and admission");
}
static std::uint64_t clockPublicationR96R1=0;
static void newerPacketR96R1() {
    nowH18=clockPublicationR96R1;frameH18.input.tick=nowH18;++frameH18.input.sample;
    g_phys_frame6698.publish(frameH18);g_phys_ready_sample6698=frameH18.input.sample;
}
static void clockR96R1() {
    seedR96R1(960102,2000);retire(2010);
    auto& out=frameH18.output;out.impaleSerial=960103;out.impaleHit47.serial=960103;
    out.impaleHit47.tick=out.impaleHit47.impactTick=3001;
    nowH18=3000;clockPublicationR96R1=3001;clockReadHookR96R1=&newerPacketR96R1;
    const auto drives=drivesH18;ImpalePartTick59c7g();
    requireR96R1(!pin()&&drivesH18==drives,"new frame cannot fabricate a first corpse pin");
    contact();tick(3060); // Poll is due50ms after the new frame's actual3001ms admission.
    requireR96R1(pin()&&drivesH18>drives,"post-entry publication is not consumed as clock rewind");
    requireR96R1(requestsR96R1(960103)==1,"clock recovery admits the immutable hit only once");

    seedR96R1(960104,4000);retire(4010);out.impaleSerial=960105;out.impaleHit47.serial=960105;
    out.impaleHit47.tick=out.impaleHit47.impactTick=4101;frameH18.input.tick=4101;++frameH18.input.sample;
    g_phys_frame6698.publish(frameH18);g_phys_ready_sample6698=frameH18.input.sample;nowH18=4100;
    const EffectsR96R1 future;ImpalePartTick59c7g();
    requireR96R1(!pin()&&future.unchanged()&&requestsR96R1(960105)==0,
        "genuinely future frame still grants no admission or native action");

    seedR96R1(960106,5000);const EffectsR96R1 rewind;tick(4999);contact();tick(5060);
    requireR96R1(!pin()&&rewind.unchanged(),"actual clock rewind retires the original pending request");
}
static void expirationR96R1() {
    seedR96R1(960107,6000);const EffectsR96R1 expired;
    for(auto age:{10u,1701u,4000u,4499u})publishGapR96R1(6000+age);
    requireR96R1(expired.unchanged()&&!pin(),"repeated pending gaps never query or drive a body");
    contact();tick(10500);
    requireR96R1(!pin()&&expired.unchanged(),"original4500ms victim deadline expires after repeated gaps");
    tick(10560);
    requireR96R1(!pin()&&expired.unchanged()&&requestsR96R1(960107)==1,
        "expired consumed request cannot restart or renew its deadline");

    seedR96R1(960108,11000);publishGapR96R1(15480);contact();tick(15499);
    requireR96R1(pin(),"same original request can acquire on the last in-window millisecond");
}
static void revocationsR96R1() {
    for(unsigned why=0;why<38;++why) {
        const auto when=20000+why*100;seedR96R1(960200+why,when);
        const auto saved=frameH18;const EffectsR96R1 before;
        nowH18=when+10;frameH18.input.tick=nowH18;++frameH18.input.sample;
        switch(why) {
        case 0:frameH18.input.grip[0]=0;break;
        case 1:frameH18.input.tracked[0]=false;break;
        case 2:frameH18.input.allowed=false;break;
        case 3:++frameH18.profiles[0].key;break;
        case 4:++frameH18.profiles[0].generation;break;
        case 5:++frameH18.input.bodyEpoch;break;
        case 6:++frameH18.request;break;
        case 7:g_pose_epoch6689.fetch_add(1);break;
        case 8:g_weapon_request_epoch6694.fetch_add(1);break;
        case 9:kcd2vr::tracking59c6::epoch.fetch_add(1);break;
        case 10:kcd2vr::contact176::recoveryEpoch55c1.fetch_add(1);break;
        case 11:g_pose_policy_generation57.fetch_add(2);break;
        case 12:g_gameplay_allowed=false;break;
        case 13:g_ctx_menu=true;break;
        case 14:g_ctx_dialogue=true;break;
        case 15:g_ctx_cutscene=true;break;
        case 16:animation_filter::nativeBodyOwned112=true;break;
        case 17:kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,1);break;
        case 18:kcd2vr::gore32::impaleHang=false;break;
        case 19:frameH18.input.impaleEnabled178r15=false;break;
        case 20:frameH18.input.impaleHang32=false;break;
        case 21:frameH18.output.impaleReleased=true;break;
        case 22:frameH18.output.impaleLatchRequest=false;break;
        case 23:++frameH18.output.impaleSerial;break;
        case 24:++frameH18.output.impaleEntity;break;
        case 25:frameH18.output.impaleSide32=1;break;
        case 26:frameH18.profiles[0].primary=false;break;
        case 27:frameH18.profiles[0].classId=9;break;
        case 28:frameH18.input.haveWorld=false;break;
        case 29:frameH18.world[7]=2;break;
        case 30:frameH18.output.weapons[0].p.y=-.26;break;
        case 31:equippedH18=false;break;
        case 32:frameH18.input.tick=nowH18-81;break;
        case 33:frameH18.input.tick=nowH18+1;break;
        case 34:frameH18.output.weapons[0].p.x=std::numeric_limits<double>::quiet_NaN();break;
        case 35:g_phys_ready_sample6698=0;break;
        case 36:frameH18.input.sample=g_phys_ready_sample6698.load()-1;break;
        case 37:frameH18.output.impaleActive=true;break;
        }
        g_phys_frame6698.publish(frameH18);ImpalePartTick59c7g();
        requireR96R1(!pin()&&before.unchanged(),"revocation during mismatch grants no native authority");
        globalsR96R1();frameH18=saved;contact();tick(when+60);
        if(pin())std::printf("revocation_case=%u\n",why);
        requireR96R1(!pin()&&before.unchanged(),"true revocation cannot replay consumed request after recovery");
    }
}
static void activeR96R1() {
    seedR96R1(960400,30000);contact();tick(30060);
    requireR96R1(pin(),"active control owns an actual first pin");
    const auto drives=drivesH18;publishGapR96R1(30070);
    requireR96R1(!pin()&&drivesH18==drives,"active anchor releases on unavailable committed frame");
    tick(30120);
    requireR96R1(!pin()&&drivesH18==drives,"retired active anchor cannot become unpinned pending recovery");

    seedR96R1(960401,31000);retire(31010);
    auto& out=frameH18.output;out.impaleSerial=960402;out.impaleHit47.serial=960402;
    out.impaleHit47.tick=out.impaleHit47.impactTick=31020;
    const EffectsR96R1 unseen;publishGapR96R1(31020);
    requireR96R1(unseen.unchanged()&&requestsR96R1(960402)==0,"uncommitted new serial cannot borrow pending admission");
    tick(31030);contact();tick(31090);
    requireR96R1(pin()&&requestsR96R1(960402)==1,"first coherent new request still makes positive progress");
}
int main(int argc,char** argv) {
    const char* mode=argc>1?argv[1]:"all";bool selected=false;
    const auto run=[&](const char* name,auto fn){if(!std::strcmp(mode,"all")||!std::strcmp(mode,name)){selected=true;fn();}};
    run("publication",publicationR96R1);run("clock",clockR96R1);run("expiration",expirationR96R1);
    run("revocations",revocationsR96R1);run("active",activeR96R1);
    if(!selected)return 2;
    std::printf("PASS_IMPALE_NATIVE_R96R1 checks=%u case=%s; actual coordinator, native/time/status leaves controlled\n",checksR96R1,mode);
    return 0;
}
