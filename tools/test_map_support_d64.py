"""Exercise actual D64 right-primary/left-support readers; native leaves only are substituted."""
from pathlib import Path
import ast, json, hashlib
from m41_test_support import options, compile_and_run

args = options('support_d64')
src, out = args.source_root, args.out
tree = ast.parse((src/'tools/test_map_pose_reader_m39.py').read_text())
prefix = next(ast.literal_eval(n.value) for n in tree.body if isinstance(n, ast.Assign)
              and any(isinstance(t, ast.Name) and t.id == 'prefix' for t in n.targets))
old = 'bool ReadCommittedPacket6695(CommittedFrame6695& p){p=packet;return packetOK;}'
leaf = r"""unsigned reads=0, mutateAt=0, mutateKind=0;
bool ReadCommittedPacket6695(CommittedFrame6695& p){
 if(++reads==mutateAt){switch(mutateKind){case 0:++packet.stamp.sample;break;
  case 1:++packet.stamp.epoch;break;case 2:++packet.stamp.actor;break;
  case 3:packet.world[4]+=.01f;break;case 4:--packet.stamp.tick;break;
  case 5:packet.mapFoldTrackedM39=false;break;case 6:packetOK=false;break;case 7:packet.publicationR24+=2;break;}}
 p=packet;return packetOK;
}
"""
assert prefix.count(old)==1
prefix=prefix.replace(old,leaf)
body=(src/'src/subsystems/vrmap/committed_pose_service.inl').read_text()
proto='bool ReadHeldMapRightPoseM41(float (&out)[8],unsigned* refusal=nullptr,std::uintptr_t* sourceEntity=nullptr,std::uint64_t* sourceEpoch=nullptr,std::uint64_t* sourceTick=nullptr,HeldMapPairM39* pair=nullptr) noexcept;\n'
test=r"""
#include <limits>
#include <initializer_list>
unsigned checks=0, failures=0;
#define C(x) do {++checks;if(!(x)){++failures;std::printf("FAIL line%d %s\n",__LINE__,#x);}} while(0)
void reset(){packet={};liveTracking={};packetOK=liveOK=calibration=true;tracking[0]=tracking[1]=true;
 claim[0]=claim[1]=false;item[0]=item[1]=equipment=0;clockNow=1000;g_census_mode=2;g_pose_epoch6689=1;
 diagnostics::lab::reference=false;animation_filter::nativeBodyOwned112=false;kcd2vr::native_passenger_a34::busy=false;
 reads=mutateAt=mutateKind=0;packet.fistSlotH31[0].t[0]=40;packet.fistSlotH31[1].t[0]=70;
 packet.allowed[0].t[0]=400;packet.allowed[1].t[0]=700;packet.world[0]=.25f;packet.world[4]=9;
}
int main(){float primaryPose[8]{},left[8]{};float grip=0;HeldMapPairM39 primary{};
 const auto primaryRead=[&](){return ReadHeldMapRightPoseM41(primaryPose,nullptr,nullptr,nullptr,nullptr,&primary);};
 const auto supportRead=[&](){return ReadHeldMapSupportLeftD64(left,grip,primary);};
 reset();C(primaryRead()&&primary.valid&&primaryPose[4]==40&&primary.actor==packet.stamp.actor&&
   primary.epoch==1&&primary.sample==5&&primary.tick==1000&&primary.publicationR24==10&&std::memcmp(primary.world,packet.world,sizeof(primary.world))==0);
 C(supportRead()&&left[4]==70&&grip==.8f); // LEFT native socket and LEFT grip, never primary/right or allowed-wrist fallback.
 for(unsigned k=0;k<8;++k){reset();C(primaryRead());switch(k){case 0:primary.valid=false;break;
  case 1:++primary.actor;break;case 2:++primary.epoch;break;case 3:++primary.sample;break;
  case 4:--primary.tick;break;case 5:primary.world[7]+=.01f;break;case 6:primary.publicationR24+=2;break;case 7:primary.publicationR24=0;break;}C(!supportRead()&&grip==0);}
 // Mutation before the first support packet and between the two support packet reads.
 for(unsigned location=1;location<=2;++location)for(unsigned k=0;k<8;++k){reset();C(primaryRead());
  mutateKind=k;mutateAt=reads+location;C(!supportRead()&&grip==0);}
 // Same-tick new sample and same-stamp changed world must both refuse.
 reset();C(primaryRead());++packet.stamp.sample;C(!supportRead());
 reset();C(primaryRead());packet.world[5]+=.001f;C(!supportRead());
 for(unsigned k=0;k<12;++k){reset();C(primaryRead());switch(k){case 0:item[1]=1;break;
  case 1:claim[1]=true;break;case 2:packet.mapPresentationOwnerM34[1]=1;break;
  case 3:packet.fistSlotValidH31[1]=false;break;case 4:packet.valid[1]=false;break;
  case 5:tracking[1]=false;break;case 6:calibration=false;break;
  case 7:animation_filter::nativeBodyOwned112=true;break;case 8:kcd2vr::native_passenger_a34::busy=true;break;
  case 9:diagnostics::lab::reference=true;break;case 10:equipment=1;break;case 11:g_census_mode=1;break;}
  C(!supportRead());}
 // Left occupancy removes only support; right primary remains usable.
 reset();item[1]=1;C(primaryRead()&&!supportRead());
 reset();item[0]=1;C(!primaryRead()&&!primary.valid);C(!supportRead());
 for(unsigned k=0;k<11;++k){reset();C(primaryRead());switch(k){case 0:liveOK=false;break;
  case 1:liveTracking.allowed=false;break;case 2:liveTracking.head_valid=false;break;
  case 3:liveTracking.left_valid=false;break;case 4:liveTracking.right_valid=false;break;
  case 5:liveTracking.head_held=true;break;case 6:liveTracking.left_held=true;break;
  case 7:liveTracking.right_held=true;break;case 8:liveTracking.tick=0;break;
  case 9:liveTracking.tick=919;break;case 10:liveTracking.tick=1001;break;}C(!supportRead());}
 for(float value:{-1.f,1.01f,std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()}){
  reset();C(primaryRead());packet.mapGripM39[1]=value;C(!supportRead());}
 for(float value:{0.f,.5f,1.f}){reset();C(primaryRead());packet.mapGripM39[1]=value;C(supportRead()&&grip==value);}
 for(unsigned age:{80u,81u}){reset();packet.stamp.tick=1000-age;liveTracking.tick=1000-age;
  C(primaryRead()==(age==80));C(supportRead()==(age==80));}
 reset();C(primaryRead());packet.fistSlotH31[1].t[2]=std::numeric_limits<float>::quiet_NaN();C(!supportRead());
 std::printf("D64 actual right-primary/left-support checks=%u failures=%u nativeLeaves=SUBSTITUTED headset=NOT_RUN\n",checks,failures);
 return failures?1:0;
}
"""
cpp=out/'support.cpp'
cpp.write_text(prefix+proto+body+test)
p,log=compile_and_run(args,cpp,'support')
report={'exit':p.returncode,'passed':p.returncode==0,'result':log,
        'actual_body_sha256':hashlib.sha256(body.encode()).hexdigest(),
        'source_file_sha256':hashlib.sha256((src/'src/subsystems/vrmap/committed_pose_service.inl').read_bytes()).hexdigest(),
        'fixture_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'asan':True,'native_leaves':'SUBSTITUTED','headset':'NOT_RUN'}
(out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
print(log)
assert report['passed'],(p.returncode,log)
