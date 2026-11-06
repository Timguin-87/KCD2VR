#!/usr/bin/env python3
"""Run the actual crossbow source publication API and its old-hole mutation."""
from pathlib import Path
import argparse, hashlib, json, os, subprocess
from host_compiler import command, executable_path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',required=True,type=Path);ap.add_argument('--compiler',default='cl');a=ap.parse_args()
 out=a.out.resolve();out.mkdir(parents=True,exist_ok=True);runs=[]
 source=ROOT/'tests/crossbow_source_publication_r93_test.cpp';header=ROOT/'src/subsystems/ranged_reload/crossbow_native_xr34.h'
 for name,opt,negative in [('debug','/Od',False),('optimized','/O2',False),('old_refresh_withdrawal','/Od',True)]:
  folder=out/name;folder.mkdir(exist_ok=True);fixture=source.read_text()
  if negative:
   h=header.read_text().replace('    if(sourceWriterR93.test_and_set', '    sourceInterested.store(false,std::memory_order_release);\n    if(sourceWriterR93.test_and_set',1)
   for relative in ['../../vrik/instant_activities_d72.h','../../core/snapshot_mailbox.h','reload_diagnostics_d48.h','options_d65c.h','crossbow_clock_entry_d87.h','crossbow_refusal_d85.h','crossbow_constraint_d78.h']:
    h=h.replace('"'+relative+'"','"'+(header.parent/relative).resolve().as_posix()+'"')
   (folder/'mutated.h').write_text(h);fixture=fixture.replace('"subsystems/ranged_reload/crossbow_native_xr34.h"','"mutated.h"')
  test=folder/'test.cpp';test.write_text(fixture);exe=executable_path(folder/'test')
  cmd=command(test,exe,[ROOT/'src',ROOT/'tests/syntax_shims'],a.compiler)
  cmd=[opt if c=='/O2' else c for c in cmd]
  b=subprocess.run(cmd,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(folder/'build.log').write_text(b.stdout)
  if b.returncode:raise RuntimeError(b.stdout)
  r=subprocess.run([str(exe)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(folder/'run.log').write_text(r.stdout)
  ok=r.returncode==1 and 'FAIL_R93' in r.stdout if negative else r.returncode==0
  runs.append(dict(name=name,passed=ok,expected_failure=negative,output=r.stdout,command=cmd));print(name+': '+r.stdout,flush=True)
  if not ok:raise RuntimeError(name+' failed')
 (out/'RESULT.json').write_text(json.dumps(dict(passed=True,runs=runs,source_sha256={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [source,header,Path(__file__)]},limits=['Actual publication API. Scheduling interleaved at registration callback; no game or headset outcome asserted.']),indent=2))
if __name__=='__main__':main()
