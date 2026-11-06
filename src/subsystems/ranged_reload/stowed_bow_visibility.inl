static bool StowedBowImmersive54() noexcept {
    return CombatCensusMode()>=3&&g_cal_state.load(std::memory_order_acquire)==2&&
        !g_ctx_cutscene.load(std::memory_order_acquire)&&!g_ctx_menu.load(std::memory_order_acquire)&&
        !diagnostics::lab::NativeReference();
}
// Only our own native write from a verified visible state grants a restore receipt.
// The existing attachment lock serializes this receipt with the native-tick restore.
static void StowedBowForget59c14() noexcept {
    g_hidden_bow_entity59c14.store(nullptr);g_hidden_bow_seen59c14.store(0);
    g_hidden_bow_id59c14=0;
}
static bool StowedBowRelease59c14(std::uint64_t /*now*/) noexcept {
    void* e=g_hidden_bow_entity59c14.load();if(!e)return true;
    const auto id=g_hidden_bow_id59c14;
    // A cached pointer is not a lifetime. Resolve the saved entity ID before reading it.
    // D75: elapsed time/read gaps cannot erase our cleanup debt. Slow first
    // placement must still be able to reveal this exact original-visible bow.
    if(!id)return false;
    void* current=EntityById6694(id);if(!current)return false; // unknown: retain bounded receipt, do not read cached pointer
    if(current!=e){StowedBowForget59c14();return true;}
    if(!PhysEntityIdentity135(e,id))return false;
    bool hidden=false;if(!EntityHiddenFlag59c14(e,hidden))return false;
    if(!hidden){StowedBowForget59c14();return true;}
    current=EntityById6694(id);if(!current)return false;
    if(current!=e){StowedBowForget59c14();return true;}
    if(!PhysEntityIdentity135(e,id))return false;
    if(!EntityHide59c14(e,false))return false;
    // A successful native call is not confirmation that the requested bit remained clear.
    current=EntityById6694(id);if(!current)return false;
    if(current!=e){StowedBowForget59c14();return true;}
    if(!PhysEntityIdentity135(e,id))return false;
    if(!EntityHiddenFlag59c14(e,hidden)||hidden)return false;
    StowedBowForget59c14();g_bow_shows59c14.fetch_add(1);
    diagnostic_trace::Write("R59.16C10R14 STOWED_BOW hidden=0 (drawn / mode off / owner stopped)");
    return true;
}
static void StowedBowHide59c14(void* entity,std::uint64_t now) noexcept {
    const int mode=g_hide_stowed_bow59c14.load(std::memory_order_relaxed);
    const float pitch=g_hmd_pitch_deg59c14.load(std::memory_order_relaxed);
    const bool want=StowedBowImmersive54()&&(mode==2||(mode==1&&pitch>g_hide_bow_pitch_deg59c14.load(std::memory_order_relaxed)));
    if(!want){if(g_hidden_bow_entity59c14.load()==entity)(void)StowedBowRelease59c14(now);return;}
    std::uint32_t id=0;
    if(!entity||!now||!Read(static_cast<unsigned char*>(entity)+0x0C,id)||!PhysEntityIdentity135(entity,id))return;
    void* owned=g_hidden_bow_entity59c14.load();
    if(owned==entity&&g_hidden_bow_id59c14!=id){
        StowedBowForget59c14();owned=nullptr;
    }
    bool hidden=false;if(!EntityHiddenFlag59c14(entity,hidden))return;
    if(hidden){
        if(owned==entity)g_hidden_bow_seen59c14.store(now);
        return; // Never adopt a native-prehidden entity or replace a different owned receipt.
    }
    if(owned==entity)StowedBowForget59c14(); // Native code already restored it; new hide needs a new receipt.
    else if(owned&&!StowedBowRelease59c14(now))return; // Do not strand a still-current previous hide.
    // Restoring the previous entity may call native code; recheck before the new transition.
    if(EntityById6694(id)!=entity||!PhysEntityIdentity135(entity,id)||
       !EntityHiddenFlag59c14(entity,hidden)||hidden)return;
    bool invoked=false;const bool applied=EntityHide59c14(entity,true,&invoked);
    if(!invoked)return; // Version/pin refusal did not touch the entity and grants no ownership.
    void* current=EntityById6694(id);
    if(current&&current!=entity)return;
    // A native call can apply Hide then fault, or its readback can be temporarily unavailable.
    // Retain our original-visible write receipt so a later draw can restore it; never adopt
    // an entity that was hidden before this call. A known-visible result needs no restoration.
    const bool readback=current==entity&&PhysEntityIdentity135(entity,id)&&EntityHiddenFlag59c14(entity,hidden);
    if(readback&&!hidden)return;
    g_hidden_bow_id59c14=id;
    g_hidden_bow_seen59c14.store(now);g_hidden_bow_entity59c14.store(entity);
    if(!applied||!readback)return;
    const auto n=g_bow_hides59c14.fetch_add(1)+1;
    if(n<=3||n%50==0){char m[160]{};std::snprintf(m,sizeof(m),"R59.16C10R14 STOWED_BOW hidden=1 mode=%d hmdPitch=%.0f count=%llu",mode,double(pitch),(unsigned long long)n);diagnostic_trace::Write(m);}
}
// R59.16C10R15: the 14 Oct log had 0 STOWED_BOW receipts - a holstered bow is NOT a separate entity (Holster despawns the
// world entity); it is drawn by Henry's character attachment (weapon_attachment_slot "weapon_bow"). Hide that attachment
// with the game's own IAttachment::HideAttachment (CAttachmentBONE vt+0x88 = 0x5AF5E8, sets/clears flags 0x70000), found by
// CAttachmentManager::GetInterfaceByName (vt+0x40 = 0x971B2C; manager = CCharInstance+8, vtable 0x4060548). Henry's
// character = manager(+0x9D0) -> player(+0x38) -> entity(+0x38) -> GetCharacter(0) (entity vt+0x328 = 0x6411AC).
