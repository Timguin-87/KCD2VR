// D33: one owner for every stowed bone attachment on Henry (stowed_hide_hw.h): station = all hidden, normal play = back
// ones hidden (VrHideBackWeapons), old stowed-bow mode still applies to the bow list.
// Draw requests never reveal unrelated stowed slots. Restore exactly what we hid.
// R121: the two switches live in stowed_hide_hw.h (hideAtStationsSetting / hideBackSetting), Experiments GEAR page 2.
static bool StationVisualHW() noexcept {
    return g_cal_state.load(std::memory_order_acquire)==2&&!g_ctx_cutscene.load(std::memory_order_acquire)&&
        !diagnostics::lab::NativeReference();
}
static bool AtCraftStationHW() noexcept {
    const auto& w=kcd2vr::activity_input163::witness175;
    return w.ready.load(std::memory_order_acquire)&&
        (w.active[unsigned(kcd2vr::activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire)||
         w.active[unsigned(kcd2vr::activity_input163::Kind175::Smithing)].load(std::memory_order_acquire));
}
// One current per-slot policy is shared by holder visibility and the independently
// rendered entity binding. Native hand sockets are absent from this table.
static bool StowedLegacyNameD63(const char* name) noexcept {
    const int count=std::clamp(g_bow_att_count59c15.load(std::memory_order_acquire),0,4);
    for(int i=0;i<count;++i)if(!std::strcmp(name,g_bow_att_names59c15[i]))return true;
    return false;
}
static kcd2vr::stowed_hide_hw::Inputs StowedInputsD63() noexcept {
    namespace sh=kcd2vr::stowed_hide_hw;
    static std::atomic<bool> previousBowWant{false};
    const int mode=g_hide_stowed_bow59c14.load(std::memory_order_relaxed);
    const float threshold=g_hide_bow_pitch_deg59c14.load(std::memory_order_relaxed)-
        (previousBowWant.load(std::memory_order_relaxed)?5.f:0.f);
    sh::Inputs in{};in.immersive=StowedBowImmersive54();in.stationVisual=StationVisualHW();in.atStation=AtCraftStationHW();
    in.backVisibility=in.stationVisual; // visibility is independent of gameplay/menu pose admission
    in.hideAtStations=sh::hideAtStationsSetting.load(std::memory_order_relaxed);in.hideBack=sh::hideBackSetting.load(std::memory_order_relaxed);
    in.legacyBowWant=in.immersive&&(mode==2||(mode==1&&g_hmd_pitch_deg59c14.load(std::memory_order_relaxed)>threshold));
    previousBowWant.store(in.legacyBowWant,std::memory_order_relaxed);
    return in;
}
#include "station_weapon_entities_d56.inl"
static void StowedBowAttachmentTick59c15(std::uint64_t now) noexcept {
    namespace sh=kcd2vr::stowed_hide_hw;
    static sh::Owner owner{}; static void* hiddenExtra[4]{}; static std::uint64_t next=0; static unsigned logs=0;
    static std::uint64_t lastKey=~0ull;
    const int mode=g_hide_stowed_bow59c14.load(std::memory_order_relaxed);
    // Same hysteretic policy as the native attachment callback; no global draw grace.
    const sh::Inputs in=StowedInputsD63();
    const std::uint64_t key=(in.immersive?1u:0u)|(in.stationVisual?2u:0u)|(in.atStation?4u:0u)|(in.hideAtStations?8u:0u)|
        (in.hideBack?16u:0u)|(in.legacyBowWant?64u:0u);
    if(key==lastKey&&now<next){
        // R120 (6 Nov log): the stowed crossbow is an ITEM ENTITY bound through
        // CEntityAttachment (D63 bindingVftRva=3A68F68 on weapon_crossbow), so the
        // holder flag and the BONE render-leaf skip never stop its draw; only the
        // entity's own hidden state does. The game re-shows item entities itself
        // (CEntity::Hide(false) from its item attach code on draw/holster/rebind),
        // so the entity hide is re-checked EVERY frame here instead of every 250 ms.
        // Cheap: only wanted/owned slots are looked up, Hide is called only when visible.
        if(sh::wantSlot({"","",sh::Place::Back},in,true)||(in.atStation&&in.hideAtStations&&in.stationVisual)){
            unsigned sourceR120=0;std::uint64_t lifetimeR120=0;
            void* chR120=BowAttachPins59c15()?HenryCharacter59c15(&sourceR120,&lifetimeR120):nullptr;
            if(chR120&&lifetimeR120)StationEntitiesTickD56(chR120,lifetimeR120,in);
        }
        return;
    }
    lastKey=key; next=now+250;
    const bool pinsD44=BowAttachPins59c15();unsigned sourceD44=0;std::uint64_t lifetimeD44=0;
    void* ch=pinsD44?HenryCharacter59c15(&sourceD44,&lifetimeD44):nullptr;
    static std::uint64_t sourceDueD44=0;static unsigned lastSourceD44=~0u;static bool lastPinsD44=false;
    const bool captureD45=kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Bug,now)||
        kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,now);
    const bool traceD44=captureD45&&(sourceD44!=lastSourceD44||pinsD44!=lastPinsD44||now>=sourceDueD44);
    if(traceD44){
        lastSourceD44=sourceD44;lastPinsD44=pinsD44;sourceDueD44=now+2000;char m[256]{};
        std::snprintf(m,sizeof(m),"D44 STOWED_SOURCE pins=%d source=%s char=%p station=%d stationVisual=%d hideStation=%d owned=%d",
            pinsD44,StowedCharacterSourceD44(sourceD44),ch,in.atStation,in.stationVisual,in.hideAtStations,owner.anyHidden());diagnostic_trace::Write(m);
    }
    // Unknown is not a different character. Keep our receipts without touching
    // cached pointers, and resolve the same names on a verified current owner
    // next time. Otherwise one read miss strands an invisible stowed weapon.
    if(!ch)return;
    StationEntitiesTickD56(ch,lifetimeD44,in);
    if(owner.onOwnerD44(reinterpret_cast<std::uintptr_t>(ch),lifetimeD44)){for(auto& h:hiddenExtra)h=nullptr;
        if(ch&&captureD45){char m[400]{};int n=std::snprintf(m,sizeof(m),"D33 STOWED_ATTACHMENTS char=%p",ch);
            for(const auto& slot:sh::kSlots)
                if(n>0&&n<int(sizeof(m)))n+=std::snprintf(m+n,sizeof(m)-n," %s=%d",slot.name,CharAttachment59c15(ch,slot.name)?1:0);
            diagnostic_trace::Write(m);}}
    const int count=std::clamp(g_bow_att_count59c15.load(std::memory_order_acquire),0,4);
    auto inLegacy=[&](const char* name){for(int i=0;i<count;++i)if(!std::strcmp(g_bow_att_names59c15[i],name))return true;return false;};
    unsigned foundD44=0,hiddenD44=0,wantedD44=0,ownedD44=0;
    for(std::size_t i=0;i<sh::kCount;++i){
        const auto& slot=sh::kSlots[i];
        void* att=CharAttachment59c15(ch,slot.name);unsigned flags=0;
        if(att&&!AttachmentFlags59c15(att,flags))att=nullptr;
        const bool want=sh::wantSlot(slot,in,inLegacy(slot.name));
        if(att)foundD44|=1u<<i;if(flags&0x70000u)hiddenD44|=1u<<i;if(want)wantedD44|=1u<<i;
        const auto act=owner.stepFlagsD44(i,reinterpret_cast<std::uintptr_t>(att),flags,want);
        if(act==sh::Act::Hide||act==sh::Act::Show){
            const bool ok=AttachmentHide59c15(att,act==sh::Act::Hide);
            owner.applied(i,act,reinterpret_cast<std::uintptr_t>(att),ok);
            if(captureD45&&logs++<60){char m[200]{};std::snprintf(m,sizeof(m),"D33 STOWED %s attachment=%s place=%s station=%d back=%d bowMode=%d ok=%d",
                act==sh::Act::Hide?"HIDE":"SHOW",slot.name,slot.place==sh::Place::Back?"BACK":"FRONT",in.atStation?1:0,in.hideBack?1:0,mode,ok?1:0);diagnostic_trace::Write(m);}
        }
        if(owner.hidden[i])ownedD44|=1u<<i;
    }
    if(traceD44){char m[192]{};std::snprintf(m,sizeof(m),"D44 STOWED_STATE station=%d foundMask=%03X observedHiddenMask=%03X wantedMask=%03X ownedMask=%03X",
        in.atStation,foundD44,hiddenD44,wantedD44,ownedD44);diagnostic_trace::Write(m);}
    // custom legacy names outside the known set keep the old single-owner rule
    for(int i=0;i<count;++i){
        bool known=false;for(const auto& slot:sh::kSlots)if(!std::strcmp(slot.name,g_bow_att_names59c15[i]))known=true;
        if(known)continue;
        void* att=CharAttachment59c15(ch,g_bow_att_names59c15[i]);unsigned flags=0;
        if(!att||!AttachmentFlags59c15(att,flags)){hiddenExtra[i]=nullptr;continue;}
        const bool isHidden=(flags&0x70000u)!=0;
        if(hiddenExtra[i]==att&&(flags&0x70000u)&&(flags&0x70000u)!=0x70000u)hiddenExtra[i]=nullptr;
        const bool want=sh::wantSlot(sh::Slot{g_bow_att_names59c15[i],"",sh::Place::Back},in,true);
        if(want&&!isHidden){if(AttachmentHide59c15(att,true))hiddenExtra[i]=att;}
        else if(!want&&hiddenExtra[i]==att&&isHidden){if(AttachmentHide59c15(att,false))hiddenExtra[i]=nullptr;}
        else if(!want)hiddenExtra[i]=nullptr;
    }
}
// Called under the existing attachment transaction lock (including native-tick restore).
static void StowedBowShow59c14(void* onlyIf,std::uint64_t now) noexcept {
    void* e=g_hidden_bow_entity59c14.load();if(!e||(onlyIf&&e!=onlyIf))return;
    (void)StowedBowRelease59c14(now);
}
// Read the current world transform from THIS Henry attachment's character.
// The pose producer and attachment consumer can run at different points of a
// joystick turn. Reusing frame.world put the weapon one body transform behind.
