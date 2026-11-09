#pragma once
// R123 weapon grip (Miki 6 Nov, R118 run 13:45-: "the native grip position is slipping away - the sword was then gripped
// badly - and it was rotated forward"). The melee grip was learned from 6 steady frames of Henry's ANIMATED hand. KCD2
// clips turn the weapon bone in the fist all the time (log: only 47 of 565 HAND_F10 ITEM rows of the longsword had the
// native item-to-wrist relation near its authored rest, most were 20-40 or 70-90 deg away), so the learn froze one clip
// pose: applied grip 93 deg / 4.9 cm off the authored socket, blade tilted 46 deg along the hand, equal to the clip pose
// at 14:03:00.553. ON (default): a melee primary hand uses the authored wrist socket relation (rest skeleton wrist +
// the weapon's own hand socket, the same reference the shield uses since B4) at once, no learning. OFF: the old learn.
// Crossbow: a 6-frame learn that settles ON the rest relation is the bind frame before the draw clip moved the crossbow
// (log: draws alternate between that and the real hold 12 cm away, D72), so it is skipped for up to kRestSkipCap samples.
#include <atomic>

namespace kcd2vr::melee_grip_r123 {
inline std::atomic<bool> enabled{true};
inline constexpr const char* kKey="VrMeleeAuthoredGripR123";
// melee weapons except shields (B4 reference already) and polearms (R26 slt_0 helper)
constexpr bool eligible(int classId,bool melee,bool shield) noexcept {return melee&&!shield&&classId!=7;}
inline constexpr double kRestPosM=.03,kRestAngleRad=.2618; // 3 cm / 15 deg
inline constexpr unsigned kRestSkipCap=45;                 // ~0.5 s at 90 Hz, then the learn takes what it sees
constexpr bool restFrame(double posM,double angleRad,unsigned skipped) noexcept {
    return posM<kRestPosM&&angleRad<kRestAngleRad&&skipped<kRestSkipCap;
}
// R140 (Miki 7 Nov: "bow or crossbow come out in wrong orientation ... it works all the time by the first pull").
// the bow draw starts on the same bind/rest relation as the crossbow (physical_native.inl, the D72 note at the
// one-sample shortcut: "Bow draw can initially match the authored wrist while the attachment is still moving"), so
// the rest skip covers the bow (class 9) too. a learn that still lands on the rest relation (the 45-sample cap ran
// out, e.g. a slow first draw after a load) is provisional: for kRestWindow more samples a settled hold that is NOT
// on the rest relation replaces it. per draw, cleared on stow / new item generation, no latch.
constexpr bool rangedRest(int classId) noexcept {return classId==9||classId==10||classId==14||classId==15;}
constexpr bool restRelation(double posM,double angleRad) noexcept {return posM<kRestPosM&&angleRad<kRestAngleRad;}
inline constexpr unsigned kRestWindow=45; // ~0.5 s at 90 Hz after a rest-relation learn
constexpr bool skipRest(double posM,double angleRad,unsigned skipped,bool provisional) noexcept {
    return restRelation(posM,angleRad)&&(provisional||skipped<kRestSkipCap);
}
}
