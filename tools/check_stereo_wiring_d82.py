#!/usr/bin/env python3
"""Check D82 source integration contracts; this is not GPU/Windows execution."""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
from check_stereo_menu_d82 import function

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args(); args.out.mkdir(parents=True, exist_ok=True)
    paths = ['src/dllmain.cpp', 'src/mod.cpp', 'src/device_hook.cpp', 'src/native_upscaler_stereo_d82.cpp',
             'src/native_stereo_preflight_v6602.cpp', 'src/native_stereo_eye_control_v6604.inc',
             'src/temporal_compose_publication_d82.h', 'src/subsystems/base/startup_setup_s15.inl',
             'cmake/module_sources.cmake', 'cmake/module_groups_m2.json']
    source = {path:(ROOT/path).read_text() for path in paths}
    checks = []

    def check(name, passed, path):
        checks.append(dict(name=name, passed=bool(passed), source=path))

    worker = function(source['src/dllmain.cpp'], 'static DWORD WINAPI ModThreadMain(')
    dllmain = function(source['src/dllmain.cpp'], 'BOOL APIENTRY DllMain(')
    check('Worker logging precedes early capture, plugins and companion startup',
          worker.index('unified_log_d81::Initialize()') < worker.index('InstallEarly(wanted,alternate)') < worker.index('LoadAsiPlugins()') < worker.index('companion_loader59c4::Install()'), 'src/dllmain.cpp')
    check('DllMain creates the worker and does not install vendor hooks', 'CreateThread(nullptr, 0, ModThreadMain' in dllmain and 'InstallEarly' not in dllmain, 'src/dllmain.cpp')
    # R149: production mode 4 (DLSS alternate-eye) also captures the paired DLSS creation.
    check('Early capture honors persisted Enabled and production mode',
          'const int mode=kcd2vr::startup_s15::ProductionMode(ini);' in worker and 'const bool alternate=mode==4;' in worker and
          'ReadBool(ini,L"Enabled",true)&&(mode==3||alternate)' in worker, 'src/dllmain.cpp')
    apply = function(source['src/mod.cpp'], 'bool Mod::ApplyMenuState(')
    check('Menu saves requested mode without directly starting stage4',
          # R149: only alternate-eye (mode 4) starts AFR (stereo 1) directly; native stage4 is never started here.
          'const int stereo = alternate_eye ? 1 : 0;' in apply and 'const bool alternate_eye = state.stereo_mode==4;' in apply and
          'vr_production_stereo_mode_.store(requested_stereo_mode' in apply and 'StartStereoDesign(' not in apply and 'SetStereoExperiment(2' not in apply, 'src/mod.cpp')
    check('Existing settings writer persists the requested mode for next-launch capture',
          'ok = wi("VrStereoExperiment", vr_production_stereo_mode_.load(std::memory_order_acquire)) && ok;' in source['src/mod.cpp'], 'src/mod.cpp')
    modrun = function(source['src/mod.cpp'], 'void Mod::Run(')
    check('Config load is followed by capture retry before startup dialog', modrun.index('if (!LoadConfig())') < modrun.index('InstallEarly(') < modrun.index('startup_s15::FirstRun()'), 'src/mod.cpp')
    check('Pair provider reads actual native technology with an unknown fallback', 'SetTechnologyReader(+[]() noexcept' in modrun and 'return value?SafeCVarGetInt(value,-1):-1;' in modrun, 'src/mod.cpp')
    check('Production start tests current technology readiness before stage4', 'native_upscaler_stereo_d82::CanEnterStereo(SafeCVarGetInt(nativeUpscalerCvarD82.load(std::memory_order_acquire),-1)))' in modrun and modrun.index('native_upscaler_stereo_d82::CanEnterStereo(') < modrun.index('native_stereo_outer_lab::StartStereoDesign(4'), 'src/mod.cpp')
    device = source['src/device_hook.cpp']
    queue = function(device, 'void __stdcall DeviceHook::DetourExecuteCommandLists(')
    submitted = queue.index('s_original_execute_command_lists(queue_this, num_command_lists, command_lists);')
    ready = queue.index('const bool d82_sdk_ready=')
    claim = queue.index('temporal_pair_d85::ClaimRightCopy(')  # D85: strict D82 claim + bounded fallback
    at = queue.index('const bool temporal_ok=(d82_temporal&&!d85_via_fallback)')
    check('Queue submits the game list before checking exact composition receipts and claiming RIGHT',
          submitted < ready < claim < at and 'PairReady(d82_composition.epoch,d82_composition.frame)' in queue[ready:at], 'src/device_hook.cpp')
    check('Queue rechecks temporal receipt before both-eye publication lock',
          'PairReady(d82_composition.epoch,d82_composition.frame)' in queue[at:at+500] and
          queue.index('if(right_ok && temporal_ok)',at) < queue.index('std::lock_guard<std::mutex> lock(s_bridge_capture_mutex)',at) < queue.index('s_bridge_left_submitted_serial.store(',at), 'src/device_hook.cpp')
    candidate = function(device, 'temporal_compose_d82::Stamp V6615IncomingRightCompositionD82(')
    check('Composition receipt is taken from the actual submitted command-list generation',
          't.list!=reinterpret_cast<ID3D12GraphicsCommandList*>(lists[i])' in candidate and
          't.temporal_recording.SubmittedCandidate(' in candidate, 'src/device_hook.cpp')
    compose = function(source['src/native_stereo_eye_control_v6604.inc'], 'void __fastcall ComposeCallDetour(')
    check('Temporal composition scope requires exact retail caller and owned RIGHT view',
          'caller==g_wh+0x864659' in compose and 't_d82_final_compose_flow==f' in compose and
          'view.eye==1&&view.epoch==f->epoch&&view.frame==f->frame&&view.pipeline==f->pipeline' in compose and
          'EndTemporalCompositionD82()' in compose, 'src/native_stereo_eye_control_v6604.inc')
    check('Native stage checks exact epoch/frame before explicit capture', 'native_upscaler_stereo_d82::PairReady(f.epoch,f.frame)' in source['src/native_stereo_eye_control_v6604.inc'], 'src/native_stereo_eye_control_v6604.inc')
    check('Both preflight start paths publish the new epoch to the provider', source['src/native_stereo_preflight_v6602.cpp'].count('native_upscaler_stereo_d82::BeginEpoch(epochD82);')==2, 'src/native_stereo_preflight_v6602.cpp')
    aggregate = source['src/native_upscaler_stereo_d82.cpp']
    for name in ['InstallEarly', 'SetMode']:
        body = function(aggregate, 'void '+name+'(')
        # R149: alternate-eye is DLSS-only; FSR/XeSS twins stay off in that mode.
        check(name+' enables both providers independently of selected technology', 'native_dlss_stereo_d82::SetEnabled(wanted);' in body and 'native_fsr_stereo_d82::SetEnabled(wanted&&!alternate);' in body and 'native_dlss_stereo_d82::SetAlternateEyeMode(alternate);' in body, 'src/native_upscaler_stereo_d82.cpp')
    pair = function(aggregate, 'bool PairReady(')
    check('Provider pins technology to the epoch before considering Native passthrough', 'const int tech=ActualTechnology();' in pair and 'pairEpoch!=bound||tech!=epochTechnology.load(std::memory_order_acquire)' in pair and pair.index('tech!=epochTechnology.load') < pair.index('if(tech==0)return true;'), 'src/native_upscaler_stereo_d82.cpp')
    begin = function(aggregate, 'void BeginEpoch(')
    check('New epoch snapshots actual technology before publishing the epoch number', begin.index('epochTechnology.store(ActualTechnology()') < begin.index('epoch.store(value'), 'src/native_upscaler_stereo_d82.cpp')
    check('FSR1 and DLSS2 use exact epoch/frame receipts with other modes refused', 'if(tech==2)return native_dlss_stereo_d82::PairReady(pairEpoch,frame);' in pair and 'if(tech==1)return native_fsr_stereo_d82::PairReady(pairEpoch,frame);' in pair and pair.rstrip().endswith('return false;\n}'), 'src/native_upscaler_stereo_d82.cpp')
    check('Startup Keep list is selected by actual config compatibility', 'technologiesForProduction(s->productionMode,LaunchCompatible(*s,false))' in source['src/subsystems/base/startup_setup_s15.inl'], 'src/subsystems/base/startup_setup_s15.inl')
    for path in ['src/native_dlss_stereo_d82.cpp', 'src/native_fsr_stereo_d82.cpp', 'src/native_upscaler_stereo_d82.cpp']:
        check(path+' is included exactly once in production source ownership', source['cmake/module_sources.cmake'].count(path)==1 and source['cmake/module_groups_m2.json'].count('"'+path+'"')==1, 'cmake/module_sources.cmake')
    report = dict(passed=all(item['passed'] for item in checks), checks=checks, source_sha256={path:hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
                  limits=['Source integration checks only; no native Windows compile/link, hook execution, vendor GPU or headset execution.', 'SDK completion before the selected GPU queue boundary and actual depth/motion-vector content require runtime verification.'])
    (args.out/'WIRING.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(dict(passed=report['passed'], checks=len(checks), failures=[c for c in checks if not c['passed']]), indent=2))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
