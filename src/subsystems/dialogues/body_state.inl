// V66.178R9 DIALOGUE BODY: VRIK upper body/hands keep following the headset in dialogue
// (INI VrikInDialogue). Only the pose path is allowed; g_gameplay_allowed stays false so
// physical handling, attacks, fists, hand-use and haptics remain off in dialogue.
std::atomic<bool> g_dialogue_body178r9{false};
