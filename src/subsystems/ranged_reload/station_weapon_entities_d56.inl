// D56: CEntityAttachment has an independently rendered entity. Hiding Henry's
// bone holder alone does not establish that this second consumer is hidden.
// Suppress only entities currently bound to the exact known STOWED slots. A
// native hand/tool socket never qualifies. Restore only our original-visible
// write, with fresh ID/pointer/character-lifetime checks, on detach or exit.
struct StationWeaponEntityD56 {
    void* entity=nullptr;void* attachment=nullptr;void* object=nullptr;
    std::uint32_t id=0;
};
static std::array<StationWeaponEntityD56,kcd2vr::stowed_hide_hw::kCount> g_station_entities_d56{};
static std::mutex g_station_entities_mutex_d56;
static void* g_station_character_d56=nullptr;
static std::uint64_t g_station_lifetime_d56=0;
// R120: one KCD2VR.log line per slot and body lifetime that names the route
// (entity hidden / why not) and a counter of game re-shows. Text only, never a
// gate: the hide decision is made again on every frame.
struct StationNoteR120 {std::uint32_t id=0;unsigned reason=~0u;unsigned reshows=0;};
static std::array<StationNoteR120,kcd2vr::stowed_hide_hw::kCount> g_station_notes_r120{};
static void StationEntityNoteR120(std::size_t slot,std::uint32_t id,unsigned reason) noexcept {
    // reason: 0 hidden, 1 entity lookup failed, 2 CEntity::Hide pin refused, 3 readback still visible
    if(slot>=g_station_notes_r120.size())return;
    auto& n=g_station_notes_r120[slot];
    if(n.id==id&&n.reason==reason)return;
    n.id=id;n.reason=reason;
    static const char* const why[]{"HIDDEN","ENTITY_LOOKUP_FAILED","HIDE_PIN_REFUSED","STILL_VISIBLE_AFTER_HIDE"};
    char m[256]{};std::snprintf(m,sizeof(m),
        "R120 STOWED_ENTITY slot=%s entityId=%u result=%s route=ENTITY_BINDING (item entity draws itself, the BONE leaf skip cannot hide it)",
        kcd2vr::stowed_hide_hw::kSlots[slot].name,id,why[reason<4?reason:3]);
    diagnostic_trace::Write(m);
}
static void StationEntityReshowR120(std::size_t slot,std::uint32_t id) noexcept {
    if(slot>=g_station_notes_r120.size())return;
    const unsigned n=++g_station_notes_r120[slot].reshows;
    if(n!=1&&n!=10&&n!=100&&n%1000)return;
    char m[200]{};std::snprintf(m,sizeof(m),"R120 STOWED_ENTITY_RESHOW slot=%s entityId=%u count=%u (the game showed it again, hidden again on this frame)",
        kcd2vr::stowed_hide_hw::kSlots[slot].name,id,n);
    diagnostic_trace::Write(m);
}
static bool StationEntityBoundD56(void* att,void* object,std::uint32_t id) noexcept {
    void* current=nullptr;std::uint32_t currentId=0;
    return att&&object&&id&&PhysVft6698(att,0x3A657C0)&&PhysVft6698(object,0x3A68F68)&&
        Read(static_cast<unsigned char*>(att)+0x20,current)&&current==object&&
        Read(static_cast<unsigned char*>(object)+8,currentId)&&currentId==id;
}
static bool StationEntityRestoreD56(StationWeaponEntityD56& r) noexcept {
    if(!r.entity)return true;
    // R120: the item is being drawn into a hand (draw guard armed for it). Do not
    // show it at the native hand pose; hand our hidden write to the guard, which
    // shows it at the first VR placement (or after 150 ms).
    if(kcd2vr::draw_guard_r120::armedFor(r.id)){
        std::unique_lock<std::mutex> guardLock(kcd2vr::draw_guard_r120::lock,std::try_to_lock);
        if(!guardLock.owns_lock())return false; // keep the receipt, retry next frame (never re-enter)
        for(auto& g:kcd2vr::draw_guard_r120::guards)if(g.id.load(std::memory_order_acquire)==r.id){
            void* current=EntityById6694(r.id);bool hidden=false;
            if(current==r.entity&&PhysEntityIdentity135(current,r.id)&&EntityHiddenFlag59c14(current,hidden)&&hidden)
                g.ours.store(true,std::memory_order_release);
            r={};return true;
        }
    }
    void* current=EntityById6694(r.id);
    if(!current)return false; // unresolved is not permission to touch the cached address
    if(current!=r.entity){r={};return true;}
    bool hidden=false;
    if(!PhysEntityIdentity135(current,r.id)||!EntityHiddenFlag59c14(current,hidden))return false;
    if(!hidden){r={};return true;}
    bool invoked=false;(void)EntityHide59c14(current,false,&invoked);
    if(invoked&&EntityById6694(r.id)==current&&PhysEntityIdentity135(current,r.id)&&
       EntityHiddenFlag59c14(current,hidden)&&!hidden){r={};return true;}
    return false;
}
static void StationEntitiesOwnerD56(void* character,std::uint64_t lifetime) noexcept {
    if(character==g_station_character_d56&&lifetime==g_station_lifetime_d56)return;
    // A changed body/owner lifetime revokes the cached addresses outright.
    g_station_entities_d56={};g_station_character_d56=character;g_station_lifetime_d56=lifetime;
    g_station_notes_r120={}; // new body/save load: name the route again
}
static void StationEntityHideD56(std::size_t slot,void* attachment,void* object,std::uint32_t id) noexcept {
    if(slot>=g_station_entities_d56.size()||!StationEntityBoundD56(attachment,object,id))return;
    auto& r=g_station_entities_d56[slot];
    void* entity=EntityById6694(id);
    if(!entity||!PhysEntityIdentity135(entity,id)){StationEntityNoteR120(slot,id,1);return;}
    if(r.entity&&(r.entity!=entity||r.id!=id)&&!StationEntityRestoreD56(r))return;
    bool hidden=false;if(!EntityHiddenFlag59c14(entity,hidden))return;
    if(hidden){
        // R120: our own receipt for this entity: keep it current when the game
        // rebinds the same entity with a new CEntityAttachment object.
        if(r.entity==entity&&r.id==id){r.attachment=attachment;r.object=object;}
        return; // do not adopt an entity the game hid
    }
    // R120: our receipt, entity visible again = the game re-showed it (rebind /
    // weapon action / station enter). Hide it again now, count it.
    if(r.entity==entity&&r.id==id)StationEntityReshowR120(slot,id);
    r={};
    if(!StationEntityBoundD56(attachment,object,id)||EntityById6694(id)!=entity)return;
    bool invoked=false;(void)EntityHide59c14(entity,true,&invoked);
    // Preserve a cleanup receipt even if a native call wrote then faulted.
    if(invoked)r={entity,attachment,object,id};
    bool after=false;
    StationEntityNoteR120(slot,id,!invoked?2u:(EntityHiddenFlag59c14(entity,after)&&after)?0u:3u);
}
static void StationEntitiesTickD56(void* character,std::uint64_t lifetime,const kcd2vr::stowed_hide_hw::Inputs& in) noexcept {
    std::unique_lock<std::mutex> lock(g_station_entities_mutex_d56,std::try_to_lock);
    if(!lock.owns_lock())return;
    StationEntitiesOwnerD56(character,lifetime);
    for(std::size_t i=0;i<g_station_entities_d56.size();++i){
        auto& r=g_station_entities_d56[i];
        const auto& slot=kcd2vr::stowed_hide_hw::kSlots[i];
        const bool wanted=kcd2vr::stowed_hide_hw::wantSlot(slot,in,StowedLegacyNameD63(slot.name));
        if(!wanted&&!r.entity)continue; // R120: per-frame caller: no native lookup for slots we neither want nor own
        void* att=CharAttachment59c15(character,kcd2vr::stowed_hide_hw::kSlots[i].name);
        // R120: the same entity rebound to the same back socket through a NEW
        // CEntityAttachment object is still stowed: keep the hide, no show/hide blink.
        if(r.entity&&wanted&&att&&att==r.attachment&&!StationEntityBoundD56(att,r.object,r.id)){
            void* current=nullptr;
            if(Read(static_cast<unsigned char*>(att)+0x20,current)&&current&&StationEntityBoundD56(att,current,r.id))r.object=current;
        }
        if(r.entity&&(!wanted||att!=r.attachment||!StationEntityBoundD56(att,r.object,r.id)))
            (void)StationEntityRestoreD56(r);
        if(!wanted||!att)continue;
        void* object=nullptr;std::uint32_t id=0;
        if(Read(static_cast<unsigned char*>(att)+0x20,object)&&object&&PhysVft6698(object,0x3A68F68)&&
           Read(static_cast<unsigned char*>(object)+8,id))StationEntityHideD56(i,att,object,id);
    }
}
static void StationWeaponAfterAttachmentD56(void* self,void* attachment) noexcept {
    if(!self||!attachment)return;
    const auto in=StowedInputsD63();
    if(!(in.atStation&&in.hideAtStations&&in.stationVisual)&&
       !(in.immersive&&(in.hideBack||in.legacyBowWant)))return;
    // Cheap exact Henry manager/name filter before native entity lookup. This
    // runs after the original callback so a native reshow cannot win the frame.
    void* manager=nullptr;void* character=nullptr;const char* name=nullptr;
    if(!PhysVft6698(attachment,0x3A657C0)||
       !Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       manager!=g_henry_char_attachment_manager.load(std::memory_order_acquire)||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||!character||
       !Read(static_cast<unsigned char*>(attachment)+0x10,name)||!name)return;
    char local[40]{};bool ended=false;
    for(unsigned j=0;j<sizeof(local);++j){if(!Read(name+j,local[j]))return;if(!local[j]){ended=true;break;}}
    if(!ended)return;
    std::size_t slot=0;for(;slot<kcd2vr::stowed_hide_hw::kCount;++slot)
        if(!std::strcmp(local,kcd2vr::stowed_hide_hw::kSlots[slot].name))break;
    if(slot==kcd2vr::stowed_hide_hw::kCount)return;
    if(!kcd2vr::stowed_hide_hw::wantSlot(kcd2vr::stowed_hide_hw::kSlots[slot],in,
        StowedLegacyNameD63(local)))return;
    unsigned source=0;std::uint64_t lifetime=0;
    if(HenryCharacter59c15(&source,&lifetime)!=character||!lifetime)return;
    std::unique_lock<std::mutex> lock(g_station_entities_mutex_d56,std::try_to_lock);
    if(!lock.owns_lock())return;
    StationEntitiesOwnerD56(character,lifetime);
    std::uint32_t id=0;
    if(PhysVft6698(self,0x3A68F68)&&Read(static_cast<unsigned char*>(self)+8,id))
        StationEntityHideD56(slot,attachment,self,id);
}
