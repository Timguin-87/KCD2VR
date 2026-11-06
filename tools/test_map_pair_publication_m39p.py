"""M39P regression (adapted from the ChatGPT source3 review repro): both production map
readers + the actual NativeFrameTick fold block; a completed-pose publication lands between the
LEFT and RIGHT reads with 6 cm common motion. Expect the fold fraction to equal the coherent
control. Fails on Step30 8a8c547 (0.6485 vs 1.0), passes with M39P. Host g++ ASan/UBSan.
Usage: python test_map_pair_publication_m39p.py --source <tree> --out <dir>"""
from pathlib import Path
import ast, subprocess, json, os, hashlib
import argparse
ap=argparse.ArgumentParser();ap.add_argument('--source',required=True);ap.add_argument('--out',required=True);args=ap.parse_args()
root=Path(args.out).resolve();src=Path(args.source).resolve()
for d in ('repros','validation'):(root/d).mkdir(parents=True,exist_ok=True)
tree=ast.parse((src/'tools/test_map_pose_reader_m39.py').read_text())
prefix=next(ast.literal_eval(n.value) for n in tree.body if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='prefix' for t in n.targets))
prefix=prefix.replace('bool ReadCommittedPacket6695(CommittedFrame6695& p){p=packet;return packetOK;}', '''CommittedFrame6695 secondPacket{};bool splitPublication=false;unsigned reads=0;
bool ReadCommittedPacket6695(CommittedFrame6695& p){p=(splitPublication&&reads++==1)?secondPacket:packet;return packetOK;}''')
prefix=prefix.replace('Pose HandWorld6694(Pose p,const float*){++worldCalls;return p;}', 'Pose HandWorld6694(Pose p,const float* world){++worldCalls;for(unsigned i=0;i<3;++i)p.t[i]+=world[4+i];return p;}')
prefix+='#include "'+str(src/'held_map_pose_m1.h')+'"\n' if False else ''
prefix+='#include "'+str(src/'src/held_map_pose_m1.h')+'"\n'
if 'HeldMapPairM39' in (src/'src/subsystems/vrmap/committed_pose_service.inl').read_text() and 'struct HeldMapPairM39' not in prefix:
 prefix=prefix.replace('struct Stamp{','struct Stamp{std::uint64_t sample=5;',1)
 prefix='#include <cstring>\n#include <cstdint>\nstruct HeldMapPairM39 {bool valid=false;std::uintptr_t actor=0;std::uint64_t epoch=0,sample=0,tick=0;float world[8]{};};\n'+prefix
 shellpair=True
else: shellpair='HeldMapPairM39' in prefix
prefix+='#include "'+str(src/'src/subsystems/vrmap/map_fold_m39.h')+'"\n'
body=(src/'src/subsystems/vrmap/committed_pose_service.inl').read_text()
host=(src/'src/subsystems/vrmap/runtime/held_map_m1.cpp').read_text();lo=host.index(' float hand[8]{},pose[8]{};');hi=host.index('\n\n // Bootstrap',lo); caller=host[lo:hi]
shell='''
namespace frik_pose_probe {using ::ReadHeldMapLeftPoseM1; using ::ReadHeldMapFreeRightPoseM39;'''+('using ::HeldMapPairM39;' if shellpair else '')+'''}
using namespace kcd2vr;
std::atomic<unsigned> poseRefusal19c3{0};
bool Present31(std::uint64_t){return true;} bool Allowed(std::uint64_t){return true;} bool Requested(){return true;}
struct Intent{std::uint64_t serial=77;bool open=true;}; Intent Intent31(){return {};}
struct Fold {std::uint64_t serial=0,tick=0;float fraction=-1;};
struct Pub{Fold value{};bool publish(Fold f){value=f;return true;}}foldM39;
bool closeCalled=false; bool CloseIfCurrentM39(std::uint64_t,std::uint64_t){closeCalled=true;return true;}
namespace diagnostic_trace{void Write(const char*){}}
map_fold_m39::Gesture folding39;
map_fold_m39::Result actualMapStep(){ const auto now=clockNow; reads=0;
'''+caller+'''
return fold39;
}
void prepare(std::uint64_t now,float grip){clockNow=liveTracking.tick=packet.stamp.tick=now;packet.mapGripM39[0]=grip;}
int main(){
 packet.world[3]=1.f;packet.world[7]=1.f; // physically valid identity world transform
 for(auto& p:packet.allowed)for(auto& x:p.t)x=0;
 for(auto& p:packet.fistSlotH31)for(auto& x:p.t)x=0;
 float hand[]{0,0,0,1,0,0,0,1},map[8]{},edge[3]{},local[]{-.23396474f,-.05038497f,.141948f};
 assert(held_map_pose_m1::Place(hand,map));held_map_pose_m1::Point(map,local,edge);
 for(unsigned i=0;i<3;++i)packet.allowed[0].t[i]=edge[i];
 const float gap=std::sqrt(edge[0]*edge[0]+edge[1]*edge[1]+edge[2]*edge[2]);
 prepare(1000,0);actualMapStep();prepare(1010,1);auto initial=actualMapStep();assert(initial.fraction>.999f);
 const auto savedGesture=folding39;secondPacket=packet;secondPacket.stamp.tick=1026;
 const float commonTranslation=.06f;
 for(unsigned i=0;i<3;++i)secondPacket.world[4+i]=-commonTranslation*edge[i]/gap;
 clockNow=liveTracking.tick=1026;splitPublication=true;auto mixed=actualMapStep();
 std::printf("MIXED left_tick=1010 right_tick=1026 shared_translation=%.3f true_separation=%.6f output_fraction=%.6f close=%d reads=%u\\n",commonTranslation,gap,mixed.fraction,mixed.close,reads);
 // Control uses the SAME current packet for both actual readers; poses relative to each other are unchanged.
 packet=secondPacket;splitPublication=false;folding39=savedGesture;auto coherent=actualMapStep();
 std::printf("COHERENT left_tick=1026 right_tick=1026 true_separation=%.6f output_fraction=%.6f close=%d\\n",gap,coherent.fraction,coherent.close);
 bool pass=std::fabs(mixed.fraction-coherent.fraction)<1e-5;
 std::printf("paired_publication_invariant=%d native_engine=MODELED gameplay_execution=NONE\\n",pass);
 return pass?0:1;
}
'''
p=root/'repros/map_mixed_publication.cpp';p.write_text(prefix+body+shell)
cmd=['g++','-std=c++20','-O0','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-pthread',str(p),'-o',str(root/'validation/map_mixed_publication')]
c=subprocess.run(cmd,capture_output=True,text=True);(root/'validation/map_mixed.compile.log').write_text(c.stdout+c.stderr)
result={'compile_command':cmd,'compile_exit':c.returncode,'production_reader_sha256':hashlib.sha256(body.encode()).hexdigest(),'production_caller_fragment_sha256':hashlib.sha256(caller.encode()).hexdigest()}
if c.returncode==0:
 p=subprocess.run([str(root/'validation/map_mixed_publication')],capture_output=True,text=True,env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0','UBSAN_OPTIONS':'halt_on_error=1'})
 result.update(exit=p.returncode,output=p.stdout+p.stderr);(root/'validation/map_mixed.run.log').write_text(p.stdout+p.stderr)
print(json.dumps(result,indent=2));(root/'validation/map_mixed.json').write_text(json.dumps(result,indent=2))
