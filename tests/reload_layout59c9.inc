static bool ManualItemsRead596(void* player,ManualItems596& i) noexcept {
    if(!ManualPlayer596(player))return false;
    i.equipment=ManualEquipmentPtr59c9(player);
    if(!i.equipment||!Read(static_cast<unsigned char*>(i.equipment)+0x40,i.weapon)||!PhysVft6698(i.weapon,0x3A2DA88)||
       !Read(static_cast<unsigned char*>(i.equipment)+0x48,i.ammo)||!PhysVft6698(i.ammo,0x3A2DA88)||
       !Read(static_cast<unsigned char*>(i.weapon)+0x30,i.weaponKey)||PhysEmpty6698(i.weaponKey)||
       !Read(static_cast<unsigned char*>(i.ammo)+0x30,i.ammoKey)||PhysEmpty6698(i.ammoKey))return false;
    i.component=ManualComponent59c9(i.weapon);
    std::uint64_t nocked=0;if(!i.component||!Read(static_cast<unsigned char*>(i.component)+0xE0,nocked))return false;
    i.nockedKey=nocked;i.loaded=!PhysEmpty6698(nocked);i.nock=0;
    // 2A8A0C0 stores the ammo key on the logical component, then resolves a
    // DIFFERENT receiver via 2A89ED4. 2A8A030 stores its entity at visual+DC.
    // Missing visual is not an invented entity or an empty ammunition receipt.
    i.visual=ManualVisual59c9(i.component);
    return !i.visual||Read(static_cast<unsigned char*>(i.visual)+0xDC,i.nock);
}
