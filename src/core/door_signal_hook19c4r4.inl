// Included by the native adapter and actual-callback host fixture. All game/Lua
// leaves are outside this routine; no borrowed name survives the native call.
static int __fastcall DoorGetCVarDetour19c4r4(void* self,void* handler) noexcept {
    if(!g_channel19c4r4.enabled()&&!g_channel19c4r4.pending()
       &&!kcd2vr::diagnostics::capture168::gate.mask.load(std::memory_order_relaxed)
#ifdef KCD2VR_SMALL_PICKUP_R24
       &&!g_small_pickup_readerR24.load(std::memory_order_acquire)
#endif
#ifdef KCD2VR_SWAP_NOTE_R100
       &&!kcd2vr::brush_swap_r100::notePending.load(std::memory_order_acquire)
#endif
       )
        return g_get_cvar_original19c4r4?g_get_cvar_original19c4r4(self,handler):0;
    char name[128]{};
    if(!DoorNativeName19c4r4(handler,name,sizeof(name)))
        return g_get_cvar_original19c4r4?g_get_cvar_original19c4r4(self,handler):0;
    std::uint32_t value=0;
#ifdef KCD2VR_SWAP_NOTE_R100
    // R100: the companion names the loose copy it just spawned; only the armed serial answers 1.
    if(!std::strncmp(name,kcd2vr::brush_swap_r100::notePrefix,sizeof(kcd2vr::brush_swap_r100::notePrefix)-1))
        return DoorNativeEndInteger19c4r4(handler,kcd2vr::brush_swap_r100::acceptNote(name));
#endif
#ifdef KCD2VR_PICKUP_PREPARE_NOTE_D85R5R1
    if(!std::strncmp(name,kcd2vr::diagnostics::pickup_prepare_d85r5r1::prefix,
        sizeof(kcd2vr::diagnostics::pickup_prepare_d85r5r1::prefix)-1)){
        unsigned code=0;
        if(kcd2vr::diagnostics::pickup_prepare_d85r5r1::parse(name,code))DoorPickupNoteD85R5R1(code);
        return DoorNativeEndInteger19c4r4(handler,0); // observation only, never pickup permission
    }
#endif
#ifdef KCD2VR_SMALL_PICKUP_R24
    if(!std::strncmp(name,lab_prop58::contactPrefixR24,sizeof(lab_prop58::contactPrefixR24)-1)){
        std::uint64_t key=0;const auto reader=g_small_pickup_readerR24.load(std::memory_order_acquire);
        if(reader&&lab_prop58::contactKeyR24(name,key))value=reader(key,true);
        return DoorNativeEndInteger19c4r4(handler,value);
    }
    // Fixed private opcode only. Malformed/private queries are consumed as zero,
    // never delegated as arbitrary CVar names. This branch performs NO writes.
    if(!std::strncmp(name,lab_prop58::pickupPrefixR24,sizeof(lab_prop58::pickupPrefixR24)-1)){
        std::uint64_t key=0;const auto reader=g_small_pickup_readerR24.load(std::memory_order_acquire);
        if(reader&&lab_prop58::pickupKeyR24(name,key))value=reader(key,false);
        return DoorNativeEndInteger19c4r4(handler,value);
    }
#endif
    // Read-only native curve binding runs on this Door's own Lua callback.
    // Host fixtures without a binder retain the normal private-field path.
#ifdef KCD2VR_DOOR_CURVE_SIGNAL66
    if(DoorCurveMessage66(name,DoorClock19c4r4(),value))return DoorNativeEndInteger19c4r4(handler,value);
#endif
    if(!g_channel19c4r4.field(name,DoorClock19c4r4(),value))
        return g_get_cvar_original19c4r4?g_get_cvar_original19c4r4(self,handler):0;
    return DoorNativeEndInteger19c4r4(handler,value);
}
