// Included by the existing action-owner TU; one original-first script callback.
// Only bounded startup catch-up. This never depends on hand/pelvis/physics modes.
namespace inventory_t7 {
struct Actor {std::uintptr_t game=0,player=0,entity=0;std::uint32_t id=0;
    bool operator==(const Actor& b) const noexcept {return game==b.game&&player==b.player&&entity==b.entity&&id==b.id;}
    bool operator!=(const Actor& b) const noexcept {return !(*this==b);}};
static std::atomic<bool> pending{true};
static std::atomic<std::uint64_t> servicedLoadT8{0};
static bool ReadActor(Actor& out) noexcept {
    out={};const auto base=WhBase();
    if(!retail_build112::Ready(base))return false;
    void* cry=Rd<void*>(reinterpret_cast<void*>(base+0x549D328),0,nullptr);
    if(!Vft13(cry,0x40472D0))return false;
    void* game=Rd<void*>(cry,0x88,nullptr);
    if(!Vft13(game,0x3AA5988))return false;
    void* player=Rd<void*>(game,0xA40,nullptr);
    if(!Vft13(player,0x3AA4B98))return false;
    void* entity=Rd<void*>(player,0x38,nullptr);
    const auto id=Rd<std::uint32_t>(entity,0xC,0);
    // Retail numeric System.GetEntity converts through float32 (AE5C18).
    // Never let that branch round one entity ID into another registry entry.
    if(!id||double(float(id))!=double(id)||Rd<std::uint32_t>(player,0x30,0)!=id||!Vft13(entity,0x4072C78)||Entity13(id)!=entity||
       Rd<void*>(game,0xA40,nullptr)!=player||Rd<void*>(player,0x38,nullptr)!=entity||
       Rd<void*>(cry,0x88,nullptr)!=game)return false;
    out={reinterpret_cast<std::uintptr_t>(game),reinterpret_cast<std::uintptr_t>(player),
         reinterpret_cast<std::uintptr_t>(entity),id};return true;
}
static bool StartupWanted() noexcept {
    if(ammo_service_t22::integrated&&ammo_service_t22::leftChestKnivesT26)return false; // inventory-only knives: no automatic grant
    if(!(ammo_service_t22::integrated&&ammo_service_t22::leftChestKnivesT26)&&!weapon_release_t1::TestEnabledH21())return false;
    const auto load=inventory_lifecycle_t8::state.load(std::memory_order_acquire);
    return !(load&1)&&(pending.load(std::memory_order_acquire)||servicedLoadT8.load(std::memory_order_acquire)!=load);
}
static void ServiceStartup(const native_script_t2::Context& ctx) noexcept {
    static std::uint64_t next=0,serial=0,lease=0,settledAt=0;static unsigned attempts=0;
    if(!StartupWanted()||!ctx.Current())return;
    const auto load=inventory_lifecycle_t8::state.load(std::memory_order_acquire);
    if(load&1)return;
    if(load!=lease){lease=load;attempts=0;next=ctx.now+1500;settledAt=next;pending=true;}
    if(ctx.now<next||ctx.now<settledAt)return;
    next=ctx.now+1000;
    if(++attempts>300){servicedLoadT8.store(load,std::memory_order_release);pending=false;diagnostic_trace::Write("T8 INVENTORY status=WAIT_LIMIT steadyPolling=0");return;}
    Actor actor{};if(!ReadActor(actor)||inventory_lifecycle_t8::state.load(std::memory_order_acquire)!=load)return;
    char input[128]{},output[128]{};
    std::snprintf(input,sizeof(input),"%llu|%u|%llu|%u",static_cast<unsigned long long>(++serial),actor.id,static_cast<unsigned long long>(load>>1),inventory_lifecycle_t8::recoveryLease.load(std::memory_order_acquire)==load?1u:0u);
    if(!ctx.CallPacket("kcd2vr_inventory_native_t8",input,output,sizeof(output)))return;
    Actor after{};if(!ReadActor(after)||after!=actor||inventory_lifecycle_t8::state.load(std::memory_order_acquire)!=load)return;
    char prefix[48]{};std::snprintf(prefix,sizeof(prefix),"%llu|",static_cast<unsigned long long>(serial));
    const auto n=std::strlen(prefix);if(std::strncmp(output,prefix,n))return;
    char* status=output+n;
    char* counts=std::strchr(status,'|');
    if(!counts)return;*counts++=0;int d=-1,a=-1;char extra=0;
    if(std::sscanf(counts,"%d|%d%c",&d,&a,&extra)!=2||d<0||d>100000||a<0||a>100000)return;
    inventory_lifecycle_t8::daggers.store(d);inventory_lifecycle_t8::axes.store(a);
    const bool terminal=!std::strcmp(status,"DONE")||!std::strcmp(status,"ALREADY_CLAIMED")||
        !std::strcmp(status,"CLAIM_REFUSED")||!std::strcmp(status,"PARTIAL_UNCERTAIN");
    if(terminal){servicedLoadT8.store(load,std::memory_order_release);pending=false;char line[192]{};std::snprintf(line,sizeof(line),
        "T8 INVENTORY actor=%u status=%s daggers=%d axes=%d load=%llu source=POST_LOAD_NATIVE_PLAYER hipRequired=0 bodyRequired=0 steadyPolling=0",actor.id,status,d,a,static_cast<unsigned long long>(load>>1));
        diagnostic_trace::Write(line);}
}
// Explicit top-up uses the existing original-first scheduler. A native packet
// attempt is consumed BEFORE crossing into Lua, including malformed/no reply:
// CallPacket failure can happen after CreateItem and is never proof of no grant.
static bool Wanted() noexcept {return StartupWanted()||inventory_lifecycle_t8::ReplenishWanted();}
static void ServiceReplenish(const native_script_t2::Context& ctx) noexcept {
    static std::uint64_t observed=0,next=0;static unsigned attempts=0;
    if(!inventory_lifecycle_t8::ReplenishWanted()||!ctx.Current())return;
    const auto ticket=inventory_lifecycle_t8::replenishRequest34.load(std::memory_order_acquire);
    const auto load=inventory_lifecycle_t8::state.load(std::memory_order_acquire);
    if((load&1)||(ticket>>32)!=load)return;
    if(observed!=ticket){observed=ticket;attempts=0;next=ctx.now+1500;}
    if(ctx.now<next)return;next=ctx.now+1000;
    // Held reservations/native payment own inventory until their existing
    // cleanup completes. Flights alone never block a deliberate stock top-up.
    st2::Input source{};
    if(kcd2vr::ammo_carry_t18::hands.load(std::memory_order_acquire)||
       !g_supply_input_t2.read(source)||!source.allowed||!st2::fresh(ctx.now,source.tick,80))return;
    if(++attempts>300){inventory_lifecycle_t8::replenishDone34.store(ticket,std::memory_order_release);
        diagnostic_trace::Write("K34 REPLENISH status=WAIT_LIMIT repeat=EXPLICIT_MENU_ONLY");return;}
    Actor actor{};
    if(!ReadActor(actor)||!ctx.Current()||inventory_lifecycle_t8::state.load(std::memory_order_acquire)!=load||
       inventory_lifecycle_t8::replenishRequest34.load(std::memory_order_acquire)!=ticket)return;
    const auto serial=ticket&0xffffffffull;
    char input[128]{},output[128]{};
    std::snprintf(input,sizeof(input),"%llu|%u|%llu",static_cast<unsigned long long>(serial),actor.id,
        static_cast<unsigned long long>(load>>1));
    const auto previousDone=inventory_lifecycle_t8::replenishDone34.exchange(ticket,std::memory_order_acq_rel);
    const bool called=ctx.CallPacket("kcd2vr_inventory_replenish_k34",input,output,sizeof(output));
    Actor after{};
    if(!called||!ctx.Current()||!ReadActor(after)||after!=actor||inventory_lifecycle_t8::state.load(std::memory_order_acquire)!=load){
        diagnostic_trace::Write("K34 REPLENISH status=UNOBSERVED retry=NONE repeat=EXPLICIT_MENU_ONLY");return;}
    char prefix[48]{};std::snprintf(prefix,sizeof(prefix),"%llu|",static_cast<unsigned long long>(serial));
    const auto n=std::strlen(prefix);int d=-1,a=-1;char status[32]{},extra=0;
    if(std::strncmp(output,prefix,n)||std::sscanf(output+n,"%31[^|]|%d|%d%c",status,&d,&a,&extra)!=3||
       d<0||d>100000||a<0||a>100000){
        diagnostic_trace::Write("K34 REPLENISH status=BAD_REPLY retry=NONE");return;}
    if(!std::strcmp(status,"WAIT_BUSY")||!std::strcmp(status,"WAIT_OWNER")||!std::strcmp(status,"WAIT_WORLD")){
        // These exact Lua statuses are emitted only before inventory access.
        // A positive no-mutation receipt may defer the SAME menu click.
        auto expected=ticket;
        (void)inventory_lifecycle_t8::replenishDone34.compare_exchange_strong(expected,previousDone,std::memory_order_acq_rel);
    }else if(!std::strcmp(status,"DONE")||!std::strcmp(status,"PARTIAL_UNCERTAIN")){
        inventory_lifecycle_t8::daggers.store(d);inventory_lifecycle_t8::axes.store(a);
        inventory_lifecycle_t8::stockRevision34.fetch_add(1,std::memory_order_acq_rel);
    }
    char line[192]{};std::snprintf(line,sizeof(line),"K34 REPLENISH serial=%llu status=%s daggers=%d axes=%d target=5 retry=NONE",
        static_cast<unsigned long long>(serial),status,d,a);diagnostic_trace::Write(line);
}
static void Service(const native_script_t2::Context& ctx) noexcept {
    ServiceStartup(ctx);ServiceReplenish(ctx);
}
static bool Install() noexcept {
    return native_script_t2::Register(&Service,&Wanted)&&native_script_t2::Install();
}
} // namespace inventory_t7

