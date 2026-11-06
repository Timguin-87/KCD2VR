// R94: preserve only a successfully announced recipe zone owned by this exact native prepare.
// Game-thread state. No native writes; a changed owner/slot/zone simply resumes the existing path.
struct NativeComboZoneOwnerR94 {
    NativeOwner104 owner{};
    NativeParryOpponent176 opponent{};
    std::uint64_t gesture=0,key=0,generation=0,request=0,body=0,source=0;
    int zone=-1;
};
static NativeComboZoneOwnerR94 g_combo_zone_owner_r94{};
static bool NativeComboZoneOwnerSameR94(const NativeOwner104& a,const NativeOwner104& b) noexcept {
    return a.player==b.player&&a.entity==b.entity&&a.actor==b.actor&&a.state==b.state&&a.soul==b.soul&&a.epoch==b.epoch;
}
static bool NativeComboZoneContextR94(const NativeComboZoneOwnerR94& z,const NativeOwner104& owner,
    const ph::Command& c) noexcept {
    const auto& p=g_prepared_input_r18;
    return kcd2vr::combo_zone_assist_cz::enabled.load(std::memory_order_relaxed)&&
        !c.kick59&&!c.controlledHoldH26&&!c.cutFeint62&&!c.feintFE&&z.gesture&&z.gesture==c.gesture59c3&&z.key==c.key&&z.generation==c.generation&&
        z.request==c.epoch&&z.body==c.bodyEpoch&&z.request==g_weapon_request_epoch6694.load()&&z.body==g_pose_epoch6689.load()&&
        NativeComboZoneOwnerSameR94(z.owner,owner)&&
        z.source&&z.source==g_input_source_generation_h14&&p.source==z.source&&p.gesture==z.gesture&&
        p.key==z.key&&p.generation==z.generation&&p.request==z.request&&
        NativeComboZoneOwnerSameR94(p.owner,owner);
}
static bool NativeComboZoneCurrentR94(const NativeComboZoneOwnerR94& z,const NativeOwner104& owner,
    const ph::Command& c) noexcept {
    if(!NativeComboZoneContextR94(z,owner,c))return false;
    NativeOwner104 again{};NativeParryOpponent176 opponent{};unsigned char slot=0;int zone=-1;
    if(!NativeResolve104(owner.player,again)||!NativeComboZoneOwnerSameR94(again,owner)||
       !z.opponent.id||!NativeParryOpponentRead176(again,opponent)||opponent.actor!=z.opponent.actor||
       opponent.state!=z.opponent.state||opponent.entity!=z.opponent.entity||opponent.id!=z.opponent.id||
       !Read(static_cast<unsigned char*>(owner.state)+0x868,slot)||slot!=1||
       !Read(static_cast<unsigned char*>(owner.state)+0xF8,zone)||zone!=z.zone)return false;
    return NativeComboZoneContextR94(z,again,c);
}
static void NativeRememberComboZoneR94(const NativeOwner104& owner,const ph::Command& c,int zone,const NativeParryOpponent176& opponent) noexcept {
    g_combo_zone_owner_r94={};
    if(zone!=0&&zone!=1&&zone!=2&&zone!=5)return;
    NativeComboZoneOwnerR94 z{owner,opponent,c.gesture59c3,c.key,c.generation,c.epoch,c.bodyEpoch,g_input_source_generation_h14,zone};
    if(NativeComboZoneCurrentR94(z,owner,c))g_combo_zone_owner_r94=z;
}
static bool NativePreserveComboZoneR94(const NativeOwner104& owner,const ph::Command& c) noexcept {
    if(!g_phys_native6698.attackPrepared||g_phys_native6698.prepareGestureC10R18!=c.gesture59c3||
       !NativeComboZoneCurrentR94(g_combo_zone_owner_r94,owner,c)){
        g_combo_zone_owner_r94={};return false;
    }
    return true;
}
