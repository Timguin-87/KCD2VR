// R59C1: called ONLY by PhysicalNativeFrameTick6698, not the mod worker.
static void WatchedObjects59c1() noexcept {
    if(!LifecycleActive165())return;
    static std::uint64_t next=0;const auto now=std::uint64_t(GetTickCount64());if(now<next)return;next=now+500;
    std::array<il165::Event,4> observed{};
    const auto session=g_lifecycle_session165.load();
    const auto count=g_lifecycle_watch165.snapshot59c1(session,now,observed);
    auto& activity=kcd2vr::activity_input163::witness175;
    for(std::size_t i=0;i<count;++i){auto e=observed[i];e.kind=il165::Kind::ObjectPose;e.tick=now;e.valid=0;
        // Resolve the ID anew: never call through a pointer retained from pickup.
        void* entity=EntityById6694(e.target);e.result=entity?1:0;
        if(entity==reinterpret_cast<void*>(e.entity)){
            std::uint32_t id=0;if(LifecycleEntity165(e.entity,id)&&id==e.target){
                e.valid=3;Matrix34V6550 tm{};if(GetEntityWorldTmV6552(entity,id,tm)){
                    e.valid|=4;std::copy_n(tm.m,12,e.worldMatrix59c1);
                    e.point[0]=tm.m[3];e.point[1]=tm.m[7];e.point[2]=tm.m[11];
                }
            }
        }else if(entity)e.result=-1; // recycled identity, not the watched object
        e.state=0;if(activity.ready.load())for(unsigned k=0;k<3;++k)if(activity.active[k].load())e.state|=1<<k;
        e.action=activity.SmithPhaseC1();LifecycleEmit165(e);
    }
}
// V66.178R19 F5/F11 item census (game thread, observe-only).
// What is in each of Henry's hands (attach-manager hand key -> CItem -> entity), the entity's NAME
// (exact KCD2 CEntity vtable +0x90 = GetName: `mov rax,[rcx+0xE0]; ret`, read from WHGame.dll
// 2026-10-09 together with +0x08 GetId and +0x100 GetWorldTM already in use), its class id and
// world position, once per change (at most every 100 ms). No writes, no Lua, no item spawning.
// Expected use: reload (F11 rung 1: bow/crossbow/arrow/bolt entities as they appear in the hands)
// and workstations (F5 rung 2: flask, mortar, hammer, tongs ... as the game hands them to Henry).
using EntityNameGetterFn178r19=const char*(__fastcall*)(void*);
static bool EntityNameV178r19(void* ent,std::uint32_t expected,char* out,std::size_t n) noexcept {
    if(!out||n<2)return false;out[0]=0;void* vft=nullptr;if(!ValidateEntityV6583(ent,expected,vft))return false;
    EntityNameGetterFn178r19 get=nullptr;if(!Read((unsigned char*)vft+0x90,get)||!get)return false;
    const char* p=nullptr;__try{p=get(ent);}__except(EXCEPTION_EXECUTE_HANDLER){p=nullptr;}
    if(!p)return false;std::size_t i=0;
    for(;i+1<n;++i){char c=0;if(!Read(p+i,c))return false;if(!c)break;out[i]=(c>=32&&c<127)?c:'?';}
    out[i]=0;return i>0;
}
// Optional observer lookup has an independent read-only failure boundary.
// A diagnostic request must never trip the physical gameplay fault latch.
// After one native read exception this observer stops for the process; there
// is no timed retry or reset of an uncertain gameplay/native ownership debt.
static void* ItemCensusLookupD48(std::uint64_t key) noexcept {
    static std::atomic<bool> disabled{false};if(disabled.load(std::memory_order_acquire))return nullptr;
    void* registry=nullptr;const auto base=WhBase();
    if(!Read(reinterpret_cast<void*>(base+0x5487A78),registry)||!registry)return nullptr;
    __try {return reinterpret_cast<void*(__fastcall*)(void*,const std::uint64_t*)>(base+0x1E3E090)(static_cast<unsigned char*>(registry)+0x168,&key);}
    __except(EXCEPTION_EXECUTE_HANDLER){
        disabled.store(true,std::memory_order_release);
        diagnostic_trace::Write("D48 ITEM_CENSUS lookup=FAULT_DISABLED observerOnly=1 gameplayFaultWritten=0 retry=NONE");return nullptr;
    }
}
static void ItemCensusImpl178r19(bool reloadOn,bool interactionOn) noexcept { // public entry: frik_pose_probe::ItemCensus178r19 (outside the anonymous namespace)
    WatchedObjects59c1();
    static std::uint64_t prevKey[2]{~0ull,~0ull};static std::uint32_t prevEntity[2]{};static std::uint64_t lastTick=0;static bool wasOn=false;
    static std::uint64_t f5Session=0,sampled[2]{};
    const bool on=reloadOn||interactionOn;
    if(!on){if(wasOn){prevKey[0]=prevKey[1]=~0ull;prevEntity[0]=prevEntity[1]=0;wasOn=false;}return;}
    const auto session=g_lifecycle_session165.load();if(f5Session!=session){f5Session=session;sampled[0]=sampled[1]=0;}
    wasOn=true;const auto now=std::uint64_t(GetTickCount64());if(lastTick&&now-lastTick<100)return;lastTick=now;
    if(!retail_build112::Ready(WhBase()))return;
    void* manager=g_henry_item_attach_manager.load();if(!PhysVft6698(manager,0x3EC9958))return;
    for(int side=0;side<2;++side){
        std::uint64_t key=0;if(!NativeHandKey6697(manager,side,key))continue;
        const bool empty=PhysEmpty6698(key);
        // Cheap hand-key changes may be noticed at 10 Hz. Unchanged items need
        // native resolution/world reads only at 2 Hz, while recording is active.
        if(key==prevKey[side]&&sampled[side]&&now-sampled[side]<500)continue;
        std::uint32_t entityId=0;void* ent=nullptr;void* observedItem=nullptr;int classId=-1;char name[64]{};Matrix34V6550 tm{};bool tmOk=false,keyRead=false;
        if(!empty){
            void* item=ItemCensusLookupD48(key);observedItem=item;void* link=nullptr;void* def=nullptr;void* cls=nullptr;std::uint64_t again=0;
            keyRead=PhysVft6698(item,0x3A2DA88)&&Read(static_cast<unsigned char*>(item)+0x30,again)&&again==key;
            if(keyRead&&Read(static_cast<unsigned char*>(item)+0x88,link)&&link&&Read(static_cast<unsigned char*>(link)+0x30,entityId)&&entityId){
                ent=EntityById6694(entityId);
                if(Read(static_cast<unsigned char*>(item)+0x48,def)&&(PhysVft6698(def,0x3A4E390)||PhysVft6698(def,0x3A4D890))&&
                   Read(static_cast<unsigned char*>(def)+0xE8,cls)&&cls){int c=-1;if(Read(static_cast<unsigned char*>(cls)+8,c)&&c>=0&&c<=32)classId=c;}

            }
        }
        const bool changed=key!=prevKey[side]||entityId!=prevEntity[side];
        const bool sample=(interactionOn||reloadOn)&&(changed||!sampled[side]||now-sampled[side]>=500);
        if(!changed&&!sample)continue;
        if(ent){(void)EntityNameV178r19(ent,entityId,name,sizeof(name));tmOk=GetEntityWorldTmV6552(ent,entityId,tm);}
        if(sample){float position[3]{tm.m[3],tm.m[7],tm.m[11]};
            if(interactionOn)LifecycleHandSample36(side,key,observedItem,ent,entityId,name,tmOk?position:nullptr);
            if(reloadOn){rl36::Event e{};e.kind=rl36::Kind::HandCensus;e.session=g_reload_generation36.load();e.tick=now;
                e.side=side;e.ammoKey=empty?0:key;e.handId=entityId;e.handRead=empty||tmOk;e.ammoRead=keyRead;
                e.poseRead59c1=tmOk;if(tmOk)std::copy_n(tm.m,12,e.matrix59c1);ReloadEmit36(e);}
            sampled[side]=now;}
        if(!changed)continue;prevKey[side]=key;prevEntity[side]=entityId;
        char m[384]{};
        if(empty)std::snprintf(m,sizeof(m),"V66.178R19 ITEM_CENSUS side=%c hand=EMPTY key=%016llX scope=%s%s",side?'L':'R',(unsigned long long)key,reloadOn?"F11-RELOAD ":"",interactionOn?"F5":"");
        else std::snprintf(m,sizeof(m),"V66.178R19 ITEM_CENSUS side=%c hand=ITEM key=%016llX entity=%u name=\"%s\" class=%d world=(%.3f,%.3f,%.3f) tm=%d scope=%s%s writes=NONE",
            side?'L':'R',(unsigned long long)key,entityId,name[0]?name:"?",classId,tmOk?double(tm.m[3]):0.0,tmOk?double(tm.m[7]):0.0,tmOk?double(tm.m[11]):0.0,tmOk?1:0,reloadOn?"F11-RELOAD ":"",interactionOn?"F5":"");
        diagnostic_trace::Write(m);
    }
}
