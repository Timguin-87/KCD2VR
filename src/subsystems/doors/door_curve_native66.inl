// Included after native_door19c4.inl declares door4 and after existing validated
// entity/character/model/helper readers. Called ONLY by the claimed AnimDoor Lua
// getter on that Door's native callback. No worker, playback, seek, or load call.
#include "core/door_path66.h"

namespace door_curve_detail66 {
namespace d=kcd2vr::door_path66;
struct Rig {void* entity=nullptr;void* character=nullptr;void* anim=nullptr;void* owner=nullptr;void* joints=nullptr;
 bool left=false;std::uint64_t topology=0;};
inline std::uint64_t hash(std::uint64_t h,std::uint64_t v){
 for(unsigned k=0;k<8;++k){h^=(v>>(k*8))&255u;h*=1099511628211ull;}return h;
}
inline d::Rotation multiply(d::Rotation a,d::Rotation b){return {
 a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
 a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};}
inline d::Pose compose(d::Pose a,d::Pose b){return {d::add(a.p,d::rotate(a.q,b.p)),multiply(a.q,b.q)};}
inline bool convert(const QuatT28& p,d::Pose& out){
 out={{p.t[0],p.t[1],p.t[2]},{p.q[0],p.q[1],p.q[2],p.q[3]}};return d::valid(out);
}
inline bool alias(const char* text,char (&out)[32],bool& left){
 if(!text)return false;unsigned n=0;for(;n<31&&text[n];++n)out[n]=text[n];out[n]=0;
 const char* p=out;if(n==25&&!std::memcmp(p,"stealth_",8))p+=8;else if(n!=17)return false;
 if(std::memcmp(p,"door_",5)||(p[5]!='l'&&p[5]!='r')||p[6]!='_'||(p[7]!='b'&&p[7]!='f')||
    p[8]!='_'||(p[9]!='o'&&p[9]!='c')||std::memcmp(p+10,"_player",8))return false;
 left=p[5]=='l';return true;
}
inline bool rig(const door4::Request& r,bool left,Rig& out){
 out={};const auto base=WhBase();
 if(!SupportedWhBuild(base)||!r.id||!r.entity||EntityById6694(r.id)!=reinterpret_cast<void*>(r.entity))return false;
 void* entity=reinterpret_cast<void*>(r.entity);std::uintptr_t vf=0,fn=0;
 if(!PhysVft6698(entity,kCEntityVftRva)||!Read(entity,vf)||!Read(reinterpret_cast<void*>(vf+0x328),fn)||
    fn!=base+kCEntityGetCharacterRva)return false;
 void* character=CharacterForEntity6689(entity);void* anim=nullptr;void* owner=nullptr;void* joints=nullptr;
 if(!ValidateCharInstance(character,anim)||!Read(static_cast<unsigned char*>(character)+0xA70,owner)||
    !PhysVft6698(owner,kCDefaultSkeletonVftRva)||!Read(static_cast<unsigned char*>(owner)+8,joints)||!joints)return false;
 // Retail joint-count leaf 1AAF020 reads exactly table[-4]. Do not call the
 // Henry helper ReadJointCount: its >=8 anatomical minimum excludes Door's six.
 std::uint32_t count=0;if(!Read(static_cast<unsigned char*>(joints)-4,count)||count!=6)return false;
 constexpr std::uint32_t leftCrc[]{0x50256111u,0xBBB56880u,0xD78F8711u,0x559EC994u,0xCC97982Eu,0x87D03C54u};
 constexpr std::uint32_t rightCrc[]{0x9D39FE0Au,0x56B1685Eu,0xB6097CC2u,0xB0E35325u,0x29EA029Fu,0x87D03C54u};
 constexpr int parents[]{-1,0,0,1,1,1};std::uint64_t fingerprint=1469598103934665603ull;
 for(int i=0;i<6;++i){std::uint32_t crc=0;int parent=-2;
  if(!Read(static_cast<unsigned char*>(joints)+i*0x30+8,crc)||crc!=(left?leftCrc[i]:rightCrc[i])||
     !ReadParentDirect(owner,i,parent)||parent!=parents[i])return false;
  fingerprint=hash(hash(fingerprint,crc),std::uint64_t(parent));
 }
 out={entity,character,anim,owner,joints,left,fingerprint};return true;
}
inline bool sameRig(const Rig& a,const Rig& b){return a.entity==b.entity&&a.character==b.character&&a.anim==b.anim&&
 a.owner==b.owner&&a.joints==b.joints&&a.left==b.left&&a.topology==b.topology;}
inline bool world(const Rig& rig,d::Pose& out){
 ModelWorld6692 w{};if(!ReadModelWorld6692(rig.character,w)||!std::isfinite(w.scale)||std::abs(w.scale-1.f)>.001f)return false;
 out={{w.t.x,w.t.y,w.t.z},{w.q.x,w.q.y,w.q.z,w.q.w}};return d::valid(out);
}
inline bool liveClip(const Rig& rig,int animation){
 void* entries=nullptr;unsigned count=0;int current=-1;
 return Read(static_cast<unsigned char*>(rig.anim)+0x48,entries)&&entries&&
     Read(static_cast<unsigned char*>(entries)-4,count)&&count==1&&
     Read(static_cast<unsigned char*>(entries)+0x10,current)&&current==animation;
}
inline bool pivot(const Rig& rig,int animation,float phase,d::Point constantP,d::Pose& out){
 float root[7]{},leaf[7]{};const auto rootCrc=rig.left?0x50256111u:0x9D39FE0Au;
 const auto pivotCrc=rig.left?0xBBB56880u:0x56B1685Eu;
 if(animation_filter::SampleCurveReference(rig.anim,animation,rootCrc,phase,root)!=0||
    animation_filter::SampleCurveReference(rig.anim,animation,pivotCrc,phase,leaf)!=0)return false;
 // The exact retail channel profile is rotation-only. A replacement clip with
 // any sampled pivot translation is outside this binder, not silently erased.
 if(leaf[4]!=0.f||leaf[5]!=0.f||leaf[6]!=0.f)return false;
 const d::Pose a{{root[4],root[5],root[6]},{root[0],root[1],root[2],root[3]}};
 // Exact sixteen retail object clips: root has P+R; pivot has R only. The
 // sampler initializes missing P to ZERO while returning success. Restore the
 // native default local translation; never erase the authored one-metre pivot.
 const d::Pose b{constantP,{leaf[0],leaf[1],leaf[2],leaf[3]}};
 if(!d::valid(a)||!d::valid(b))return false;out=compose(a,b);return d::valid(out);
}
struct Cache {std::uint64_t serial=0,firstTick=0,lastAttempt=0;unsigned attempts=0;bool ready=false,pending=false;Rig rig{};char name[32]{};
 animation_filter::CurveReferenceInfo clip{};d::Point constantP{},anchor{};d::Curve curve{};};
inline Cache cache;
inline thread_local bool pendingResult=false;
inline bool requestSource(const door4::Request& r,const d::Source& s){return s.operation==r.serial&&s.player==r.player&&
 s.entity==r.entity&&s.target==r.id&&s.side==r.side&&s.poseEpoch==r.epoch&&s.trackingEpoch==r.trackingEpoch46&&
 s.generation==r.sourceGeneration&&s.settingsEpoch==r.inputSettingsEpoch46&&
 s.experimentEpoch==r.inputExperimentEpoch65&&s.press;}
}

// Only the most recent same-thread bind call can be pending. The caller keeps
// native rate0 while pending and retries via the SAME claimed Lua operation.
static bool DoorCurvePending66() noexcept {return door_curve_detail66::pendingResult;}

static bool DoorCurveBind66(const door4::Request& request,const char* actualClip,float actualTime,
 kcd2vr::door_path66::Curve& out,kcd2vr::door_path66::Pose& modelWorld) noexcept {
 namespace dc=door_curve_detail66;namespace d=kcd2vr::door_path66;
 dc::pendingResult=false;
 const auto source=out.source;out={};modelWorld={};char name[32]{};bool left=false;
 const auto refusedH4=[&](const char* why,int line){namespace h4=diagnostics::interaction_cycle_h4;const auto now=std::uint64_t(GetTickCount64());if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){auto e=h4::doorEvent(request,h4::Stage::DoorProgress,now,why,actualTime,false);e.key=source.clip;e.detail=line;h4::copy(e.label,sizeof(e.label),"DoorCurveBind66 EXISTING_RETURN");h4::observe(e,now);}return false;};
 if(!dc::requestSource(request,source)||!std::isfinite(actualTime)||actualTime<0||actualTime>1||
    !dc::alias(actualClip,name,left)||!door4::finite(request.surface66))return refusedH4("RESIDENT_CURVE_REQUEST_ALIAS_TIME_OR_SURFACE_INVALID",__LINE__);
 dc::Rig rig{};d::Pose frame{};
 if(!dc::rig(request,left,rig)||!dc::world(rig,frame))return refusedH4("RESIDENT_CURVE_EXACT_SIX_JOINT_RIG_OR_WORLD_REFUSED",__LINE__);
 auto& cache=dc::cache;
 if(cache.serial==request.serial){
  if(!dc::sameRig(cache.rig,rig)||std::memcmp(cache.name,name,sizeof(name))||
     !dc::liveClip(rig,cache.clip.id))return refusedH4("RESIDENT_CURVE_CACHED_RIG_ALIAS_OR_LIVE_CLIP_CHANGED",__LINE__);
  // Regrip source adoption belongs to root's exact same-door lifecycle. Curve
  // geometry/anchor remain immutable; a held original world point is not relearnt.
  if(cache.ready){out=cache.curve;out.source=source;out.source.clip=cache.clip.alias;out.source.topology=rig.topology;
   out.source.curve=cache.curve.source.curve;modelWorld=frame;return d::valid(out);}
 }
 const auto now=std::uint64_t(GetTickCount64());
 if(cache.serial!=request.serial){cache={};cache.serial=request.serial;cache.firstTick=now;cache.rig=rig;std::memcpy(cache.name,name,sizeof(name));}
 else {
  if(!cache.pending||cache.attempts>=8||now<cache.firstTick||now-cache.firstTick>250){cache.pending=false;return refusedH4("RESIDENT_CURVE_RETRY_LIMIT_8_OR_250MS_EXPIRED",__LINE__);}
  if(now<cache.lastAttempt||now-cache.lastAttempt<16){dc::pendingResult=true;return refusedH4("RESIDENT_CURVE_PENDING_MINIMUM_16MS_RETRY",__LINE__);}
 }
 ++cache.attempts;cache.lastAttempt=now;cache.pending=false;
 animation_filter::CurveReferenceInfo clip{};
 if(!animation_filter::ResolveCurveReference(rig.anim,name,clip)||!clip.alias||
    !dc::liveClip(rig,clip.id)||(cache.attempts>1&&(clip.id!=cache.clip.id||clip.alias!=cache.clip.alias)))return refusedH4("RESIDENT_CURVE_REFERENCE_ALIAS_RESIDENCY_OR_CLIP_REPLACED",__LINE__);
 cache.clip=clip;
 QuatT28 restRoot{},restPivot{};d::Pose a{},b{};
 if(!ReadCompanion(rig.owner,0,restRoot)||!ReadCompanion(rig.owner,1,restPivot)||
    !dc::convert(restRoot,a)||!dc::convert(restPivot,b))return refusedH4("RESIDENT_CURVE_REST_ROOT_OR_PIVOT_READ_INVALID",__LINE__);
 cache.constantP=d::rotate(d::conjugate(a.q),d::sub(b.p,a.p));
 // Current native helper is a validation read, never the animation trajectory.
 ph::Pose helper{};d::Pose sampled{};
 if(!PhysHelper34(rig.entity,left?"door_left_pivot":"door_right_pivot",helper,true)||
    !dc::pivot(rig,cache.clip.id,actualTime,cache.constantP,sampled))return refusedH4("RESIDENT_CURVE_PIVOT_HELPER_OR_ACTUAL_PHASE_SAMPLE_REFUSED",__LINE__);
 const d::Pose live{{helper.p.x,helper.p.y,helper.p.z},{helper.q.x,helper.q.y,helper.q.z,helper.q.w}};
 const auto error=d::sub(live.p,sampled.p);const double cosine=std::abs(live.q.x*sampled.q.x+live.q.y*sampled.q.y+
 live.q.z*sampled.q.z+live.q.w*sampled.q.w);
 if(!d::valid(live))return refusedH4("RESIDENT_CURVE_NATIVE_PIVOT_POSE_NONFINITE",__LINE__);
 if(d::dot(error,error)>.01*.01||cosine<std::cos(.02*.5)){
  // Native clock can lead the just-evaluated pose by one frame. Only this
  // comparison is retryable; missing residency, profile/rig/source replacement,
  // invalid world or foreign animation stays refused. Sampling never advances it.
  cache.pending=cache.attempts<8&&now>=cache.firstTick&&now-cache.firstTick<=250;
  dc::pendingResult=cache.pending;return refusedH4("RESIDENT_CURVE_NATIVE_PIVOT_VS_SAMPLED_PHASE_MISMATCH",__LINE__);
 }
 const d::Point point{request.surface66.x,request.surface66.y,request.surface66.z};
 cache.anchor=d::rotate(d::conjugate(live.q),d::sub(d::local(frame,point),live.p));
 cache.curve.source=source;cache.curve.source.clip=cache.clip.alias;cache.curve.source.topology=rig.topology;
 auto fingerprint=dc::hash(dc::hash(rig.topology,cache.clip.alias),request.serial);
 for(unsigned i=0;i<33;++i){d::Pose pivot{};const float phase=float(i)/32;
  if(!dc::pivot(rig,cache.clip.id,phase,cache.constantP,pivot))return refusedH4("RESIDENT_CURVE_RESIDENT_33_PHASE_SAMPLE_REFUSED",__LINE__);
  const auto handle=dc::compose(pivot,{cache.anchor,{}});cache.curve.samples[i]={phase,handle};
  for(double v:{handle.p.x,handle.p.y,handle.p.z,handle.q.x,handle.q.y,handle.q.z,handle.q.w}){
   std::uint64_t bits=0;std::memcpy(&bits,&v,sizeof(bits));fingerprint=dc::hash(fingerprint,bits);}
 }
 cache.curve.source.curve=fingerprint?fingerprint:1;
 dc::Rig final{};d::Pose finalWorld{};
 if(!d::valid(cache.curve)||!dc::rig(request,left,final)||!dc::sameRig(rig,final)||
    !dc::world(final,finalWorld)||!dc::liveClip(final,cache.clip.id))return refusedH4("RESIDENT_CURVE_FINAL_TOPOLOGY_OR_CLIP_REVALIDATION_REFUSED",__LINE__);
 const auto frameError=d::sub(finalWorld.p,frame.p);
 const double frameQ=std::abs(finalWorld.q.x*frame.q.x+finalWorld.q.y*frame.q.y+finalWorld.q.z*frame.q.z+finalWorld.q.w*frame.q.w);
 if(d::dot(frameError,frameError)>1e-10||frameQ<1-1e-8)return refusedH4("RESIDENT_CURVE_WORLD_FRAME_CHANGED_DURING_SAMPLING",__LINE__);
 cache.ready=true;out=cache.curve;modelWorld=frame;return true;
}
