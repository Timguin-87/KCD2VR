// D65 corrected: release->constructed native firing action->callback scope->Shoot.
// Exact retail evidence in validation/lane_b. Native speed/cost/payload stay native.
using BowShootFnD65C=void(__fastcall*)(void*,void*,unsigned,const float*,void*,float*,void*,void*,float,bool);
using BowCtorFnD65C=void*(__fastcall*)(void*,void*,void*,unsigned,unsigned,float,const float*);
using BowFireActionFnD65C=void(__fastcall*)(void*);
static BowShootFnD65C g_bow_shoot_original_d65c=nullptr;
static BowCtorFnD65C g_bow_ctor_original_d65c=nullptr;
static BowFireActionFnD65C g_bow_fire_original_d65c=nullptr;
static std::optional<DetourModKit::hook::Hook> g_bow_shoot_hook_d65c,g_bow_ctor_hook_d65c,g_bow_fire_hook_d65c;
static kcd2vr::owned_inline_d65::Route g_bow_shoot_route_d65c,g_bow_ctor_route_d65c,g_bow_fire_route_d65c;
static std::atomic<bool> g_bow_shoot_owned_d65c{false},g_bow_ctor_owned_d65c{false},g_bow_fire_owned_d65c{false};
static constexpr unsigned char kBowShootBytesD65C[]{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x57};
static constexpr unsigned char kBowCtorBytesD65C[]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x41,0x8b,0xd9,0x48,0x8b,0xf9};
static constexpr unsigned char kBowFireBytesD65C[]{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x57};
static bool BowReadD65C(std::uintptr_t p,void* out,std::size_t n) noexcept {return p&&SehCopy(reinterpret_cast<const void*>(p),out,n);}
static bool BowCaptureD65C() noexcept {
    using namespace diagnostics::capture168;
    return diagnostic_trace::Enabled()&&(active(Domain::Combat,GetTickCount64())||active(Domain::Reload,GetTickCount64()));
}
static void BowNoteD65C(const char* kind,const char* why) noexcept {
    if(!BowCaptureD65C())return;char b[240]{};std::snprintf(b,sizeof(b),"D65 BOW_SHOOT %s reason=%s",kind,why);diagnostic_trace::Write(b);
}
static bool ShootPrologueMatchesD65C(const unsigned char* bytes,std::size_t n) noexcept {
    const auto b=WhBase();if(!b||n>sizeof(kBowShootBytesD65C)||!retail_build112::Ready(b))return false;
    if(MatchBytes(b+0x289F7A0,bytes,n))return true;
    return g_bow_shoot_owned_d65c.load(std::memory_order_acquire)&&g_bow_shoot_original_d65c&&
        g_bow_shoot_hook_d65c&&g_bow_shoot_route_d65c.current(BowReadD65C)&&!std::memcmp(kBowShootBytesD65C,bytes,n);
}
static bool BowHooksCurrentD65C() noexcept {
    return g_bow_shoot_owned_d65c.load(std::memory_order_acquire)&&g_bow_ctor_owned_d65c.load(std::memory_order_acquire)&&
        g_bow_fire_owned_d65c.load(std::memory_order_acquire)&&retail_build112::Ready(WhBase())&&
        g_bow_shoot_route_d65c.current(BowReadD65C)&&g_bow_ctor_route_d65c.current(BowReadD65C)&&g_bow_fire_route_d65c.current(BowReadD65C);
}
struct BowShootUpD65C {float v[3];unsigned char valid;unsigned char pad[3];};
static void __fastcall BowShootDetourD65C(void* manager,void* actor,unsigned flags,const float* dir,void* up,float* pos,
                                         void* launcher,void* payload,float speed,bool flag) noexcept {
    namespace aim=kcd2vr::bow_physical_aim_d65c;
    const auto original=g_bow_shoot_original_d65c;if(!original)return;
    if(KCD2VR_GRASP_CALLER()==WhBase()+0x289A107&&dir&&up&&pos&&BowHooksCurrentD65C()){
        aim::Snapshot s{};const auto why=aim::take(actor,launcher,s);
        if(why==aim::Refusal::None){
            float localDir[3],localPos[3];BowShootUpD65C localUp{};
            if(SehCopy(up,&localUp,sizeof(localUp))){
                for(int i=0;i<3;++i){localDir[i]=float(s.dir[i]);localPos[i]=float(s.nock[i]);}
                double u[3]{s.up[0],s.up[1],s.up[2]};const double d=u[0]*s.dir[0]+u[1]*s.dir[1]+u[2]*s.dir[2];
                for(int i=0;i<3;++i)u[i]-=d*s.dir[i];
                const double n=std::sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);
                if(n>1e-3){for(int i=0;i<3;++i)localUp.v[i]=float(u[i]/n);localUp.valid=1;
                    original(manager,actor,flags,localDir,&localUp,localPos,launcher,payload,speed,flag);
                    if(BowCaptureD65C()){char row[384]{};std::snprintf(row,sizeof(row),
                        "D65 BOW_SHOOT aim=VR release=%llu action=%p key=%llX ageMs=%llu dir=(%.4f,%.4f,%.4f) nock=(%.3f,%.3f,%.3f) nativeSpeed=%.3f",
                        (unsigned long long)s.serial,s.action,(unsigned long long)s.key,(unsigned long long)(GetTickCount64()-s.tick),
                        s.dir[0],s.dir[1],s.dir[2],s.nock[0],s.nock[1],s.nock[2],double(speed));diagnostic_trace::Write(row);}
                    return;
                }
            }
            BowNoteD65C("aim=NATIVE","UP_COPY_OR_BASIS_INVALID");
        } else if(aim::shotScope)BowNoteD65C("aim=NATIVE",aim::name(why));
    }
    original(manager,actor,flags,dir,up,pos,launcher,payload,speed,flag);
}
static void* __fastcall BowFiringCtorD65C(void* self,void* owner,void* context,unsigned a,unsigned b,float charge,const float* offset) noexcept {
    const auto original=g_bow_ctor_original_d65c;if(!original)return nullptr;
    namespace aim=kcd2vr::bow_physical_aim_d65c;
    // Allocation reuse revokes the old action BEFORE native construction, even
    // if its original constructor faults. CAS retirement cannot revoke a newer draft.
    aim::retireReusedAction(self);
    void* result=original(self,owner,context,a,b,charge,offset);
    // Every constructor visit observes address reuse; a release binds only the
    // exact factory caller, verified completed type and identical owner interface.
    if(KCD2VR_GRASP_CALLER()==WhBase()+0x2886A18&&result==self&&Rd<std::uintptr_t>(result,0,0)==WhBase()+0x470A4B8&&BowHooksCurrentD65C())
        aim::bindConstructed(result,owner);
    else {auto* saved=aim::releaseDraft;aim::releaseDraft=nullptr;aim::bindConstructed(result,owner);aim::releaseDraft=saved;}
    return result;
}
// Keep compare-exchange's mutable expected value in its own normally-returning
// POD frame. Inlining that scope inside a native SEH finally funclet leaves stale
// stack-lifetime poison with optimized MSVC ASAN after exceptional unwind.
__declspec(noinline) static void BowFiringScopeFinishD65C(
        const kcd2vr::bow_physical_aim_d65c::Snapshot* previous,std::uint64_t serial) noexcept {
    namespace aim=kcd2vr::bow_physical_aim_d65c;
    aim::shotScope=previous;aim::retire(serial);
}
static void __fastcall BowFiringActionD65C(void* action) noexcept {
    const auto original=g_bow_fire_original_d65c;if(!original)return;
    namespace aim=kcd2vr::bow_physical_aim_d65c;aim::Snapshot s{};
    const auto why=BowHooksCurrentD65C()&&Rd<std::uintptr_t>(action,0,0)==WhBase()+0x470A4B8?
        aim::enter(action,GetTickCount64(),s):aim::Refusal::NoRelease;
    // A nested unrelated action must never borrow the outer action's scope.
    // /EHsc does not unwind C++ RAII on a native SEH handled above this hook.
    // POD-only finally restores TLS and retires this release on EVERY exit;
    // preserve propagation to the game's existing outer handler.
    const auto* previous=aim::shotScope;
    aim::shotScope=why==aim::Refusal::None?&s:nullptr;
    __try {original(action);}
    __finally {BowFiringScopeFinishD65C(previous,why==aim::Refusal::None?s.serial:0);}
    if(why!=aim::Refusal::None&&why!=aim::Refusal::WrongAction&&why!=aim::Refusal::NoRelease&&why!=aim::Refusal::Disabled)BowNoteD65C("action=NATIVE",aim::name(why));
}
template<class Fn,std::size_t N>
static bool InstallBowPartD65C(std::optional<DetourModKit::hook::Hook>& holder,Fn& original,
        kcd2vr::owned_inline_d65::Route& route,std::atomic<bool>& owned,const char* name,std::uintptr_t rva,
        const unsigned char (&pin)[N],Fn detour) noexcept {
    const auto b=WhBase();owned.store(false,std::memory_order_release);
    if(holder){
        // Re-enable only an unchanged original or our exact attributed route.
        if(!MatchBytes(b+rva,pin,N)&&!route.current(BowReadD65C))return false;
        if(!holder->enable())return false;
        const bool ok=original&&route.current(BowReadD65C);owned.store(ok,std::memory_order_release);return ok;
    }
    if(!MatchBytes(b+rva,pin,N))return false;
    DetourModKit::hook::InlineRequest req{name,DetourModKit::hook::Target{DetourModKit::Address{b+rva}},DetourModKit::hook::Options{}};
    auto hook=DetourModKit::hook::inline_at(std::move(req),detour);if(!hook)return false;
    holder=std::move(*hook);original=holder->template original<Fn>();
    if(!original||!PinOwnCallbackT2(reinterpret_cast<const void*>(detour))||!holder->enable())return false;
    const bool ok=route.capture(b+rva,reinterpret_cast<std::uintptr_t>(detour),BowReadD65C);owned.store(ok,std::memory_order_release);return ok;
}
static bool DisarmBowShootD65C() noexcept {
    namespace aim=kcd2vr::bow_physical_aim_d65c;aim::hooksReady.store(false,std::memory_order_release);aim::retire();
    bool ok=true;
    const auto disarm=[&](auto& h,auto& owned){owned.store(false,std::memory_order_release);if(h&&h->is_enabled())ok=bool(h->disable())&&!h->is_enabled()&&ok;};
    disarm(g_bow_shoot_hook_d65c,g_bow_shoot_owned_d65c);disarm(g_bow_fire_hook_d65c,g_bow_fire_owned_d65c);disarm(g_bow_ctor_hook_d65c,g_bow_ctor_owned_d65c);
    return ok;
}
static bool InstallBowShootD65C() noexcept {
    namespace aim=kcd2vr::bow_physical_aim_d65c;aim::hooksReady.store(false,std::memory_order_release);aim::retire();
    const auto b=WhBase();
    constexpr unsigned char fireCall[]{0xe8,0x99,0x56,0,0,0x49,0x8b,0x0e,0x48,0x8b,1,0xff,0x90,0xc8,1,0,0};
    constexpr unsigned char ctorCall[]{0xe8,0x64,0xd2,0,0};
    constexpr unsigned char actionCall[]{0xe8,0x92,0x31,0,0};
    constexpr unsigned char velocity[]{0xf3,0x0f,0x10,0x84,0x24,0x10,1,0,0,0xf3,0x0f,0x59,0x44,0x85,0};
    constexpr unsigned char upValid[]{0x41,0x80,0x7f,0x0c,0};
    if(!retail_build112::Ready(b)||!MatchBytes(b+0x289A102,fireCall,sizeof(fireCall))||
       !MatchBytes(b+0x2886A13,ctorCall,sizeof(ctorCall))||!MatchBytes(b+0x2896831,actionCall,sizeof(actionCall))||
       !MatchBytes(b+0x289F832,velocity,sizeof(velocity))||!MatchBytes(b+0x289F8A3,upValid,sizeof(upValid))){BowNoteD65C("install=REFUSED","RETAIL_PINS");return false;}
    const bool ok=InstallBowPartD65C(g_bow_ctor_hook_d65c,g_bow_ctor_original_d65c,g_bow_ctor_route_d65c,g_bow_ctor_owned_d65c,
            "kcd2vr_d65_firing_ctor",0x2893C7C,kBowCtorBytesD65C,static_cast<BowCtorFnD65C>(&BowFiringCtorD65C))&&
        InstallBowPartD65C(g_bow_fire_hook_d65c,g_bow_fire_original_d65c,g_bow_fire_route_d65c,g_bow_fire_owned_d65c,
            "kcd2vr_d65_firing_action",0x2896788,kBowFireBytesD65C,static_cast<BowFireActionFnD65C>(&BowFiringActionD65C))&&
        InstallBowPartD65C(g_bow_shoot_hook_d65c,g_bow_shoot_original_d65c,g_bow_shoot_route_d65c,g_bow_shoot_owned_d65c,
            "kcd2vr_d65_bow_shoot",0x289F7A0,kBowShootBytesD65C,static_cast<BowShootFnD65C>(&BowShootDetourD65C));
    if(!ok){(void)DisarmBowShootD65C();BowNoteD65C("install=REFUSED","OWNED_HOOK_ROUTE");return false;}
    aim::hooksReady.store(true,std::memory_order_release);BowNoteD65C("install=READY","EXACT_FIRING_ACTION");return true;
}
