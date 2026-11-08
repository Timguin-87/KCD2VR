# Diagnostics

recording is optional. gameplay never depends on a recorder key, and nothing you set for playing needs a recorder to
be running.

the recorder keys work only with **Developer mode** ticked in the KCD2VR Setup window (`VrDeveloperMode`, read once
at game start). it is the mod's own setting, the game's -devmode start option is not needed. without it the recorder
keys do nothing, F7, F6, F1, F10 and Shift+F8 always work.

## keys

| key | what |
| --- | --- |
| F2 | rendering / bugs recorder: off -> bugs -> bugs + timing comparison -> bugs + elbow/shoulder comparison -> off. includes sword, bow and crossbow draw lifecycle and placement. |
| Shift+F2 | performance + motion trace, without the heavy interaction recording. |
| F3 | combat recorder on/off: requests, native admission, reaction/paired action identity and results. it does not change the combat mode. |
| F5 | interactions and minigames: off -> interaction recording -> physical Grip/spatial door comparison -> guided crafting/legacy door comparison -> off. target, action, attachment, return, transfer, storage haptic, smith fall/LT, alchemy rate, dice state, physical reload ownership. |
| Shift+F7 | VRIK recorder: off -> baseline -> fresh publication comparison -> elbow preference comparison -> off. raw goal, final wrist, contact identity/normal, freshness, release correction, hand references. |
| F7 | opens/closes the VR menu (not a recorder). |
| F6 | switches Theater (flat screen) <-> Immersive. stays until the next F6 or the next native menu/scene change. an L3+R3 click only switches back to Immersive. |
| F9 | physics recorder: corpse service, contact/raw ray and native-held stage. it does not change the physics mode. |
| Shift+F9 | read-only upstream detail ladder under the same physics recording. |
| F11 | reload recorder: off -> reload recording -> staged reload comparison -> off. string/tool/bolt identity, grasp, native cock/load state. |
| F1 | body follow on/off (BODY only). |
| F10 | hand mode. Shift+F10 / Ctrl+F10 are developer keys. |
| Shift+F8 | calibration (T-pose). |

each recorder runs 180 s and then stops by itself. keys can run together. stopping a key stops new rows, rows already
captured can still drain for up to 5 s under the same session. map profiling needs F2 or Shift+F2, opening the map
alone doesn't start it.

## screen messages

short mod status lines in the headset (a kick, crossbow or door refusal and the like) show ONLY while F2 records.
the other recorders write the same reason to KCD2VR.log ("VR status: ...") but never on screen. with F2 off nothing is
shown. the gore pak's own messages are separate.

## one log

everything the mod writes goes into KCD2VR.log: startup, companion loading, map performance, finger references, all the
F-key recordings. no extra files: no finger caches, no DetourModKit_Log, no perf/startup/diagnostics side logs, no .bak
of the ini. the last-good INI is kept in memory. capture toggles never open, truncate or close another file. game logs
and other loaders' logs are not the mod's.

with recording off a few lifecycle lines still go to the log (startup, build marker, OpenXR session state, the first
head pose after start or recenter, stereo/upscaler faults). they are short and never on screen.

separate game starts can append to the same KCD2VR.log, the `C32 PROCESS pid=` line marks each start.

## what a good recording looks like

- one key for one action: F3 + Shift+F7 for a dodge, F5 + Shift+F7 or F11 for a reload, F9 for a corpse. 180 s is
  plenty. say the time when it went wrong.
- keep the DLL build marker (first lines of the log), the INI and the companion with the recording. the folder name is
  not the build that made the log.
- the INI is saved on exit. it does not tell which options were active during an earlier action.
- recording costs something, especially while contacts change all the time. use it to find one transition, not as a
  permanent setting.

## reading some of the rows

F3 combat zones: every `H29 STRIKE_RESULT` is followed by one `D85 F3 ZONE` row with `origin` (where the swing
started), `sent` (the zone our native release carried), `requested` (+0xF0), `committed` (+0x180), `executed` (the zone
the native attack row really ran, Henry frame = symmetric(+0x150)), `rowZone` (+0x150 as stored), `inputClass`
(+0x270) and a verdict. the native combo matcher compares the executed zone with the recipe GuardZone, so this row
tells you directly why a combo step did or did not count. COMBAT_STATE rows show `inputClass=` and `executed=` with
the same meaning.

F3 `R3_NATIVE` (kicks) also writes `opponent`, `combatObj`, `combatObjVt`, `combatObjVf18`, `route` and
`cooldownLeftMs`. combatObj is actor+450, the thing the game asks (1315CF8) before it picks combat or free kick rows
without an opponent.

Shift+F7 `FRESH_POSE_UNCAPPED` (Fresh Pose Uncapped mode): native frame, camera source, result/reason, publication
status, locate duration, sample sequence and source clocks, target and valid/tracked masks. `waitNs=0` means there is
no token wait, not that the OpenXR lookup is free. a successful publication can still have partial tracking, look at
the masks.

F5 / Shift+F7 / F11 `RELOAD_NATIVE_DECISION`: clock refusal, native layer/scope, interval/event from/to/permitted
values before restoration. 32 edges per domain per session.

things to keep in mind:
- a row missing can mean a dropped or out-of-window observation, not that the native callback never ran. queue
  overflow and exhausted budgets are reported.
- CPU scopes can be nested. don't add inclusive timings together and don't call them GPU time.
- "READY", "hook installed", "script returned", "SetVelocity returned" are narrower than a moved body, a grabbed item,
  a landed hit or a felt haptic pulse.
- F5 samples the dice action map/cursor only, it does not move the cup. F9 provider READY is not a successful grab.
