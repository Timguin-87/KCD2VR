#!/usr/bin/env python3
"""Current production interaction recovery adapters and targeted negative controls.

Native engine/memory/render/ExecuteBuffer leaves are substituted explicitly.
No Windows SEH, DLL, retail Lua VM, game, or headset execution is claimed.
"""
from pathlib import Path
import argparse
import concurrent.futures
import hashlib
import json
import re
import subprocess
from check_interactions_d84 import extract

ROOT = Path(__file__).resolve().parents[1]


def replace_one(text, old, new):
    if text.count(old) != 1:
        raise ValueError(f"Mutation requires one exact current source match: {old!r}")
    return text.replace(old, new, 1)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    out = a.out.resolve(); out.mkdir(parents=True, exist_ok=True)
    prop = ROOT/'src/vrik/staged_pickup596c2.inl'
    text = prop.read_text()
    consumer = out/'prop_current.inc'; consumer.write_text(extract(prop))
    build = out/'build_current.inc'
    build.write_text(text[text.index('static bool BuildStagedPickup596('):text.index('static bool TryStagedPickup596(')])
    lab = (ROOT/'src/vrik/lab_prop58.inl').read_text()
    start = lab.index('namespace lab_prop58n {'); end = lab.index('// <game>/Mods', start)
    tick = lab.index('static void Tick(', end); close = lab.index('} // namespace lab_prop58n', tick)
    lab_adapter = out/'lab_current.inc'
    lab_adapter.write_text(lab[start:end] + 'static bool LabPakPresent() noexcept {return P::pakState.load()==1;}\n'
                           'static void Receipt(std::uint64_t,const char*,bool) noexcept {}\n' + lab[tick:close] + '}\n')
    # Only native manifest/log leaves above are substituted. Full Tick, fault
    # recovery, impact Seen and budgets remain literal current production.
    mutations = []
    gap = out/'prop_gap_mutation.inc'
    gap.write_text(replace_one(consumer.read_text(), 'if(observationGapR96&&p.sourceTickR96', 'if(false&&observationGapR96&&p.sourceTickR96'))
    mutations.append(('negative_prop_gap', 'prop_recovery_r96_test.cpp', {'PROP_ADAPTER_D78_INC':gap, 'PICKUP_BUILD_D78_INC':build}))
    clock = out/'build_clock_mutation.inc'
    clock.write_text(replace_one(build.read_text(), 'now=std::uint64_t(GetTickCount64()); // sample AFTER these exact copied sources/native reads',
                                '/* negative control: retain earlier callback clock */'))
    mutations.append(('negative_prop_clock', 'prop_recovery_r96_test.cpp', {'PROP_ADAPTER_D78_INC':consumer, 'PICKUP_BUILD_D78_INC':clock}))
    mortar = out/'mortar_clock_mutation.inc'
    mortar_text = (ROOT/'src/subsystems/alchemy/native_mortar69.inl').read_text()
    mortar.write_text(replace_one(mortar_text,
        'const bool haveFrameR96=kcd2vr::frik_pose_probe::WorkstationFrame46(f);now=std::uint64_t(GetTickCount64());',
        'const bool haveFrameR96=kcd2vr::frik_pose_probe::WorkstationFrame46(f);'))
    mutations.append(('negative_mortar_clock', 'mortar_recovery_r96_test.cpp', {'KCD2VR_NATIVE_MORTAR69_HEADER':mortar}))
    held = out/'held_retirement_mutation.inc'
    held_text = (ROOT/'src/subsystems/physics/held_attachment_owner.inl').read_text()
    held.write_text(replace_one(held_text,
        'if(a.owner.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||',
        'if(false&&(a.owner.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||').replace(
        'a.owner.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)){heldT4::Retire',
        'a.owner.epoch!=g_pose_epoch6689.load(std::memory_order_acquire))){heldT4::Retire', 1))
    mutations.append(('negative_held_retirement', 'held_writer_recovery_r96_test.cpp', {'KCD2VR_HELD_ATTACHMENT_R96':held}))
    door_source = ROOT/'src/subsystems/doors/hand_only_h40.h'
    door = out/'door_clock_mutation.h'
    door_text = replace_one(door_source.read_text(), 'return fn?fn():entry;', 'return entry;')
    # Preserve dependency identity when the literal header is outside its tree.
    door_text = re.sub(r'#include "([^"]+)"', lambda m:'#include "'+str((door_source.parent/m[1]).resolve())+'"', door_text)
    door.write_text(door_text)
    mutations.append(('negative_door_clock', 'door_recovery_r96_test.cpp', {'KCD2VR_HAND_DOOR_HEADER_R96':door}))
    lab_mutation = out/'lab_quarantine_mutation.inc'
    lab_mutation.write_text(replace_one(lab_adapter.read_text(), 'c.latched=P::faultBlockedR96();', 'c.latched=P::latched.load();'))
    mutations.append(('negative_lab_quarantine', 'lab_prop_recovery_r96_test.cpp', {'KCD2VR_LAB_PROP_R96':lab_mutation}))
    cases = [
        ('script', 'native_script_recovery_r96_test.cpp', {}),
        ('prop', 'prop_recovery_r96_test.cpp', {'PROP_ADAPTER_D78_INC':consumer,'PICKUP_BUILD_D78_INC':build}),
        ('mortar', 'mortar_recovery_r96_test.cpp', {}),
        ('held', 'held_writer_recovery_r96_test.cpp', {}),
        ('door', 'door_recovery_r96_test.cpp', {}),
        ('lab', 'lab_prop_recovery_r96_test.cpp', {'KCD2VR_LAB_PROP_R96':lab_adapter}),
        ('instant', 'instant_pickup_d72_test.cpp', {}),
    ]
    jobs = [(name+'_'+mode, test, macros, flag, False) for name,test,macros in cases
            for mode,flag in [('debug','-O0'),('optimized','-O2')]]
    jobs += [(name,test,macros,'-O2',True) for name,test,macros in mutations]
    jobs += [('mortar_retained','subsystems/alchemy/native_mortar69_test.cpp',{},'-O2',False),
             ('mortar_withdraw_retained','alchemy_mortar_withdraw_native_d78r2_test.cpp',{},'-O2',False),
             ('door_press_retained','door_press_why_r9_test.cpp',{},'-O2',False),
             ('prop_clock_retained','prop_clock_r92r2_test.cpp',{'PROP_ADAPTER_D78_INC':consumer},'-O2',False),
             ('small_pickup_current','small_pickup_d81_test.cpp',{},'-O2',False)]

    def run(job):
        name,test,macros,opt,negative=job
        exe=out/name; dep=out/(name+'.d')
        command=['g++','-std=c++20','-UNDEBUG','-pthread','-D__fastcall=',opt,'-g0','-MMD','-MF',str(dep)]
        command += ['-I'+str(ROOT/x) for x in ('src','src/vrik','tests','tests/syntax_shims','tests/openxr_sdk')]
        command += ['-D'+key+'="'+str(value)+'"' for key,value in macros.items()]
        command += [str(ROOT/'tests'/test),'-o',str(exe)]
        compiled=subprocess.run(command,capture_output=True,text=True)
        executed=subprocess.run([str(exe)],capture_output=True,text=True) if not compiled.returncode else None
        output=compiled.stdout+compiled.stderr+(executed.stdout+executed.stderr if executed else '')
        (out/(name+'.log')).write_text(output)
        # A negative control must compile and reach an assertion/check failure;
        # a crash, timeout, compiler failure or missing input is never a pass.
        rejected=bool(executed and ((executed.returncode==1 and 'FAIL' in output) or
                      (executed.returncode==-6 and 'Assertion' in output)))
        passed=compiled.returncode==0 and executed is not None and (rejected if negative else executed.returncode==0)
        row={'name':name,'passed':passed,'compile_exit':compiled.returncode,
             'run_exit':executed.returncode if executed else None,'expected_assertion_rejection':negative,
             'output':(executed.stdout+executed.stderr).strip() if executed else compiled.stderr,
             'command':command,'log':str(out/(name+'.log'))}
        print(name+': '+('PASS' if passed else 'FAIL'),flush=True)
        return row

    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        rows=list(pool.map(run,jobs))
    frik=(ROOT/'src/frik_pose_probe.cpp').read_text()
    setter=frik[frik.index('static void SetWeaponControlModeImpl('):frik.index('static void SetWeaponControlModeImpl(')+8000]
    callback=frik[frik.index('static void __fastcall EntityAttachmentUpdateDetourV6688('):]
    setter_ok=setter.index('g_weapon_request_epoch6694.fetch_add') < setter.index('NativeHeldInvalidateT4();') < setter.index('g_weapon_control_mode.store')
    order_ok=callback.index('NativeHeldAttachmentT4(self,attachment)') < callback.index('PhysicalAttachmentResult101::Busy)return;') < callback.index('CrossbowAttachmentXR34(self,attachment)')
    door_clock='clockR96.store(&NativeClockR96,std::memory_order_release)' in (ROOT/'src/subsystems/doors/hand_only_h40_service.inl').read_text()
    rows.append({'name':'actual_shared_integration_source_order','passed':setter_ok and order_ok and door_clock,
                 'kind':'source inspection, not execution','mode_cancel_after_request_before_publish':setter_ok,
                 'unretired_native_collider_blocks_crossbow':order_ok,'door_native_clock_connected':door_clock})
    dependencies={ROOT/'src/frik_pose_probe.cpp', ROOT/'src/subsystems/doors/hand_only_h40_service.inl',
                  ROOT/'src/subsystems/physics/native_held_owner_t4.inl',prop,ROOT/'src/vrik/lab_prop58.inl',Path(__file__),ROOT/'src/build_identity.h'}
    for dep in out.glob('*.d'):
        data=dep.read_text().replace('\\\n',' ')
        for word in data.split(':',1)[1].split():
            path=Path(word)
            if path.is_file(): dependencies.add(path.resolve())
    report={'passed':all(r['passed'] for r in rows),'results':rows,
            'source_sha256':{str(x.relative_to(ROOT)):hashlib.sha256(x.read_bytes()).hexdigest() for x in sorted(dependencies) if x.is_relative_to(ROOT)},
            'generated_sha256':{str(x.relative_to(out)):hashlib.sha256(x.read_bytes()).hexdigest() for x in sorted(dependencies) if x.is_relative_to(out)},
            'native_engine_execution':False,'Windows_SEH_executed':False,'DLL_built':False,'game_headset_executed':False,
            'retail_AnimDoor_Lua_rerun':'NOT_RUN: current inputs do not include retail Scripts.pak',
            'scope':'Actual production adapters; only described native/memory/render/transport leaves substituted. No successful native engine accounting is inferred from host results.'}
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f"R96 interactions: {sum(r['passed'] for r in rows)}/{len(rows)} entries passed",flush=True)
    return 0 if report['passed'] else 1


if __name__=='__main__':
    raise SystemExit(main())
