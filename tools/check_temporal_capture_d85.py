#!/usr/bin/env python3
"""Replay production temporal capture/queue code with D3D12 and OS leaves substituted."""
from pathlib import Path
import render_host_compiler as host
import argparse,hashlib,json,os,subprocess
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--skip-sanitizers',action='store_true');a=p.parse_args()
root=Path(__file__).resolve().parents[1];out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
s=(root/'src/device_hook.cpp').read_text()
def function(key):
 begin=s.index(key);b=s.index('{',begin);i=b+1;depth=1
 while depth:
  depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[begin:i]
left=function('bool DeviceHook::BridgeCaptureCurrentMainTargetToLeft(')
reset=function('HRESULT __stdcall DeviceHook::DetourResetCommandListD82(')
queue=function('void __stdcall DeviceHook::DetourExecuteCommandLists(')
# Full production transaction, through the actual native queue call and its
# new temporal publication. Historical native-only observers after this point
# are intentionally outside this portable fixture.
queue=queue[:queue.index('    bool v6612_present_boundary_submitted=false;')]+'}\n'
bootstrap=function('void V6612BootstrapPhaseD85(')
phase=function('void DeviceHook::SetV6610OutputCensusPhase(')
actual=left+'\n'+reset+'\n'+bootstrap+'\n'+phase+'\n'+queue
census=function('struct V6612ListTag {')+';\nstd::mutex g_v6612_list_mutex;std::array<V6612ListTag,16>g_v6612_live_lists{};std::size_t g_v6612_live_count=0;\n'+function('void V6612DiscardMainWriterD85(ID3D12GraphicsCommandList* list) noexcept\n{')+'\n'+function('bool V6615IncomingLockedRightMainWriter(')
(out/'temporal_capture_adapter_actual.inc').write_text(actual)
policy=(root/'src/temporal_capture_transaction_d85.h').read_text();adapter=(root/'src/temporal_capture_device_d85.inl').read_text()
def run(name,pol=policy,ad=adapter,ac=actual,flags=('-O2',),negative=False,ce=census):
 d=out/name;d.mkdir(exist_ok=True)
 for fn,content in [('temporal_capture_transaction_d85.h',pol),('temporal_capture_device_d85.inl',ad),('temporal_capture_adapter_actual.inc',ac),('temporal_capture_census_actual.inc',ce)]: (d/fn).write_text(content)
 cmd=['g++','-std=c++20','-pthread','-Wall','-Wextra',*flags,'-I'+str(d),'-I'+str(root/'src'),str(root/'tests/temporal_capture_device_d85_test.cpp'),'-o',str(host.binary(d/'replay'))]
 c=subprocess.run(host.command(cmd),text=True,capture_output=True);(d/'compile.log').write_text(c.stdout+c.stderr)
 if c.returncode: raise RuntimeError(str(d/'compile.log'))
 r=subprocess.run([str(host.binary(d/'replay'))],text=True,capture_output=True,env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'},timeout=30);(d/'run.log').write_text(r.stdout+r.stderr)
 ok=(r.returncode!=0) if negative else (r.returncode==0)
 if not ok:raise RuntimeError(str(d/'run.log'))
 return {'name':name,'passed':ok,'exit_code':r.returncode,'output':r.stdout.strip()}
results={'kind':'actual production capture adapter and extracted queue/reset/LEFT-copy functions; OS/D3D leaves substituted; F2 receipt gates/rate limits execute production observer','windows_build':False,'headset_test':False,'runs':[],'negative_controls':[]}
results['runs'].append(run('optimized'))
if not a.skip_sanitizers:results['runs'].append(run('asan_ubsan',flags=('-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer')))
mutations=[
 ('skip_sdk_receipt',policy,adapter.replace('native_upscaler_stereo_d82::PairReady(epoch,frame)','true'),actual),
 ('publish_without_left',policy.replace('!current_.cancelled&&current_.leftQueued&&current_.rightComplete','!current_.cancelled&&current_.rightComplete'),adapter,actual),
 ('failed_reset_discards',policy.replace('if(!succeeded||!Pending()', 'if(!Pending()'),adapter,actual),
 ('accept_wrong_recording',policy.replace('receipt.recording==current_.recording&&',''),adapter,actual),
 ('allow_pending_rollover',policy.replace('if(Pending()||!epoch||!frame)','if(!epoch||!frame)'),adapter,actual),
 ('ignore_mode_epoch',policy,adapter.replace('native_upscaler_stereo_d82::PairReady(ticket.epoch,ticket.frame)','true'),actual),
 ('skip_right_copy',policy,adapter.replace('list->CopyResource(pins->right,source);',''),actual),
 ('release_unsubmitted_pins',policy,adapter.replace('if(p.left&&p.fenceOwner&&p.fence&&p.fence!=UINT64_MAX)', 'if(p.left&&p.fenceOwner&&p.fence!=UINT64_MAX)'),actual),
 ('skip_left_reference_pin',policy,adapter,actual.replace('d85_left_pin->left->AddRef();','')),
 ('record_ahead_state_used_for_left',policy,adapter,actual.replace('const auto source_state=state_override?*state_override:s_bridge_current_main_state;','const auto source_state=s_bridge_current_main_state;')),
 ('phase_overwrites_pending',policy,adapter,actual.replace('if(eye==0&&g_temporal_owner_d85.Pending())','if(false)')),
 ('queue_not_serialized',policy,adapter,actual.replace('std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);','')),
 ('skip_right_reference_pin',policy,adapter.replace('pins->right->AddRef();',''),actual),
 ('capture_diagnostics_ignore_f2',policy,adapter.replace('cap::active(cap::Domain::Bug,now)','true'),actual),
 ('capture_diagnostics_unbounded',policy,adapter.replace('if(rate.session==session&&now>=rate.lastMs&&now-rate.lastMs<2000)return;',''),actual),
 ('capture_submission_refusal_silent',policy,adapter.replace('    namespace cap=diagnostics::capture168;','    if(stage==TemporalCaptureTraceD85::SubmitRefused)return;\n    namespace cap=diagnostics::capture168;'),actual),
 ('publication_loses_attempt_epoch',policy,adapter.replace('native_upscaler_stereo_d82::PairOwner{owned.epoch,native_upscaler_stereo_d82::BoundTechnology()}','native_upscaler_stereo_d82::PairOwner{}'),actual),
]
for name,pol,ad,ac in mutations:
 assert (pol,ad,ac)!=(policy,adapter,actual),name
 results['negative_controls'].append(run(name,pol,ad,ac,negative=True))
mutated_census=census.replace('if(((t.main_bind_mask|t.main_barrier_mask)&1u)&&t.frame[0]==target)return false;','')
assert mutated_census!=census
results['negative_controls'].append(run('pending_left_writers_ignored',negative=True,ce=mutated_census))
# Production wiring contracts guard the capture endpoint and legacy exclusion.
barrier=function('void __stdcall DeviceHook::DetourResourceBarrier(')
assert barrier.index('RecordTemporalFinalMainD85(list_this')<barrier.index('hud_layer_v6664::OnPresentBarrier(')
assert 'if(!d82_temporal && g_v6615_active.load' in s
assert 'if(eye==0&&g_temporal_owner_d85.Pending())' in s
assert 'g_temporal_owner_d85.NoteLeftBoundary(' in s and 'g_temporal_owner_d85.CompleteRight(' in s
assert queue.index('transaction(g_temporal_mutex_d85)')<queue.index('s_original_execute_command_lists(queue_this')<queue.index('FinishTemporalMainSubmissionD85(')
assert 'std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);' in function('void DeviceHook::SetV6610OutputCensusPhase(')
assert 'std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);' in function('void DeviceHook::ResetBridgeCapture(')
results['source_sha256']={f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['src/device_hook.cpp','src/temporal_capture_transaction_d85.h','src/temporal_capture_device_d85.inl','tests/temporal_capture_device_d85_test.cpp']}
(out/'RESULT.json').write_text(json.dumps(results,indent=2)+'\n');print(json.dumps(results,indent=2))
