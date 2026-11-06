// C51R1 reconciliation: exercise the real tracking publisher and activity reader.
// Native game/XR leaves are supplied by the retained explicit host stubs.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
#include <limits>

namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {} }
using namespace kcd2vr::frik_pose_probe;

int main() {
    namespace ws = kcd2vr::workstation_sample46;
    namespace track = kcd2vr::tracking59c6;
    hostTickCount64 = 10000;
    g_mode = 0;
    g_census_mode = 0;
    g_cal_state = 2;
    g_pose_epoch6689 = 7;
    track::epoch = 9;
    kcd2vr::reading_pose32::blocked = true;
    TrackingPose head{}, left{}, right{};
    head.py = 1.7f;
    left.px = -.25f; left.py = 1.2f; left.pz = -.3f;
    right.px = .25f; right.py = 1.2f; right.pz = -.3f;
    head.qw = left.qw = right.qw = 1.f;
    auto publish = [&](bool gameplay, bool hv = true, bool lv = true, bool rv = true) {
        PublishTrackingSnapshot(head, hv, left, lv, right, rv, .2f, .3f, .4f, .5f,
            gameplay, hostTickCount64, hostTickCount64 * 1000000ull, track::epoch.load());
    };
    ws::Frame frame{};

    // A blocked frame cannot be promoted just because a visual owner appears later.
    g_dialogue_body178r9 = false;
    publish(false);
    assert(!WorkstationFrame46(frame) && !frame.sequence && !g_gameplay_allowed.load());
    g_dialogue_body178r9 = true;
    assert(!WorkstationFrame46(frame) && !frame.sequence && !g_gameplay_allowed.load());

    // The next actual publication already carries the scoped workstation pose
    // permission. It never turns on world gameplay, combat or interaction authority.
    ++hostTickCount64;
    publish(false);
    assert(WorkstationFrame46(frame) && frame.allowed && !g_gameplay_allowed.load());
    assert(frame.poseEpoch == 7 && frame.sourceEpoch == 9 && frame.headValid);
    assert(frame.hand[0].valid && frame.hand[1].valid);
    assert(frame.hand[0].position[0] == right.px && frame.hand[1].position[0] == left.px);
    assert(frame.hand[0].grip == .3f && frame.hand[1].grip == .2f);

    // Presentation grace is not fresh gesture work.
    ++hostTickCount64;
    publish(false, true, true, false);
    assert(WorkstationFrame46(frame) && frame.allowed && !frame.hand[0].valid && frame.hand[1].valid);
    ++hostTickCount64;
    publish(false, false);
    assert(!WorkstationFrame46(frame) && !frame.sequence);

    ++hostTickCount64;
    publish(false);
    assert(WorkstationFrame46(frame) && frame.allowed);
    ++track::epoch;
    assert(!WorkstationFrame46(frame) && !frame.sequence);
    ++hostTickCount64;
    publish(false);
    hostTickCount64 += 81;
    assert(!WorkstationFrame46(frame) && !frame.sequence);

    g_dialogue_body178r9 = false;
    publish(false);
    assert(!WorkstationFrame46(frame) && !frame.sequence && !g_gameplay_allowed.load());
    ++hostTickCount64;
    publish(true);
    assert(WorkstationFrame46(frame) && frame.allowed && g_gameplay_allowed.load());

    // The default current-time family overlays coordinates, not permission.
    // Two tracked locates discharge its deliberate reacquisition quarantine.
    track::enabled = true;
    auto family = [&]() {
        track::Frame f{};
        const TrackingPose* poses[3]{&head, &left, &right};
        for (int i = 0; i < 3; ++i) {
            const auto& v = *poses[i];
            f.poses[i] = {v.px, v.py, v.pz, v.qx, v.qy, v.qz, v.qw};
            f.valid[i] = f.tracked[i] = true;
        }
        f.controllersSampled = true;
        f.epoch = track::epoch.load();
        f.tick = hostTickCount64;
        f.ns = track::NowNs();
        assert(track::Publish(f));
    };
    ++hostTickCount64;
    family();
    ++hostTickCount64;
    family();
    publish(false);
    assert(WorkstationFrame46(frame) && !frame.allowed && !g_gameplay_allowed.load());
    g_dialogue_body178r9 = true;
    assert(WorkstationFrame46(frame) && !frame.allowed && !g_gameplay_allowed.load());
    ++hostTickCount64;
    family();
    publish(false);
    assert(WorkstationFrame46(frame) && frame.allowed && !g_gameplay_allowed.load());
    hostTickCount64 += 61;
    assert(!WorkstationFrame46(frame) && !frame.sequence); // HMD keeps its 60 ms limit.

    // G1: exercise the actual committed WORLD-palm reader, rather than a
    // substitute point provider. Only the workstation's presentation permission
    // is admitted; world gameplay remains disabled throughout this block.
    namespace activity=kcd2vr::activity_input163;
    namespace hands=kcd2vr::minigame_hands45;
    namespace station=kcd2vr::workstation178r20c1;
    track::enabled=false;
    hands::setMode(hands::Alchemy,hands::Gesture);
    kcd2vr::combat_rules178r10::workstationEnabled=true;
    activity::witness175.Clear();activity::witness175.ready=true;
    constexpr std::uintptr_t actor=0x123400,player=0x567800;
    activity::witness175.Begin(activity::Kind175::Alchemy,0x987600);
    activity::trackedHands59c7f[unsigned(activity::Kind175::Alchemy)]=true;
    station::heldToolActivity59c10=int(station::Kind::Alchemy);
    g_committed_character6689=reinterpret_cast<void*>(actor);
    g_last_player_entity=reinterpret_cast<void*>(player);
    g_pose_restore_fault6689=false;
    CommittedFrame6695 packet{};
    packet.stamp={g_pose_epoch6689.load(),31,hostTickCount64,actor};
    packet.doorPlayer38=player;packet.doorTrackingEpoch66=track::epoch.load();
    packet.valid[0]=packet.valid[1]=packet.palmValid59c9[0]=packet.palmValid59c9[1]=true;
    packet.palm59c9[0]={.2f,.3f,.4f};packet.palm59c9[1]={-.2f,.3f,.4f};
    // 90-degree WORLD-Z rotation, scale 2, then translation. This deliberately
    // differs from the raw XR frame to catch space mixing and omitted scale.
    packet.world[2]=packet.world[3]=std::sqrt(.5f);packet.world[4]=10;
    packet.world[5]=20;packet.world[6]=30;packet.world[7]=2;
    const auto commit=[&]() {
        ++hostTickCount64;hands::publish(hands::Alchemy,hostTickCount64);
        station::heldToolActivityTick59c10=hostTickCount64;
        g_tool_context19c4r2=hostTickCount64<<8;
        publish(false); // actual raw publication preserves no-world authority
        packet.stamp.tick=hostTickCount64;
        ++packet.stamp.sample;(void)g_committed_frame6695.publish(packet);
        g_committed_pose_valid6689=true;
    };
    float x=-99,y=-99,z=-99;std::uint64_t tick=1,poseEpoch=1,sourceEpoch=1;std::uintptr_t owner=1;
    const auto read=[&](int side=0) {
        return ReadWorkstationPalm67(side,x,y,z,tick,poseEpoch,sourceEpoch,owner);
    };
    const auto refused=[&](int side=0) {
        tick=poseEpoch=sourceEpoch=1;owner=1;
        assert(!read(side)&&!tick&&!poseEpoch&&!sourceEpoch&&!owner);
    };
    commit();assert(WorkstationFrame46(frame)&&frame.allowed&&!g_gameplay_allowed.load());
    assert(read()&&std::fabs(x-9.4f)<.0001f&&std::fabs(y-20.4f)<.0001f&&std::fabs(z-30.8f)<.0001f);
    assert(tick==packet.stamp.tick&&poseEpoch==frame.poseEpoch&&sourceEpoch==frame.sourceEpoch&&owner==player);
    assert(read(1)&&std::fabs(x-9.4f)<.0001f&&std::fabs(y-19.6f)<.0001f);
    // H21 destination aim and palm use ONE committed packet/calibrated WORLD
    // basis. Position uses world scale2; a direction never acquires that scale.
    packet.palmDirection59c9[0]={1,0,0};commit();float directionH21[3]{};
    assert(ReadWorkstationPalmAimH21(0,x,y,z,tick,poseEpoch,sourceEpoch,owner,directionH21));
    assert(std::fabs(x-9.4f)<.0001f&&std::fabs(y-20.4f)<.0001f&&std::fabs(directionH21[0])<.0001f&&std::fabs(directionH21[1]-1)<.0001f);
    assert(tick==packet.stamp.tick&&poseEpoch==packet.stamp.epoch&&sourceEpoch==packet.doorTrackingEpoch66&&owner==player);
    packet.palmDirection59c9[0]={};commit();
    assert(!ReadWorkstationPalmAimH21(0,x,y,z,tick,poseEpoch,sourceEpoch,owner,directionH21)&&!tick&&!poseEpoch&&!sourceEpoch&&!owner);
    assert(read()); // no aim permission must not regress the proven near palm
    packet.palmDirection59c9[0]={1,0,0};commit();
    ++hostTickCount64;hostTickCount64+=80;
    assert(!ReadWorkstationPalmAimH21(0,x,y,z,tick,poseEpoch,sourceEpoch,owner,directionH21));
    commit();

    refused(-1);refused(2);
    packet.valid[0]=false;commit();refused();packet.valid[0]=true;
    packet.palmValid59c9[0]=false;commit();refused();packet.palmValid59c9[0]=true;
    packet.world[4]=std::numeric_limits<float>::quiet_NaN();commit();refused();packet.world[4]=10;
    packet.stamp.actor=actor+8;commit();refused();packet.stamp.actor=actor;
    packet.doorPlayer38=player+8;commit();refused();packet.doorPlayer38=player;
    packet.doorTrackingEpoch66=0;commit();refused();packet.doorTrackingEpoch66=track::epoch.load();

    // A packet is never relabeled with a newly sampled source epoch. The host
    // must compare this returned provenance with the current raw XR frame.
    commit();const auto producedEpoch=packet.doorTrackingEpoch66;
    ++track::epoch;++hostTickCount64;publish(false);
    hands::publish(hands::Alchemy,hostTickCount64);station::heldToolActivityTick59c10=hostTickCount64;
    g_tool_context19c4r2=hostTickCount64<<8;
    assert(WorkstationFrame46(frame)&&frame.sourceEpoch!=producedEpoch);
    assert(read()&&sourceEpoch==producedEpoch&&sourceEpoch!=frame.sourceEpoch);
    // Recenter invalidates the committed pose epoch itself, even if the packet
    // and both actual tracking hands remain temporally fresh.
    ++g_pose_epoch6689;refused();packet.stamp.epoch=g_pose_epoch6689.load();
    packet.doorTrackingEpoch66=track::epoch.load();commit();assert(read());
    g_pose_restore_fault6689=true;refused();g_pose_restore_fault6689=false;
    g_committed_pose_valid6689=false;refused();g_committed_pose_valid6689=true;
    g_committed_character6689=reinterpret_cast<void*>(actor+8);refused();
    g_committed_character6689=reinterpret_cast<void*>(actor);
    g_last_player_entity=reinterpret_cast<void*>(player+8);refused();g_last_player_entity=reinterpret_cast<void*>(player);
    hands::setMode(hands::Alchemy,hands::Native);refused();hands::setMode(hands::Alchemy,hands::Gesture);
    g_tool_context19c4r2=(hostTickCount64<<8)|16;refused();g_tool_context19c4r2=hostTickCount64<<8;
    // Fresh current raw tracking cannot make an 81 ms committed palm fresh.
    packet.stamp.tick=hostTickCount64-81;(void)g_committed_frame6695.publish(packet);refused();
    commit();assert(read());
    ++hostTickCount64;publish(false,true,true,false);
    assert(WorkstationFrame46(frame)&&!frame.hand[0].valid);
    // The copied presentation palm may still satisfy the existing tiny tracking
    // grace. A Gesture consumer must also require THIS raw hand's validity.
    assert(read()&&tick==packet.stamp.tick); // not a new or valid raw work frame
    std::puts("workstation_publication51: actual publisher/reader permission, no late promotion, fresh hands, source epoch and timeout PASS");
    std::puts("workstation_palm67: actual committed WORLD transform, player/actor/pose joins, no source relabel, recenter/stale/invalid refusal PASS");
}
