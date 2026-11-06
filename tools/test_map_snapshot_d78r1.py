"""Current map readers/presenters with explicit engine leaves, using a host C++ compiler.

This does not compile Windows SEH or inject the game. The maintained Windows
presenter tests still exercise the actual guarded filename reader with MSVC.
"""
from pathlib import Path
import argparse
import ast
import hashlib
import json
import os
import shutil
import subprocess


def literal(path, name):
    for node in ast.parse(path.read_text()).body:
        if isinstance(node, ast.Assign) and any(isinstance(t, ast.Name) and t.id == name for t in node.targets):
            return ast.literal_eval(node.value)
    raise ValueError(name)


def function(source, name):
    start = source.index(name)
    end = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source-root', type=Path, default=Path(__file__).resolve().parents[1])
parser.add_argument('--out', type=Path, required=True)
parser.add_argument('--cxx', default='g++')
parser.add_argument('--sanitize', action='store_true')
args = parser.parse_args()
root = args.source_root.resolve()
out = args.out.resolve()
out.mkdir(parents=True, exist_ok=True)
cxx = shutil.which(args.cxx)
if not cxx:
    raise SystemExit('Host compiler unavailable: ' + args.cxx)
receipts = []


def run_fixture(name, source, expected_failure=False):
    cpp = out / (name + '.cpp')
    binary = out / (name + ('.exe' if os.name == 'nt' else ''))
    cpp.write_text(source)
    command = [cxx, '-std=c++20', '-O1', '-g', '-UNDEBUG', '-I', str(root / 'src'), str(cpp), '-o', str(binary)]
    if args.sanitize:
        command[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
    build = subprocess.run(command, capture_output=True, text=True)
    (out / (name + '.build.log')).write_text(build.stdout + build.stderr)
    if build.returncode:
        raise RuntimeError(build.stdout + build.stderr)
    run = subprocess.run([str(binary)], capture_output=True, text=True)
    log = run.stdout + run.stderr
    (out / (name + '.run.log')).write_text(log)
    good = run.returncode != 0 and 'FAIL' in log if expected_failure else run.returncode == 0 and 'failures=0' in log
    receipt = dict(name=name, passed=good, expected_failure=expected_failure, exit=run.returncode,
                   source_sha256=hashlib.sha256(source.encode()).hexdigest(), command=command, output=log)
    receipts.append(receipt)
    print(log.strip())
    if not good:
        raise AssertionError(receipt)


old = root / 'src/subsystems/vrmap/committed_pose_service.inl'
paired = root / 'src/subsystems/vrmap/paired_pose_snapshot_d78r1.inl'
header = (root / 'src/frik_pose_probe.h').read_text()
start = header.index('struct HeldMapSnapshotD78R1 {')
snapshot_type = header[start:header.index('\n};', start) + 3]
prefix = literal(root / 'tools/test_map_pose_reader_m39.py', 'prefix')
prefix = prefix.replace('bool ReadCommittedPacket6695(CommittedFrame6695& p){p=packet;return packetOK;}', '''
unsigned packetReads=0;bool advanceAfterRead=false;
bool ReadCommittedPacket6695(CommittedFrame6695& p){++packetReads;p=packet;
 if(advanceAfterRead){++packet.publicationR24;++packet.stamp.sample;packet.fistSlotH31[1].t[0]=99;}
 return packetOK;}
''')
prefix += snapshot_type + '\n'
test = r'''
unsigned checks=0,failures=0;
void Check(bool good,const char* why){++checks;if(!good){++failures;std::printf("FAIL %s\n",why);}}
void Reset(){packet={};liveTracking={};g_census_mode=2;g_pose_epoch6689=1;g_weapon_request_epoch6694=1;
 g_last_player_entity=reinterpret_cast<void*>(0x1234);diagnostics::lab::reference=false;
 animation_filter::nativeBodyOwned112=false;kcd2vr::native_passenger_a34::busy=false;
 calibration=packetOK=liveOK=tracking[0]=tracking[1]=true;claim[0]=claim[1]=false;
 item[0]=item[1]=equipment=0;clockNow=1000;packetReads=worldCalls=0;advanceAfterRead=false;
 packet.fistSlotH31[0].t[0]=4;packet.fistSlotH31[1].t[0]=7;}
int main(){HeldMapSnapshotD78R1 s{};unsigned why=0;
 Reset();Check(ReadHeldMapSnapshotD78R1(s,true,&why)&&why==0&&s.right[4]==4&&s.left[4]==7&&
 s.haveLeft&&s.leftGrip==.8f&&s.sourceEntity==0x1234&&s.sourceEpoch==1&&s.sourceTick==1000&&s.pair.publicationR24==10,"one complete pair");
 Check(packetReads==1,"both sockets and grip require one committed read");
 Reset();advanceAfterRead=true;Check(ReadHeldMapSnapshotD78R1(s,true,&why)&&s.haveLeft&&s.left[4]==7&&s.pair.publicationR24==10&&packet.publicationR24==11,"publication advance cannot mix support hand");
 Check(packetReads==1,"publication race does not retry or wait");
 Reset();Check(ReadHeldMapSnapshotD78R1(s,false,&why)&&!s.haveLeft&&s.leftGrip==0&&s.left[4]==0&&packetReads==1&&worldCalls==1,"single hand avoids support work");
 for(unsigned side=0;side<2;++side){
  for(unsigned gate=0;gate<6;++gate){Reset();
   if(gate==0)item[side]=1;if(gate==1)tracking[side]=false;if(gate==2)claim[side]=true;
   if(gate==3)packet.mapPresentationOwnerM34[side]=1;if(gate==4)packet.fistSlotValidH31[side]=false;if(gate==5)packet.valid[side]=false;
   const bool ok=ReadHeldMapSnapshotD78R1(s,true,&why);
   Check(side?(ok&&!s.haveLeft):(!ok&&!s.sourceEntity&&!s.pair.valid&&!s.right[4]),"selected hand admission and cleared failure");
  }
 }
 for(unsigned gate=0;gate<13;++gate){Reset();
  if(gate==0)g_census_mode=1;if(gate==1)diagnostics::lab::reference=true;if(gate==2)calibration=false;
  if(gate==3)animation_filter::nativeBodyOwned112=true;if(gate==4)equipment=1;if(gate==5)packetOK=false;
  if(gate==6)packet.stamp.tick=919;if(gate==7)packet.stamp.tick=1001;if(gate==8)packet.stamp.tick=0;
  if(gate==9)kcd2vr::native_passenger_a34::busy=true;if(gate==10)packet.fistSlotH31[0].t[1]=NAN;
  if(gate==11)packet.fistSlotH31[0].q[1]=INFINITY;if(gate==12)packet.mapPresentationOwnerM34[0]=512;
  Check(!ReadHeldMapSnapshotD78R1(s,true,&why)&&why&&!s.sourceEntity&&!s.pair.valid&&!s.right[4],"primary refusal clears prior output");
 }
 for(unsigned gate=0;gate<15;++gate){Reset();
  if(gate==0)liveOK=false;if(gate==1)liveTracking.allowed=false;
  if(gate==2)liveTracking.head_valid=false;if(gate==3)liveTracking.left_valid=false;if(gate==4)liveTracking.right_valid=false;
  if(gate==5)liveTracking.head_held=true;if(gate==6)liveTracking.left_held=true;if(gate==7)liveTracking.right_held=true;
  if(gate==8)liveTracking.tick=919;if(gate==9)liveTracking.tick=1001;if(gate==10)liveTracking.tick=0;
  if(gate==11)packet.mapGripM39[1]=NAN;if(gate==12)packet.mapGripM39[1]=1.1f;
  if(gate==13)packet.mapFoldTrackedM39=false;if(gate==14)packet.publicationR24=0;
  Check(ReadHeldMapSnapshotD78R1(s,true,&why)&&!s.haveLeft&&!s.leftGrip&&!s.left[4],"strict support refusal retains primary only");
 }
 Reset();g_pose_epoch6689=2;Check(ReadHeldMapSnapshotD78R1(s,true,&why)&&!s.sourceEntity&&!s.sourceEpoch&&!s.sourceTick,"old body cannot bootstrap presenter");
 Reset();g_last_player_entity=reinterpret_cast<void*>(0x9999);Check(ReadHeldMapSnapshotD78R1(s,true,&why)&&!s.sourceEntity,"replaced player cannot bootstrap presenter");
 Reset();float pose[8]{},grip=0;HeldMapPairM39 pair{};
 Check(ReadHeldMapRightPoseM41(pose,&why,nullptr,nullptr,nullptr,&pair)&&pose[4]==4,"legacy right uses same socket admission");
 Check(ReadHeldMapSupportLeftD64(pose,grip,pair)&&pose[4]==7&&grip==.8f,"legacy support contract retained");
 packet.publicationR24++;Check(!ReadHeldMapSupportLeftD64(pose,grip,pair),"legacy separate support still rejects changed publication");
 std::printf("D78R1 map snapshot checks=%u failures=%u nativeLeaves=SUBSTITUTED\n",checks,failures);return failures?1:0;
}
'''
run_fixture('snapshot', prefix + old.read_text() + paired.read_text() + test)

# The refactor also serves existing independent readers. Retain the complete
# D64 mutation/ownership suite against these same production functions.
support_fixture = root / 'tools/test_map_support_d64.py'
support_prefix = literal(root / 'tools/test_map_pose_reader_m39.py', 'prefix').replace(
    'bool ReadCommittedPacket6695(CommittedFrame6695& p){p=packet;return packetOK;}',
    literal(support_fixture, 'leaf'))
run_fixture('retained_support', support_prefix + literal(support_fixture, 'proto') +
            old.read_text() + literal(support_fixture, 'test'))

# Reuse maintained assertions, extracting current production orchestration.
# Only the Windows memory/string read is substituted in this host-only run.
presenter = (root / 'src/subsystems/vrmap/runtime/held_map_m1.cpp').read_text()
feed = (root / 'src/subsystems/vrmap/runtime/held_map_feed_t2.cpp').read_text()
native_string_leaf = r'''
bool FilenameMatchesM43(const char* file,const char* expected) noexcept {
 if(!file||!expected)return false;char actual[160]{};unsigned i=0;
 for(;i<159&&file[i];++i){char c=file[i];if(c=='\\')c='/';if(c>='A'&&c<='Z')c=char(c-'A'+'a');actual[i]=c;}
 return !file[i]&&!std::strcmp(actual,expected);
}
'''
for fixture, label in [('test_m41_presenter.py', 'world'), ('test_native_paper_presenter_m43.py', 'native')]:
    path = root / 'tools' / fixture
    pre = literal(path, 'prefix').replace('#define GetTickCount64 WinGetTickCount64\n#include <windows.h>\n#undef GetTickCount64\n', '')
    middle = literal(path, 'middle')
    cases = literal(path, 'test').replace('NativeFrameTick();', '::NativeFrameTick();')
    actual_frame = function(presenter, 'void NativeFrameTick() noexcept')
    before = (pre + function(feed, 'bool StaticModelM41(') + middle + native_string_leaf +
              function(presenter, 'bool StaticModelReadyRawD72(').replace('StaticModelReadyRawD72', 'StaticModelReadyM41') + '\n')
    after = '\n' + function(presenter, 'void Shutdown() noexcept') + '\n' + cases
    run_fixture(label + '_presenter', before + actual_frame + after)
    # This negative control must be caught by the retained presenter assertions.
    broken = actual_frame.replace('!StaticModelReadyM41(entity,expected)', 'false', 1)
    assert broken != actual_frame
    run_fixture(label + '_model_gate_control', before + broken + after, expected_failure=True)

report = dict(passed=all(x['passed'] for x in receipts), compiler=cxx, sanitized=args.sanitize,
              native_leaves='SUBSTITUTED', windows_seh='NOT_RUN', game_dll='NOT_BUILT', headset='NOT_RUN',
              sanitizer_environment={'ASAN_OPTIONS': os.environ.get('ASAN_OPTIONS', '')},
              sources={str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest() for p in
                       [old, paired, root / 'src/subsystems/vrmap/runtime/held_map_m1.cpp']}, fixtures=receipts)
(out / 'RESULT.json').write_text(json.dumps(report, indent=2) + '\n')
