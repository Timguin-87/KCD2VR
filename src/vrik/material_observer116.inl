// Read-only retail material dispatch. See review/V66.116/native/MATERIAL_ABI.md.
// No effect is requested by this observer; all originals run exactly once.
using MaterialDispatch116=bool(__fastcall*)(void*,const void*);
using MaterialLookup116=std::uint16_t(__fastcall*)(void*,const char*,int);
using MaterialExecute116=bool(__fastcall*)(void*,std::uint16_t,const void*);
static MaterialDispatch116 g_material_dispatch116=nullptr;
static MaterialLookup116 g_material_lookup116=nullptr;
static MaterialExecute116 g_material_execute116=nullptr;
static std::optional<DetourModKit::hook::Hook> g_material_hooks116[3];
static std::atomic<std::uintptr_t> g_material_contract116{0};
static std::atomic<std::uint64_t> g_material_serial116{0},g_material_limited116{0};
struct MaterialScope116 {cc::Event event{};bool active=false;};
static thread_local MaterialScope116 g_material_scope116;
static bool MaterialContext116(const void* context,cc::Event& event) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    const auto mode=diagnostics::lab::Get(diagnostics::lab::Key::F3);
    if(!context||(mode!=1&&mode!=3&&!PredictionCapture120()&&!cc::captureOnly168r1(now))||!retail_build112::Ready(WhBase())||
       g_material_contract116.load()!=WhBase()||(mode==1&&!cc::active(now))||
       (mode==3&&now>g_contact_until114.load()))return false;
    // +B24CA0 resolves context+0 and +28 as actor entity IDs. Reject unrelated
    // battles using the already-published identity before reading live backlinks.
    ContactIdentity114 id{};std::uint32_t source=0,target=0;
    ReadScope101R2 scope;
    if(!g_contact_identity114.read(id)||!id.id||!id.tick||now<id.tick||now-id.tick>80||
       id.epoch!=g_pose_epoch6689.load()||id.entity!=g_last_player_entity.load()||
       !Read(context,source)||!Read(static_cast<const unsigned char*>(context)+0x28,target)||
       (source!=id.id&&target!=id.id))return false;
    void* manager=g_henry_item_attach_manager.load();void* player=nullptr;
    NativeOwner104 owner{};std::uint32_t playerId=0,entityId=0;
    if(!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||
       !NativeResolve104(player,owner)||owner.entity!=id.entity||
       !Read(static_cast<unsigned char*>(player)+0x30,playerId)||playerId!=id.id||
       !Read(static_cast<unsigned char*>(owner.entity)+0xC,entityId)||entityId!=id.id)return false;
    static std::atomic<std::uint64_t> due{0};auto next=due.load();
    if(now<next||!due.compare_exchange_strong(next,now+20)){++g_material_limited116;return false;}
    event={};event.kind=cc::Kind::Material;event.tick=now;event.mode=int(mode);
    event.sequence=++g_material_serial116;event.poseEpoch=id.epoch;
    event.actor=reinterpret_cast<std::uintptr_t>(context);event.key[0]=source;event.key[1]=target;
    event.trackingFlags=source==id.id?1u:2u;
    const auto trace169r1=kcd2vr::diagnostics::trace164::for_owner(reinterpret_cast<std::uintptr_t>(id.collisions));
    event.parent164[0]=trace169r1.prediction;event.parent164[1]=trace169r1.sweep;event.parent164[2]=trace169r1.contact;
    return true;
}
static bool MaterialService116(void* self) noexcept {
    void* service=nullptr;
    return g_material_scope116.active&&retail_build112::Ready(WhBase())&&
        Read(reinterpret_cast<void*>(WhBase()+0x492D920),service)&&service==self&&PhysVft6698(self,0x4056C88);
}
static bool __fastcall MaterialDispatchDetour116(void* self,const void* context) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::MaterialDispatch};
    const auto saved=g_material_scope116;g_material_scope116={};
    g_material_scope116.active=MaterialContext116(context,g_material_scope116.event);
    bool result=false;{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::MaterialDispatch};result=g_material_dispatch116?g_material_dispatch116(self,context):false;}
    if(g_material_scope116.active){auto e=g_material_scope116.event;e.flags=0;e.value=result?1.f:0.f;cc::push(e,e.mode==3||PredictionCapture120());}
    g_material_scope116=saved;return result;
}
static std::uint16_t __fastcall MaterialLookupDetour116(void* self,const char* row,int surface) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::MaterialLookup};
    // Copy only within the exact combat dispatcher, on its original thread.
    cc::Event e{};bool keep=false;
    {ReadScope101R2 scope;
     if(MaterialService116(self)&&row){e=g_material_scope116.event;e.flags=1;e.id=surface;
        for(std::size_t i=0;i<sizeof(e.name);++i){char c=0;
            if(!Read(row+i,c))break;
            if(c<0x20||c>0x7e){keep=c==0&&i>0;break;}
            if(i+1==sizeof(e.name))break;e.name[i]=c;
        }
     }}
    std::uint16_t result=0;{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::MaterialLookup};result=g_material_lookup116?g_material_lookup116(self,row,surface):std::uint16_t(0);}
    if(keep){e.layer=result;cc::push(e,e.mode==3||PredictionCapture120());}return result;
}
static bool __fastcall MaterialExecuteDetour116(void* self,std::uint16_t effect,const void* params) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::MaterialExecute};
    cc::Event e{};bool keep=false;
    {ReadScope101R2 scope;
     if(MaterialService116(self)&&params){e=g_material_scope116.event;e.flags=2;e.layer=effect;
        const auto p=static_cast<const unsigned char*>(params);
        float pos[3]{},normal[3]{};
        keep=Read(p+0x34,pos)&&Read(p+0x64,normal)&&Read(p+0x74,e.phase);
        for(int i=0;i<3;++i){e.hand[i]=pos[i];e.hand[i+3]=normal[i];keep=keep&&std::isfinite(pos[i])&&std::isfinite(normal[i]);}
        keep=keep&&std::isfinite(e.phase);
     }}
    bool result=false;{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::MaterialExecute};result=g_material_execute116?g_material_execute116(self,effect,params):false;}
    if(keep){e.value=result?1.f:0.f;cc::push(e,e.mode==3||PredictionCapture120());}return result;
}
static bool MaterialContract116(std::uintptr_t base) noexcept {
    if(!retail_build112::Ready(base)||!SupportedWhBuild(base))return false;
    constexpr std::uintptr_t rvas[]{0xB24A0C,0x52AA38,0x52A9A4};
    constexpr unsigned char bytes[][16]{
        {0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x18,0x55,0x57,0x41,0x57,0x48,0x8d},
        {0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x08,0x49,0x89,0x6b,0x10,0x49,0x89,0x73,0x18,0x49},
        {0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x08,0x49,0x89,0x6b,0x10,0x49,0x89,0x73,0x18,0x57}};
    for(int i=0;i<3;++i){unsigned char actual[16]{};
        if(!Read(reinterpret_cast<void*>(base+rvas[i]),actual)||std::memcmp(actual,bytes[i],16))return false;}
    std::uintptr_t method=0;
    return Read(reinterpret_cast<void*>(base+0x4056C88+0x30),method)&&method==base+0x52AA38&&
        Read(reinterpret_cast<void*>(base+0x4056C88+0x50),method)&&method==base+0x52A9A4&&
        Read(reinterpret_cast<void*>(base+0x3C25040),method)&&method==base+0xB24CA0;
}
static void ShutdownMaterial116() noexcept {
    g_material_contract116.store(0,std::memory_order_release);
    // Disable the outer scope first; then remove the inner observers through the
    // same native-hook shutdown lifecycle as the existing combat observers.
    for(auto& h:g_material_hooks116){if(h)(void)h->disable();h.reset();}
    g_material_dispatch116=nullptr;g_material_lookup116=nullptr;g_material_execute116=nullptr;
}
static bool ArmMaterial116() noexcept {
    const auto base=WhBase();if(base&&g_material_contract116.load()==base)return true;
    if(!MaterialContract116(base))return false;
    auto arm=[&](int index,std::uintptr_t rva,auto replacement,auto& original){
        static const char* names[]{"kcd2vr_v66116_material_dispatch","kcd2vr_v66116_material_lookup","kcd2vr_v66116_material_execute"};
        DetourModKit::hook::InlineRequest request{names[index],DetourModKit::hook::Target{DetourModKit::Address{base+rva}},DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(request),replacement);if(!h)return false;
        g_material_hooks116[index]=std::move(*h);
        using Fn=std::decay_t<decltype(original)>;original=g_material_hooks116[index]->original<Fn>();
        return original&&g_material_hooks116[index]->enable();
    };
    if(!arm(0,0xB24A0C,&MaterialDispatchDetour116,g_material_dispatch116)||
       !arm(1,0x52AA38,&MaterialLookupDetour116,g_material_lookup116)||
       !arm(2,0x52A9A4,&MaterialExecuteDetour116,g_material_execute116)){
        ShutdownMaterial116();return false;
    }
    g_material_contract116.store(base,std::memory_order_release);return true;
}
