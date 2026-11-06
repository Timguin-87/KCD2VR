#!/usr/bin/env python3
"""R89 dice: zoom stays after the dice game + frozen dice image after a zoom edit.

Part A (C++, real header src/activity_lens59c10r1.h): the activity lens exit hold.
  Frame flow from D85R5 KCD2VR1 23:50-23:52: dice running with the game's Minigame
  camera (manager+0x338, vft 408EB60, native 0.6 rad = 34.377 deg), then the dice
  witness clears while the game still shows that camera. The hold must write the
  gameplay FOV until the first non-Minigame camera, then never again.
Part B (source, src/mod.cpp + host.inl + device_hook.cpp): XR pair mode 2 is only
  set by a VR menu edit while a stereo run is live, Run drops a stale mode 2, and a
  stopped run's last pair is unpublished.
Negative controls mutate the source to the old behaviour; each must fail.
usage: python3 tools/test_dice_exit_r89.py [repo root]
"""
from __future__ import annotations
import re, subprocess, sys, tempfile, shutil
from pathlib import Path

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else Path(__file__).resolve().parents[1])

FIXTURE = r'''
#include "activity_lens59c10r1.h"
#include <cstdio>
#include <map>
#include <type_traits>
using namespace kcd2vr::activity_lens59c10r1;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL line %d: %s\n",__LINE__,#c);++fails;}}while(0)
int main(){
    constexpr std::uintptr_t b=0x180000000,actor=0x10000,player=0x20000,dice=0x30000,game=0x40000,manager=0x50000;
    std::map<std::uintptr_t,std::uint64_t> mem;
    auto read=[&](std::uintptr_t a,auto& v){auto i=mem.find(a);if(i==mem.end())return false;
        v=static_cast<std::remove_reference_t<decltype(v)>>(i->second);return true;};
    mem[player+0x40]=b+0x3A81028;mem[actor]=b+0x3A80610;mem[actor+0x1F8]=player;
    mem[player]=b+0x3AA4B98;mem[player+0x278]=actor;mem[player+0x38]=0x60000;
    mem[dice]=b+0x3F72098;mem[dice+0x18]=player;mem[dice+0x2C]=1;
    mem[b+0x549D388]=game;mem[game+0x38]=manager;mem[manager]=b+0x408E560;
    mem[manager+0x38]=0;mem[manager+0x3B]=0;
    mem[manager+0x1E8]=b+0x3A5A0F8;mem[manager+0x338]=b+0x408EB60;mem[manager+0x350]=b+0x3A5A3D0;
    const std::array<std::uintptr_t,4> none{};
    ExitHoldR89 h{};
    // One View() frame as activity_lens59c10r1.cpp runs it.
    std::uint64_t lifetime=10;
    auto frame=[&](bool witness,bool wanted){
        const auto e=witness?qualify(b,player+0x40,none,read,0,dice):Evidence{};
        const bool exiting=!witness||e.refusal==Refusal::ActivityInactive;
        // superset of View(): View reads the camera only while armed
        bool other=false;const Owner cam=exiting?playerCamera(b,read,&other):Owner::None;
        return exitHoldStep(h,DiceExitFrameR91{true,wanted,witness?dice:0,player,lifetime,
            witness?32u:0u,e,cam,true,true,other});};
    // playerCamera: the same chain qualify() checks, without an activity.
    mem[manager+0x30]=manager+0x338;CHECK(playerCamera(b,read)==Owner::Minigame);
    mem[manager+0x30]=manager+0x1E8;CHECK(playerCamera(b,read)==Owner::FirstPerson);
    mem[manager+0x30]=manager+0x350;CHECK(playerCamera(b,read)==Owner::None);
    mem[manager+0x30]=manager+0x338;mem[manager+0x38]=1;CHECK(playerCamera(b,read)==Owner::None);mem[manager+0x38]=0;
    mem[manager]=b+0x408E561;CHECK(playerCamera(b,read)==Owner::None);mem[manager]=b+0x408E560;
    mem[manager+0x338]=b+0x408EB61;CHECK(playerCamera(b,read)==Owner::None);mem[manager+0x338]=b+0x408EB60;
    CHECK(playerCamera(0,read)==Owner::None);
    // 1) never in an activity: a Minigame camera alone is not ours (lockpick etc.)
    CHECK(!frame(false,true));CHECK(!h.armed);
    // 2) dice running, Minigame camera: the normal lens path owns the frame
    mem[manager+0x30]=manager+0x338;
    CHECK(!frame(true,true));CHECK(h.armed);
    // 3) dice stopping (+0x2C running=0, witness still set), camera still Minigame: hold
    mem[dice+0x2C]=0;CHECK(frame(true,true));mem[dice+0x2C]=1;
    // 4) witness cleared, game keeps the dice camera = the logged "zoom stays": hold
    ++lifetime;for(int i=0;i<300;++i)CHECK(frame(false,true));
    // 5) immersive view off ends authority; it cannot resume from an old dice token
    CHECK(!frame(false,false));CHECK(!h.armed);CHECK(!frame(false,true));
    ++lifetime;CHECK(!frame(true,true));++lifetime;CHECK(frame(false,true));
    // 6) first FirstPerson frame: native FOV is back, hold ends for good
    mem[manager+0x30]=manager+0x1E8;CHECK(!frame(false,true));CHECK(!h.armed);
    mem[manager+0x30]=manager+0x338;CHECK(!frame(false,true));CHECK(!frame(false,true));
    // 7) next dice round, then a cutscene/dialogue camera after it: ends, no write
    ++lifetime;CHECK(!frame(true,true));CHECK(h.armed);
    ++lifetime;mem[manager+0x30]=manager+0x350;CHECK(!frame(false,true));CHECK(!h.armed);
    mem[manager+0x30]=manager+0x338;CHECK(!frame(false,true));
    // 8) debug/external camera flag after exit: ends
    ++lifetime;CHECK(!frame(true,true));++lifetime;mem[manager+0x3B]=1;CHECK(!frame(false,true));CHECK(!h.armed);mem[manager+0x3B]=0;
    // 9) qualify()/select() unchanged for the running dice
    const auto e=qualify(b,player+0x40,none,read,0,dice);CHECK(e.kindMask==32&&e.camera==Owner::Minigame);
    Policy p{100,95*.01745329251994329577f,true};CHECK(select(p,110,.6f,e)==p.radians);
    // Other supported minigames may use exactly the same camera, but never arm a dice exit.
    for(unsigned kind:{1u,4u,8u,16u}){
        h={};DiceExitFrameR91 other{true,true,0,player,50,kind,
            Evidence{Owner::Minigame,kind,Refusal::None,0x70000,0x408EB60},Owner::Minigame};
        CHECK(!exitHoldStep(h,other));CHECK(!h.armed);
        other.kindMask=0;other.lifetime=51;other.evidence={};CHECK(!exitHoldStep(h,other));
    }
    // A new activity after dice, changed player, missed lifecycle, or a bad witness revokes the lease.
    DiceExitFrameR91 running{true,true,dice,player,70,32,e,Owner::Minigame};
    CHECK(!exitHoldStep(h,running));CHECK(h.armed);
    auto after=running;after.kindMask=1;after.dice=0;after.lifetime=71;
    CHECK(!exitHoldStep(h,after));CHECK(!h.armed);
    CHECK(!exitHoldStep(h,running));after=running;after.kindMask=0;after.dice=0;after.evidence={};
    after.lifetime=71;after.player=player+1;CHECK(!exitHoldStep(h,after));CHECK(!h.armed);
    CHECK(!exitHoldStep(h,running));after.player=player;after.lifetime=73;
    CHECK(!exitHoldStep(h,after));CHECK(!h.armed);
    CHECK(!exitHoldStep(h,running));after.lifetime=71;after.known=false;
    CHECK(!exitHoldStep(h,after));CHECK(h.armed);
    after.known=true;after.policyFresh=false;CHECK(!exitHoldStep(h,after));CHECK(h.armed);
    after.policyFresh=true;CHECK(exitHoldStep(h,after));
    after.policyKnown=false;CHECK(!exitHoldStep(h,after));CHECK(h.armed);
    after.policyKnown=true;CHECK(exitHoldStep(h,after));
    std::printf(fails?"FAILED %d\n":"PASS\n",fails);return fails?1:0;
}
'''

def run_fixture(src_dir: Path) -> bool:
    with tempfile.TemporaryDirectory() as d:
        cpp = Path(d) / "t.cpp"; exe = Path(d) / "t"
        cpp.write_text(FIXTURE)
        # src_dir first (a mutated copy of the header), the real tree for the rest
        if shutil.which("cl"):
            exe = Path(d) / "t.exe"
            command=["cl", "/nologo", "/std:c++20", "/EHsc", "/W0", "/I"+str(src_dir), "/I"+str(ROOT/"src"),
                     str(cpp), "/Fe:"+str(exe), "/Fo:"+str(Path(d)/"t.obj")]
        else:
            command=["g++", "-std=c++20", "-w", "-I", str(src_dir), "-I", str(ROOT / "src"), str(cpp), "-o", str(exe)]
        c = subprocess.run(command,
                           capture_output=True, text=True)
        if c.returncode:
            print((c.stdout+c.stderr)[:2000]); return False
        r = subprocess.run([str(exe)], capture_output=True, text=True)
        print("   ", r.stdout.strip().splitlines()[-1] if r.stdout.strip() else r.stderr[:300])
        return r.returncode == 0

def mutated_header(old: str, new: str) -> bool:
    with tempfile.TemporaryDirectory() as d:
        h = (ROOT / "src/activity_lens59c10r1.h").read_text()
        assert h.count(old) >= 1, old
        (Path(d) / "activity_lens59c10r1.h").write_text(h.replace(old, new))
        return run_fixture(Path(d))

def stereo_checks(mod: str, host: str, dev: str) -> list[str]:
    bad = []
    # VR menu edit: mode 2 only while a run is live, else live mono
    m = re.search(r"if \(native_production && !native_stereo_running_r89_\.load\([^)]*\)\) \{(.*?)\} else if \(native_production\) \{",
                  host, re.S)
    if not m or "SetStereoExperiment(0, 0.0f)" not in m.group(1):
        bad.append("menu edit can set XR mode 2 without a stereo run")
    # Run publishes the live flag every loop before the stereo presentation block
    if "native_stereo_running_r89_.store(f9_postfx_step==4" not in mod:
        bad.append("run flag never published")
    # Run: no run -> a stale mode 2 is dropped
    m = re.search(r"\} else if\(f9_postfx_step!=4\) \{(.*?)v6619_xr_stereo_applied=-1;", mod, re.S)
    if not m or "StereoExperimentR89()==2" not in m.group(1) or "SetStereoExperiment(0,0.0f)" not in m.group(1):
        bad.append("Run keeps XR mode 2 without a run")
    # stop also unpublishes the last pair
    m = re.search(r"void DeviceHook::SetV6610OutputCensus\(bool enabled\)\s*\{(.*?)V6612ResetTracking\(\);", dev, re.S)
    if not m or re.search(r"if\(enabled\)\s*\{\s*std::lock_guard<std::mutex> lock\(s_bridge_capture_mutex\);\s*s_bridge_complete_pair_serial\.store\(0", m.group(1)) \
       or "s_bridge_complete_pair_serial.store(0" not in m.group(1):
        bad.append("stopped run keeps its last pair published")
    return bad

def main() -> int:
    ok = True
    print("A  exit hold, production header")
    ok &= run_fixture(ROOT / "src")
    negatives = [
        ("old behaviour: no write after exit", "    return true;\n}\n// C10R19", "    return false;\n}\n// C10R19"),
        ("other minigame does not revoke dice token", "if(!f.player||(f.kindMask!=0&&f.kindMask!=32u)||(h.armed&&f.player!=h.player)){h={};return false;}",
         "if(!f.player||(f.kindMask!=0&&f.kindMask!=32u)||(h.armed&&f.player!=h.player)){return false;}"),
        ("hold ignores player identity", "f.player!=h.player", "false"),
        ("hold ignores explicit user policy", "!f.wanted", "false"),
        ("old unavailable observation erases identity", "if(!f.known)return false;", "if(!f.known){h={};return false;}"),
        ("stale policy authorizes writes", "if(!f.policyKnown||!f.policyFresh)return false;", "if(!f.policyKnown)return false;"),
    ]
    for name, old, new in negatives:
        print(f"A- negative: {name}")
        if mutated_header(old, new):
            print("   NEGATIVE CONTROL NOT REJECTED"); ok = False
        else:
            print("   rejected")
    print("B  stereo idle guards, production source")
    mod = (ROOT / "src/mod.cpp").read_text(errors="ignore")
    host = (ROOT / "src/subsystems/hud_ui_controls/host.inl").read_text(errors="ignore")
    dev = (ROOT / "src/device_hook.cpp").read_text(errors="ignore").replace("\r\n", "\n")
    bad = stereo_checks(mod, host, dev)
    print("   PASS" if not bad else "   FAIL " + "; ".join(bad)); ok &= not bad
    negs = [
        ("D85R5 menu branch", lambda m, h, d: (m, re.sub(r"if \(native_production && !native_stereo_running_r89_.*?\} else if \(native_production\) \{",
                                                           "if (native_production) {", h, flags=re.S), d)),
        ("no Run reset", lambda m, h, d: (m.replace("xr_manager_->StereoExperimentR89()==2", "false"), h, d)),
        # R90: comment-independent mutation (the R90 port words the comment differently)
        ("clear only on arm (R88)", lambda m, h, d: (m, h, re.sub(r"(frozen dice image[^\n]*\n)    \{\n", r"\1    if(enabled) {\n", d, count=1))),
    ]
    for name, mut in negs:
        b = stereo_checks(*mut(mod, host, dev))
        print(f"B- negative: {name}: " + ("rejected (" + "; ".join(b) + ")" if b else "NEGATIVE CONTROL NOT REJECTED"))
        ok &= bool(b)
    print("ALL PASS" if ok else "FAILED")
    return 0 if ok else 1

if __name__ == "__main__":
    sys.exit(main())
