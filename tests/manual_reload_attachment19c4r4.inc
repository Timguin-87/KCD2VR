static bool ManualBoundEntity19c4r4(void* attachment,std::uint32_t entity,void* expected) noexcept {
    if(!entity||!expected||EntityById6694(entity)!=expected||!PhysVft6698(expected,0x4072C78)||
       !PhysVft6698(attachment,0x3A657C0))return false;
    void* object=nullptr;std::uint32_t id=0;
    if(!Read(static_cast<unsigned char*>(attachment)+0x20,object)||!PhysVft6698(object,0x3A68F68)||
       !Read(static_cast<unsigned char*>(object)+8,id)||id!=entity)return false;
    void* after=nullptr;
    return Read(static_cast<unsigned char*>(attachment)+0x20,after)&&after==object&&
        Read(static_cast<unsigned char*>(object)+8,id)&&id==entity&&EntityById6694(entity)==expected;
}
static bool ManualHandAttached19c4r4(const ManualBow596& m) noexcept {
    if(!m.hand||m.side!=0||!ManualPlayer596(m.player)||
       HandClaimBlockedT24(0,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),m.epoch,m.request))return false;
    // Native virtual actor-model getter +1A81DD0 is exactly player+1B0.
    // 28959D8 -> 23D1C90 uses the ammo attachment-name table +40AC298,
    // distinct from ordinary carried item's +40AC2A8 table.
    void* model=nullptr;void* owner=nullptr;std::uint32_t hand=0;const char* name=nullptr;
    if(!Read(static_cast<unsigned char*>(m.player)+0x1B0,model)||!model||
       !Read(static_cast<unsigned char*>(model)+0x20,owner)||owner!=m.player||
       !Read(static_cast<unsigned char*>(model)+0x2C+0x24*m.side,hand)||hand!=m.hand||
       !Read(reinterpret_cast<void*>(WhBase()+0x40AC298+8*m.side),name)||!name)return false;
    void* attachment=ManualFindHandAttachment19c4r4(model,name);
    if(!ManualBoundEntity19c4r4(attachment,m.hand,m.entity))return false;
    void* modelAfter=nullptr;
    return Read(static_cast<unsigned char*>(m.player)+0x1B0,modelAfter)&&modelAfter==model&&
        Read(static_cast<unsigned char*>(model)+0x20,owner)&&owner==m.player&&
        Read(static_cast<unsigned char*>(model)+0x2C+0x24*m.side,hand)&&hand==m.hand;
}
static bool ManualNockAttached19c4r4(const ManualItems596& i) noexcept {
    if(!i.loaded||!i.nock||!i.component||!i.visual||i.nockedKey!=i.ammoKey||ManualVisual59c9(i.component)!=i.visual)return false;
    void* weapon=nullptr;std::uint64_t key=0;std::uint32_t id=0;
    if(!Read(static_cast<unsigned char*>(i.visual)+0x58,weapon)||weapon!=i.weapon||
       !Read(static_cast<unsigned char*>(i.component)+0xE0,key)||key!=i.nockedKey||
       !Read(static_cast<unsigned char*>(i.visual)+0xDC,id)||id!=i.nock)return false;
    void* entity=EntityById6694(i.nock);
    void* attachment=ManualFindNockAttachment19c4r4(i.visual);
    if(!ManualBoundEntity19c4r4(attachment,i.nock,entity))return false;
    return Read(static_cast<unsigned char*>(i.component)+0xE0,key)&&key==i.nockedKey&&
        Read(static_cast<unsigned char*>(i.visual)+0xDC,id)&&id==i.nock&&
        Read(static_cast<unsigned char*>(i.visual)+0x58,weapon)&&weapon==i.weapon&&ManualVisual59c9(i.component)==i.visual;
}

// C42: show ONLY the actual native-created right-hand ammunition attachment.
// No replacement entity, transform, item key or inventory write. The exact
// active manual owner rechecks visibility: native attachment refresh can hide
// the SAME arrow after the first create ACK. Never reveal an unrelated entity.
static bool ManualArrowVisible42(const ManualBow596& m,bool* restored54=nullptr,unsigned* hiddenLayers59=nullptr) noexcept {
    if(restored54)*restored54=false;
    if(hiddenLayers59)*hiddenLayers59=0;
    if(!ManualHandAttached19c4r4(m)||!BowAttachPins59c15())return false;
    void* model=nullptr;const char* name=nullptr;
    if(!Read(static_cast<unsigned char*>(m.player)+0x1B0,model)||!model||
       !Read(reinterpret_cast<void*>(WhBase()+0x40AC298),name)||!name)return false;
    void* a=ManualFindHandAttachment19c4r4(model,name);
    if(!ManualBoundEntity19c4r4(a,m.hand,m.entity))return false;
    bool hidden=false;if(!AttachmentHidden59c15(a,hidden))return false;
    const bool wasHidden54=hidden;
    if(hidden&&(!ManualHandAttached19c4r4(m)||!AttachmentHide59c15(a,false)))return false;
    // The attachment and its actual native-created CEntity have independent
    // visibility bits. Showing only the former does not prove the arrow draws.
    // Never reveal an unbound, replaced or foreign entity.
    bool entityHidden=false;
    if(!ManualHandAttached19c4r4(m)||!EntityHiddenFlag59c14(m.entity,entityHidden))return false;
    const bool wasEntityHidden59=entityHidden;
    if(hiddenLayers59)*hiddenLayers59=(wasHidden54?1u:0u)|(wasEntityHidden59?2u:0u);
    if(entityHidden&&(!ManualHandAttached19c4r4(m)||!EntityHide59c14(m.entity,false)))return false;
    const bool visible=ManualHandAttached19c4r4(m)&&AttachmentHidden59c15(a,hidden)&&!hidden&&
        EntityHiddenFlag59c14(m.entity,entityHidden)&&!entityHidden;
    if(visible&&restored54)*restored54=wasHidden54||wasEntityHidden59;
    return visible;
}
