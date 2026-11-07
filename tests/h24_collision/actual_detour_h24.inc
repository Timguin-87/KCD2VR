static int __fastcall PhysCollisionProduceDetourH24(void* world,unsigned flags,const void* event,void* queuedOut) noexcept {
    return kcd2vr::native_weapon_contact_h24::observeProducer(world,flags,event,queuedOut,
        g_collision_contract_h24.load(std::memory_order_acquire),
        [](const void* p,void* out,std::size_t size){return SehCopy146(p,out,size);},
        [](){return std::uint64_t(GetTickCount64());},[](){std::uintptr_t current=0;
            (void)SehCopy146(reinterpret_cast<const void*>(g_base+0x492D830),&current,sizeof(current));return current;},
        [](void* w,unsigned f,const void* e,void* out){return g_phys_produce_original_h24?g_phys_produce_original_h24(w,f,e,out):0;});
}
int PhysCollisionDetour(const void* ev) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::PhysCollision};
    // Logged delivery remains diagnostic-only; the real producer stamped the
    // contact source/time at occurrence, before this event could be delayed.
    int r=1;{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::PhysCollision};r=g_phys_original?g_phys_original(ev):1;}
    if(!Active()||!ev||Rd<std::int32_t>(ev,0x8,-1)!=2)return r;
    g_counts[3].fetch_add(1,std::memory_order_relaxed);
    const void* fd[2]{Rd<const void*>(ev,0x20,nullptr),Rd<const void*>(ev,0x28,nullptr)};
    const int fi[2]{Rd<std::int32_t>(ev,0x30,-1),Rd<std::int32_t>(ev,0x34,-1)};
    const bool p0=fi[0]==2&&IsPlayer(fd[0]),p1=fi[1]==2&&IsPlayer(fd[1]);
    if(!p0&&!p1)return r;
    g_counts[4].fetch_add(1,std::memory_order_relaxed);
    if(!Allow(3,20))return r;
    const int self=p0?0:1,other=1-self;
    auto v=[&](std::uintptr_t o){return std::array<float,3>{Rd<float>(ev,o,0),Rd<float>(ev,o+4,0),Rd<float>(ev,o+8,0)};};
    const auto pt=v(0x3C),nrm=v(0x48),v0=v(0x54),v1=v(0x60);
    const float rel=std::sqrt((v0[0]-v1[0])*(v0[0]-v1[0])+(v0[1]-v1[1])*(v0[1]-v1[1])+(v0[2]-v1[2])*(v0[2]-v1[2]));
    const char* kind=fi[other]==0?"TERRAIN":fi[other]==1?"STATIC(wall/building)":fi[other]==2?"ENTITY":"OTHER";
    char m[700]{};std::snprintf(m,sizeof(m),"V66.134R1-F3R1 PHYS_COLLISION player=side%d other=%s otherEntity=%u otherForeign=%d pt=(%.2f,%.2f,%.2f) n=(%.2f,%.2f,%.2f) relSpeed=%.2f "
        "mass=%.1f/%.1f part=%d/%d mat=%d/%d penetration=%.3f impulse=%.2f radius=%.3f",
        self,kind,fi[other]==2?EntityId(fd[other]):0u,fi[other],pt[0],pt[1],pt[2],nrm[0],nrm[1],nrm[2],rel,
        Rd<float>(ev,0x6C+4*self,0),Rd<float>(ev,0x6C+4*other,0),Rd<std::int32_t>(ev,0x74+4*self,-1),Rd<std::int32_t>(ev,0x74+4*other,-1),
        int(Rd<std::int16_t>(ev,0x7C+2*self,-1)),int(Rd<std::int16_t>(ev,0x7C+2*other,-1)),
        Rd<float>(ev,0x84,0),Rd<float>(ev,0x88,0),Rd<float>(ev,0x8C,0));
    diagnostic_trace::Write(m);return r;
}
