// Existing Flash worker only; exact holder/display API used by pause/wait.
// Native controls and authored child coordinates stay untouched.
static bool ApplyActivityOverlayS1(unsigned index,bool enabled,const ui_surface_geometry::Hud& geometry,float size) noexcept {
    static ui_modal_overlay43::Transaction fit[2];
    static std::uint64_t generation[2]{~0ull,~0ull};
    if(index>=2||!retail_build112::Ready(WhBase())||!g_display_verified170)return false;
    if(!enabled&&!fit[index].Active())return true;
    HudHolder holder{};void* element=nullptr;
    const char* name=index==0?"LockPicking":"Pickpocketing";
    if(!AcquireElementDirect(holder,name,element)){
        ReleaseHudHolder(holder);if(!enabled)fit[index].Forget();return !enabled;
    }
    // AcquireElementDirect can use the original lookup, bypassing our observer.
    // Join the current owned holder here too; visibility never comes from lookup.
    if(g_activity_element_s1[index].exchange(element,std::memory_order_acq_rel)!=element)
        g_activity_generation_s1[index].fetch_add(1,std::memory_order_acq_rel);
    const auto live=g_activity_generation_s1[index].load(std::memory_order_acquire);
    if(generation[index]!=live){fit[index].Forget();generation[index]=live;}
    struct Backend {
        unsigned index=0;void* element=nullptr;void* clip=nullptr;void* get=nullptr;void* set=nullptr;std::uint64_t generation=0;
        bool Alive()const noexcept{return generation==g_activity_generation_s1[index].load(std::memory_order_acquire)&&
            element==g_activity_element_s1[index].load(std::memory_order_acquire);}
        bool Resolve(const char* name,std::uintptr_t& id)noexcept {
            if(!Alive()||!FindMovieClip(element,name,clip,get,set)||!Alive())return false;
            id=reinterpret_cast<std::uintptr_t>(clip);return true;
        }
        bool Get(ui_native_canvas::Geometry& value)noexcept {
            alignas(16) std::array<unsigned char,0x38> bytes{};
            if(!Alive()||!CallFlashDisplayFnSEH(get,clip,bytes.data())||!Alive())return false;
            value=ui_native_canvas::Unpack(bytes);return true;
        }
        bool Set(ui_native_canvas::Geometry value)noexcept {
            alignas(16) auto bytes=ui_native_canvas::Pack(value);
            return Alive()&&CallFlashDisplayFnSEH(set,clip,bytes.data())&&Alive();
        }
    } io;io.index=index;io.element=element;io.generation=live;
    // Actual retail GFX direct parents at (960.2,540) and (959.8,540).
    // Lock, pick, cursor and point scale TOGETHER; no per-cursor game writes.
    const bool ok=fit[index].Run(io,reinterpret_cast<std::uintptr_t>(element),enabled,geometry,
        std::clamp(size,.45f,1.15f),index==0?"mc_LockPick":"dialog");
    ReleaseHudHolder(holder);return ok;
}
void UpdateActivityOverlaysS1(const ui_native_state::Snapshot& snapshot,bool enabled,bool geometryReady,
                            const ui_surface_geometry::Hud& geometry,std::uint64_t surface,float size,std::uint64_t now) noexcept {
    struct Retry {std::uint64_t generation=~0ull,surface=~0ull,next=0;bool wanted=false;float size=-1;unsigned remaining=0;};
    static Retry retry[2];
    for(unsigned i=0;i<2;++i){auto& r=retry[i];const auto kind=i==0?ui_native_state::LockPicking:ui_native_state::Pickpocketing;
        const bool wanted=enabled&&snapshot.Known(kind)&&snapshot.Open(kind);
        const auto generation=g_activity_generation_s1[i].load(std::memory_order_acquire);
        if(wanted!=r.wanted||generation!=r.generation||surface!=r.surface||size!=r.size){
            r.wanted=wanted;r.generation=generation;r.surface=surface;r.size=size;r.remaining=8;r.next=now;
        }
        if(!r.remaining||now<r.next||(wanted&&!geometryReady))continue;
        --r.remaining;r.next=now+150;
        const bool ok=ApplyActivityOverlayS1(i,wanted,geometry,size);
        if(ok)r.remaining=0;
        if(wanted&&(ok||!r.remaining)){char line[230]{};std::snprintf(line,sizeof(line),
            "S1 ACTIVITY_OVERLAY kind=%s result=%s size=%.3f parent=%s nativePad=UNCHANGED steadyFlashCalls=0",
            i==0?"LOCKPICKING":"PICKPOCKETING",ok?"READBACK_VERIFIED":"DEFERRED",double(size),i==0?"mc_LockPick":"dialog");
            diagnostic_trace::Write(line);}
    }
}
