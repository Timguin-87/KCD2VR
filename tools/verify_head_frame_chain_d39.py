#!/usr/bin/env python3
"""Join actual frame admission, paired publication, packet admission and camera.

The retained D37B fixture supplies explicit native/mailbox leaves. Unlike a
gate-only test, this executes the source's paired-publication scope/block and
exact camera reader with a missing wrist and fresh head for over three seconds.
"""
from pathlib import Path
import argparse
import subprocess
import tempfile
from run_bow_camera_d37b import extract

HERE = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument("--source", type=Path, default=HERE)
p.add_argument("--output", type=Path)
args = p.parse_args()
source = args.source.resolve()
output = args.output or Path(tempfile.mkdtemp(prefix="kcd2vr_head_chain_d39_"))
output.mkdir(parents=True, exist_ok=True)
(output / "generated_bow_camera_d37b.inc").write_text(extract(source))
coherent = (source / "src/vrik/coherent_pose.inl").read_text()
foundation = (source / "src/subsystems/vrik/pose_frame_foundation.inl").read_text()
start = coherent.index("struct FramePublicationScope51 {")
scope = coherent[start:coherent.index("static bool FistContactFresh174", start)]
start = coherent.index("    FramePublicationScope51 publication51;")
end = coherent.index("    publication51.finish();", start)
publication = coherent[start:coherent.index("\n", end)]
def assignment(name):
    start = coherent.index(name + "=")
    return coherent[start:coherent.index(";", start)+1]
def gate(name):
    start = foundation.index("    const bool " + name + "=")
    return foundation[start:foundation.index(";", start)+1]

fixture = (HERE / "tests/bow_camera_d37b_test.cpp").read_text()
fixture = fixture.replace('#include "vrik/tracked_frame51.h"', '#include "vrik/tracked_frame51.h"\n#include "vrik/body_follow_r38.h"\n#include "vrik/station_body_h41.h"')
fixture = fixture.replace("bool headEvidence122=false;", "bool headEvidence122=false,valid[2]{true,true};std::uint64_t publicationR24=0;")
fixture = fixture.replace("struct PacketMailbox55 {bool read", "struct PacketMailbox55 {bool publish(const CommittedFrame6695& p){packet51=p;return packetReady51;}bool read")
start = fixture.index("static void publishFrame(){")
end = fixture.index("\nstatic bool call(", start)
publisher = r'''
namespace pc=kcd2vr::pose_core;
ACTUAL_PUBLICATION_SCOPE
static bool publisherOwnerCurrentD39=true;
struct PublicationTxD39 {
    f::Frame frame51{},acquisitionReference54{};
    f::State frameState51{};
    kcd2vr::station_body_h41::State stationBodyH41{};
    kcd2vr::body_follow_r38::State bodyFollowR38{};
    std::uint64_t control51=2,epoch=1;
    bool traceH29=false,tracePacketPublishedH29=false;
    struct {std::uint64_t serial=0,action=0;} acquisition54;
    struct {bool head_held=false,right_held=false,left_held=false,right_valid=true,left_valid=true;std::uint64_t sequence=0,tick=0;} tracking;
};
struct PublicationStateD39 {
    void* character=reinterpret_cast<void*>(0x100);
    f::State trackedFrame51{};
    kcd2vr::station_body_h41::State stationBodyH41{};
    kcd2vr::body_follow_r38::State bodyFollowR38{};
    f::Frame ordinaryReference54{},acquisitionReference54{};
    std::uint64_t acquisitionSerial54=0,acquisitionAction54=0;
};
static PublicationStateD39 pairedStateD39{};
static bool rightValidD39=true,leftValidD39=true;
// The parent source's CoherentOwnerCurrent57 was independently checked for the
// ordinary path; this explicit native owner boundary is kept available here.
static bool CoherentOwnerCurrent57(const PublicationTxD39&){return publisherOwnerCurrentD39;}
static void publishFrame(){
    PublicationTxD39 tx{};tx.frame51=frame;tx.control51=g_frame_control51.load();tx.epoch=g_pose_epoch6689.load();
    tx.tracking.sequence=tx.tracking.tick=now51;
    tx.tracking.right_valid=rightValidD39;tx.tracking.left_valid=leftValidD39;
    tx.tracking.right_held=!rightValidD39;tx.tracking.left_held=!leftValidD39;
    auto& s=pairedStateD39;CommittedFrame6695 packet{};
    ACTUAL_PACKET_STAMP packet.headEvidence122=true;
    ACTUAL_VALID_RIGHT ACTUAL_VALID_LEFT
    packet.world[0]=float(frame.rotation.x);packet.world[1]=float(frame.rotation.y);packet.world[2]=float(frame.rotation.z);packet.world[3]=float(frame.rotation.w);
    packet.world[4]=float(frame.root.x);packet.world[5]=float(frame.root.y);packet.world[6]=float(frame.root.z);packet.world[7]=float(frame.scale);
    const auto rejectPublish169r2=[](unsigned){g_committed_pose_valid6689=false;};
ACTUAL_PAIRED_PUBLICATION
    // This is the same final ordinary-owner check preceding the committed flag.
    if(!CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}
    g_committed_pose_valid6689=true;frame=tx.frame51;
    world51={frame.rotation,frame.root,frame.scale};g_tool_context19c4r2=now51<<8;
}
'''.replace("ACTUAL_PUBLICATION_SCOPE", scope).replace("ACTUAL_PAIRED_PUBLICATION", publication)
publisher = publisher.replace("ACTUAL_VALID_RIGHT", assignment("packet.valid[0]")).replace("ACTUAL_VALID_LEFT", assignment("packet.valid[1]"))
publisher = publisher.replace("ACTUAL_PACKET_STAMP", assignment("packet.stamp"))
fixture = fixture[:start]+publisher+fixture[end:]
main_at = fixture.index("\nint main(){using namespace kcd2vr::frik_pose_probe;")
fixture = fixture[:main_at]
chain = r'''
namespace kcd2vr::frik_pose_probe {
static void currentHeadChainD39(int missingSide){
    rightValidD39=leftValidD39=true;seed();
    f::State producer{};unsigned missingFrames=0,readerFailures=0,wrongMask=0;double maxDrift=0;
    kcd2vr::camera_hold_e1::maxHoldMs=0; // a held old base must not mask failure
    for(unsigned n=0;n<200;++n){
        now51+=16;const bool full=true,readingVisual32=false;
        struct {bool head_valid=true,head_held=false,right_valid=true,left_valid=true,right_held=false,left_held=false;} ts;
        if(n){if(missingSide==0)ts.right_valid=false;else ts.left_valid=false;}
        ts.right_held=!ts.right_valid;ts.left_held=!ts.left_valid;
        struct {bool legsReady128=true;} s;void* character=reinterpret_cast<void*>(0x100);ModelWorld6692 frameWorld51{};
ACTUAL_FRAME_GATE
ACTUAL_NEUTRAL_GATE
        f::Input in{};in.allowed=ordinaryFrame51;in.eyeNeutral=ordinaryFresh51;in.hmdHeight=true;
        in.stamp={1,now51,now51,0x100};in.control=2;in.scale=1;in.timeNs=now51*1000000ull;
        in.restHead=in.calibratedHead={0,0,1.7};in.restPelvis={0,0,.9};
        in.nativeHead={0,0,1.7+.05*std::sin(double(n))};in.nativePelvis={0,0,.9+.04*std::cos(double(n))};
        in.bow=true;in.drawing=true;in.bowKey=in.bowGeneration=in.bowRequest=1;
        if(!f::advance(producer,in,frame))++missingFrames;
        frame.root={10,20,5};frame.rotation={};frame.scale=1;
        rightValidD39=ts.right_valid;leftValidD39=ts.left_valid;publishFrame();
        CommittedFrame6695 packet{};
        if(!ReadCommittedPacket6695(packet)||packet.valid[0]!=ts.right_valid||packet.valid[1]!=ts.left_valid)++wrongMask;
        float x=0,y=0,z=0,hx=0,hy=0;bool admitted=false;
        if(!TrackedFrameCameraCoreE1(10.10f,20.10f,6.50f,.12f,.9928f,.02f,x,y,z,hx,hy,admitted)||!admitted)++readerFailures;
        else maxDrift=std::max(maxDrift,std::abs(double(z)-6.756));
    }
    ck(missingFrames==0,"actual admission produces fresh frames through one-controller loss");
    ck(wrongMask==0,"actual paired publisher preserves invalid-wrist mask and current packet admission");
    ck(readerFailures==0,"actual packet/camera/eye chain remains available over three seconds with hold disabled");
    ck(maxDrift<1e-5,"actual camera base remains independent of animated native head height");
    std::printf("HEAD_CHAIN side=%d lost frames=%u cameraReaderFailures=%u wristMaskFailures=%u driftMm=%.9f\n",missingSide,missingFrames,readerFailures,wrongMask,maxDrift*1000);
    rightValidD39=leftValidD39=true;
}
}
int main(){using namespace kcd2vr::frik_pose_probe;
 currentHeadChainD39(0);currentHeadChainD39(1);
 std::printf("HEAD_FRAME_CHAIN %u/%u checks passed; source publication/admission/camera exact, native leaves synthetic\n",checks-failures,checks);
 return failures?1:0;
}
'''.replace("ACTUAL_FRAME_GATE", gate("ordinaryFrame51")).replace("ACTUAL_NEUTRAL_GATE", gate("ordinaryFresh51"))
fixture += chain
path = output / "production_head_frame_chain_d39.cpp"
path.write_text(fixture)
binary = output / "production_head_frame_chain_d39"
subprocess.run(["c++", "-std=c++23", "-O1", "-pthread", "-I", str(output), "-I", str(source / "tests/shims"),
                "-I", str(source / "src"), str(path), "-o", str(binary)], check=True)
result = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
print(result.stdout, end="")
(output / "head_frame_chain_result.txt").write_text(result.stdout)
raise SystemExit(result.returncode)
