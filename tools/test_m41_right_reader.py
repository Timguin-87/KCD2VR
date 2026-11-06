from pathlib import Path
import ast,subprocess,json,hashlib
from m41_test_support import options,compile_and_run
args=options('right_reader');src=args.source_root;out=args.out
tree=ast.parse((src/'tools/test_map_pose_reader_m39.py').read_text());prefix=next(ast.literal_eval(n.value)for n in tree.body if isinstance(n,ast.Assign)and any(isinstance(t,ast.Name)and t.id=='prefix'for t in n.targets))
body=(src/'src/subsystems/vrmap/committed_pose_service.inl').read_text().split('// Optional second hand')[0]
proto='bool ReadHeldMapRightPoseM41(float (&out)[8],unsigned* refusal=nullptr,std::uintptr_t* sourceEntity=nullptr,std::uint64_t* sourceEpoch=nullptr,std::uint64_t* sourceTick=nullptr,HeldMapPairM39* pair=nullptr) noexcept;\n'
test=r'''
int main(){float out[8]{};unsigned reason=0,n=0;std::uintptr_t source=0;std::uint64_t epoch=0,tick=0;
#define C(x) do{assert(x);++n;}while(0)
packet.fistSlotH31[0].t[0]=40;packet.fistSlotH31[1].t[0]=70;
C(ReadHeldMapRightPoseM41(out,&reason,&source,&epoch,&tick)&&out[4]==40&&reason==0&&source==0x1234&&epoch==1&&tick==1000);
C(ReadHeldMapLeftPoseM1(out)&&out[4]==70);
item[1]=1;C(ReadHeldMapRightPoseM41(out)&&!ReadHeldMapLeftPoseM1(out));item[1]=0;
item[0]=1;C(!ReadHeldMapRightPoseM41(out,&reason)&&reason==4&&ReadHeldMapLeftPoseM1(out));item[0]=0;
packet.valid[1]=false;C(ReadHeldMapRightPoseM41(out));packet.valid[1]=true;
packet.valid[0]=false;C(!ReadHeldMapRightPoseM41(out));packet.valid[0]=true;
tracking[0]=false;C(!ReadHeldMapRightPoseM41(out));tracking[0]=true;
claim[0]=true;C(!ReadHeldMapRightPoseM41(out)&&ReadHeldMapLeftPoseM1(out));claim[0]=false;
claim[1]=true;C(ReadHeldMapRightPoseM41(out));claim[1]=false;
packet.mapPresentationOwnerM34[0]=1;C(!ReadHeldMapRightPoseM41(out));packet.mapPresentationOwnerM34[0]=0;
packet.fistSlotValidH31[0]=false;C(!ReadHeldMapRightPoseM41(out));packet.fistSlotValidH31[0]=true;
packet.stamp.tick=919;C(!ReadHeldMapRightPoseM41(out));packet.stamp.tick=1001;C(!ReadHeldMapRightPoseM41(out));packet.stamp.tick=1000;
calibration=false;C(!ReadHeldMapRightPoseM41(out));calibration=true;
animation_filter::nativeBodyOwned112=true;C(!ReadHeldMapRightPoseM41(out));animation_filter::nativeBodyOwned112=false;
kcd2vr::native_passenger_a34::busy=true;C(!ReadHeldMapRightPoseM41(out));kcd2vr::native_passenger_a34::busy=false;
C(ReadHeldMapRightPoseM41(out)&&out[4]==40);
printf("M41 actual committed RIGHT/retained LEFT %u/%u\n",n,n);
}
'''
(out/'readers.cpp').write_text(prefix+proto+body+test)
p,log=compile_and_run(args,out/'readers.cpp','readers')
assert p.returncode==0 and '17/17'in log,(p.returncode,log)
(out/'RESULT.json').write_text(json.dumps({'exit':p.returncode,'actual_body_sha256':hashlib.sha256(body.encode()).hexdigest(),'result':log,'native_leaves':'SUBSTITUTED','headset':'NOT_RUN'},indent=2))
print(log)
