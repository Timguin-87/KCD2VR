// Included after physical/equipment/query definitions. Game-thread diagnostic only.
// Uses the already-qualified ray leaf and a PRIVATE fault/budget policy. It never
// trips the gameplay collision circuit, changes a pose, or submits a hit/effect.
namespace featureProbe59c6 {
namespace fp=kcd2vr::feature59c6;
struct Result {const char* status="UNAVAILABLE";npc172::Witness witness{};V3 point{},normal{};float distance=0;};
static bool g_fault=false;
static Result Ray(V3 from,V3 delta,const npc172::Exclusions& skip)noexcept{
 Result out;const float length=Len(delta);const auto base=WhBase();
 if(g_fault||g_collision_binding6692.load(std::memory_order_acquire)!=1||!base||!std::isfinite(length)||length<.002f||length>.601f)return out;
 void* world=nullptr;std::uintptr_t vf=0,method=0;
 if(!Read(reinterpret_cast<void*>(base+wc::kWorldSlotRva),world)||!world||!Read(world,vf)||vf!=base+wc::kWorldVtableRva||
    !Read(reinterpret_cast<void*>(vf+wc::kQueryVtableOffset),method)||method!=base+wc::kQueryMethodRva){out.status="WORLD_BINDING";return out;}
 alignas(16) std::array<std::byte,0x50> hit{};
 auto excluded=skip; // native signature is non-const; keep caller's exclusions immutable
 const int count=RayWorldGuarded6692(reinterpret_cast<RayWorldIntersectionFn6692>(base+wc::kHelperRva),world,&from,&delta,hit.data(),true,excluded.bodies.data(),int(excluded.bodyCount));
 if(count<0){g_fault=true;out.status="NATIVE_FAULT";diagnostic_trace::Write("R59.16C6 FEATURE_PROBE_FAULT native ray failed; probe latched off; gameplay collision state unchanged");return out;}
 if(count==0){out.status="CLEAR";return out;}
 out.status="INVALID_RAY";if(count!=1)return out;
 std::memcpy(&out.distance,hit.data(),4);std::memcpy(&out.point,hit.data()+0x24,sizeof(V3));std::memcpy(&out.normal,hit.data()+0x30,sizeof(V3));
 if(!std::isfinite(out.distance)||out.distance<0||out.distance>length+.05f)return out;
 const float n=Len(out.normal),e=Len(Sub(out.point,Add(from,Mul(delta,out.distance/length))));
 if(!std::isfinite(n)||n<.8f||n>1.2f||!std::isfinite(e)||e>.025f)return out;
 out.normal=Mul(out.normal,1.f/n);void* body=nullptr;void* owner=nullptr;std::uintptr_t bv=0,foreign=0,ev=0;
 std::memcpy(&body,hit.data()+8,sizeof(body));std::memcpy(&out.witness.part,hit.data()+0x14,4);
 std::int16_t surface=-1;std::memcpy(&surface,hit.data()+0x18,2);out.witness.surfaceIdx=surface>=0?int(surface):-1;
 out.witness.collider=reinterpret_cast<std::uintptr_t>(body);out.witness.mask=npc172::BodiesAndWorld;
 if(body&&Read(body,bv)&&bv>=base&&bv-base<0x6000000){out.witness.vft=bv-base;out.witness.body=npc172::classify(bv-base);
  if(Read(reinterpret_cast<void*>(bv+0x50),foreign)&&foreign==base+0x5D6F44&&Read(static_cast<unsigned char*>(body)+0x20,owner)&&owner&&
     Read(owner,ev)&&ev==base+0x4072C78&&Read(static_cast<unsigned char*>(owner)+0xC,out.witness.entityId)){
    out.witness.owner=reinterpret_cast<std::uintptr_t>(owner);out.witness.ownerRead=true;}else owner=nullptr;}
 if(skip.excludes(owner,body)){out.status="SELF_EXCLUDED";return out;}
 out.status="HIT";return out;
}
static bool Exclusions(npc172::Exclusions& out)noexcept{
 const auto previous=g_npc_query172;g_npc_query172=NpcQuery172{};g_npc_query172.requested=true;
 const bool ok=PrepareNpcQuery172();if(ok)out=g_npc_query172.skip;g_npc_query172=previous;return ok;
}
static void Tick()noexcept{
 namespace cap=kcd2vr::diagnostics::capture168;
 static fp::Tracker trackers[2];static std::uint64_t session=0,lastNs=0,lastReport=0;static unsigned turn=0;
 const auto now=std::uint64_t(GetTickCount64());
 if(!cap::active(cap::Domain::Combat,now)){trackers[0].clear();trackers[1].clear();session=lastNs=lastReport=0;return;}
 const auto activeSession=cap::gate.session[unsigned(cap::Domain::Combat)].load();
 if(session!=activeSession){trackers[0].clear();trackers[1].clear();session=activeSession;lastNs=lastReport=0;}
 const auto ns=kcd2vr::tracking59c6::NowNs();if(lastNs&&ns>=lastNs&&ns-lastNs<50000000)return;lastNs=ns; // <=20 Hz
 PhysFrame6698 frame{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
 const bool valid=PhysicalCombat177Enabled()&&g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
  !animation_filter::nativeBodyOwned112.load()&&g_phys_frame6698.read(frame)&&ready&&frame.input.sample==ready&&
  frame.request==g_weapon_request_epoch6694.load()&&frame.input.bodyEpoch==g_pose_epoch6689.load()&&frame.input.allowed;
 fp::Batch batches[2]{};
 for(int side=0;side<2;++side){fp::Sample v{};
  if(valid){const auto& p=frame.profiles[side];v.local=fp::shape(p);
   v.owner={reinterpret_cast<std::uintptr_t>(frame.player),p.key,p.generation,frame.input.bodyEpoch,frame.request,kcd2vr::contact176::recoveryEpoch55c1.load(),p.entity,side};
   v.weapon=frame.output.weapons[side];v.world=frame.input.world;v.scale=frame.input.worldScale;
   v.sample=frame.input.sample;v.tick=frame.input.tick;v.ns=frame.input.timeNs;v.guard=frame.output.guardBySide30c1[side];
   v.eligible=frame.player&&p.side==side&&p.primary&&frame.output.owned[side]&&frame.output.active[side]&&frame.input.tracked[side]&&
    !frame.output.handlingOnly55c1[side]&&!frame.output.displayLatched[side];}
  batches[side]=trackers[side].step(v,now,ns);
 }
 npc172::Exclusions excluded{};bool need=false;for(const auto& b:batches)need|=b.stage==fp::Stage::Query;
 const auto begin=ContactMicros6695();
 const bool can=need&&!g_fault&&g_collision_binding6692.load(std::memory_order_acquire)==1&&Exclusions(excluded);
 unsigned rays=0;const bool statusDue=!lastReport||now<lastReport||now-lastReport>=1000;
 for(int k=0;k<2;++k){const int side=(k+turn)%2;const auto& b=batches[side];
  if(statusDue){char text[384]{};std::snprintf(text,sizeof(text),"R59.16C6 FEATURE_PROBE stage=%s feature=%s side=%c key=%016llX sample=%llu guard=%d queryReady=%d raysMax=6 geometry=BOUNDS_PROXY offensive=0 nativeAttack=NONE",
   fp::name(b.stage),fp::name(b.kind),side?'L':'R',(unsigned long long)b.owner.key,(unsigned long long)b.sample,b.guardNow,can);diagnostic_trace::Write(text);}
  if(b.stage!=fp::Stage::Query)continue;
  for(unsigned i=0;i<b.count;++i){Result result{};const auto& seg=b.segments[i];
   if(!can)result.status=g_fault?"PROBE_LATCHED_OFF":"QUERY_UNAVAILABLE";
   else if(rays>=6||ContactMicros6695()-begin>=500)result.status="BUDGET_EXHAUSTED";
   else if(g_fault)result.status="PROBE_LATCHED_OFF";
   else{V3 start{float(seg.from.x),float(seg.from.y),float(seg.from.z)},delta{float(seg.to.x-seg.from.x),float(seg.to.y-seg.from.y),float(seg.to.z-seg.from.z)};const auto queryStart=ContactMicros6695();result=Ray(start,delta,excluded);++rays;
     const auto queryUs=ContactMicros6695()-queryStart;
     if(queryUs>2000&&!g_fault){g_fault=true;diagnostic_trace::Write("R59.16C6 FEATURE_PROBE_FAULT query exceeded 2000us; optional probe stopped; native gameplay query policy unchanged");}}
   if(!cap::active(cap::Domain::Combat,GetTickCount64())||session!=cap::gate.session[unsigned(cap::Domain::Combat)].load())return;
   char text[720]{};std::snprintf(text,sizeof(text),"R59.16C6 FEATURE_CONTACT feature=%s side=%c point=%u key=%016llX generation=%llu sample=%llu sourceTick=%llu ageMs=%llu guardFrom=%d guardNow=%d speed=%.3f query=%s target=%u body=%s part=%d surfaceRaw=%d collider=%llX p=(%.3f,%.3f,%.3f) geometry=SPARSE_BOUNDS_PROXY attack=NOT_IMPLEMENTED damage=NONE effects=NONE physicsWrites=NONE",
    fp::name(b.kind),side?'L':'R',seg.feature,(unsigned long long)b.owner.key,(unsigned long long)b.owner.generation,(unsigned long long)b.sample,(unsigned long long)b.tick,
    (unsigned long long)(now>=b.tick?now-b.tick:0),b.guardFrom,b.guardNow,seg.speed,result.status,result.witness.entityId,npc172::name(result.witness.body),result.witness.part,result.witness.surfaceIdx,(unsigned long long)result.witness.collider,double(result.point.x),double(result.point.y),double(result.point.z));diagnostic_trace::Write(text);
  }
 }
 turn=1-turn;if(statusDue)lastReport=now;
}
} // namespace
