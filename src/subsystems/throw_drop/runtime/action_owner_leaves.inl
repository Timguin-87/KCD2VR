weapon_release_t1::Result DropWeaponT1(const weapon_release_t1::Request& r) noexcept {
    if(!OnOwnerThread13())return QueueWeaponDropT9(r);
    OwnerTransaction19c4 transaction;if(!transaction)return {};
    return DropWeaponImplT1(r);
}
void TickWeaponDropT1(bool allowed,std::uintptr_t player,std::uint64_t bodyEpoch,std::uint64_t requestEpoch) noexcept {
    (void)g_drop_context_t9.publish({player,bodyEpoch,requestEpoch,GetTickCount64(),allowed});
    if(!OnOwnerThread13())return;
    OwnerTransaction19c4 transaction;if(transaction)TickWeaponDropImplT1(allowed,player,bodyEpoch,requestEpoch);
}
bool ReadCommittedDropT2(weapon_release_t1::CommittedT2& out) noexcept {return g_committed_drop_t2.read(out)&&out.serial;}
// D85R5 restored the finite-inventory knife service at the left hip. This
// setter is its actual acquisition toggle; shared bow/bolt/feedback services
// continue to drain their own existing leases independently of this setting.
void SetThrowingSupplyT2(bool on) noexcept {supply_t2::enabled.store(on,std::memory_order_release);}
bool ThrowingSupplyT2() noexcept {return supply_t2::enabled.load(std::memory_order_acquire);}
bool NativeDrawEntryQualified62(std::uintptr_t base) noexcept {return SupplyDrawEntryQualified62(base);}
void PublishThrowingSupplyT2(const supply_t2::Input& in) noexcept {(void)g_supply_input_t2.publish(in);}
bool ReadThrowingSupplyT2(supply_t2::Lease& out) noexcept {return g_supply_lease_t2.read(out)&&out.held&&supply_t2::fresh(GetTickCount64(),out.tick,150);}
