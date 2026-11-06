// Exact-retail observation at the common return of WHGame+687BBC.
// Rbx is the caller's completed query; +8/+10 were filled by +687E24.
// No ABI calls, argument/result edits or native eligibility overrides.
static std::optional<DetourModKit::hook::Hook> g_close_query164;
static void CloseQuery164(DetourModKit::hook::MidContext& ctx) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::CloseQuery};
    using namespace DetourModKit::hook;
    const auto now=std::uint64_t(GetTickCount64());
    const auto intent168=diagnostics::intent168::current;
    const bool reference=diagnostics::lab::Get(diagnostics::lab::Key::F3)==1||CaptureOnly168r1(); // read-only query coverage, not native-pose mode
    if((!reference&&!intent168.serial)||!cp122::active(now)||!retail_build112::Ready(nhp134::g_base)||
       !diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    const auto* query=reinterpret_cast<const unsigned char*>(gpr(ctx,Gpr::Rbx));
    const void* actor=nhp134::Rd<const void*>(query,8,nullptr);
    const void* owner=reinterpret_cast<const void*>(gpr(ctx,Gpr::Rdi));
    if(!query||nhp134::Rd<const void*>(owner,8,nullptr)!=actor||
       (!reference&&reinterpret_cast<std::uintptr_t>(actor)!=intent168.actor))return;
    const void *entity=nullptr,*state=nullptr;
    if(!nhp134::CombatActor(actor,entity,state)||!nhp134::IsPlayer(entity))return;
    static std::atomic<std::uint64_t> due{0};auto next=due.load();
    if(!intent168.serial&&(now<next||!due.compare_exchange_strong(next,now+25)))return;
    const void* opponent=nhp134::Rd<const void*>(query,0x10,nullptr);
    const void *oe=nullptr,*os=nullptr;
    if(opponent)(void)nhp134::CombatActor(opponent,oe,os);
    cp122::Event e{};e.kind=cp122::Kind::CloseQuery;e.tick=now;e.sequence=diagnostics::trace164::serial();
    e.actor=reinterpret_cast<std::uintptr_t>(actor);e.aliasHash=reinterpret_cast<std::uintptr_t>(query);
    e.key[0]=nhp134::EntityId(entity);e.key[1]=nhp134::EntityId(oe);
    e.id=nhp134::Rd<int>(query,0x4C,-1);e.mode=nhp134::Rd<int>(query,0x3C,-1);
    e.layer=nhp134::Rd<int>(query,0x9C,-1);e.flags=opponent?1u:0u;
    e.trackingFlags=nhp134::Rd<unsigned char>(query,0x94,0xFF);
    e.parent164[0]=nhp134::Rd<unsigned>(query,0x98,0);
    if(reinterpret_cast<std::uintptr_t>(actor)==intent168.actor){e.parent164[1]=intent168.serial;e.parent164[2]=unsigned(intent168.source);}
    // Raw completed criteria preserve currently unlabelled fields. They are
    // not accepted-action or successful-kick flags.
    if(!nhp134::SehCopy146(query,e.contact114,sizeof(e.contact114)))return;
    cp122::push(e);
}
static bool InstallCloseQuery164(std::uintptr_t b) noexcept {
    if(g_close_query164)return true;
    if(!retail_build112::Ready(b))return false;
    constexpr unsigned char expected[]{0x48,0x8B,0x4C,0x24,0x28,0x48,0x33,0xCC};
    unsigned char bytes[sizeof(expected)]{};
    if(!ReadCopy6689(reinterpret_cast<void*>(b+0x687C54),bytes,sizeof(bytes))||std::memcmp(bytes,expected,sizeof(bytes)))return false;
    // Pin the builder call as well as the observer instruction.
    constexpr unsigned char call[]{0xE8,0x2F,0x02,0x00,0x00};
    unsigned char actual[sizeof(call)]{};
    if(!ReadCopy6689(reinterpret_cast<void*>(b+0x687BF0),actual,sizeof(actual))||std::memcmp(actual,call,sizeof(call)))return false;
    using namespace DetourModKit::hook;Options o{};o.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{"kcd2vr_close_query164",Target{DetourModKit::Address{b+0x687C54}},o},CloseQuery164);
    if(!h)return false;g_close_query164=std::move(*h);
    if(!g_close_query164->enable()){g_close_query164.reset();return false;}
    return true;
}
