// R44C1: R42's alleged registry is RBP in CE3F40, whose prologue sets
// RBP = RSP - 0x13FA0. The registration nodes are stack temporaries, not a
// persistent float store. Matching five values cannot establish a write owner.
// No hook, polling, memory scan or RPG write is installed from this setting.
void SetPerfectBlockWindow178r42(int value) noexcept {
    if(value>=0)diagnostic_trace::Write("V66.178R44C1 PERFECT_BLOCK_WINDOW unavailable=UNPROVEN_PLAYER_WINDOW legacyAbsoluteValue=IGNORED native=UNCHANGED stackRegistryScan=RETIRED");
}
