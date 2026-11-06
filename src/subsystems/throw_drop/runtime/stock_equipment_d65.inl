// D65: unreadable equipment is not equivalent to an equipped key.
enum class AmmoEquipmentD65C : unsigned char { Unknown,Clear,Equipped };
static AmmoEquipmentD65C AmmoEquipmentStateD65C(void* manager,std::uint64_t key) noexcept {
    std::uint64_t slots[7]{};
    if(!manager||!key||!SehCopy(static_cast<unsigned char*>(manager)+0x48,slots,sizeof(slots)))return AmmoEquipmentD65C::Unknown;
    for(auto value:slots)if(value==key)return AmmoEquipmentD65C::Equipped;
    return AmmoEquipmentD65C::Clear;
}
static bool AmmoStockPermittedD65C(AmmoEquipmentD65C equipment,unsigned flags,int count,int family,paid20::StockRole role) noexcept {
    if(equipment==AmmoEquipmentD65C::Unknown||(flags&2u)||count<1)return false;
    if(equipment==AmmoEquipmentD65C::Clear)return !(flags&1u);
    // Only verified legacy dagger stock gets the equipped-stack exception.
    // Axes/recovered Ammo keep the old clear-equipment rule. Native cost/ACK unchanged.
    return family==0&&role==paid20::StockRole::LegacyMelee&&count>=2;
}
