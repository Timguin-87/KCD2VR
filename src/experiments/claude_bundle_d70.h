#pragma once
#include <atomic>
#include <cstdint>
namespace kcd2vr::claude_d70 {
// D77 separates only the three existing D70 effects. Atomic mask preserves a
// coherent legacy all-on/all-off load; no combat algorithm or tuning changes.
inline std::atomic<unsigned> combatMaskD77{0};
inline std::atomic<bool> hudWalkoff{false};
inline std::atomic<std::uint64_t> combatRevision{1};
inline void SetMaskD77(unsigned mask) noexcept {mask&=7u;if(combatMaskD77.exchange(mask,std::memory_order_acq_rel)!=mask)combatRevision.fetch_add(1,std::memory_order_acq_rel);}
inline bool StabFix() noexcept {return (combatMaskD77.load(std::memory_order_acquire)&1u)!=0;}
inline bool UnarmedFix() noexcept {return (combatMaskD77.load(std::memory_order_acquire)&2u)!=0;}
inline bool MasterstrikeFix() noexcept {return (combatMaskD77.load(std::memory_order_acquire)&4u)!=0;}
inline void SetD77(bool stab,bool unarmed,bool masterstrike) noexcept {SetMaskD77((stab?1u:0u)|(unarmed?2u:0u)|(masterstrike?4u:0u));}
// Legacy API is retained for baseline fixtures; runtime effect sites use their
// individual getter. It must never become a hidden fourth menu master switch.
inline bool CombatFix() noexcept {return combatMaskD77.load(std::memory_order_acquire)==7u;}
inline void SetCombatFix(bool on) noexcept {SetMaskD77(on?7u:0u);}
// No installed INI rewrite. A missing new key inherits the exact saved legacy
// value; explicit per-effect OFF wins over an old ON. New saves write only the
// three independent keys, preserving unrelated/legacy user file content.
template<class Read> inline void LoadD77(Read read) {
 const bool legacy=read("VrStabUnarmedMasterstrikeD70",true) /* R124: missing = ON (Miki's INI has the three D77 keys ON) */;
 SetD77(read("VrStabFollowThroughD77",legacy),read("VrUnarmedClinchD77",legacy),read("VrMasterstrikeFamiliesD77",legacy));
}
template<class Write> inline bool SaveD77(Write write) {
 const auto mask=combatMaskD77.load(std::memory_order_acquire);bool ok=true;
 ok=write("VrStabFollowThroughD77",(mask&1u)!=0)&&ok;
 ok=write("VrUnarmedClinchD77",(mask&2u)!=0)&&ok;
 ok=write("VrMasterstrikeFamiliesD77",(mask&4u)!=0)&&ok;return ok;
}
inline bool HudWalkoff() noexcept {return hudWalkoff.load(std::memory_order_acquire);}
inline void SetHudWalkoff(bool on) noexcept {hudWalkoff.store(on,std::memory_order_release);}
// Preserve a committed thrust only while both independently sampled tips still
// advance. Age is a cap, never authority for a contact on withdrawal.
inline bool ForwardThrust(std::uint64_t now,std::uint64_t committed,int kind,
                          double modelAxial,double trackedAxial,double modelForward,double trackedForward) noexcept {
 return StabFix()&&kind==1&&committed&&now>=committed&&now-committed<=250&&
        modelAxial>0&&trackedAxial>0&&modelForward>0&&trackedForward>0;
}
}
