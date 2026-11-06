// Passive return observer for the exact retail entity physics getter used by
// native AddConstraint. V147 also permits one verified read at native selection;
// no persistent body pointer, physics write, constraint or background polling.
using InteractionBodyFn122=void*(__fastcall*)(void*);
static InteractionBodyFn122 g_interaction_body_original122=nullptr;
static std::optional<DetourModKit::hook::Hook> g_interaction_body_hook122;
static std::atomic<std::uintptr_t> g_interaction_body_contract122{0};
static kcd2vr::interaction_body_entry_r93::OwnedEntry g_interaction_body_entry_r93;
static bool CopyInteractionBodyEntryR93(std::uintptr_t at,void* out,std::size_t size) noexcept {
    auto* bytes=static_cast<unsigned char*>(out);
    for(std::size_t i=0;i<size;++i)if(!Read(reinterpret_cast<void*>(at+i),bytes[i]))return false;
    return true;
}
static bool InteractionBodyEntryQualifiedR93(std::uintptr_t b) noexcept {
    namespace entry=kcd2vr::interaction_body_entry_r93;
    if(!b||!retail_build112::Ready(b)||!entry::context(b,CopyInteractionBodyEntryR93))return false;
    // An existing handle/contract without its exact enabled receipt is not a
    // pristine path. This also rejects a foreign replacement after enable.
    if(g_interaction_body_hook122||g_interaction_body_contract122.load(std::memory_order_acquire))
        return g_interaction_body_contract122.load(std::memory_order_acquire)==b&&g_interaction_body_hook122&&
            g_interaction_body_original122&&g_interaction_body_entry_r93.qualifies(b,
                reinterpret_cast<std::uintptr_t>(g_interaction_body_original122),
                g_interaction_body_hook122->is_enabled(),CopyInteractionBodyEntryR93);
    return entry::pristine(b,CopyInteractionBodyEntryR93);
}
static std::atomic<std::uint64_t> g_interaction_body_seen135{0},g_interaction_body_nonnull135{0};
struct InteractionWatch122 {void* entity=nullptr;std::uint32_t id=0;std::uint64_t sequence=0,tick=0,lab=0;int hand=-1;bool passive=false,handValid=false;float handX=0,handY=0,handZ=0,maxReach=0;char action[64]{};};
static kcd2vr::core::SnapshotMailbox<InteractionWatch122> g_interaction_watch122;
static void WatchInteractionBody122(void* entity,std::uint32_t id,std::uint64_t sequence,const char* action) noexcept {
    InteractionWatch122 w{};std::uint32_t current=0;const auto f5=diagnostics::lab::Get(diagnostics::lab::Key::F5);
    if((f5>=1||cc::interactionOnly168r1(GetTickCount64()))&&g_interaction_body_contract122.load()==WhBase()&&
       retail_build112::Ready(WhBase())&&PhysVft6698(entity,0x4072C78)&&id&&
       Read(static_cast<unsigned char*>(entity)+0xC,current)&&current==id)
        {w.entity=entity;w.id=id;w.sequence=sequence;w.tick=std::uint64_t(GetTickCount64());w.lab=diagnostics::lab::Snapshot();
         InteractionHandContext158 hc{};if(g_interaction_hand_context158.read(hc)&&hc.valid&&w.tick>=hc.tick&&w.tick-hc.tick<=120){w.hand=hc.side;w.passive=hc.passive;w.handValid=true;w.handX=hc.x;w.handY=hc.y;w.handZ=hc.z;w.maxReach=hc.maxReach;}
         if(action)for(std::size_t i=0;i<sizeof(w.action)-1&&action[i];++i)w.action[i]=action[i];}
    g_interaction_watch122.publish(w);
    if(w.entity){cc::Event e{};e.kind=cc::Kind::InteractionBody;e.tick=w.tick;e.sequence=sequence;e.id=int(id);
        e.actor=reinterpret_cast<std::uintptr_t>(entity);e.flags=0;cc::push(e);}
}
static void ObserveInteractionBody122(void* self,void* body,std::uintptr_t caller,bool requestProbe147=false) noexcept {
    const auto now=std::uint64_t(GetTickCount64()),b=WhBase();InteractionWatch122 w{};
    if(!b||g_interaction_body_contract122.load()!=b||!retail_build112::Ready(b)||
       (diagnostics::lab::Get(diagnostics::lab::Key::F5)<1&&!cc::interactionOnly168r1(now))||!cc::active(now)||
       !g_interaction_watch122.read(w)||!w.entity||self!=w.entity||w.lab!=diagnostics::lab::Snapshot()||
       now<w.tick||now-w.tick>3000)return;
    ReadScope101R2 reads;std::uint32_t id=0;std::uintptr_t vft=0;
    if(!PhysVft6698(self,0x4072C78)||!Read(static_cast<unsigned char*>(self)+0xC,id)||id!=w.id)return;
    // A returned pointer is observed identity only; never saved for later use.
    const bool readVft=body&&Read(body,vft);const auto rva=readVft&&vft>=b&&vft-b<0x6000000?vft-b:0;
    g_interaction_body_seen135.fetch_add(1,std::memory_order_relaxed);if(body)g_interaction_body_nonnull135.fetch_add(1,std::memory_order_relaxed);
    static std::atomic<std::uint64_t> due{0};auto next=due.load();
    if(!requestProbe147&&(now<next||!due.compare_exchange_strong(next,now+10)))return;
    cc::Event e{};e.kind=cc::Kind::InteractionBody;e.tick=now;e.sequence=w.sequence;e.id=int(id);e.flags=requestProbe147?2:1;
    e.actor=reinterpret_cast<std::uintptr_t>(self);e.key[0]=reinterpret_cast<std::uintptr_t>(body);e.key[1]=rva;
    e.aliasHash=caller>=b&&caller-b<0x6000000?caller-b:0;e.value=float(now-w.tick);e.trackingFlags=readVft?1u:0u;
    cc::push(e);
}
// Invoked synchronously immediately after the exact Henry interaction selects
// this entity. Same contract/getter as the passive observer, never an SDK slot.
static std::atomic<bool> g_interaction_probe_fault147{false};
static const char* InteractionBodyClass164(std::uintptr_t rva) noexcept {
    // Retail RTTI at vft[-1] -> COL 40BD8D8 -> CPhysicalEntity.
    // This is the base physical class, not proof of a dynamic rigid body.
    if(retail_build112::Ready(WhBase())){
        if(rva==0x3A578C0)return "CPhysicalEntity_BASE";
        if(rva==0x3A48DC0)return "CRigidEntity";
        if(rva==0x3A48B30)return "CArticulatedEntity";
        if(rva==0x3A82570)return "CLivingEntity";
    }
    return rva?"UNCLASSIFIED_RETAIL_BODY":"NONE_OR_UNREADABLE";
}
static void* InteractionBodyRead147(void* target) noexcept {
    __try {return g_interaction_body_original122(target);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_interaction_probe_fault147=true;return nullptr;}
}
static void ProbeInteractionBody147(void* target,std::uint32_t id,std::uint64_t sequence) noexcept {
    InteractionWatch122 w{};const auto now=std::uint64_t(GetTickCount64());std::uint32_t actual=0;
    if(g_interaction_probe_fault147.load()||!g_interaction_body_original122||
       (diagnostics::lab::Get(diagnostics::lab::Key::F5)<1&&!cc::interactionOnly168r1(now))||!retail_build112::Ready(WhBase())||
       g_interaction_body_contract122.load()!=WhBase()||!g_interaction_watch122.read(w)||
       w.entity!=target||w.id!=id||w.sequence!=sequence||now<w.tick||now-w.tick>20||
       w.lab!=diagnostics::lab::Snapshot()||!PhysVft6698(target,0x4072C78)||
       !Read(static_cast<unsigned char*>(target)+0xC,actual)||actual!=id)return;
    // One request is one observation. This token is diagnostic deduplication only.
    static thread_local std::uint64_t lastSequence=0;
    if(!sequence||sequence==lastSequence)return;lastSequence=sequence;
    auto* body=InteractionBodyRead147(target);
    if(!g_interaction_probe_fault147.load()){
        LifecycleBody36(target,body,0);
        ObserveInteractionBody122(target,body,0,true);
        std::uintptr_t bodyVft=0;const bool bodyReadable=body&&Read(body,bodyVft);const auto base=WhBase();
        const auto bodyRva=bodyReadable&&bodyVft>=base&&bodyVft-base<0x6000000?bodyVft-base:0;
        double handDistance=-1.0;Matrix34V6550 tm{};
        if(w.handValid&&GetEntityWorldTmV6552(target,id,tm)){
            const double dx=double(w.handX)-tm.m[3],dy=double(w.handY)-tm.m[7],dz=double(w.handZ)-tm.m[11];handDistance=std::sqrt(dx*dx+dy*dy+dz*dz);
        }
        char m[768]{};std::snprintf(m,sizeof(m),
            "V66.158 HIGGS_TARGET seq=%llu targetId=%u action='%s' source=%s hand=%d passive=%d handDistanceM=%.3f maxReachM=%.2f body=%p bodyVftRva=%llX bodyReadable=%d classification=%s grabPoint=UNPROVEN constraint=NONE mutation=NONE",
            (unsigned long long)sequence,id,w.action,w.handValid?"VRIK_HAND":"VISUAL_OR_NATIVE",w.hand,w.passive?1:0,handDistance,double(w.maxReach),body,
            (unsigned long long)bodyRva,bodyReadable?1:0,InteractionBodyClass164(bodyRva));
        diagnostic_trace::Write(m);
    }
}
static void* __fastcall InteractionBodyDetour122(void* self) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::InteractionBody};
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    void* result=g_interaction_body_original122?g_interaction_body_original122(self):nullptr;
    LifecycleBody36(self,result,caller);
    ObserveInteractionBody122(self,result,caller);return result;
}
#include "interaction_body_contract122.inl"
static bool ArmInteractionBody122() noexcept {
    const auto b=WhBase();if(b&&g_interaction_body_contract122.load()==b)return InteractionBodyEntryQualifiedR93(b);
    if(g_interaction_body_hook122||!InteractionBodyContract122(b))return false;
    using namespace DetourModKit::hook;Options o{};o.fail_if_already_hooked=true;
    auto h=inline_at(InlineRequest{"kcd2vr_interaction_body122",Target{DetourModKit::Address{b+0x8A49FC}},o},&InteractionBodyDetour122);
    if(!h)return false;g_interaction_body_hook122=std::move(*h);
    g_interaction_body_original122=g_interaction_body_hook122->original<InteractionBodyFn122>();
    if(!g_interaction_body_original122||!g_interaction_body_hook122->enable())return false;
    if(!g_interaction_body_entry_r93.publish(b,reinterpret_cast<std::uintptr_t>(g_interaction_body_original122),
        g_interaction_body_hook122->is_enabled(),CopyInteractionBodyEntryR93)){
        (void)g_interaction_body_hook122->disable();return false;
    }
    g_interaction_body_contract122=b;return true;
}
static void ShutdownInteractionBody122() noexcept {
    g_interaction_body_entry_r93.clear();g_interaction_body_contract122=0;g_interaction_watch122.publish({});
    if(g_interaction_body_hook122)(void)g_interaction_body_hook122->disable();
    g_interaction_body_hook122.reset();g_interaction_body_original122=nullptr;
}
