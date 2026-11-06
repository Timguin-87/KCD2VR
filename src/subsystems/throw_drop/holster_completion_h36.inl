// H36R1 native-frame observations only. No item/attachment/native writes.
// The original native DRAW/STOW remains the sole mutation owner.
struct LeftSocketH36 {
    void* model=nullptr;void* attachment=nullptr;void* attachmentManager=nullptr;
    void* object=nullptr;std::uintptr_t character=0;std::uint32_t entity=0;
};
struct LeftBindingH36 {
    LeftSocketH36 hand{},stowed{};void* item=nullptr;void* link=nullptr;void* entity=nullptr;
    std::uint64_t key=0,generation=0,body=0,request=0,nativeMembership=0;void* player=nullptr;void* manager=nullptr;
    int set=-1,stowedIndex=-1;std::array<char,64> stowedName{};bool valid=false;
};
static bool SameSocketGraphH36(const LeftSocketH36& a,const LeftSocketH36& b) noexcept {
    return a.model&&a.model==b.model&&a.attachment==b.attachment&&a.attachmentManager==b.attachmentManager&&
        a.character&&a.character==b.character;
}
static bool ReadStowedNameH36(int index,std::array<char,64>& name) noexcept {
    name={};const auto b=WhBase();if(index<0||index>7||!retail_build112::Ready(b))return false;
    // Exact10850D0 table lookup;1084F38 enumerates the eight authored slots.
    std::uintptr_t first=0,last=0,entry=0,text=0;int firstId=0;
    if(!Read(reinterpret_cast<void*>(b+0x5325648),first)||!Read(reinterpret_cast<void*>(b+0x5325650),last)||
       !Read(reinterpret_cast<void*>(b+0x5325660),firstId)||last<first||(last-first)%8||
       firstId<0||firstId>7||index<firstId||std::uint64_t(index-firstId)>=(last-first)/8||
       !Read(reinterpret_cast<void*>(first+8*std::uintptr_t(index-firstId)),entry)||!entry||
       !Read(reinterpret_cast<void*>(entry+8),text)||!text)return false;
    bool ended=false;for(unsigned n=0;n<name.size();++n){if(!Read(reinterpret_cast<void*>(text+n),name[n]))return false;
        if(!name[n]){ended=n!=0;break;}}
    std::uintptr_t firstAfter=0,lastAfter=0,entryAfter=0,textAfter=0;int idAfter=0;
    return ended&&Read(reinterpret_cast<void*>(b+0x5325648),firstAfter)&&firstAfter==first&&
        Read(reinterpret_cast<void*>(b+0x5325650),lastAfter)&&lastAfter==last&&
        Read(reinterpret_cast<void*>(b+0x5325660),idAfter)&&idAfter==firstId&&
        Read(reinterpret_cast<void*>(first+8*std::uintptr_t(index-firstId)),entryAfter)&&entryAfter==entry&&
        Read(reinterpret_cast<void*>(entry+8),textAfter)&&textAfter==text;
}
static bool ReadLeftSocketH36(const PhysEquipment6698& e,const char* name,LeftSocketH36& out) noexcept {
    out={};const auto b=WhBase();if(!retail_build112::Ready(b)||!name||!name[0]||
        !PhysVft6698(e.player,0x3AA4B98)||!PhysVft6698(e.manager,0x3EC9958))return false;
    void* owner=nullptr;void* back=nullptr;void* model=nullptr;void* modelOwner=nullptr;
    if(!Read(static_cast<unsigned char*>(e.manager)+0x38,owner)||owner!=e.player||
       !Read(static_cast<unsigned char*>(e.player)+0x9D0,back)||back!=e.manager||
       !Read(static_cast<unsigned char*>(e.player)+0x1B0,model)||!model||
       !Read(static_cast<unsigned char*>(model)+0x20,modelOwner)||modelOwner!=e.player)return false;
    // Null/faulted lookup is UNKNOWN, never an empty-socket acknowledgement.
    void* attachment=NativeHeldAttachmentFindT4(model,name);void* am=nullptr;void* character=nullptr;void* object=nullptr;
    if(!PhysVft6698(attachment,0x3A657C0)||!Read(static_cast<unsigned char*>(attachment)+0x28,am)||!am||
       !Read(static_cast<unsigned char*>(am)+0x70,character)||!PhysVft6698(character,0x40602F8)||
       reinterpret_cast<std::uintptr_t>(character)!=g_player_char37.load(std::memory_order_acquire)||
       !Read(static_cast<unsigned char*>(attachment)+0x20,object))return false;
    std::uint32_t entity=0;if(object&&(!PhysVft6698(object,0x3A68F68)||!Read(static_cast<unsigned char*>(object)+8,entity)||!entity))return false;
    void* modelAfter=nullptr;void* ownerAfter=nullptr;void* objectAfter=nullptr;void* characterAfter=nullptr;void* amAfter=nullptr;
    if(!Read(static_cast<unsigned char*>(e.player)+0x1B0,modelAfter)||modelAfter!=model||
       !Read(static_cast<unsigned char*>(model)+0x20,ownerAfter)||ownerAfter!=e.player||
       !Read(static_cast<unsigned char*>(attachment)+0x20,objectAfter)||objectAfter!=object||
       !Read(static_cast<unsigned char*>(attachment)+0x28,amAfter)||amAfter!=am||
       !Read(static_cast<unsigned char*>(am)+0x70,characterAfter)||characterAfter!=character)return false;
    out={model,attachment,am,object,reinterpret_cast<std::uintptr_t>(character),entity};return true;
}
static bool ReadLeftHandH36(const PhysEquipment6698& e,LeftSocketH36& out) noexcept {
    const char* name=nullptr;return retail_build112::Ready(WhBase())&&
        Read(reinterpret_cast<void*>(WhBase()+0x40AC2B0),name)&&name&&ReadLeftSocketH36(e,name,out);
}
static const PhysEquipmentItem6698* LeftItemH36(const PhysEquipment6698& e,const ph::Command& c,bool exactGeneration) noexcept {
    if(e.count<0||e.count>int(e.items.size()))return nullptr;
    for(int n=0;n<e.count;++n){const auto& item=e.items[n];
        if(item.profile.key==c.key&&item.profile.side==1&&item.set==c.set&&(!exactGeneration||item.profile.generation==c.generation))return &item;}
    return nullptr;
}
static bool ReadLeftBindingH36(const PhysEquipment6698& e,const ph::Command& c,LeftBindingH36& out) noexcept {
    out={};const auto* item=LeftItemH36(e,c,true);if(!item||!item->item||!item->worldLink54||!item->entity||!item->profile.entity||
        !PhysVft6698(item->item,0x3A2DA88)||!PhysVft6698(item->worldLink54,0x3EC85E0))return false;
    std::uint64_t key=0;void* link=nullptr;void* entity=nullptr;void* backing=nullptr;std::uint32_t id=0;
    if(!Read(static_cast<unsigned char*>(item->item)+0x30,key)||key!=c.key||
       !Read(static_cast<unsigned char*>(item->item)+0x88,link)||link!=item->worldLink54||
       !Read(static_cast<unsigned char*>(link)+0x30,id)||id!=item->profile.entity||
       !Read(static_cast<unsigned char*>(link)+0x38,entity)||entity!=item->entity||
       !Read(static_cast<unsigned char*>(link)+0x58,backing)||backing!=item->item||!ReadLeftHandH36(e,out.hand))return false;
    out.item=item->item;out.link=link;out.entity=entity;out.key=c.key;out.generation=c.generation;
    out.nativeMembership=g_left_native_membership_h36.load(std::memory_order_acquire);out.body=c.bodyEpoch;out.request=c.epoch;out.player=e.player;out.manager=e.manager;out.set=c.set;out.valid=true;return true;
}
static bool FindOriginalStowedH36(const PhysEquipment6698& e,LeftBindingH36& b) noexcept {
    int found=-1;for(int n=0;n<8;++n){std::array<char,64> name{};LeftSocketH36 s{};
        if(!ReadStowedNameH36(n,name)||!ReadLeftSocketH36(e,name.data(),s)||!s.object)continue;
        // Before DRAW the held socket is empty: use the original item's exact entity.
        std::uint32_t id=0;if(!Read(static_cast<unsigned char*>(b.link)+0x30,id)||s.entity!=id)continue;
        if(found!=-1)return false;found=n;b.stowed=s;b.stowedName=name;b.stowedIndex=n;}
    return found!=-1&&b.stowed.attachment!=b.hand.attachment&&b.stowed.model==b.hand.model&&b.stowed.character==b.hand.character;
}
static bool CurrentOriginalStowedH36(const PhysEquipment6698& e,const LeftBindingH36& b,LeftSocketH36& s) noexcept {
    std::array<char,64> name{};return b.stowedIndex>=0&&ReadStowedNameH36(b.stowedIndex,name)&&name==b.stowedName&&
        ReadLeftSocketH36(e,name.data(),s)&&SameSocketGraphH36(s,b.stowed);
}

static bool CurrentEquipmentKeysH36(const PhysEquipment6698& e) noexcept {
    for(int side=0;side<2;++side){std::uint64_t key=0;
        if(!e.handKnown[side]||!NativeHandKey6697(e.manager,side,key))return false;
        if(PhysEmpty6698(key))key=0;if(key!=e.handKey[side])return false;}
    return true;
}

static bool SocketStillCurrentH36(const LeftSocketH36& s) noexcept {
    void* object=nullptr;void* character=nullptr;void* manager=nullptr;std::uint32_t id=0;
    return PhysVft6698(s.attachment,0x3A657C0)&&Read(static_cast<unsigned char*>(s.attachment)+0x20,object)&&object==s.object&&
        Read(static_cast<unsigned char*>(s.attachment)+0x28,manager)&&manager==s.attachmentManager&&
        Read(static_cast<unsigned char*>(s.attachmentManager)+0x70,character)&&reinterpret_cast<std::uintptr_t>(character)==s.character&&
        (!object||(PhysVft6698(object,0x3A68F68)&&Read(static_cast<unsigned char*>(object)+8,id)&&id==s.entity));
}

// H36R2: a fresh STOW can begin with an already-held native item. Preserve
// actual empty destination sockets from BEFORE that original call, not an
// inferred previous DRAW or a slot guessed from weapon type.
struct LeftStowDestinationsH36 {
    std::array<LeftSocketH36,8> sockets{};
    std::array<std::array<char,64>,8> names{};
    unsigned knownEmpty=0;
};
static bool CaptureStowDestinationsH36(const PhysEquipment6698& e,const LeftBindingH36& b,
                                      LeftStowDestinationsH36& out) noexcept {
    out={};std::uint32_t id=0;
    if(!b.valid||e.leftNativeMembershipH36!=b.nativeMembership||!b.hand.object||!Read(static_cast<unsigned char*>(b.link)+0x30,id)||
       !id||b.hand.entity!=id||!SocketStillCurrentH36(b.hand))return false;
    for(int n=0;n<8;++n){LeftSocketH36 slot{};std::array<char,64> name{};
        if(!ReadStowedNameH36(n,name)||!ReadLeftSocketH36(e,name.data(),slot))continue;
        if(slot.attachment==b.hand.attachment||slot.model!=b.hand.model||slot.character!=b.hand.character)return false;
        if(slot.object){if(slot.entity==id)return false;continue;}
        out.sockets[n]=slot;out.names[n]=name;out.knownEmpty|=1u<<n;
    }
    if(!SocketStillCurrentH36(b.hand)||!CurrentEquipmentKeysH36(e))return false;
    for(int n=0;n<8;++n)if((out.knownEmpty&(1u<<n))&&!SocketStillCurrentH36(out.sockets[n]))return false;
    return g_left_native_membership_h36.load(std::memory_order_acquire)==b.nativeMembership;
}
static bool CurrentStowDestinationH36(const PhysEquipment6698& e,const LeftBindingH36& b,
                                     const LeftStowDestinationsH36& before,LeftSocketH36& out) noexcept {
    bool found=false;out={};
    for(int n=0;n<8;++n)if(before.knownEmpty&(1u<<n)){
        std::array<char,64> name{};LeftSocketH36 slot{};
        if(!ReadStowedNameH36(n,name)||name!=before.names[n]||
           !ReadLeftSocketH36(e,name.data(),slot)||!SameSocketGraphH36(slot,before.sockets[n]))return false;
        if(slot.object&&slot.entity==b.hand.entity){if(found)return false;out=slot;found=true;}
    }
    return found&&SocketStillCurrentH36(out);
}

// H36R3: ABEE24(C_Item*) is the actual native existing-link-or-pending-item
// predicate. A prior unfinished world job is NOT the new original DRAW.
static bool InitialItemAbsentH36r3(void* item) noexcept {
 const auto b=WhBase();if(!retail_build112::Ready(b))return false;
 constexpr unsigned char pin[]{0x48,0x89,0x5C,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x33,0xDB,0x48,0x8B,0xF9};
 unsigned char actual[sizeof(pin)]{};if(!Read(reinterpret_cast<void*>(b+0xABEE24),actual)||std::memcmp(actual,pin,sizeof(pin)))return false;
 __try{return !reinterpret_cast<bool(__fastcall*)(void*)>(b+0xABEE24)(item);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool ReadInitialLeftDrawH36r3(const PhysEquipment6698&e,const ph::Command&c,LeftBindingH36&out,
 kcd2vr::initial_draw_h36r3::Watch&watch) noexcept {
 namespace initial=kcd2vr::initial_draw_h36r3;out={};watch={};
 const auto* item=LeftItemH36(e,c,true);void* link=nullptr;std::uint64_t key=0;
 if(!initial::callbackReady.load(std::memory_order_acquire)||!item||!item->item||item->worldLink54||item->entity||item->profile.entity||
    e.handKey[1]||!PhysVft6698(item->item,0x3A2DA88)||!Read(static_cast<unsigned char*>(item->item)+0x30,key)||key!=c.key||
    !Read(static_cast<unsigned char*>(item->item)+0x88,link)||link||!ReadLeftHandH36(e,out.hand)||out.hand.object||
    !InitialItemAbsentH36r3(item->item)||!SocketStillCurrentH36(out.hand)||!CurrentEquipmentKeysH36(e)||
    !PhysVft6698(item->item,0x3A2DA88)||!Read(static_cast<unsigned char*>(item->item)+0x30,key)||key!=c.key||
    !Read(static_cast<unsigned char*>(item->item)+0x88,link)||link)return false;
 const auto membership=g_left_native_membership_h36.load(std::memory_order_acquire);
 if(e.leftNativeMembershipH36!=membership)return false;
 watch={0,c.key,c.bodyEpoch,c.epoch,c.sample,c.tick,membership,reinterpret_cast<std::uintptr_t>(e.manager),
  reinterpret_cast<std::uintptr_t>(e.player),g_player_char37.load(std::memory_order_acquire),reinterpret_cast<std::uintptr_t>(item->item)};
 if(!initial::arm(watch))return false;
 out.item=item->item;out.key=c.key;out.generation=c.generation;out.body=c.bodyEpoch;out.request=c.epoch;
 out.player=e.player;out.manager=e.manager;out.set=c.set;out.nativeMembership=membership;out.valid=true;return true;
}
