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
