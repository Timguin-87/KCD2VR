static bool PhysEquipmentSlot126(const PhysEquipment6698& e,const ph::Command& c) noexcept {
    if(c.set<0||c.set>3||c.side<0||c.side>1||!e.manager||e.count<0||e.count>int(e.items.size()))return false;
    for(int i=0;i<e.count;++i){const auto& item=e.items[i];
        if(item.profile.key!=c.key||item.profile.generation!=c.generation||item.profile.side!=c.side||item.set!=c.set)continue;
        if(item.set==3&&item.slot126==2&&item.profile.classId==7){
            std::uint64_t key45=0;return Read(static_cast<unsigned char*>(e.manager)+0x78,key45)&&key45==c.key;
        }
        if(item.slot126<0||item.slot126>1)return false;
        // Equipment main/offhand columns are NOT anatomical hand indexes:
        // a left-held bow occupies SecondaryMainHand (column zero).
        std::uint64_t live=0;return Read(static_cast<unsigned char*>(e.manager)+0x48+c.set*16+item.slot126*8,live)&&live==c.key;
    }
    return false;
}
static bool PhysEquipmentEntries62() noexcept {
    const auto base=WhBase();if(!retail_build112::Ready(base))return false;
    for(const auto& fp:ph::equipmentFingerprint114){unsigned char bytes[16]{};
        if(Read(reinterpret_cast<void*>(base+fp.rva),bytes)&&!std::memcmp(bytes,fp.bytes.data(),16))continue;
        // The process-live T2 supply hook intercepts the ordinary draw wrapper.
        // Only that hook's exact installed bytes may replace this one pin.
        if(fp.rva==0x8CEA48&&kcd2vr::action_time_owner59c11::NativeDrawEntryQualified62(base))continue;
        diagnostic_trace::Write("C62 EQUIPMENT_REJECT reason=NATIVE_ENTRY_IDENTITY nativeRequest=NOT_CALLED");return false;
    }
    return true;
}
static bool PhysEquipmentCommand6698(const PhysEquipment6698& e,const ph::Command& c,bool* enteredD48=nullptr) noexcept {
    if(enteredD48)*enteredD48=false;
    // T2 explicitly drawn stock is not a normal equipped loadout slot. Its
    // observed hand/attachment lease permits only the existing per-WUID native
    // stow request; acceptance remains the subsequent real empty-hand witness.
    if(c.kind==ph::CommandKind::Holster&&c.side==0&&c.set<0&&c.bodyEpoch==e.epoch&&c.epoch==e.request){
        for(int n=0;n<e.count;++n)if(e.items[n].profile.key==c.key&&e.items[n].profile.generation==c.generation&&
            PhysSupplyHeldT2(e,e.items[n],0)&&retail_build112::Ready(WhBase())){
            if(!PhysEquipmentEntries62())return false;
            __try{if(enteredD48)*enteredD48=true;return reinterpret_cast<bool(__fastcall*)(void*,std::uint64_t)>(WhBase()+0x8CF158)(e.manager,c.key);}
            __except(EXCEPTION_EXECUTE_HANDLER){return false;}
        }
        return false;
    }
    if(c.set<0||c.set>3||!PhysVft6698(e.manager,0x3EC9958))return false;
    bool exact=false;for(int i=0;i<e.count;++i)if(e.items[i].profile.key==c.key&&e.items[i].profile.generation==c.generation)exact=true;
    if(!exact||c.bodyEpoch!=e.epoch||c.epoch!=e.request)return false;
    if(!PhysEquipmentSlot126(e,c)){
        diagnostic_trace::Write("V66.126 EQUIPMENT_REJECT reason=LIVE_LOADOUT_SLOT keyHandMapping=SEPARATE");return false;
    }
    const bool comparison=FullWeaponMode120(g_weapon_control_mode.load());
    if(comparison){
        if(!PhysEquipmentEntries62())return false;
    }
    __try {
        if(comparison){
            // Per-item native requests. Never sheath an independently gripped
            // offhand item, or fabricate success from a wrapper's return value.
            if(c.kind==ph::CommandKind::Holster){
                if(e.handKey[c.side]!=c.key)return false;
                if(!PhysLeftEquipmentInvokeH36(e,c,[&]{if(enteredD48)*enteredD48=true;return reinterpret_cast<bool(__fastcall*)(void*,std::uint64_t)>(WhBase()+0x8CF158)(e.manager,c.key);})){
                    diagnostic_trace::Write("V66.116 HOLSTER nativeReturned=0 attachmentChange=NOT_ACKNOWLEDGED");return false;
                }
            }else if(c.kind==ph::CommandKind::Draw){
                if(e.handKey[c.side])return false;
                if(c.set!=3&&e.selected!=c.set){
                    if(e.handKey[0]||e.handKey[1])return false;
                    // A normal selector return closes its synchronous work;
                    // only exact readback permits this SAME command's draw.
                    if(enteredD48)*enteredD48=true;
                    const bool selected=PhysSelectEquipmentInvokeD48(e,c,[&]{
                        reinterpret_cast<void(__fastcall*)(void*,int)>(WhBase()+0x8EEA04)(e.manager,c.set);});
                    if(enteredD48)*enteredD48=false;if(!selected)return false;
                }
                if(!PhysLeftEquipmentInvokeH36(e,c,[&]{if(enteredD48)*enteredD48=true;
                    auto originalD77=[&](){reinterpret_cast<void(__fastcall*)(void*,std::uint64_t,int)>(WhBase()+0x8CEA48)(e.manager,c.key,2);return true;};
                    return PhysObserveDrawInvokeD77(e,c,originalD77);}))return false;
            }else return false;
            return true; // request returned; only subsequent hand keys acknowledge it
        }
        if(c.set==3)return false;
        if(c.kind==ph::CommandKind::Draw){if(e.handKey[0]||e.handKey[1])return false;
            if(enteredD48)*enteredD48=true;
            const bool selected=PhysSelectEquipmentInvokeD48(e,c,[&]{
                reinterpret_cast<void(__fastcall*)(void*,int)>(WhBase()+0x8EEA04)(e.manager,c.set);});
            if(enteredD48)*enteredD48=false;if(!selected)return false;
            if(!PhysLeftEquipmentBeginH36(e,c)||!PhysLeftEquipmentEnterH36(e,c))return false;
            if(enteredD48)*enteredD48=true;auto originalD77=[&](){reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x2AA24A0)(e.manager);return true;};(void)PhysObserveDrawInvokeD77(e,c,originalD77);
        }else{if(e.selected!=c.set||e.handKey[c.side]!=c.key||!PhysLeftEquipmentBeginH36(e,c)||!PhysLeftEquipmentEnterH36(e,c))return false;
            if(enteredD48)*enteredD48=true;reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x8EE994)(e.manager); }
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){PhysFault6698("V66.98 PHYSICAL FAULT draw/holster; no repeated request");return false;}
}
// These small policies are tested through the full production translation unit.
