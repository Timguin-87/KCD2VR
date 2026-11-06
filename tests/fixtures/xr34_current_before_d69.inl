// Exact pre-diagnostic D69 function, mechanically renamed only for side-by-side read/call trace comparison.
static bool XR34CurrentBeforeD69(const xr34::Owner& o) noexcept {
    // INVESTIGATE_D67R1 CROSSBOW_OWNER: D66 has fresh SOURCE_OK followed by
    // START_REFUSED and some NON_FRAME_THREAD_NATIVE_PASS receipts. This
    // sticky owner-thread check is distinct from the serialized current
    // alchemy/pickup callback scopes. Trace the exact rejected leaf and native
    // controller call stack before qualifying worker migration; preserving
    // current identity/queue/hand ownership is essential. Native nock success
    // is not proof this gesture clock was acquired. See OPEN_SOURCE_POINTS.txt.
    const auto b=g_base.load(std::memory_order_acquire);const auto& s=o.source;
    const auto identity=xr34::identityReader.load(std::memory_order_acquire);
    if(!b||!xr34::Registered()||!identity||!identity(o)||!s.allowed||
       !OnOwnerThread13()||o.controller!=reinterpret_cast<std::uintptr_t>(g_henry_ctrl.load())||
       !IsHenryController(reinterpret_cast<void*>(o.controller)))return false;
    const xr34::Ops io{&XR34ReadBytes};
    const auto member=xr34::membership(io,b,o);
    if(member!=xr34::Member::Queued&&member!=xr34::Member::Root)return false;
    std::uintptr_t v=0,main=0,parent=0,child=0,context=0,characters=0,character=0,item=0,ammo=0;
    std::uint64_t key=0;unsigned actor=0,token=0;int status=-1;
    if(!XR34Read(s.player,v)||v!=b+0x3AA4B98||!XR34Read(s.player+0x30,actor)||actor!=s.actor||
       !XR34Read(s.equipment+0x40,item)||item!=s.weaponItem||!XR34Read(item,v)||v!=b+0x3A2DA88||
       !XR34Read(item+0x30,key)||key!=s.key||!XR34Read(s.equipment+0x48,ammo)||ammo!=s.ammoItem||
       !XR34Read(ammo,v)||v!=b+0x3A2DA88||!XR34Read(ammo+0x30,key)||key!=s.ammoKey||!XR34Read(s.equipment+0x50,main)||main!=o.main||
       !XR34Read(main,v)||v!=b+0x3E7B1B0||!XR34Read(main+0x98,parent)||parent!=o.parent||
       !XR34Read(parent,v)||v!=b+0x3E78CF8||!XR34Read(parent+0x80,main)||main!=o.main||
       !XR34Read(parent+0x70,child)||child!=o.child||!XR34Read(child,v)||v!=b+0x3A450B0||
       !XR34Read(child+8,context)||context!=o.context||!XR34Read(child+0x54,token)||token!=o.token||
       !XR34Read(child+0x28,status)||status!=(member==xr34::Member::Queued?1:2)||
       !XR34Read(o.controller+0x20,context)||context!=o.context||
       !XR34Read(o.controller+0x28,characters)||!XR34Read(characters+0x10,character)||character!=s.character)return false;
    void* actorNow=nullptr;void* equipmentNow=nullptr;
#if defined(KCD2VR_XR34_HOST_TEST)
    // Explicit native read-only leaves only; the fixture still executes all
    // real current-owner/container/token/backlink qualification above.
    actorNow=HostXR34ResolveMainOwner(o.main);
    if(actorNow==reinterpret_cast<void*>(s.player))equipmentNow=HostXR34Equipment(actorNow);
#else
    __try {
        actorNow=reinterpret_cast<void*(__fastcall*)(void*)>(b+0x9115E0)(reinterpret_cast<void*>(o.main+0x50));
        if(actorNow==reinterpret_cast<void*>(s.player))
            equipmentNow=reinterpret_cast<void*(__fastcall*)(void*)>(b+0x7F3304)(actorNow);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
#endif
    return actorNow==reinterpret_cast<void*>(s.player)&&equipmentNow==reinterpret_cast<void*>(s.equipment);
}
