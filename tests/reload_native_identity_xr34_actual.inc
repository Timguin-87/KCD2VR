static bool NativeHandKey6697(void* manager,int side,std::uint64_t& key) noexcept {
    key=0;if(!manager||side<0||side>1)return false;
    const auto base=WhBase();void* player=nullptr;void* slots=nullptr;void* back=nullptr;
    std::uintptr_t vf=0;
    if(!base||!Read(manager,vf)||vf!=base+0x3EC9958ull||
       !Read(static_cast<unsigned char*>(manager)+0x38,player)||!player||
       !Read(player,vf)||vf!=base+0x3AA4B98ull||
       !Read(static_cast<unsigned char*>(player)+0x9D0,back)||back!=manager||
       !Read(static_cast<unsigned char*>(player)+0x990,slots)||!slots||
       !Read(static_cast<unsigned char*>(slots)+0x260+side*8,key))return false;
    // Verify the small chain after the read; changing equipment ownership is
    // not permission to keep a previously cached hand transform.
    void* again=nullptr;std::uint64_t keyAgain=0;
    return Read(static_cast<unsigned char*>(manager)+0x38,again)&&again==player&&
        Read(static_cast<unsigned char*>(player)+0x990,again)&&again==slots&&
        Read(static_cast<unsigned char*>(slots)+0x260+side*8,keyAgain)&&keyAgain==key;
}

static bool PhysReadItemWorld54(void* item,void*& link,std::uint32_t& id) noexcept {
    link=nullptr;id=0;
    return item&&Read(static_cast<unsigned char*>(item)+0x88,link)&&
        (!link||Read(static_cast<unsigned char*>(link)+0x30,id));
}

static bool PhysCachedItemMatches54(const PhysEquipmentItem6698& x,std::uint64_t key) noexcept {
    std::uint64_t actual=0;void* link=nullptr;std::uint32_t id=0;
    return key&&x.profile.key==key&&PhysVft6698(x.item,0x3A2DA88)&&
        Read(static_cast<unsigned char*>(x.item)+0x30,actual)&&actual==key&&
        PhysReadItemWorld54(x.item,link,id)&&link==x.worldLink54&&id==x.profile.entity&&
        (!id||x.entity); // retry a present entity ID whose model was not ready yet
}

struct PhysLive103 {
    void* manager=nullptr;void* player=nullptr;
    std::uint64_t request=0,epoch=0,hand[2]{},slots[6]{},extra45=0;int selected=-1;
};
static bool PhysReadLive103(PhysLive103& live) noexcept {
    live={};live.request=g_weapon_request_epoch6694.load();live.epoch=g_pose_epoch6689.load();
    live.manager=g_henry_item_attach_manager.load();
    if(!PhysVft6698(live.manager,0x3EC9958)||!Read(static_cast<unsigned char*>(live.manager)+0x38,live.player)||!PhysVft6698(live.player,0x3AA4B98))return false;
    void* back=nullptr;
    if(!Read(static_cast<unsigned char*>(live.player)+0x9D0,back)||back!=live.manager||
       !Read(static_cast<unsigned char*>(live.manager)+0x40,live.selected)||live.selected<0||live.selected>2||
       !Read(static_cast<unsigned char*>(live.manager)+0x48,live.slots)||
       !Read(static_cast<unsigned char*>(live.manager)+0x78,live.extra45))return false;
    for(int side=0;side<2;++side){if(!NativeHandKey6697(live.manager,side,live.hand[side]))return false;if(PhysEmpty6698(live.hand[side]))live.hand[side]=0;}
    // If equipment changes halfway through the read, wait for the next callback.
    std::uint64_t after[6]{},extra45=0;int selected=-1;
    if(!Read(static_cast<unsigned char*>(live.manager)+0x48,after)||std::memcmp(after,live.slots,sizeof(after))||
       !Read(static_cast<unsigned char*>(live.manager)+0x40,selected)||selected!=live.selected||
       !Read(static_cast<unsigned char*>(live.manager)+0x78,extra45)||extra45!=live.extra45)return false;
    return live.manager==g_henry_item_attach_manager.load()&&live.request==g_weapon_request_epoch6694.load()&&live.epoch==g_pose_epoch6689.load();
}
