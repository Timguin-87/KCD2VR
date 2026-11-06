#!/usr/bin/env python3
"""Run actual D78 native socket reader/observer/writer with retail leaves mocked."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
from test_crossbow_mechanism_d78 import function
from host_compiler import command as host_command, executable_path

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--compiler", default="g++")
    parser.add_argument("--msvc-asan", action="store_true")
    parser.add_argument("--publication-only", action="store_true")
    parser.add_argument("--bolt-only", action="store_true")
    parser.add_argument("--grip-only", action="store_true")
    args = parser.parse_args()
    msvc = Path(args.compiler).name.lower() in ("cl", "cl.exe")
    out = args.out.resolve()
    assert ROOT not in out.parents
    out.mkdir(parents=True, exist_ok=True)
    ranged_path = ROOT / "src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl"
    mechanism_path = ranged_path.with_name("crossbow_mechanism_d78.inl")
    ranged = ranged_path.read_text()
    actual = ranged[ranged.index("// Calibration has no destination or timestamp."):ranged.index("static bool RangedPinsH21(")]
    types = (ROOT / "src/frik_pose_probe.cpp").read_text()
    types = types[types.index("struct RangedSocketXR34 {"):types.index("struct CommittedFrame6695 {")]
    (out / "crossbow_pose_types_r95_actual.inc").write_text(types)
    native_lookup = function(actual, "static bool RangedBoundToolIdentityD72(").rstrip()
    # The qualified lookup leaf calls direct WHGame code. The host substitutes
    # only that native execution leaf; descriptor reads/witnesses/writers below
    # remain actual production code and are exercised against fixture memory.
    mock_lookup = """static bool RangedBoundToolIdentityD72(const xcXR34::Owner& o,std::uint32_t id,void* e,int& side,void*& a) noexcept {
    if(o.source.classId!=15)return RangedBoundHandIdentityXR34(o,id,e,side,a);
    a=nullptr;side=-1;if(!d78BindingOK||id!=77||e!=entity.data()||!ManualBoundEntity19c4r4(attachment.data(),id,e))return false;
    void* manager=nullptr;void* c=nullptr;if(!Read(attachment.data()+0x28,manager)||!manager||!Read(static_cast<unsigned char*>(manager)+0x70,c))return false;
    g_ranged_mechanism_character_d78.store(reinterpret_cast<std::uintptr_t>(c));a=attachment.data();side=1-o.source.primary;return true;
}"""
    assert actual.count(native_lookup) == 1
    actual = actual.replace(native_lookup, mock_lookup)
    (out / "crossbow_pose_d78_actual.inc").write_text(actual)
    mechanism = mechanism_path.read_text()
    weapon_identity = function(mechanism, "static bool RangedWeaponPoseIdentityD78(").rstrip()
    mock_weapon_identity = """static bool RangedWeaponPoseIdentityD78(const xcXR34::Owner& o,void*& c,void*& skel,int& joint) noexcept {
    c=weaponCharD78();skel=weaponSkeletonD78();joint=1;
    return weaponBindingD78&&contract&&o.source.weaponItem==source.weaponItem&&xcXR34::sameSource(o.source,source);
}"""
    assert mechanism.count(weapon_identity) == 1
    (out / "crossbow_mechanism_d78_actual.inc").write_text(mechanism.replace(weapon_identity, mock_weapon_identity))
    base_fixture = (ROOT / "tests/crossbow_pose_xr34_test.cpp").read_text()
    base_fixture = base_fixture.replace("void* component=nullptr;bool loaded=false;", "void* component=nullptr;void* visual=nullptr;bool loaded=false;")
    base_fixture = base_fixture.replace("struct CommittedFrame6695 {pm::Stamp stamp{};", "struct CommittedFrame6695 {std::uint64_t publicationR24=2;pm::Stamp stamp{};")
    base_fixture = base_fixture.replace("source={};", "source=xcXR34::Source{};")
    base_fixture = base_fixture.replace('#include "core/haptics.h"', '#include "core/haptics.h"\n#include "subsystems/ranged_reload/ranged_draw_policy_d72.h"')
    leaves = """
// Explicit native leaves for the current D78 attachment slice on this host.
static bool d78BindingOK=true;
static bool CrossbowReloadEnabledD65C() noexcept {return true;}
static void (*worldReadReentryR93R2)()=nullptr;
static bool ReadModelWorld6692(void*,ModelWorld6692& out) noexcept {out=world;if(worldReadReentryR93R2){auto fn=worldReadReentryR93R2;worldReadReentryR93R2=nullptr;fn();}return true;}
static std::atomic<bool> g_committed_pose_valid6689{true};
static std::uint64_t completedPublicationR93R2=2;
static std::uint64_t ReadCompletedPublicationT23() noexcept {return completedPublicationR93R2;}
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{1};}
struct PhysTryLock6698 {bool owned=true;};
namespace rdrawD72=kcd2vr::ranged_draw_d72;
static void RangedDrawPlacedD72(const rdrawD72::Identity&,bool,bool) noexcept {}
static bool ReadReloadPassengerXR34(xcXR34::Passenger&,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept;
static bool ReloadPassengerCurrentXR34(const xcXR34::Passenger&,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept;
"""
    base_fixture = base_fixture.replace("#ifdef KCD2VR_XR34_ACTUAL_INCLUDE", leaves + "#ifdef KCD2VR_XR34_ACTUAL_INCLUDE")
    (out / "crossbow_pose_xr34_test.cpp").write_text(base_fixture)
    (out / "crossbow_socket_d78_test.cpp").write_text((ROOT / "tests/crossbow_socket_d78_test.cpp").read_text())
    binding = (ROOT / "src/subsystems/ranged_reload/manual_reload_attachment19c4r4.inl").read_text()
    (out / "manual_arrow_binding64.inc").write_text(binding[binding.index("static bool ManualBoundEntity19c4r4("):binding.index("static bool ManualNockAttached19c4r4(")])
    hand_claim = (ROOT / "src/frik_pose_probe.cpp").read_text()
    (out / "generated_hand_claim_t24.inc").write_text(function(hand_claim, "static bool HandClaimBlockedT24(int side,std::uintptr_t character,std::uint64_t body,std::uint64_t request) noexcept {"))
    runs = []
    selected_test = "crossbow_grip_r95r2_test.cpp" if args.grip_only else "crossbow_bolt_binding_r95r1_test.cpp" if args.bolt_only else "crossbow_publication_r95_test.cpp" if args.publication_only else "crossbow_socket_d78_test.cpp"
    if args.bolt_only or args.grip_only:(out / selected_test).write_text((ROOT / "tests" / selected_test).read_text())
    (out / "crossbow_publication_r95_test.cpp").write_text((ROOT / "tests/crossbow_publication_r95_test.cpp").read_text())

    def build_run(name, directory, flags, fail=False):
        executable = executable_path(directory / name)
        command = host_command(directory / selected_test, executable,
            [directory, ROOT / "src", ROOT / "tests", ROOT / "tests/syntax_shims", out], args.compiler)
        if msvc:
            command = [("/Od" if "-O0" in flags else "/O2") if x=="/O2" else x for x in command]
            if "-fsanitize=address" in flags: command.insert(1,"/fsanitize=address")
        else:
            command = [x for x in command if x!="-O2"] + flags + ["-fmax-errors=8", "-D__fastcall=", "-D__except(x)=catch(...)"]
        build = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (directory / (name + "_build.log")).write_text(build.stdout)
        if build.returncode:
            raise RuntimeError(name + " compile failed\n" + build.stdout[-5000:])
        env = os.environ.copy()
        env["ASAN_OPTIONS"] = "detect_leaks=0"
        run = subprocess.run([str(executable)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        executable.unlink(missing_ok=True)
        (directory / (name + ".log")).write_text(run.stdout)
        passed = (run.returncode == 1 and ("FAIL_GRIP_R95R2" if args.grip_only else "FAIL_BOLT_R95R1" if args.bolt_only else "FAIL_PUBLICATION_R95" if args.publication_only else "FAIL_SOCKET_D78") in run.stdout) if fail else run.returncode == 0
        runs.append({"name": name, "command": command, "exit_code": run.returncode, "expected_failure": fail,
                     "passed": passed, "output": run.stdout})
        print(name + ": " + run.stdout, end="", flush=True)
        if not passed:
            raise RuntimeError(name + " unexpected execution result")

    build_run("socket_debug", out, ["-O0", "-g"])
    build_run("socket_optimized", out, ["-O2"])
    if msvc and args.msvc_asan:
        build_run("socket_asan", out, ["-O0", "-fsanitize=address"])
    if not msvc:
        build_run("socket_asan_ubsan", out, ["-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"])
    if args.bolt_only:
        mutations = {
            "old_no_pending_bolt": ("result.handBoltPendingR95=pending;result.handBoltBound=!pending;", "if(pending)return false;result.handBoltBound=true;"),
            "old_bolt_hidden": ("ManualArrowVisible42(visible)&&", ""),
            "detached_bolt_reenters_pending": ("(!complete&&join.bound)||", ""),
        }
        for name,(old,new) in mutations.items():
            assert actual.count(old)==1,(name,actual.count(old))
            shadow=out/name;shadow.mkdir(exist_ok=True)
            (shadow/"crossbow_pose_d78_actual.inc").write_text(actual.replace(old,new))
            (shadow/"crossbow_pose_xr34_test.cpp").write_text(base_fixture)
            for filename in ("crossbow_socket_d78_test.cpp",selected_test):(shadow/filename).write_text((out/filename).read_text())
            build_run(name,shadow,["-O0","-g"],True)
        (out/"RESULT.json").write_text(json.dumps({"passed":all(r["passed"] for r in runs),"runs":runs,
            "source_sha256":{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in (ranged_path,ROOT/"tests"/selected_test,ROOT/"tests/crossbow_pose_xr34_test.cpp",Path(__file__))},
            "limits":["Production pending-bolt identity, deadline, native binding and actual attachment writer execute.","Native pointer memory, existing qualified visibility leaf and placements are controlled fixture leaves. No DLL, game, headset or native stock/effect execution."]},indent=2)+"\n")
        return
    if args.grip_only:
        mutations = {
            "old_unscaled_native_grip_receipt": ("h.x*world.scale/(sx*natural[7]),h.y*world.scale/(sy*natural[7]),h.z*world.scale/(sz*natural[7])", "h.x/(sx*natural[7]),h.y/(sy*natural[7]),h.z/(sz*natural[7])"),
            "old_animated_descriptor_recaptures_spanner": ("result.grip=previous.grip;", "(void)previous;"),
            "old_heavy_passenger_gap_withdraws": ("!ReloadPoseWriterCurrentXR34(state.owner,passenger.publication))return retainOrHold();", "!ReloadPoseWriterCurrentXR34(state.owner,passenger.publication))return body.publicationR24!=ReadCompletedPublicationT23()?retain():withdraw();"),
            "old_heavy_mechanism_gap_withdraws": ("!xcXR34::sameMechanismClockD78(mechanism.clock,passenger.clock)))return retainOrHold();", "!xcXR34::sameMechanismClockD78(mechanism.clock,passenger.clock)))return withdraw();"),
            "old_heavy_retention_ignores_withdrawal": ("current.clockOwned&&current.placeTool&&current.phase!=xcXR34::Phase::Retiring&&", "current.clockOwned&&current.phase!=xcXR34::Phase::Retiring&&"),
            "old_heavy_retention_ignores_tool_replacement": ("Read(reinterpret_cast<unsigned char*>(state.owner.main)+0x80,nativeTool)&&nativeTool==tool&&", ""),
            # R97 merge: the R96 wrist hold after retention must also stop on a withdrawn placeTool.
            "old_heavy_hold_ignores_withdrawal": ("!holdNow.clockOwned||!holdNow.placeTool||", "!holdNow.clockOwned||"),
            "old_heavy_hold_ignores_tool_replacement": ("!Read(reinterpret_cast<unsigned char*>(state.owner.main)+0x80,holdTool)||holdTool!=tool||", ""),
            "old_heavy_final_write_ignores_withdrawal": ("!current.clockOwned||!current.placeTool||!xcXR34::sameOwner(current.owner,state.owner)||", "!current.clockOwned||!xcXR34::sameOwner(current.owner,state.owner)||"),
        }
        for name,(old,new) in mutations.items():
            assert actual.count(old)==1, (name,actual.count(old))
            shadow=out/name;shadow.mkdir(exist_ok=True)
            (shadow/"crossbow_pose_d78_actual.inc").write_text(actual.replace(old,new))
            (shadow/"crossbow_pose_xr34_test.cpp").write_text(base_fixture)
            for filename in ("crossbow_socket_d78_test.cpp",selected_test):
                (shadow/filename).write_text((out/filename).read_text())
            build_run(name,shadow,["-O0","-g"],True)
        paths=[ranged_path,mechanism_path,ranged_path.with_name("crossbow_native_xr34.h"),ROOT/"src/frik_pose_probe.cpp",ROOT/"tests/crossbow_grip_r95r2_test.cpp",ROOT/"tests/crossbow_socket_d78_test.cpp",ROOT/"tests/crossbow_pose_xr34_test.cpp",Path(__file__)]
        (out/"RESULT.json").write_text(json.dumps({"passed":all(r["passed"] for r in runs),"runs":runs,
            "source_sha256":{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
            "limits":["Actual native-grip capture, body staging, attachment readback, retained placement and heavy mounted writer execute.","Native memory, direct lookup, live context/clock and PRS/API calls remain fixture leaves; no DLL, game/headset or actual rendered visibility proof."]},indent=2)+"\n")
        return
    if args.publication_only:
        mutations = {
            "ignore_target_publication": ("target.publicationR95!=body.publicationR24||", ""),
            "ignore_source_publication": ("source.bodyPublicationR95!=body.publicationR24||source.auxiliaryUnavailableR95||", "source.auxiliaryUnavailableR95||"),
            "discard_pending_placement": ("if(ReadCompletedPublicationT23()&1)return retain();", "if(ReadCompletedPublicationT23()&1)return withdraw();"),
        }
        for name,(old,new) in mutations.items():
            assert actual.count(old)==1, (name,actual.count(old))
            shadow=out/name;shadow.mkdir(exist_ok=True)
            (shadow / "crossbow_pose_d78_actual.inc").write_text(actual.replace(old,new))
            (shadow / "crossbow_pose_xr34_test.cpp").write_text(base_fixture)
            for filename in ("crossbow_socket_d78_test.cpp","crossbow_publication_r95_test.cpp"):
                (shadow/filename).write_text((out/filename).read_text())
            build_run(name,shadow,["-O0","-g"],True)
        (out/"RESULT.json").write_text(json.dumps({"passed":all(r["passed"] for r in runs),"runs":runs,
            "source_sha256":{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in (ranged_path,ROOT/"src/frik_pose_probe.cpp",ROOT/"tests/crossbow_publication_r95_test.cpp",Path(__file__))},
            "limits":["Actual staging builder, paired packet consumers, binding readback, pending retention and native PRS selection execute.","Native memory, direct retail lookup, atomic packet delivery, placement and live-context leaves are fixtures. The R95 adversary supplies explicit packets and disables the legacy coherent-source provider.","Host debug/optimized; sanitizer executions are identified per run. No Windows DLL, game, headset, native stock inventory or real scheduling validation."]},indent=2)+"\n")
        return
    mutations = {
        "heavy_relative_recaptures_grip": ("(measured&&capture.skeleton!=mechanism.skeleton)", "(measured&&(capture.skeleton!=mechanism.skeleton||std::memcmp(&capture.relative,&mechanism.relative,sizeof(capture.relative))))"),
        "heavy_skips_publication": ("!body.publicationR24||(body.publicationR24&1)||body.publicationR24!=ReadCompletedPublicationT23()", "!body.publicationR24||(body.publicationR24&1)"),
        "heavy_skips_final_validation": ("GetEntityWorldTmV6552(entity,id,achieved)&&currentHeavyR93R2()", "GetEntityWorldTmV6552(entity,id,achieved)"),
        "free_hand_after_mount": ("const auto model=state.toolMountedD78?", "const auto model=false?"),
        "animated_bolt_grip": ("result.grip=previous.grip;", "(void)previous;"),
        "animated_descriptor_recaptures_grip": ("previous.valid&&RangedSameGripBindingXR34(previous.socket,socket)", "previous.valid&&RangedSameSocketXR34(previous.socket,socket)"),
        "invalid_target_clears_grip": ("packet.reloadTargetXR34=result;return;", "g_ranged_grip_xr34={};packet.reloadTargetXR34=result;return;"),
        "mounted_position_only": ("if(p.helperFrameD78||p.poseBasisD78)for", "if(p.helperFrameD78)for"),
    }
    for name, (old, new) in mutations.items():
        shadow = out / name
        shadow.mkdir(exist_ok=True)
        assert actual.count(old) == 1
        (shadow / "crossbow_pose_d78_actual.inc").write_text(actual.replace(old, new))
        (shadow / "crossbow_pose_xr34_test.cpp").write_text(base_fixture)
        (shadow / "crossbow_socket_d78_test.cpp").write_text((out / "crossbow_socket_d78_test.cpp").read_text())
        build_run(name, shadow, ["-O0", "-g"], True)
    shadow = out / "heavy_spatial_requires_old_relative"
    shadow.mkdir(exist_ok=True)
    current_mechanism = (out / "crossbow_mechanism_d78_actual.inc").read_text()
    prior = "capture.character!=r.character||capture.skeleton!=r.skeleton||"
    assert current_mechanism.count(prior)==1
    (shadow / "crossbow_mechanism_d78_actual.inc").write_text(current_mechanism.replace(prior,prior+"std::memcmp(&capture.relative,&r.relative,sizeof(r.relative))||"))
    (shadow / "crossbow_pose_d78_actual.inc").write_text(actual)
    (shadow / "crossbow_pose_xr34_test.cpp").write_text(base_fixture)
    (shadow / "crossbow_socket_d78_test.cpp").write_text((out / "crossbow_socket_d78_test.cpp").read_text())
    build_run("heavy_spatial_requires_old_relative", shadow, ["-O0", "-g"], True)
    shadow = out / "old_henry_publication_dependency"
    shadow.mkdir(exist_ok=True)
    old_mechanism = (out / "crossbow_mechanism_d78_actual.inc").read_text()
    assert old_mechanism.count("!xcXR34::ReadMechanismWitnessD86(passenger,now)") == 1
    old_mechanism = old_mechanism.replace("!xcXR34::ReadMechanismWitnessD86(passenger,now)", "!xcXR34::ReadPassenger(passenger,now)||passenger.bootstrap")
    assert old_mechanism.count("!ReloadMechanismCurrentD86(passenger,now)") == 2
    old_mechanism = old_mechanism.replace("!ReloadMechanismCurrentD86(passenger,now)", "!ReloadPassengerCurrentXR34(passenger,source.character,source.body,now)")
    (shadow / "crossbow_mechanism_d78_actual.inc").write_text(old_mechanism)
    (shadow / "crossbow_pose_d78_actual.inc").write_text(actual)
    (shadow / "crossbow_pose_xr34_test.cpp").write_text(base_fixture)
    (shadow / "crossbow_socket_d78_test.cpp").write_text((out / "crossbow_socket_d78_test.cpp").read_text())
    build_run("old_henry_publication_dependency", shadow, ["-O0", "-g"], True)
    shadow = out / "old_completed_body_observer_dependency"
    shadow.mkdir(exist_ok=True)
    current_mechanism = (out / "crossbow_mechanism_d78_actual.inc").read_text()
    assert current_mechanism.count("source=passenger.frame;") == 1
    (shadow / "crossbow_mechanism_d78_actual.inc").write_text(current_mechanism.replace(
        "source=passenger.frame;", 'source=passenger.frame;if(!RangedSourceXR34(source)){note("CURRENT_SOURCE");return;}'))
    (shadow / "crossbow_pose_d78_actual.inc").write_text(actual)
    (shadow / "crossbow_pose_xr34_test.cpp").write_text(base_fixture)
    (shadow / "crossbow_socket_d78_test.cpp").write_text((out / "crossbow_socket_d78_test.cpp").read_text())
    build_run("old_completed_body_observer_dependency", shadow, ["-O0", "-g"], True)
    shadow = out / "old_heavy_exact_source_writer"
    shadow.mkdir(exist_ok=True)
    (shadow / "crossbow_pose_d78_actual.inc").write_text(actual.replace(
        "!ReadReloadPassengerXR34(passenger,source.character,source.body,GetTickCount64())", "!xcXR34::ReadPassenger(passenger,GetTickCount64())"))
    (shadow / "crossbow_pose_xr34_test.cpp").write_text(base_fixture)
    (shadow / "crossbow_socket_d78_test.cpp").write_text((out / "crossbow_socket_d78_test.cpp").read_text())
    build_run("old_heavy_exact_source_writer", shadow, ["-O0", "-g"], True)
    paths = [ranged_path, mechanism_path, ranged_path.with_name("crossbow_native_xr34.h"), ranged_path.with_name("reload_diagnostics_d48.h"), ranged_path.with_name("crossbow_clock_entry_d87.h"), ROOT / "tests/crossbow_socket_d78_test.cpp",
             ROOT / "tests/crossbow_pose_xr34_test.cpp", Path(__file__).resolve()]
    report = {"passed": all(r["passed"] for r in runs), "runs": runs,
              "source_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
              "native_lookup_replaced": [{"signature": "RangedBoundToolIdentityD72", "original_sha256": hashlib.sha256(native_lookup.encode()).hexdigest()},
                                         {"signature": "RangedWeaponPoseIdentityD78", "original_sha256": hashlib.sha256(weapon_identity.encode()).hexdigest()}],
              "limits": ["Direct retail lookup calls, native pose getter and job scheduling are explicit fixture leaves.",
                         "Production mechanism witness/source mailboxes, descriptor reads, observer publication, mounted/hand target math, PRS writer selection and readback admission execute unchanged; live native clock identity is an explicit leaf.",
                         "No game executable, Windows SEH, renderer, headset, animation progression or inventory outcome validation.",
                         "MSVC AddressSanitizer enabled; UBSan unavailable." if msvc and args.msvc_asan else "ASan/UBSan not run with MSVC." if msvc else "ASan and UBSan enabled; LeakSanitizer disabled on this host."]}
    (out / "RESULT.json").write_text(json.dumps(report, indent=2) + "\n")


if __name__ == "__main__":
    main()
