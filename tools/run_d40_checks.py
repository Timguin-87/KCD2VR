#!/usr/bin/env python3
"""Integrated D40 source/host gates. No game code is loaded or executed.

Requires Python 3 and GCC/G++ on Linux/WSL; the companion Lua fixture requires
liblua5.4. --with-packaging additionally requires CMake/Make. --dll verifies the
exact inspected retail DLL and runs the new native-turn adapter. Supplying an
immutable D39 --baseline-source adds behavioral negative controls. An unavailable
Windows/headset environment is always recorded, never relabelled a pass.
"""
from pathlib import Path
import argparse, hashlib, json, os, subprocess, sys, time
ROOT=Path(__file__).resolve().parents[1]
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out',type=Path,required=True);p.add_argument('--jobs',type=int,default=2)
    p.add_argument('--with-packaging',action='store_true');p.add_argument('--cmake',default='cmake')
    p.add_argument('--dll',type=Path);p.add_argument('--baseline-source',type=Path)
    a=p.parse_args();out=a.out.resolve()
    if out==ROOT or ROOT in out.parents:p.error('--out must be outside the source tree')
    out.mkdir(parents=True,exist_ok=True);py=sys.executable
    def tool(name,*args):return [py,str(ROOT/'tools'/name),*(str(x) for x in args)]
    retained=tool('run_d39_checks.py','--out',out/'retained','--jobs',a.jobs)
    if a.with_packaging:retained+=['--with-packaging','--cmake',a.cmake]
    groups=[('retained_foundation',retained),('live_runtime',tool('run_live_runtime_d40.py','--out',out/'live_runtime')),
            ('clinch_intent',tool('run_clinch_d40.py','--out',out/'clinch')),
            ('npc_physics',tool('run_npc_physics_d40.py','--output',out/'npc_physics')),
            ('pickup',tool('run_pickup_d40.py','--out',out/'pickup','--jobs',a.jobs))]
    if a.baseline_source:
        groups[-2][1].extend(['--baseline',str(a.baseline_source.resolve())])
        groups[-1][1].extend(['--baseline-source',str(a.baseline_source.resolve())])
    skipped=[]
    if a.dll:
        rotation=tool('run_combat_no_turn_d40.py','--dll',a.dll.resolve(),'--out',out/'combat_turn')
        if a.baseline_source:rotation+=['--baseline',str(a.baseline_source.resolve())]
        groups.append(('combat_rotation',rotation))
    else:skipped.append({'name':'combat_rotation_and_native_pins','reason':'Supply --dll with the exact reviewed WHGame.dll; no game binary is bundled.'})
    rows=[]
    for name,command in groups:
        start=time.monotonic();log=out/(name+'.log')
        print(name+': running',flush=True)
        with log.open('w') as stream:
            try:
                r=subprocess.run(command,cwd=ROOT,stdout=stream,stderr=subprocess.STDOUT,timeout=1800,
                                 env=dict(os.environ,PYTHONDONTWRITEBYTECODE='1'))
                row={'name':name,'command':command,'exit':r.returncode,'passed':r.returncode==0}
            except (OSError,subprocess.TimeoutExpired) as e:row={'name':name,'command':command,'passed':False,'error':str(e)}
        row.update(log=str(log),seconds=round(time.monotonic()-start,3));rows.append(row)
        print(name+': '+('PASS' if row['passed'] else 'FAIL'),flush=True)
    receipt={'selected_groups_passed':all(x['passed'] for x in rows),'results':rows,'not_run':skipped,
             'scope':'Focused production host/source adapters and companion checks, not the entire historical suite.',
             'native_windows_build':'NOT_RUN','windows_menu_rendering':'NOT_RUN','native_game':'NOT_RUN','headset':'NOT_RUN',
             'build_identity':(ROOT/'src/build_identity.h').read_text().strip(),
             'runner_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    (out/'D40_RESULTS.json').write_text(json.dumps(receipt,indent=2)+'\n')
    print(str(sum(x['passed'] for x in rows))+'/'+str(len(rows))+' selected groups passed; '+str(len(skipped))+' optional groups not run')
    return 0 if receipt['selected_groups_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
