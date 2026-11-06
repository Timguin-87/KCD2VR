// C45 MINIGAME HANDS: the pure gate (src/vrik/minigame_hands45.h) and its place in workstation178r20c1::ownsActivityWrist45.
#include "vrik/workstation178r20c1.h"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace mh = kcd2vr::minigame_hands45;
namespace ws = kcd2vr::workstation178r20c1;
static bool is(const char* a, const char* b) { return a && b && std::strcmp(a, b) == 0; }
int main() {
    unsigned checks = 0; const auto ck = [&](bool ok) { ++checks; assert(ok); };
    // defaults: SEMI-PASSENGER, no witness
    ck(mh::mode.load() == mh::SemiPassenger && mh::witness.load() == 0);
    ck(mh::clampMode(1) == 1 && mh::clampMode(0) == 0 && mh::clampMode(7) == 0 && mh::clampMode(-1) == 0);
    // pack / unpack
    ck(mh::pack(mh::None, 1000) == 0 && mh::pack(mh::Alchemy, 0) == 0);
    ck(mh::kindOf(mh::pack(mh::Alchemy, 1000)) == mh::Alchemy && mh::tickOf(mh::pack(mh::Alchemy, 1000)) == 1000);
    ck(mh::kindOf(mh::pack(mh::Smithing, 123456789ull)) == mh::Smithing && mh::tickOf(mh::pack(mh::Smithing, 123456789ull)) == 123456789ull);
    // the gate: every refusal in order, then open
    const auto a = mh::pack(mh::Alchemy, 1000), s = mh::pack(mh::Smithing, 1000);
    ck(is(mh::reason(mh::Native, true, s, 1000), "MINIGAME_HANDS_NATIVE"));
    ck(is(mh::reason(mh::SemiPassenger, false, s, 1000), "WORKSTATION_INI_DISABLED"));
    ck(is(mh::reason(mh::SemiPassenger, true, 0, 1000), "NO_MINIGAME_WITNESS"));
    ck(is(mh::reason(mh::SemiPassenger, true, mh::pack(3u, 1000), 1000), "NO_MINIGAME_WITNESS")); // kind 3 is not a minigame
    ck(is(mh::reason(mh::SemiPassenger, true, s, 1081), "MINIGAME_WITNESS_STALE"));
    ck(is(mh::reason(mh::SemiPassenger, true, s, 999), "MINIGAME_WITNESS_STALE"));  // witness from the future
    ck(is(mh::reason(mh::SemiPassenger, true, s, 0), "MINIGAME_WITNESS_STALE"));
    ck(mh::reason(mh::SemiPassenger, true, s, 1000) == nullptr && mh::reason(mh::SemiPassenger, true, s, 1080) == nullptr);
    ck(mh::reason(mh::SemiPassenger, true, a, 1040) == nullptr);
    ck(is(mh::label(0), "NATIVE") && is(mh::label(1), "SEMI_PASSENGER") && is(mh::witnessName(2), "SMITHING") && is(mh::witnessName(0), "NONE"));
    // ownsActivityWrist45: the setting alone opens the SMITHING / ALCHEMY wrist, never ownsWrist (F5 / F10 routes)
    kcd2vr::combat_rules178r10::workstationEnabled = true;
    ck(!ws::ownsWrist(1000) && !ws::semiPassengerOwns45(1000) && !ws::ownsActivityWrist45(1000));
    mh::publish(mh::Smithing, 1000);
    ck(!ws::ownsWrist(1000) && ws::semiPassengerOwns45(1000) && ws::ownsActivityWrist45(1000));
    ck(!ws::ownsActivityWrist45(1081));                         // 80 ms freshness
    mh::mode = mh::Native; ck(!ws::ownsActivityWrist45(1000));  // NATIVE: closed
    mh::mode = mh::SemiPassenger; ck(ws::ownsActivityWrist45(1000));
    kcd2vr::combat_rules178r10::workstationEnabled = false; ck(!ws::ownsActivityWrist45(1000)); // VrWorkstation INI off wins
    kcd2vr::combat_rules178r10::workstationEnabled = true;
    mh::publish(mh::None, 1000); ck(!ws::ownsActivityWrist45(1000)); // witness withdrawn (minigame left / controls not native)
    // the F5 route is unchanged: rung 2 + armed opens ownsWrist with or without the setting
    mh::mode = mh::Native; kcd2vr::combat_rules178r10::f5Rung = 2; ws::setExperiment36(true);
    ck(ws::ownsWrist(1000) && ws::ownsActivityWrist45(1000));
    ws::setExperiment36(false); ck(!ws::ownsWrist(1000) && !ws::ownsActivityWrist45(1000));
    // C63: exact current Alchemy passenger retains native torso/head. Only
    // armed F5=2 is the full-controller test; F5=3 is gaze-guided native work.
    mh::mode=mh::SemiPassenger;mh::publish(mh::Alchemy,2000);
    kcd2vr::combat_rules178r10::f5Rung=0;
    ck(ws::passengerAlchemy63(2000)&&!ws::fullVrAlchemy63(2000));
    kcd2vr::combat_rules178r10::f5Rung=2;ws::setExperiment36(true);
    ck(ws::fullVrAlchemy63(2000)&&!ws::passengerAlchemy63(2000)&&!ws::semiPassengerOwns45(2000));
    ck(ws::ownsActivityWrist45(2000));
    kcd2vr::combat_rules178r10::f5Rung=3;ws::setExperiment36(true);
    ck(ws::passengerAlchemy63(2000)&&!ws::fullVrAlchemy63(2000)&&ws::semiPassengerOwns45(2000));
    ck(!ws::passengerAlchemy63(2081)&&!ws::fullVrAlchemy63(2081));
    mh::publish(mh::Smithing,2000);ck(!ws::passengerAlchemy63(2000)&&!ws::fullVrAlchemy63(2000));
    std::printf("minigame_hands45: %u checks PASS\n", checks);
}
