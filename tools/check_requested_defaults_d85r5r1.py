#!/usr/bin/env python3
"""Check the actual one-time defaults migration and optional profile preservation.

Compiles only the two production migration blocks with INI/runtime leaves
substituted. It does not build a DLL or open a game-side INI.
"""
import argparse
from collections import defaultdict
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def settings(path):
    section = ''
    values = defaultdict(list)
    for raw in path.read_text(encoding='utf-8-sig').splitlines():
        row = raw.strip()
        if not row or row.startswith((';', '#')):
            continue
        if row.startswith('[') and row.endswith(']'):
            section = row[1:-1]
        elif '=' in row:
            key, value = row.split('=', 1)
            values[(section, key.strip())].append(value.strip())
    return values


PRELUDE = r'''
#include <atomic>
#include <cstdio>
#include <map>
#include <string>
using BOOL=int; constexpr int FALSE=0;
static std::map<std::string,std::string> ini;
static int writes=0; static std::string failedKey;
static int ReadIniIntExplicit(const char*,const char*,const char* k,int fallback){
 auto i=ini.find(k);return i==ini.end()?fallback:std::stoi(i->second);
}
static BOOL WritePrivateProfileStringA(const char*,const char* k,const char* v,const char*){
 ++writes;if(failedKey=="*"||failedKey==k)return FALSE;ini[k]=v;return 1;
}
namespace prompt_policy_d80 {static std::atomic<float> minigameScale{.45f};}
namespace roomscale_d76 {static bool enabled=false;static void SetEnabled(bool v){enabled=v;}}
static std::atomic<bool> g_roomscale_follow58{false},vrik_body_follow_{true};
namespace frik_pose_probe {static bool body=true;static void SetBodyFollow128(bool v){body=v;}}
namespace CameraHook {static bool follow=false;static void SetRoomscaleFollow(bool v){follow=v;}}
static void apply(){const char* explicit_ini="fixture";
'''
TAIL = r'''
}
static int checks=0;
#define C(x) do {++checks;if(!(x)){std::fprintf(stderr,"check %d failed: %s\n",checks,#x);return 1;}} while(0)
int main(){
 ini={{"VrRepairDefaultsD81","1"},{"VrMinigamePromptScale","0.45"},{"VrNativeRoomscale1to1D76","false"},
      {"VrRoomscaleFollow","false"},{"VrikBodyFollow6DOF","true"},{"Calibration","original"},
      {"VrMotionTrialD67","2"},{"VrFrikElbows","false"},{"graphics","current"}};
 const auto original=ini;apply();
 C(writes==2);C(ini["VrRepairDefaultsD85R5R1"]=="1");C(ini["VrMinigamePromptScale"]=="0.90");
 C(ini["VrNativeRoomscale1to1D76"]=="false");C(ini["VrRoomscaleFollow"]=="false");
 C(ini["VrikBodyFollow6DOF"]=="true");C(ini["Calibration"]=="original");
 C(ini["VrMotionTrialD67"]=="2"&&ini["VrFrikElbows"]=="false"&&ini["graphics"]=="current");
 C(prompt_policy_d80::minigameScale.load()==.9f&&!roomscale_d76::enabled&&!g_roomscale_follow58);
 C(vrik_body_follow_&&frik_pose_probe::body&&!CameraHook::follow);
 // A later deliberate setting survives reload after the marker is saved.
 ini["VrMinigamePromptScale"]="0.60";ini["VrNativeRoomscale1to1D76"]="false";
 ini["VrikBodyFollow6DOF"]="false";writes=0;
 prompt_policy_d80::minigameScale=.6f;roomscale_d76::enabled=false;
 vrik_body_follow_=false;frik_pose_probe::body=false;CameraHook::follow=false;apply();
 C(writes==0);C(ini["VrMinigamePromptScale"]=="0.60");C(ini["VrNativeRoomscale1to1D76"]=="false");
 C(prompt_policy_d80::minigameScale==.6f&&!roomscale_d76::enabled&&!vrik_body_follow_);
 C(!frik_pose_probe::body&&!CameraHook::follow);
 // Read-only INI cannot silently prevent this run's requested runtime default.
 ini=original;failedKey="*";writes=0;apply();C(writes==1);C(ini==original);
 C(!roomscale_d76::enabled&&prompt_policy_d80::minigameScale==.9f&&!vrik_body_follow_);
 C(fr ik_placeholder);
 return 0;
}
'''.replace('C(fr ik_placeholder);', '''C(!frik_pose_probe::body&&!CameraHook::follow);
 // Partial failure must not mark migration complete; a later retry can finish.
 ini=original;failedKey="VrMinigamePromptScale";writes=0;apply();
 C(writes==1&&ini.count("VrRepairDefaultsD85R5R1")==0);
 C(ini["VrNativeRoomscale1to1D76"]=="false"&&!roomscale_d76::enabled);
 failedKey="";writes=0;apply();C(writes==2&&ini["VrRepairDefaultsD85R5R1"]=="1");
 C(ini["VrNativeRoomscale1to1D76"]=="false");
 // Both ON and OFF states survive a fresh prompt migration, in memory and on
 // disk; the final profile is not evidence about an earlier game attempt.
 for(unsigned bits=0;bits<8;++bits){
  ini=original;ini["VrNativeRoomscale1to1D76"]=(bits&1)?"true":"false";
  ini["VrRoomscaleFollow"]=(bits&2)?"true":"false";
  ini["VrikBodyFollow6DOF"]=(bits&4)?"true":"false";
  roomscale_d76::enabled=bits&1;g_roomscale_follow58=bool(bits&2);
  vrik_body_follow_=frik_pose_probe::body=CameraHook::follow=bool(bits&4);
  const auto saved=ini;writes=0;apply();
  C(writes==2&&ini["VrMinigamePromptScale"]=="0.90");
  for(const char* key:{"VrNativeRoomscale1to1D76","VrRoomscaleFollow","VrikBodyFollow6DOF"})C(ini[key]==saved.at(key));
  C(roomscale_d76::enabled==bool(bits&1)&&g_roomscale_follow58==bool(bits&2));
  C(vrik_body_follow_==bool(bits&4)&&frik_pose_probe::body==bool(bits&4)&&CameraHook::follow==bool(bits&4));
 }
 std::printf("requested_defaults: %d checks PASS\\n",checks);''')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original-profile', type=Path)
    p.add_argument('--output', type=Path)
    args = p.parse_args()
    text = (ROOT / 'src/mod.cpp').read_text()
    first = text[text.index('    const bool applyRepairD85R5R1='):text.index('    // Launcher selection is startup-only.')]
    start = text.index('    if(applyRepairD85R5R1){', text.index('    // Apply for this run even if the INI could not be written.'))
    second = text[start:text.index('\n    }', start) + len('\n    }')]
    with tempfile.TemporaryDirectory(prefix='kcd2vr-default-check-') as tmp:
        cpp, exe = Path(tmp) / 'defaults.cpp', Path(tmp) / 'defaults'
        cpp.write_text(PRELUDE + first + second + TAIL)
        c = subprocess.run(['g++', '-std=c++17', '-O2', str(cpp), '-o', str(exe)], text=True, capture_output=True)
        if c.returncode:
            raise SystemExit(c.stderr)
        result = subprocess.run([str(exe)], text=True, capture_output=True)
    report = {'actual_migration_blocks': result.returncode == 0, 'output': result.stdout + result.stderr,
              'native_leaves': 'INI write and runtime setting sinks substituted', 'dll_built': False}
    if args.original_profile:
        before, after = settings(args.original_profile), settings(ROOT / 'KCD2VR.ini')
        allowed = {('Settings', 'VrMinigamePromptScale'): ['0.90'],
                   ('Settings', 'VrRepairDefaultsD85R5R1'): ['1']}
        changes = []
        unexpected = []
        for key in sorted(before.keys() | after.keys()):
            if before.get(key) == after.get(key):
                continue
            changes.append({'section': key[0], 'key': key[1], 'before': before.get(key), 'after': after.get(key)})
            if key not in allowed or after.get(key) != allowed[key]:
                unexpected.append(key)
        report['profile_changes'] = changes
        report['all_other_setting_occurrences_preserved'] = not unexpected
    report['passed'] = report['actual_migration_blocks'] and report.get('all_other_setting_occurrences_preserved', True)
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    raise SystemExit(0 if report['passed'] else 1)


if __name__ == '__main__':
    main()
