// Separate from Lua reservation bookkeeping. A CLEARED reply cannot discharge
// a real native inventory mutation. Process-retained only: NOT a save journal.
enum class AmmoEscrowPhaseT19 { Empty, MoveUnknown, Unconverted, ConversionUnknown, ReturnUnknown };
struct AmmoEscrowReceiptT19 {
    st2::Input owner{};
    std::uint64_t serial=0,key=0;
    void* item=nullptr;void* definition=nullptr;void* source=nullptr;void* world=nullptr;
    int side=-1,kind=-1;
    AmmoEscrowPhaseT19 phase=AmmoEscrowPhaseT19::Empty;
};
static AmmoEscrowReceiptT19 g_ammo_escrow19[16];
