#!/usr/bin/env python3
"""Execute actual cache admission; replay R3 lookup/admission as a negative."""
import argparse,hashlib,json,re,subprocess
from pathlib import Path
import render_host_compiler as host
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--baseline',type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
def function(text,start):
    i=text.index(start);m=re.search(r'\)\s*(?:noexcept\s*)?\{',text[i:]);assert m
    k=i+m.end()-1;depth=1;j=k+1
    while depth:
        depth+=(text[j]=='{')-(text[j]=='}');j+=1
    return text[i:j]
current_device=(root/'src/device_hook.cpp').read_text();current_xr=(root/'src/xr_manager.cpp').read_text()
pub=function(current_device,'static void V6634PublishWriteSlotLocked(')
def build(label,source_root,before=False):
    out=a.out/label;out.mkdir(exist_ok=True)
    d=(source_root/'src/device_hook.cpp').read_text();x=(source_root/'src/xr_manager.cpp').read_text()
    access='\n'.join(function(d,s) for s in ('bool DeviceHook::TryGetBridgeCompletePair(','bool DeviceHook::ReadBridgePairIdentity59c6('))
    blit=function(x,'bool XrManager::BlitBridgePairStableToEyes(')
    # Execute literal admission through the first resource-descriptor read.
    admission=blit[:blit.index('    D3D12_RESOURCE_DESC left_desc{};')]+'    return true;\n}'
    commit=blit[blit.index('    if(refresh_pair) {\n        bridge_present_cache_valid_=true;'):]
    commit=commit[:commit.index('\n    static std::atomic<unsigned long long> stable_blits')]
    legacy=function(current_device,'BridgePairStateV6547& BridgePairStateForV6547(')
    for n,t in [('cache_publisher_actual.inc',pub),('cache_accessors_actual.inc',access),('cache_xr_admission_actual.inc',admission),('cache_commit_actual.inc',commit),('cache_legacy_state_actual.inc',legacy)]:
        (out/n).write_text(t+'\n')
    exe=host.binary(out/'cache_owner');cmd=['g++','-std=c++20','-O1','-Wall','-Wextra','-Wno-unused-parameter','-Wno-unused-variable','-pthread','-I'+str(root/'src'),'-I'+str(out)]
    if before:cmd+=['-DBEFORE_GUARDS']
    cmd += [str(root/'src/native_upscaler_stereo_d82.cpp'),str(root/'tests/temporal_cache_owner_d85_test.cpp'),'-o',str(exe)]
    r=subprocess.run(host.command(cmd),capture_output=True,text=True);(out/'build.log').write_text(r.stdout+r.stderr)
    assert r.returncode==0,(out/'build.log').read_text()
    r=subprocess.run([str(exe)],capture_output=True,text=True);(out/'run.log').write_text(r.stdout+r.stderr)
    assert r.returncode==(1 if before else 0),r.stdout+r.stderr
    if before:assert 'FAIL !Acquire()' in r.stderr and 'FAIL !XrAdmission(xr,false)' in r.stderr
    return {'exit':r.returncode,'output':r.stdout+r.stderr,'source_root':str(source_root),'command':cmd}
rows={'current':build('current',root)}
if a.baseline:rows['r3_negative']=build('r3_negative',a.baseline,True)
# Confirm the runtime call site passes the lock-captured owner, not a new live label.
assert '&pair_camera_frame,&pair_owner)' in current_xr
assert 'fresh_pair,left,right,&safe_to_drop,pair_owner)' in current_xr
assert 'g_v6634_pub_owner_d85=owner;' in current_device
assert 'tag.serial,pst.owner);' in current_device
temporal=(root/'src/temporal_capture_device_d85.inl').read_text()
assert 'native_upscaler_stereo_d82::PairOwner{owned.epoch,native_upscaler_stereo_d82::BoundTechnology()}' in temporal
native_writer=current_device[current_device.index('const auto d82_pair_frame='):current_device.index('V6634PublishWriteSlotLocked(s_bridge_left_capture,s_bridge_right_capture,pair_serial,{});')]
assert '!d82_temporal' in native_writer
rows['files']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in [
    'src/device_hook.cpp','src/xr_manager.cpp','src/native_upscaler_stereo_d82.cpp',
    'tests/temporal_cache_owner_d85_test.cpp','tools/test_temporal_cache_owner_d85.py']}
rows['limits']='Production publisher/getters, actual XR admission prefix and successful-cache commit; controlled backend/resource leaves. Remaining Windows/D3D12/XR body and headset NOT_RUN.'
(a.out/'RESULT.json').write_text(json.dumps(rows,indent=2)+'\n')
print(rows['current']['output'].strip())
if a.baseline:print('PASS: actual R3 accessors/XR admission reproduce stale-pair eligibility (expected negative).')
