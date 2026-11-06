// C40 IMPALE BY ARMOUR: native armour read (combat TU, geom27). Included after decap59c7e::Evaluate (needs
// ActorForEntity). Native game thread only, ONCE per candidate thrust (a completed, undefended, body-contact thrust
// receipt while VrImpaleByArmour is on), never per frame. Read-only: the very aggregate the game's own flowgraph
// armour node computes (Process 2AAEAAC -> 2AAEBB1). No setter, no state write, no hook.
// Retail chain (WHGame 6A350E20), static evidence:
//  combat actor +1F8 = C_Human (+278 links back); C_Human +668 = soul (vt 3F44F28).
//  soul vt+1E0 = 1A947E0 (lea rax,[rcx+198]) -> equipment holder (vt 3A33798); holder vt+8 = 1A77FB0 (mov rax,[rcx+120])
//   -> equipment; worn layers = vector [+40,+48) of item pointers (what 1808F7D8C returns to the flow node).
//  2A9D09C(equipment, ctx{&table 5324AE0, item-stat service [548E338] (vt 3AE4410), float* stab[4], slash[4], smash[4]})
//   builds a std::function (impl vt 471CFB0, call 2AB06AC) and visits every worn layer; 2AB06AC adds the service's
//   vt+128 = 686758 (record +F0/+F4/+F8 x condition factor 687A7C) into [zone] when the layer zone (+24) is 0..3.
namespace impale40n {
namespace i40=kcd2vr::impale40;
static int g_pins40=-1;
static bool Pins40() noexcept {
    if(g_pins40>=0)return g_pins40==1;
    if(!g_base||!retail_build112::Ready(g_base))return false; // not cached: the module may not be ready yet
    struct P{std::uintptr_t rva;unsigned char b[16];};
    static constexpr P pins[]{
        {0x2A9D09C,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57}},
        {0x2AB06AC,{0x48,0x89,0x5c,0x24,0x18,0x57,0x48,0x83,0xec,0x40,0x48,0x8b,0x05,0x03,0xe9,0xe4}},
        {0x686758,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57}},
        {0x1A947E0,{0x48,0x8d,0x81,0x98,0x01,0x00,0x00,0xc3,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc}},
        {0x1A77FB0,{0x48,0x8b,0x81,0x20,0x01,0x00,0x00,0xc3,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc}}};
    int ok=1;
    for(const auto& p:pins){unsigned char a[16]{};if(!SehCopy146(reinterpret_cast<const void*>(g_base+p.rva),a,16)||std::memcmp(a,p.b,16)){ok=0;break;}}
    if(ok&&(Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+0x3F44F28),0x1E0,0)!=g_base+0x1A947E0||
            Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+0x3A33798),0x8,0)!=g_base+0x1A77FB0||
            Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+0x3AE4410),0x128,0)!=g_base+0x686758||
            Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+0x471CFC0),0,0)!=g_base+0x2AB06AC))ok=0;
    g_pins40=ok;
    diagnostic_trace::Write(ok?"C40 IMPALE_ARMOUR pins OK (flow armour aggregate 2A9D09C, layer lambda 2AB06AC, defence 686758)":"C40 IMPALE_ARMOUR pin MISMATCH: armour unknown (medium) for every impale");
    return ok==1;
}
struct Ctx40 {const void* table;const void* service;float* stab;float* slash;float* smash;};
static_assert(sizeof(Ctx40)==0x28);
static bool AggregateGuarded40(const void* equipment,Ctx40& c) noexcept {
    using Fn=void(__fastcall*)(const void*,Ctx40*);
    __try{reinterpret_cast<Fn>(g_base+0x2A9D09C)(equipment,&c);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static i40::ArmourStatus ReadArmour40(std::uint32_t target,std::uint64_t now,i40::Verdict& v) noexcept {
    using S=i40::ArmourStatus;static bool fault=false;
    if(fault)return S::Fault;
    if(!Pins40())return S::PinMismatch;
    const void* actor=ActorForEntity(target,now);if(!actor)return S::NoActor;
    const void* human=Rd<const void*>(actor,0x1F8,nullptr);
    if(!human||Rd<const void*>(human,0x278,nullptr)!=actor)return S::NoActor;
    const void* soul=Rd<const void*>(human,0x668,nullptr);
    if(!soul||Rd<std::uintptr_t>(soul,0,0)!=g_base+0x3F44F28)return S::NoSoul;
    const auto* holder=static_cast<const unsigned char*>(soul)+0x198;
    if(Rd<std::uintptr_t>(holder,0,0)!=g_base+0x3A33798)return S::NoEquipment;
    const void* equipment=Rd<const void*>(holder,0x120,nullptr);if(!equipment)return S::NoEquipment;
    const auto begin=Rd<std::uintptr_t>(equipment,0x40,0),end=Rd<std::uintptr_t>(equipment,0x48,0);
    if(end<begin||(end-begin)%8||(end-begin)/8>96)return S::Invalid;
    v.items=unsigned((end-begin)/8);
    const void* service=Rd<const void*>(reinterpret_cast<const void*>(g_base+0x548E338),0,nullptr);
    if(!service||Rd<std::uintptr_t>(service,0,0)!=g_base+0x3AE4410)return S::NoEquipment; // never trigger its lazy construction
    float stab[4]{},slash[4]{},smash[4]{};
    Ctx40 c{reinterpret_cast<const void*>(g_base+0x5324AE0),service,stab,slash,smash};
    if(!AggregateGuarded40(equipment,c)){fault=true;diagnostic_trace::Write("C40 IMPALE_ARMOUR FAULT (aggregate exception): armour unknown for this session");return S::Fault;}
    for(int i=0;i<4;++i){if(!std::isfinite(stab[i])||!std::isfinite(slash[i])||!std::isfinite(smash[i])||stab[i]<0||stab[i]>10000)return S::Invalid;
        v.stab[i]=stab[i];v.slash[i]=slash[i];v.smash[i]=smash[i];}
    return S::Read;
}
inline void Evaluate(const Pending& p,const kcd2vr::contact58c1::Receipt& r,std::uint64_t now) noexcept {
    if(!i40::enabled()||!i40::wantsArmour(r))return;
    i40::Verdict v{};v.serial=r.serial;v.tick=r.tick?r.tick:now;v.target=r.target;v.identity=r.targetIdentity;
    v.energy=std::isfinite(p.powerRatio59c8)?p.powerRatio59c8:p.energy;v.damage=r.damage;v.death=r.death;v.kind=r.kind;v.material=r.bodyMaterial;
    v.armour=ReadArmour40(r.target,now,v);
    (void)i40::verdict.publish(v);
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char line[420]{};std::snprintf(line,sizeof(line),
        "C40 IMPALE_ARMOUR serial=%llu target=%u status=%s layers=%u stab{head=%.1f arms=%.1f body=%.1f legs=%.1f} slash{body=%.1f} smash{body=%.1f} material=%d energy=%.2f dmg=%.2f death=%d source=NATIVE_FLOW_ARMOUR_AGGREGATE writes=NONE",
        (unsigned long long)r.serial,r.target,i40::name(v.armour),v.items,double(v.stab[0]),double(v.stab[1]),double(v.stab[2]),double(v.stab[3]),
        double(v.slash[2]),double(v.smash[2]),r.bodyMaterial,double(v.energy),double(r.damage),r.death);diagnostic_trace::Write(line);}
}
} // namespace impale40n
