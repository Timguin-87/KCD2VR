// DS-F62: gated grab executor implementation. Inert until Register() passes
// every gate; the drain's EXECUTOR_ABSENT drops keep it fail-closed.
namespace kcd2vr::physics::grab_executor60 {
namespace leaf=constraint_leaf60;
namespace pkt=constraint_packet60;
namespace hp=hand_proxy60;
namespace co=constraint_op60;
namespace hpt4=kcd2vr::native_hand_proxy_t4;
static std::atomic<bool> g_registered60{false};
bool Ready() noexcept {
    const auto base=hp::Base();
    return base&&retail_build112::Ready(base)&&leaf::GetterPinned(base)&&hp::ClientRegistered();
}
bool Exec(const co::Op& op) noexcept {
    if(!Ready())return false;
    const auto base=hp::Base();
    if(op.kind==co::Kind::Create){
        auto* body=reinterpret_cast<void*>(op.key.sourceBody);
        if(!body)return false;
        // DS-F62: the buddy handle must be FRESH from the proxy's achieved
        // record (valid, Active, fresh tick); the lease lives in the owner.
        hpt4::Achieved a{};
        const int side=hp::SideForBuddy(op.key.buddy);
        if(side<0||side>1||!hp::ReadAchieved(side,a)||!a.valid||a.source.physicalPresenceH24||!a.bodyHandle)return false;
        if(!a.tick||std::uint64_t(GetTickCount64())<a.tick||std::uint64_t(GetTickCount64())-a.tick>120)return false; // step cadence
        auto p=pkt::defaults();
        pkt::setBuddy(p,a.bodyHandle);
        // DS-F65/F67 (Claude review 15, Codex F59): 0x300 is inactive+ignore-
        // pair = a silent no-op. The grab hypothesis is 0x200 (ignore buddy
        // collisions, ACTIVE, local frames, identity qframes, pt zero) -
        // UNTESTED, validate against the measured follow. setFlags fails
        // closed on the inactive bit.
        if(!pkt::setFlags(p,0x200))return false;
        const auto res=leaf::ActionAddConstraint(base,body,p);
        return res.submitted()&&res.returnedId>0; // the positive handle feeds the typed-Present ACK
    }
    if(op.kind==co::Kind::Remove){
        auto* body=reinterpret_cast<void*>(op.key.sourceBody);
        if(!body)return false;
        auto p=pkt::removeDefaults();
        pkt::setRemoveId(p,op.key.id);
        const auto res=leaf::ActionRemoveConstraint(base,body,p);
        return res.submitted(); // <=0 native status = false (the machine retries/quarantines)
    }
    return false; // Drive/Cleanup have no native call yet
}
void Register() noexcept {
    if(g_registered60.exchange(true,std::memory_order_acq_rel))return;
    // DS-F62: the registration gate stays closed until the source-connected
    // fixture passes review; this body is the prepared seam only.
}
void Unregister() noexcept {g_registered60.store(false,std::memory_order_release);}
}
