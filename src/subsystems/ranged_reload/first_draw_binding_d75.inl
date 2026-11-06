// D76 review: retain the strict non-ranged graph at both first native
// observation and final readback, including callbacks before metadata exists.
// All offsets/names already have exact-retail hand attachment consumers. This
// is a read-only current binding check, not a queued draw-completion receipt.
static bool PhysicalDrawBindingGraphD76(const PhysEquipmentItem6698& item,void* self,void* attachment,
        std::uint64_t epoch,std::uint64_t request) noexcept {
    const auto side=item.profile.side;
    if(!retail_build112::Ready(WhBase())||side<0||side>1||
       epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       !PhysVft6698(self,0x3A68F68)||!PhysVft6698(attachment,0x3A657C0)||
       !PhysVft6698(item.item,0x3A2DA88)||!PhysVft6698(item.worldLink54,0x3EC85E0))return false;
    void* object=nullptr;void* manager=nullptr;void* character=nullptr;const char* name=nullptr;
    void* link=nullptr;void* entity=nullptr;void* back=nullptr;std::uint64_t key=0;std::uint32_t id=0;
    if(!Read(static_cast<unsigned char*>(attachment)+0x20,object)||object!=self||
       !Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
       manager!=g_henry_char_attachment_manager.load(std::memory_order_acquire)||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||!PhysVft6698(character,0x40602F8)||
       reinterpret_cast<std::uintptr_t>(character)!=g_player_char37.load(std::memory_order_acquire)||
       !Read(static_cast<unsigned char*>(attachment)+0x10,name)||!name)return false;
    const char* expected=side?"hand_left":"hand_right";
    for(unsigned n=0;;++n){char value=0;if(!Read(name+n,value)||value!=expected[n])return false;if(!value)break;}
    if(!Read(static_cast<unsigned char*>(item.item)+0x30,key)||key!=item.profile.key||
       !Read(static_cast<unsigned char*>(item.item)+0x88,link)||link!=item.worldLink54||
       !Read(static_cast<unsigned char*>(link)+0x30,id)||id!=item.profile.entity||
       !Read(static_cast<unsigned char*>(link)+0x38,entity)||entity!=item.entity||
       !Read(static_cast<unsigned char*>(link)+0x58,back)||back!=item.item)return false;
    // Native replacement/transfer can reenter: qualify the stored graph again
    // before granting an original-visible hide. The normal placement is unchanged.
    void* after=nullptr;void* managerAfter=nullptr;void* characterAfter=nullptr;
    return Read(static_cast<unsigned char*>(attachment)+0x20,after)&&after==self&&
        Read(static_cast<unsigned char*>(attachment)+0x28,managerAfter)&&managerAfter==manager&&
        Read(static_cast<unsigned char*>(manager)+0x70,characterAfter)&&characterAfter==character;
}
static bool PhysicalDrawHandBindingD75(const PhysEquipment6698& equipment,
        const PhysEquipmentItem6698& item,void* self,void* attachment) noexcept {
    const auto side=item.profile.side;
    return item.held&&side>=0&&side<2&&equipment.handKnown[side]&&equipment.handKey[side]==item.profile.key&&
        PhysicalDrawBindingGraphD76(item,self,attachment,equipment.epoch,equipment.request);
}
