// Actual staged consumer and servo; native object/physics leaves are spies.
#define main previous_prop_adapter_main
#include "prop_adapter5916c1_test.cpp"
#undef main
#include <cstdlib>
#include <limits>

static unsigned checksR92=0;
#define CHECK_R92(x) do { ++checksR92; if(!(x)) { \
    std::printf("FAIL %u: %s at line %d\n",checksR92,#x,__LINE__);std::exit(1); } } while(false)

static void instantR92(int side) {
    setup();
    kcd2vr::combat_rules178r10::f5Rung=0;
    kcd2vr::workstation178r20c1::experimentArmed36=false;
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,now);
    g_prop596.instantD72=true;g_prop596.instantTokenD72=101;
    g_prop596.epoch=g_prop596.bodyEpoch;g_prop596.side=side;
    g_prop596.hold.begin(g_prop596.bodyEpoch,now);
    kcd2vr::mutual_admission_t25::pickupClaimsD72[side]=101;
}

int main() {
    // These two distinct committed stamps occur in the Nov 3 R92R1 F7 capture:
    // FRAME rows 281162 and 281190. Positions/physics here are explicit test
    // substitutes, not a claim to reproduce the unseen end-row inputs.
    constexpr std::uint64_t firstSample=160622,secondSample=160626,sourceTick=415206765;
    for(int side=0;side!=2;++side) {
        instantR92(side);
        now=tracking.tick=frame.stamp.tick=sourceTick;
        frame.stamp.sample=g_prop596.sample=firstSample;
        g_prop596.hold.begin(g_prop596.bodyEpoch,now);
        g_prop596.drive.seed({},firstSample,sourceTick);
        frame.propRequested5916c1[side].p.x=.05;
        frame.stamp.sample=secondSample;
        StagedPropTick596();
        CHECK_R92(g_prop596.id==10);
        CHECK_R92(velocityCalls==0&&stowCalls==0&&placeCalls==0);
        CHECK_R92(g_prop596.drive.sequence==firstSample&&g_prop596.drive.tick==sourceTick);
        CHECK_R92(g_prop596.drive.goal.p.x==0&&g_prop596.sample==firstSample);
        CHECK_R92(kcd2vr::mutual_admission_t25::pickupClaimsD72[side]==101);
        CHECK_R92(publishCalls==1&&publishedSourceTick==sourceTick&&publishedSample==secondSample);
        CHECK_R92(notes.empty()); // ordinary gameplay does not acquire a recorder
        now=sourceTick+7;
        StagedPropTick596();
        CHECK_R92(g_prop596.id==10&&velocityCalls==0); // no duplicate impulse
        CHECK_R92(publishCalls==2&&publishedSourceTick==sourceTick&&publishedAt==sourceTick+7);
        CHECK_R92(g_prop596.drive.sequence==firstSample&&g_prop596.drive.tick==sourceTick);
        frame.stamp.sample+=4;
        now=tracking.tick=frame.stamp.tick=sourceTick+16;
        frame.propRequested5916c1[side].p.x=.06;
        StagedPropTick596();
        CHECK_R92(g_prop596.id==10&&velocityCalls==1&&stowCalls==0&&placeCalls==0);
        CHECK_R92(g_prop596.drive.sequence==secondSample+4&&g_prop596.drive.tick==sourceTick+16);
        CHECK_R92(g_prop596.drive.goal.p.x==.06&&dynamics.linear.x>0);
        // A duplicate time is not a lease extension. Real loss still releases.
        now=sourceTick+97;tracking.tick=now;frame.stamp.sample+=4;
        StagedPropTick596();
        CHECK_R92(!g_prop596.id&&velocityCalls==2&&stowCalls==0);
        CHECK_R92(kcd2vr::mutual_admission_t25::pickupClaimsD72[side]==0);
    }

    // The actual servo still refuses backwards samples/times, changed stamps
    // for the same sample, a large source gap, stale time, and physical faults.
    prop5916::Dynamics d{};d.mass=.3;ph::Pose actual{},target{};target.p.x=.02;
    for(int fault=0;fault<9;++fault) {
        prop5916::Controller drive;drive.seed({},10,1000);
        auto sample=std::uint64_t{11},tick=std::uint64_t{1000},at=std::uint64_t{1000};
        auto dynamicsCase=d;auto targetCase=target;
        switch(fault) {
        case 0:sample=9;break;
        case 1:tick=999;break;
        case 2:sample=10;tick=1001;at=1001;break;
        case 3:tick=1081;at=1081;break;
        case 4:at=1081;break;
        case 5:at=999;break;
        case 6:targetCase.p.x=.351;break;
        case 7:dynamicsCase.mass=0;break;
        case 8:targetCase.p.x=std::numeric_limits<double>::quiet_NaN();break;
        }
        CHECK_R92(drive.update(targetCase,actual,dynamicsCase,sample,tick,at).decision==prop5916::Decision::End);
        CHECK_R92(drive.sequence==10&&drive.tick==1000&&drive.goal.p.x==0);
    }

    // End details are attached to the existing F5 receipt, only when enabled.
    for(bool capture:{false,true}) {
        instantR92(0);
        kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,capture,now);
        frame.stamp.sample=2;frame.stamp.tick=999;tracking.tick=now;
        StagedPropTick596();
        CHECK_R92(!g_prop596.id&&velocityCalls==1&&stowCalls==0);
        if(capture) {
            CHECK_R92(notes.size()==2&&notes[0]=="DRIVE_CONTINUITY_ENDED");
            CHECK_R92(notes[1].find("inSample=2 priorSample=1 inTick=999 priorTick=1000 ageMs=1")!=std::string::npos);
        } else CHECK_R92(notes.empty());
    }
    previous_prop_adapter_main(); // keep the existing native-drive/lifecycle checks
    std::printf("%u R92R2 clock/consumer checks PASS; production consumer; native leaves substituted\n",checksR92);
}
