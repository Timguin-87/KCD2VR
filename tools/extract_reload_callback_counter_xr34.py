from pathlib import Path
import argparse,hashlib,json
ap=argparse.ArgumentParser();ap.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);ap.add_argument('--output',type=Path);a=ap.parse_args();S=a.source;O=a.output or S/'tests';O.mkdir(parents=True,exist_ok=True)
p=S/'src/subsystems/ranged_reload/crossbow_native_xr34.inl';s=p.read_text();start=s.index('using XR34StartFn=');end=s.index('static std::array<std::optional<DetourModKit::hook::Hook>,3> g_crossbow_hooks_xr34;',start);types=s[start:end]
start=s.index('static std::atomic<unsigned> g_crossbow_inflight_xr34');end=s.index('static void XR34ExistingCompleteObserver',start);callbacks=s[start:end]
owner=S/'src/action_time_owner59c11.cpp';text=owner.read_text();start=text.index('static std::atomic_flag g_owner_transaction19c4');end=text.index('// Owner state is changed',start);transaction=text[start:end]
q=O/'reload_callback_counter_xr34_actual.inc';q.write_text('// Exact current native typedefs, actual OwnerTransaction and complete three callback wrappers; explicit original/post leaves only.\n'+transaction+'\n'+types+'\n'+callbacks,newline='\n')
(O/'reload_callback_counter_xr34_provenance.json').write_text(json.dumps({'sources':{str(x.relative_to(S)):hashlib.sha256(x.read_bytes()).hexdigest() for x in [p,owner]},'generatedSHA256':hashlib.sha256(q.read_bytes()).hexdigest(),'scope':'Exact3nativeentrywrappers and actual owner transaction. Native original/post observers explicit leaves, no WHGame execution.'},indent=2),newline='\n')
