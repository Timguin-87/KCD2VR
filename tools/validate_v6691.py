#!/usr/bin/env python3
"""Execute current V66.91 diagnostic contracts. NOT native MSVC or headset proof."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,importlib.util,json,re,shutil,struct,subprocess,sys,tempfile
R=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('previous',R/'tools/validate_v6690.py');previous=importlib.util.module_from_spec(spec);spec.loader.exec_module(previous)
old=previous.old

def rtti_check(path,manifest):
    data=path.read_bytes();pe=struct.unpack_from('<I',data,60)[0];num=struct.unpack_from('<H',data,pe+6)[0];opt=struct.unpack_from('<H',data,pe+20)[0];base=struct.unpack_from('<Q',data,pe+48)[0];sections=[]
    for i in range(num):
        j=pe+24+opt+i*40;_,a,n,p=struct.unpack_from('<IIII',data,j+8);sections.append((a,n,p))
    def read(a,n):
        for va,size,p in sections:
            if va<=a and a+n<=va+size:return data[p+a-va:p+a-va+n]
        raise ValueError('unmapped native RTTI')
    source=read_source(R/'src/diagnostics/combo_snapshot.h')
    for v in manifest['rtti']:
        address=int(v['vft'],16);col=struct.unpack('<Q',read(address-8,8))[0]-base;off=struct.unpack('<I',read(col+4,4))[0];td=struct.unpack('<I',read(col+12,4))[0]
        assert off==v['object_offset'] and read(td+16,len(v['name'])+1)==v['name'].encode()+b'\0',v
        found=re.search(v['layout_field']+r'=(0x[0-9a-fA-F]+)',source);assert found and int(found[1],16)==address,'decoder vtable changed'
    return len(manifest['rtti'])

def mutations(out):
    jobs=[('wrong_henry_adopted','combo_snapshot.h','(expectedEntity&&i.entity!=expectedEntity)','false'),('wrong_state_stride','combo_snapshot.h','VectorCount(v,40,64','VectorCount(v,32,64'),('truncation_treated_complete','combo_snapshot.h','&&!a.truncated&&!b.truncated',''),('native_called_twice','combo_observe.h','    original();','    original();original();')]
    results=[]
    for name,file,find,replace in jobs:
        with tempfile.TemporaryDirectory() as tmp:
            t=Path(tmp);(t/'tests').mkdir();shutil.copytree(R/'src/diagnostics',t/'src/diagnostics');shutil.copy2(R/'tests/combo_snapshot_test.cpp',t/'tests/combo_snapshot_test.cpp');p=t/'src/diagnostics'/file;s=p.read_text();assert find in s;p.write_text(s.replace(find,replace,1))
            c=subprocess.run(['g++','-std=c++17',str(t/'tests/combo_snapshot_test.cpp'),'-o',str(t/'test')],capture_output=True,text=True);assert c.returncode==0,c.stderr
            test=subprocess.run([str(t/'test')],capture_output=True,text=True);(out/f'mutant_{name}.log').write_text(test.stdout+test.stderr);assert test.returncode!=0,name+' survived'
            results.append({'mutant':name,'compiled':True,'tests_rejected':True})
    return results

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--whgame',type=Path);ap.add_argument('--simple-pak',type=Path);ap.add_argument('--build-dir',type=Path,default=R/'build-contracts');ap.add_argument('--skip-host-shims',action='store_true');ap.add_argument('--skip-mutations',action='store_true');a=ap.parse_args();out=R/'delivery';out.mkdir(exist_ok=True)
    report={'build':'V66.91','native_windows_compile':'NOT_RUN','headset':'NOT_RUN','physical_combo_recognition':'NOT_IMPLEMENTED','native_damage_injection':'NONE'}
    for name,expected in old.generated().items():assert (R/name).read_text()==expected,'stale extracted adapter '+name
    for name in ('FROZEN_SOURCE_SHA256.json','V66.89_PRESERVED_SOURCE_SHA256.json','V66.90_PRESERVED_SOURCE_SHA256.json'):
        m=json.loads((R/'continuity'/name).read_text());report[name]=len(m)
        for path,h in m.items():assert hashlib.sha256((R/path).read_bytes()).hexdigest()==h,'frozen change '+path
    marker='KCD2VR-V66.91-COMBO-PROGRESSION-DIAGNOSTICS'
    for name in ('src/mod.cpp','build_release.bat'):assert marker in (R/name).read_text(),name
    if a.whgame:
        report['prior_native']=old.native_check(a.whgame);report['playback_native']=previous.checks(a.whgame,R/'continuity/native_v6690/ANIMATION_QUEUE_ABI.json');manifest=R/'continuity/native_v6691/COMBO_MANAGER_ABI.json';report['combo_native']=previous.checks(a.whgame,manifest);report['combo_native']['rtti_identity_checks']=rtti_check(a.whgame,json.loads(manifest.read_text()))
    else:report['native']='NOT_RUN --whgame required'
    profiles=json.loads((R/'continuity/semantic_index/combo/recipe_profiles.json').read_text());report['simple_combos_comparison']=profiles['summary']
    if a.simple_pak:assert hashlib.sha256(a.simple_pak.read_bytes()).hexdigest()==profiles['source']['archive_sha256'],'Simple PAK provenance';report['simple_archive_hash']='PASS'
    execute=old.execute
    execute(['cmake','-S',R/'tests','-B',a.build_dir,'-DCMAKE_BUILD_TYPE=Release'],out/'contracts_configure.log');execute(['cmake','--build',a.build_dir,'--parallel','4'],out/'contracts_build.log');text=execute(['ctest','--test-dir',a.build_dir,'--output-on-failure','-V'],out/'contracts_ctest.log')
    counts=re.findall(r': [A-Za-z0-9_]+=(\d+)/(\d+)',text);assert len(counts)==6 and all(x==y for x,y in counts),counts;report['cpp_test_groups']=len(counts);report['cpp_assertions']=sum(int(x) for x,y in counts)
    report['python_tests']={}
    for name in ('test_diagnostic_report.py','test_combo_report.py'):
        text=execute([sys.executable,R/'tests'/name,'-v'],out/(name+'.log'));m=re.search(r'Ran (\d+) tests',text);assert m;report['python_tests'][name]=int(m[1])
    # Sanitizers exercise portable decoder/wrapper only, not native Windows SEH.
    with tempfile.TemporaryDirectory() as tmp:
        t=Path(tmp);execute(['g++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer',R/'tests/combo_snapshot_test.cpp','-o',t/'asan'],out/'combo_sanitizer_compile.log');execute([t/'asan'],out/'combo_sanitizer_run.log');report['combo_host_sanitizers']='ASAN_UBSAN_PASS'
    if not a.skip_host_shims:
        with tempfile.TemporaryDirectory() as tmp:
            t=Path(tmp);objs=[]
            for name in ('frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace'):
                obj=t/(name+'.o');execute(['g++','-std=c++17','-O0','-Wformat=2','-Werror=format','-I'+str(R/'tests/syntax_shims'),'-I'+str(R/'src'),'-c',R/'src'/f'{name}.cpp','-o',obj],out/f'{name}_host_object.log');objs.append(obj)
            (t/'main.cpp').write_text('int main(){return 0;}\n');execute(['g++',*objs,t/'main.cpp','-o',t/'mock_link'],out/'host_link.log')
        report['host_shim']='4 full TUs compile/link with explicit Windows/DetourModKit replacements; full mod.cpp NOT compiled; NOT MSVC'
    else:report['host_shim']='SKIPPED'
    report['mutations']=old.mutation_checks(out)+previous.mutants(out)+mutations(out) if not a.skip_mutations else 'SKIPPED'
    (out/'VERIFICATION.json').write_text(json.dumps(report,indent=2)+'\n');print('V66.91 verification PASS. Native Windows/injection/headset NOT tested.')
if __name__=='__main__':
    try:main()
    except Exception as e:print('FAIL:',e,file=sys.stderr);raise SystemExit(1)
