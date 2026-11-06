#!/usr/bin/env python3
"""Replay the actual small-pickup setup function with hook/clock leaves replaced.

No game, DLL, physics mutation or Windows hook is executed. Optional --baseline
must fail the same cold-start and failed-registration cases after compiling.
"""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
PRELUDE = r'''
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "core/load_rearm_r96.h" // R97: UpdateSmallPickupReaderR24 load re-arm
static bool wanted=true, retail=true, owner=true, installOK=true;
static unsigned installs=0, registrations=0, failures=0;
static bool hasReader=false, failAlways=false;
static std::uint64_t now=100;
static std::uint64_t GetTickCount64(){return now;}
static std::uintptr_t WhBase(){return 1;}
namespace kcd2vr {
namespace instant_d72 {static bool Interactions(){return wanted;}}
namespace instant_pickup_d72 {static std::atomic<bool> stopping{false};}
}
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail;}}
namespace action_time_owner59c11 {static bool Installed(){return owner;}}
static std::atomic<bool> g_instant_pickup_ready_d72{false};
static bool InstallInstantPickupD72(){++installs;if(installOK)g_instant_pickup_ready_d72=true;return installOK;}
static std::uint32_t SmallPickupAdmissionR24(std::uint64_t,bool) noexcept{return 0;}
namespace kcd2vr::door_signal19c4r4 {
using Reader=std::uint32_t(*)(std::uint64_t,bool) noexcept;
static bool SetSmallPickupReaderR24(Reader r){
 ++registrations;
 if(failAlways||failures){if(failures)--failures;return false;}
 hasReader=bool(r);return true;
}
}
'''
TAIL = r'''
static unsigned checks=0;
#define CK(v) do{++checks;if(!(v)){std::fprintf(stderr,"check %u failed: %s\n",checks,#v);return 1;}}while(0)
int main(int argc,char** argv){
 if(argc!=2)return 2;
 switch(std::atoi(argv[1])){
 case 0:
  UpdateSmallPickupReaderR24(false);CK(installs==1&&registrations==1&&hasReader);
  for(unsigned i=0;i<200;++i){++now;UpdateSmallPickupReaderR24(false);}
  CK(installs==1&&registrations==1);break;
 case 1:
  g_instant_pickup_ready_d72=true;failures=1;
  UpdateSmallPickupReaderR24(false);CK(registrations==1&&!hasReader);
  now=200;UpdateSmallPickupReaderR24(false);CK(registrations==1&&!hasReader);
  now=600;UpdateSmallPickupReaderR24(false);CK(registrations==2&&hasReader);
  for(unsigned i=0;i<200;++i){++now;UpdateSmallPickupReaderR24(false);}
  CK(registrations==2);break;
 case 2:
  wanted=false;
  for(unsigned i=0;i<200;++i){++now;UpdateSmallPickupReaderR24(false);}
  CK(installs==0&&registrations==0);
  wanted=true;retail=false;UpdateSmallPickupReaderR24(false);CK(installs==0);
  retail=true;owner=false;UpdateSmallPickupReaderR24(false);CK(installs==0);
  owner=true;UpdateSmallPickupReaderR24(false);CK(installs==1&&hasReader);break;
 case 3:
  installOK=false;
  for(unsigned i=0;i<200;++i){now+=500;UpdateSmallPickupReaderR24(false);}
  CK(installs==1&&registrations==0&&!hasReader);break;
 case 4:
  wanted=false;UpdateSmallPickupReaderR24(true);CK(installs==0&&registrations==1&&hasReader);
  UpdateSmallPickupReaderR24(false);CK(registrations==2&&!hasReader);
  for(unsigned i=0;i<200;++i)UpdateSmallPickupReaderR24(false);
  CK(registrations==2);break;
 case 5:
  g_instant_pickup_ready_d72=true;failAlways=true;
  for(unsigned i=0;i<200;++i){now+=500;UpdateSmallPickupReaderR24(false);}
  // R96: no permanent stop after 4 refusals; bounded retry (4 x 500 ms, then 5 s pause), never every frame.
  CK(registrations>4&&registrations<100&&!hasReader);
  {const unsigned before=registrations;
  wanted=false;UpdateSmallPickupReaderR24(false);failAlways=false;wanted=true;
  UpdateSmallPickupReaderR24(false);CK(registrations==before+1&&hasReader);}break;
 case 6:
  kcd2vr::instant_pickup_d72::stopping=true;
  for(unsigned i=0;i<200;++i)UpdateSmallPickupReaderR24(false);
  CK(installs==0&&registrations==0&&!hasReader);break;
 default:return 2;
 }
 std::printf("case %s: %u checks PASS\n",argv[1],checks);return 0;
}
'''


def extract(root):
    text = (root / "src/vrik/staged_pickup596c2.inl").read_text()
    start = text.index("static void UpdateSmallPickupReaderR24(")
    brace = text.index("{", start)
    depth = 1
    end = brace + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]


def run(root, build, name):
    cpp = build / (name + ".cpp")
    exe = build / name
    cpp.write_text(PRELUDE + extract(root) + TAIL)
    compile_result = subprocess.run(["g++", "-std=c++17", "-O2", "-I" + str(ROOT / "src"), str(cpp), "-o", str(exe)],
                                    text=True, capture_output=True)
    if compile_result.returncode:
        raise RuntimeError(compile_result.stderr)
    cases = []
    for case in range(7):
        result = subprocess.run([str(exe), str(case)], text=True, capture_output=True)
        cases.append({"case": case, "passed": result.returncode == 0,
                      "output": (result.stdout + result.stderr).strip()})
    return cases


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", type=Path)
    parser.add_argument("--report", type=Path)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="kcd2vr_pickup_bootstrap_") as temporary:
        build = Path(temporary)
        current = run(ROOT, build, "current")
        report = {"current": current, "native_leaves": "substituted", "headset": "not run"}
        passed = all(case["passed"] for case in current)
        if args.baseline:
            baseline = run(args.baseline.resolve(), build, "baseline")
            report["baseline"] = baseline
            report["negative_controls_passed"] = not baseline[0]["passed"] and not baseline[1]["passed"]
            passed &= report["negative_controls_passed"]
        report["all_passed"] = bool(passed)
        if args.report:
            args.report.parent.mkdir(parents=True, exist_ok=True)
            args.report.write_text(json.dumps(report, indent=2) + "\n")
        print(json.dumps(report, indent=2))
        return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
