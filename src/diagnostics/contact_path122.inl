// Included inside combat_diag_probe's namespace. Exact native callback stack,
// never a guessed accepted-hit result. Producers copy POD; worker formats it.
namespace cp122=kcd2vr::diagnostics::combat_capture;
static std::optional<DetourModKit::hook::Hook> g_path_hooks122[5];
static std::atomic<bool> g_path_ready122{false};
struct ContactPathScope122;
static thread_local ContactPathScope122* g_path_scope122=nullptr;
struct ContactPathScope122 {
    ContactPathScope122* prior=nullptr;void* owner=nullptr;cp122::Event e{};bool active=false,force164=false;
    diagnostics::trace164::Scope traceScope164{};
    ContactPathScope122(void* self,const void* raw,std::uint64_t seq) noexcept:prior(g_path_scope122),owner(self){
        g_path_scope122=this;const auto now=std::uint64_t(GetTickCount64());
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
        const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
        force164=diagnostics::trace164::active(now,frik_pose_probe::WeaponControlMode());
        if(!g_path_ready122.load()||!retail_build112::Ready(b)||!raw||reinterpret_cast<std::uintptr_t>(self)<0x10)return;
        void* henry=frik_pose_probe::LastPlayerEntity();std::uintptr_t vft=0;std::uint32_t hid=0,target=0;
        if(!ReadCopy6689(henry,&vft,8)||vft!=b+0x4072C78||
           !ReadCopy6689(static_cast<unsigned char*>(henry)+0xC,&hid,4)||!hid||
           !ReadCopy6689(static_cast<const unsigned char*>(raw)+0x24,&target,4))return;
        auto collisions=static_cast<unsigned char*>(self)-0x10;
        if(!ReadCopy6689(self,&vft,8)||vft!=b+0x3B912E8||
           !ReadCopy6689(collisions,&vft,8)||vft!=b+0x3B91300)return;
        const bool incoming=target==hid;
        if(!incoming){
            void* actor=nullptr;void* player=nullptr;void* entity=nullptr;void* back=nullptr;
            if(!ReadCopy6689(collisions+8,&actor,8)||!ReadCopy6689(actor,&vft,8)||vft!=b+0x3A80610||
               !ReadCopy6689(static_cast<unsigned char*>(actor)+0x218,&back,8)||back!=collisions||
               !ReadCopy6689(static_cast<unsigned char*>(actor)+0x1F8,&player,8)||!ReadCopy6689(player,&vft,8)||vft!=b+0x3AA4B98||
               !ReadCopy6689(static_cast<unsigned char*>(player)+0x278,&back,8)||back!=actor||
               !ReadCopy6689(static_cast<unsigned char*>(player)+0x38,&entity,8)||entity!=henry)return;
        }
        static std::atomic<std::uint64_t> due[2]{};auto next=due[incoming?1:0].load();
        if(now<next||!due[incoming?1:0].compare_exchange_strong(next,now+20))return;
        if(!ReadCopy6689(raw,e.contact114,sizeof(e.contact114)))return;
        active=true;
        // Scope initially masks an outer callback. Borrow only the predictor
        // whose exact collision owner matches this callback; never a time window.
        auto context164=traceScope164.prior;
        if(context164.collisions!=reinterpret_cast<std::uintptr_t>(collisions))context164={};
        context164.collisions=reinterpret_cast<std::uintptr_t>(collisions);context164.contact=seq;
        diagnostics::trace164::current=context164;
        e.parent164[0]=context164.prediction;e.parent164[1]=context164.sweep;e.parent164[2]=context164.sample;
        e.mode=context164.redirected?1:0;
        e.layer=0;e.kind=cp122::Kind::ContactPath;e.tick=now;e.sequence=seq;e.poseEpoch=diagnostics::lab::Snapshot()>>4;
        e.id=int(target);e.actor=reinterpret_cast<std::uintptr_t>(collisions);e.key[0]=reinterpret_cast<std::uintptr_t>(raw);
        e.trackingFlags=incoming?2u:1u;
    }
    ~ContactPathScope122(){if(active)cp122::push(e,force164);g_path_scope122=prior;}
};
static void ContactPathMid122(DetourModKit::hook::MidContext& ctx,unsigned stage) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::ContactPath};
    using namespace DetourModKit::hook;auto* s=g_path_scope122;
    if(!s||!s->active||stage>4||gpr(ctx,Gpr::Rbx)!=reinterpret_cast<std::uintptr_t>(s->owner))return;
    unsigned char value=static_cast<unsigned char>(gpr(ctx,Gpr::Rax));
    if(stage==3&&!ReadCopy6689(reinterpret_cast<void*>(stack_pointer(ctx)+0x30),&value,1))return;
    s->e.flags|=1u<<stage;
    // Native branch polarity: enrichment false stops; the next three true
    // results stop. Stage 4 is after the candidate copy, NOT damage acceptance.
    if(stage<4&&(stage==0?!value:value!=0))s->e.layer|=1<<stage;
}
static void ContactPath0_122(DetourModKit::hook::MidContext& c) noexcept {ContactPathMid122(c,0);}
static void ContactPath1_122(DetourModKit::hook::MidContext& c) noexcept {ContactPathMid122(c,1);}
static void ContactPath2_122(DetourModKit::hook::MidContext& c) noexcept {ContactPathMid122(c,2);}
static void ContactPath3_122(DetourModKit::hook::MidContext& c) noexcept {ContactPathMid122(c,3);}
static void ContactPath4_122(DetourModKit::hook::MidContext& c) noexcept {ContactPathMid122(c,4);}
#include "contact_path_contract122.inl"
static void ShutdownContactPath122() noexcept {
    g_path_ready122=false;for(auto& h:g_path_hooks122){if(h)(void)h->disable();h.reset();}
}
static bool InstallContactPath122(std::uintptr_t b) noexcept {
    if(g_path_ready122.load())return true;
    if(!ContactPathContract122(b))return false;
    using namespace DetourModKit::hook;
    constexpr std::uintptr_t sites[]{0xD51910,0xD51925,0xD5193E,0xD5195D,0xD51A26};
    using Callback=void(*)(MidContext&) noexcept;
    constexpr Callback callbacks[]{ContactPath0_122,ContactPath1_122,ContactPath2_122,ContactPath3_122,ContactPath4_122};
    for(int i=0;i<5;++i){Options o{};o.fail_if_already_hooked=true;
        auto h=mid_at(MidRequest{"kcd2vr_contact_path122_"+std::to_string(i),Target{DetourModKit::Address{b+sites[i]}},o},callbacks[i]);
        if(!h){ShutdownContactPath122();return false;}g_path_hooks122[i]=std::move(*h);
        if(!g_path_hooks122[i]->enable()){ShutdownContactPath122();return false;}
    }
    g_path_ready122=true;return true;
}
