// D44: the fresh C37 character is the same exact local-player owner used by
// VRIK. Stowed presentation must not depend on an inventory manager being
// published during a station transition. Every returned character still has
// the exact retail vtable and embedded attachment-manager backlink.
static void* HenryCharacter59c15(unsigned* sourceD44=nullptr,std::uint64_t* lifetimeD44=nullptr) noexcept {
    if(sourceD44)*sourceD44=0;
    if(lifetimeD44)*lifetimeD44=0;
    const auto b=WhBase();if(!b||!retail_build112::Ready(b))return nullptr;
    const auto publication=g_player_publication37.load(std::memory_order_acquire);
    const auto lifetime=g_player_lifetime_d44.load(std::memory_order_acquire);
    if(publication&1u){if(sourceD44)*sourceD44=3;return nullptr;}
    const auto canonical=g_player_char37.load(std::memory_order_acquire);
    const auto now=std::uint64_t(GetTickCount64());
    if(canonical&&PlayerIdentityAccepts37(reinterpret_cast<void*>(canonical),now)){
        void* ch=reinterpret_cast<void*>(canonical);void* back=nullptr;
        if(!PhysVft6698(ch,0x40602F8)||!PhysVft6698(static_cast<unsigned char*>(ch)+8,0x4060548)||
           !Read(static_cast<unsigned char*>(ch)+8+0x70,back)||back!=ch){if(sourceD44)*sourceD44=4;return nullptr;}
        if(publication!=g_player_publication37.load(std::memory_order_acquire)||lifetime!=g_player_lifetime_d44.load(std::memory_order_acquire)||!PlayerIdentityAccepts37(ch,now)){
            if(sourceD44)*sourceD44=3;return nullptr;}
        if(lifetimeD44)*lifetimeD44=lifetime;
        if(sourceD44)*sourceD44=1;return ch;
    }
    // Retained native route is useful during explicit presentation cleanup.
    // Recheck the two-way current manager/player link and any published local
    // entity; a stale inventory pointer cannot redirect the canonical owner.
    std::uintptr_t vt=0;void* mgr=g_henry_item_attach_manager.load(std::memory_order_acquire);
    if(!mgr||!Read(mgr,vt)||vt!=b+0x3EC9958){if(sourceD44)*sourceD44=5;return nullptr;}
    void* pl=nullptr;void* back=nullptr;
    if(!Read(static_cast<unsigned char*>(mgr)+0x38,pl)||!pl||!Read(pl,vt)||vt!=b+0x3AA4B98||
       !Read(static_cast<unsigned char*>(pl)+0x9D0,back)||back!=mgr){if(sourceD44)*sourceD44=6;return nullptr;}
    void* ent=nullptr;
    if(!Read(static_cast<unsigned char*>(pl)+0x38,ent)||!ent||!Read(ent,vt)||vt!=b+kCEntityVftRva){if(sourceD44)*sourceD44=7;return nullptr;}
    const auto publishedEntity=g_last_player_entity.load(std::memory_order_acquire);
    if(!publishedEntity||publishedEntity!=ent){if(sourceD44)*sourceD44=7;return nullptr;}
    std::uintptr_t fn=0;
    if(!Read(reinterpret_cast<const void*>(vt+0x328),fn)||fn!=b+0x6411AC){if(sourceD44)*sourceD44=8;return nullptr;}
    void* ch=nullptr;
    __try{ch=reinterpret_cast<void*(__fastcall*)(void*,int)>(fn)(ent,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){if(sourceD44)*sourceD44=8;return nullptr;}
    if(!PhysVft6698(ch,0x40602F8)||!PhysVft6698(static_cast<unsigned char*>(ch)+8,0x4060548)||
       !Read(static_cast<unsigned char*>(ch)+8+0x70,back)||back!=ch||
       (canonical&&reinterpret_cast<std::uintptr_t>(ch)!=canonical)||
       publication!=g_player_publication37.load(std::memory_order_acquire)||
       lifetime!=g_player_lifetime_d44.load(std::memory_order_acquire)||
       mgr!=g_henry_item_attach_manager.load(std::memory_order_acquire)){
        if(sourceD44)*sourceD44=9;return nullptr;
    }
    if(lifetimeD44)*lifetimeD44=lifetime;
    if(sourceD44)*sourceD44=2;return ch;
}
static const char* StowedCharacterSourceD44(unsigned source) noexcept {
    switch(source){case 1:return "C37_CURRENT";case 2:return "NATIVE_CURRENT";case 3:return "IDENTITY_PUBLICATION";
    case 4:return "CHARACTER_MANAGER";case 5:return "ITEM_MANAGER";case 6:return "PLAYER_MANAGER_LINK";
    case 7:return "PLAYER_ENTITY";case 8:return "GET_CHARACTER";case 9:return "IDENTITY_CHANGED";default:return "RETAIL_NOT_READY";}
}
