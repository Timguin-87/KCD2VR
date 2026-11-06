// D54: copy the original victim's actual runtime material instances, not a
// material filename or a guessed hair colour. Executes once, synchronously after
// our existing Lua Cut returns. Writes are confined to token-named mod helpers.
namespace materialD54=kcd2vr::gore_material_d54;
static bool g_materialFaultD54=false;
static std::uint64_t g_materialTokenD54=0; // native Tick owner; never reset/reused
static bool GoreMaterialPinsD54() noexcept {
    static int ready=0;const auto b=WhBase();if(!b||!retail_build112::Ready(b)||g_materialFaultD54)return false;
    if(ready)return ready>0;
    struct Pin {std::uintptr_t rva;unsigned n;unsigned char bytes[16];};
    static constexpr Pin pins[]{
        {0xF564EC,16,{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0xda,0x48,0x8b,0x44,0xd9,0x10,0x48,0x85,0xc0}},
        {0xF56528,16,{0x4c,0x8b,0x41,0x10,0x41,0x3b,0x50,0xfc,0x73,0x10,0x48,0x63,0xca,0x48,0x69,0xc1}},
        {0xF56538,13,{0xb0,0,0,0,0x4a,0x8b,0x44,0,0x08,0xc3,0x33,0xc0,0xc3}},
        {0x113CE50,5,{0x48,0x8b,0x41,0x28,0xc3}},
        {0x4A34E0,8,{0x8b,0xc2,0x48,0x8b,0x44,0xc1,0x10,0xc3}},
        {0x5AF4EC,16,{0x41,0x8b,0xc0,0x48,0x8d,0x0c,0xc1,0x48,0x83,0xc1,0x10,0xe9,0x4c,0xda,0x10,0}},
        {0x6BCF60,16,{0x48,0x85,0xd2,0x74,0x09,0x48,0x8b,0x02,0x48,0x8b,0xca,0xff,0x50,0x10,0x48,0x85}},
        {0x6BCF81,11,{0x48,0x8b,0x03,0x48,0x8b,0xcb,0xff,0x50,0x18,0xeb,0xe7}},
        {0x6BCF90,5,{0xf0,0xff,0x41,0x3c,0xc3}},
        {0x69636C,16,{0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x83,0xc8,0xff,0xf0,0x0f,0xc1,0x41}},
        {0xA6D6F4,16,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x56}},
        {0x1A76100,8,{0x48,0x8b,0x81,0xe0,0,0,0,0xc3}},
        {0x4B5260,9,{0x8b,0x41,0x08,0xc1,0xe8,0x04,0x24,0x01,0xc3}},
        {0x1A72CA0,5,{0x48,0x8b,0x41,0x70,0xc3}},
        {0x8CE8D0,8,{0x48,0x8b,0x81,0x90,0,0,0,0xc3}}
    };
    bool ok=true;
    for(const auto& pin:pins){unsigned char bytes[16]{};
        if(!Read(reinterpret_cast<const void*>(b+pin.rva),bytes)||std::memcmp(bytes,pin.bytes,pin.n)){ok=false;break;}}
    struct Slot {std::uintptr_t vt;unsigned slot;std::uintptr_t fn;};
    static constexpr Slot slots[]{
        {0x3A65A08,0xE0,0x113CE50},{0x3A8F798,0xE0,0x113CE50},
        {0x3A65958,0x10,0x1A72CA0},{0x3A8F6E8,0x10,0x8CE8D0},
        {0x3A3F158,0x78,0x5AF4EC},{0x3A3F158,0x80,0x4A34E0},{0x3A3F158,0x70,0xF564EC},{0x406C9F0,0x28,0xF56528},
        {0x40283F0,0x10,0x6BCF90},{0x40283F0,0x18,0x69636C},{0x40283F0,0x40,0x113CE50},
        {0x4077048,0x80,0xA6D6F4},{0x4072C78,0x90,0x1A76100},{0x4072C78,0x328,0x6411AC},{0x4072C78,0x200,0x4B5260}};
    for(const auto& s:slots){std::uintptr_t fn=0;if(!Read(reinterpret_cast<void*>(b+s.vt+s.slot),fn)||fn!=b+s.fn){ok=false;break;}}
    // GetCharacter is already detoured by our retained exact-entry observer.
    const unsigned char character[]{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0xda,0x33,0xd2,0xe8,0x09,0x5f,0xf9,0xff,0x4c};
    unsigned char actual[16]{};
    if(!(Read(reinterpret_cast<void*>(b+0x6411AC),actual)&&!std::memcmp(actual,character,16))&&
        !combat_diag_probe::NativeCharacterEntryQualified63(b))ok=false;
    ready=ok?1:-1;diagnostic_trace::Write(ok?
        "D54 GORE_MATERIAL pins=OK source=EXACT_ATTACHMENT_RUNTIME_INSTANCE destination=OWN_TOKEN_HELPER_CA_SKIN eventOnly=1":
        "D54 GORE_MATERIAL pins=MISMATCH runtimeCopy=OFF staticFallback=RETAINED");return ok;
}
static bool MaterialSnapshotD54(void* attachment,materialD54::Snapshot& out) noexcept {
    return materialD54::snapshot([](std::uintptr_t p,auto& value) noexcept{return Read(reinterpret_cast<const void*>(p),value);},
        GoreMaterialPinsD54(),WhBase(),reinterpret_cast<std::uintptr_t>(attachment),out);
}
static void* CharacterSlotD54(void* entity,unsigned slot) noexcept {
    std::uintptr_t vt=0,fn=0;const auto b=WhBase();if(!entity||slot>1||!Read(entity,vt)||vt!=b+0x4072C78||
        !Read(reinterpret_cast<void*>(vt+0x328),fn)||fn!=b+0x6411AC)return nullptr;
    void* ci=nullptr;__try{ci=reinterpret_cast<void*(__fastcall*)(void*,int)>(fn)(entity,int(slot));}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    void* animation=nullptr;return ValidateCharInstance(ci,animation)?ci:nullptr;
}
static bool MaterialSourceD54(const gp50::Event& e,void*& entity,void*& ci) noexcept {
    entity=nullptr;ci=nullptr;if(e.cut!=gp50::Cut::Head||!e.target||!e.nativeVictimD54||!e.nativeCharacterD54||!GoreMaterialPinsD54())return false;
    entity=FlinchEntityById178r11(e.target);std::uintptr_t vt=0;std::uint32_t id=0;
    if(!entity||reinterpret_cast<std::uintptr_t>(entity)!=e.nativeVictimD54||entity==g_last_player_entity.load(std::memory_order_acquire)||
        !Read(entity,vt)||vt!=WhBase()+0x4072C78||!Read(static_cast<unsigned char*>(entity)+0xC,id)||id!=e.target)return false;
    ci=CharacterSlotD54(entity,0);return reinterpret_cast<std::uintptr_t>(ci)==e.nativeCharacterD54;
}
static void* MaterialSourcePartD54(void* ci,const char* name,const char* expected) noexcept {
    if(!ci||!gp50::safeToken(name)||!gp50::safeToken(expected)||!g::headAttachment(name))return nullptr;
    void* att=CharAttachment59c15(ci,name);char raw[260]{},skin[260]{};
    if(!att||!HeadSkinPath43(att,raw,sizeof(raw)))return nullptr;gp50::normPath(raw,skin,sizeof(skin));
    return std::strcmp(skin,expected)==0?att:nullptr;
}
static int PrimaryMaterialPartD54(const gp50::Event& e) noexcept {
    int selected=-1,rank=0;
    for(int i=0;i<e.nAtt&&i<24;++i){const int r=gp50::primaryHeadSkinRankH24(e.attName[i],e.attSkin[i]);
        if(r>rank&&e.headSkin&&e.attSkin[i]&&!std::strcmp(e.headSkin,e.attSkin[i])){selected=i;rank=r;}}
    return selected;
}
static bool PrepareGoreMaterialsD54(gp50::Event& e,char (&names)[24][260],char (&head)[260]) noexcept {
    void* entity=nullptr;void* ci=nullptr;if(!MaterialSourceD54(e,entity,ci)||g_materialTokenD54==~std::uint64_t(0))return false;
    for(int i=0;i<e.nAtt&&i<24;++i){materialD54::Snapshot source{};
        void* att=MaterialSourcePartD54(ci,e.attName[i],e.attSkin[i]);
        if(att&&MaterialSnapshotD54(att,source)&&materialD54::name(
            [](std::uintptr_t p,auto& value) noexcept{return Read(reinterpret_cast<const void*>(p),value);},
            true,WhBase(),source.effective[0],names[i],sizeof(names[i])))e.attMtlD54[i]=names[i];}
    const int primary=PrimaryMaterialPartD54(e);
    if(primary>=0&&e.attMtlD54[primary]){std::snprintf(head,sizeof(head),"%s",e.attMtlD54[primary]);e.headMtl=head;}
    e.materialTokenD54=++g_materialTokenD54;return true;
}
static bool ExactStringD54(const char* current,const char* expected) noexcept {
    if(!current||!expected)return false;
    for(unsigned i=0;i<128;++i){char ch=0;if(!Read(current+i,ch)||ch!=expected[i])return false;if(!ch)return true;}return false;
}
static void* OwnMaterialEntityD54(const char* expected,std::uint32_t& id) noexcept {
    id=0;const auto b=WhBase();void* system=nullptr;std::uintptr_t vt=0,fn=0;
    if(!Read(reinterpret_cast<void*>(b+0x492D8A0),system)||!system||!Read(system,vt)||vt!=b+0x4077048||
        !Read(reinterpret_cast<void*>(vt+0x80),fn)||fn!=b+0xA6D6F4)return nullptr;
    void* entity=nullptr;__try{entity=reinterpret_cast<void*(__fastcall*)(void*,const char*)>(fn)(system,expected);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    const char* name=nullptr;unsigned flags=0;
    if(!entity||!Read(entity,vt)||vt!=b+0x4072C78||!Read(static_cast<unsigned char*>(entity)+0xC,id)||!id||
        !Read(static_cast<unsigned char*>(entity)+0xE0,name)||!ExactStringD54(name,expected)||
        !Read(static_cast<unsigned char*>(entity)+8,flags)||(flags&0x10u))return nullptr; // retired/hidden child is not presentation
    return entity;
}
static void* OwnMaterialAttachmentD54(void* ci,const char* source,bool head) noexcept {
    char cut[280]{};if(head){const auto n=std::strlen(source);if(n<5)return nullptr;
        std::snprintf(cut,sizeof(cut),"%.*s_kcdghead.skin",int(n-5),source);}
    void* mgr=static_cast<unsigned char*>(ci)+8;const int count=AttachmentCount(mgr);if(count<1||count>512)return nullptr;
    void* found=nullptr;
    for(int i=0;i<count;++i){void* a=AttachmentAt(mgr,unsigned(i));char raw[260]{},path[260]{};
        if(!a||!HeadSkinPath43(a,raw,sizeof(raw)))continue;gp50::normPath(raw,path,sizeof(path));
        if(std::strcmp(path,source)&&(!head||std::strcmp(path,cut)))continue;
        if(found)return nullptr;found=a; // exact one rendered skin; never blanket all character slots
    }return found;
}
struct MaterialTransferIOD54 {
    void* sourceEntity=nullptr;void* sourceCharacter=nullptr;void* destinationEntity=nullptr;void* destinationCharacter=nullptr;
    materialD54::Snapshot source{},destination{};unsigned slot=0;
    const gp50::Event* event=nullptr;const char* sourceName=nullptr;const char* sourceSkin=nullptr;
    const char* destinationName=nullptr;std::uint32_t destinationId=0;char destinationAttachmentName[64]{},destinationSkin[260]{};
    bool current() noexcept {
        if(!event||FlinchEntityById178r11(event->target)!=sourceEntity)return false;
        std::uint32_t currentId=0;void* own=OwnMaterialEntityD54(destinationName,currentId);
        if(own!=destinationEntity||currentId!=destinationId)return false;
        void* src=CharacterSlotD54(sourceEntity,0);void* dst=CharacterSlotD54(destinationEntity,slot);std::uintptr_t a=0,b=0;
        if(src!=sourceCharacter||dst!=destinationCharacter||
            MaterialSourcePartD54(src,sourceName,sourceSkin)!=reinterpret_cast<void*>(source.attachment)||
            CharAttachment59c15(dst,destinationAttachmentName)!=reinterpret_cast<void*>(destination.attachment)||
            !Read(reinterpret_cast<void*>(source.attachment+0x28),a)||a!=source.object||
            !Read(reinterpret_cast<void*>(destination.attachment+0x28),b)||b!=destination.object)return false;
        char raw[260]{},path[260]{};
        if(!HeadSkinPath43(reinterpret_cast<void*>(destination.attachment),raw,sizeof(raw)))return false;
        gp50::normPath(raw,path,sizeof(path));return !std::strcmp(path,destinationSkin);
    }
    bool retain(std::uintptr_t m) noexcept {
        std::uintptr_t vt=0;if(!m||!Read(reinterpret_cast<void*>(m),vt)||vt!=WhBase()+0x40283F0)return false;
        __try{reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x6BCF90)(reinterpret_cast<void*>(m));return true;}
        __except(EXCEPTION_EXECUTE_HANDLER){g_materialFaultD54=true;return false;}
    }
    void release(std::uintptr_t m) noexcept {
        __try{reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x69636C)(reinterpret_cast<void*>(m));}
        __except(EXCEPTION_EXECUTE_HANDLER){g_materialFaultD54=true;}
    }
    bool set(unsigned lod,std::uintptr_t m) noexcept {
        __try{reinterpret_cast<void(__fastcall*)(void*,void*,unsigned)>(WhBase()+0x5AF4EC)(reinterpret_cast<void*>(destination.object),reinterpret_cast<void*>(m),lod);return true;}
        __except(EXCEPTION_EXECUTE_HANDLER){g_materialFaultD54=true;return false;}
    }
    bool equals(unsigned lod,std::uintptr_t m) noexcept {std::uintptr_t got=0;return Read(reinterpret_cast<void*>(destination.object+0x10+8*lod),got)&&got==m;}
};
static void ApplyGoreMaterialPartD54(const gp50::Event& e,int part,bool head) noexcept {
    void* victim=nullptr;void* sourceCharacter=nullptr;if(part<0||part>=e.nAtt||part>=24||!MaterialSourceD54(e,victim,sourceCharacter))return;
    void* sourceAttachment=MaterialSourcePartD54(sourceCharacter,e.attName[part],e.attSkin[part]);materialD54::Snapshot source{};
    if(!sourceAttachment||!MaterialSnapshotD54(sourceAttachment,source))return;
    char name[128]{};if(head)std::snprintf(name,sizeof(name),"kcd2vr_gore_d54_%016llx_head",(unsigned long long)e.materialTokenD54);
    else std::snprintf(name,sizeof(name),"kcd2vr_gore_d54_%016llx_part_%d",(unsigned long long)e.materialTokenD54,part+1);
    std::uint32_t id=0;void* entity=OwnMaterialEntityD54(name,id);
    const unsigned slot=head?1:0;void* character=entity?CharacterSlotD54(entity,slot):nullptr;
    void* attachment=character?OwnMaterialAttachmentD54(character,e.attSkin[part],head):nullptr;materialD54::Snapshot destination{};
    if(!entity||entity==victim||entity==g_last_player_entity.load(std::memory_order_acquire)||!attachment||!MaterialSnapshotD54(attachment,destination)){
        char log[256]{};std::snprintf(log,sizeof(log),"D54 GORE_MATERIAL target=%u part=%s destination=%s result=NO_EXACT_OWN_SKIN fallback=UNCHANGED",e.target,e.attName[part],name);diagnostic_trace::Write(log);return;}
    MaterialTransferIOD54 io{victim,sourceCharacter,entity,character,source,destination,slot};unsigned changed=0;
    io.event=&e;io.sourceName=e.attName[part];io.sourceSkin=e.attSkin[part];io.destinationName=name;io.destinationId=id;
    char destinationRaw[260]{};
    if(!AttachmentName(attachment,io.destinationAttachmentName,sizeof(io.destinationAttachmentName))||
        !HeadSkinPath43(attachment,destinationRaw,sizeof(destinationRaw)))return;
    gp50::normPath(destinationRaw,io.destinationSkin,sizeof(io.destinationSkin));
    const auto result=materialD54::transfer(io,source,destination,changed);unsigned overrides=0;
    for(unsigned i=0;i<materialD54::kLods;++i)if(source.replacement[i])++overrides;
    char log[384]{};std::snprintf(log,sizeof(log),"D54 GORE_MATERIAL target=%u part=%s helper=%u slot=%u result=%s lods=%u originalOverrides=%u source=LIVE_CA_SKIN_INSTANCE appearance=HEADSET_UNVERIFIED renderVisibility=UNPROVEN fault=%d",
        e.target,e.attName[part],id,slot,materialD54::resultName(result),changed,overrides,g_materialFaultD54?1:0);diagnostic_trace::Write(log);
}
static void ApplyGoreMaterialsD54(const gp50::Event& e) noexcept {
    if(!e.materialTokenD54||!GoreMaterialPinsD54())return;
    const int head=PrimaryMaterialPartD54(e);if(head>=0)ApplyGoreMaterialPartD54(e,head,true);
    for(int i=0;i<e.nAtt&&i<24&&!g_materialFaultD54;++i)if(i!=head)ApplyGoreMaterialPartD54(e,i,false);
}
