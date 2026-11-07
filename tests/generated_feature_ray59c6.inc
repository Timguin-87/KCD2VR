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
