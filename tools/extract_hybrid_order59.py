"""Extract actual contact queue producer and Drain lease, without native execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import hashlib,json,sys
from pathlib import Path
root=Path(__file__).resolve().parents[1]
s=read_source(root/'src/diagnostics/geometry_hit178r27.inl')
chunks=[]
a=s.index('struct Pending {')
# Keep precisely declarations through the last counter semicolon (the following native section is irrelevant).
end=s.index(';',s.index('g_drop_weapon_item_live56',a))+1
chunks.append(s[a:end])
a=s.index('bool Submit(const Pending& p) noexcept {');b=s.index('static bool CallCtor',a)
chunks.append(s[a:b])
a=s.index('void Drain(void* henryActor) noexcept {');b=s.index('    if(p.recovery55c1!=',s.index('    } pendingLease{p.serial};',a))
body=s[a:b].replace('void Drain(void* henryActor) noexcept {','template<class Action> void DrainBooking59(Action action) noexcept {')
chunks.append(body+'    action(p,pendingLease);\n}\n')
out=Path(sys.argv[1]);out.write_text('\n'.join(chunks),encoding='utf-8',newline='\n')
Path(str(out)+'.json').write_text(json.dumps({'producer':'src/diagnostics/geometry_hit178r27.inl','sha256':hashlib.sha256(s.encode()).hexdigest(),'extraction':'actual Pending/queue counters, Submit/Clear, Drain queue lease; native tail deliberately not called'},indent=2)+'\n')

# Actual final native-entry/cleanup branches, tested with an explicit reentrant
# Clear before entry; only host dependencies are mocked, not the branch logic.
a=s.index('            {kcd2vr::combo_lineage19::OfferScope offer(kcd2vr::combo_lineage19::valid(comboOwner19)?&comboOwner19:nullptr);')
b=s.index('            if(!survived){g_live_fault178r35',a)
Path(str(out).replace('.inc','_live.inc')).write_text(s[a:b],encoding='utf-8',newline='\n')
a=s.index('    const bool serialEntered59=serialReservationC2.tryEnter();')
b=s.index('    if(!survived){g_stats[4].fetch_add',a)
direct=s[a:b]
observer='''        frik_pose_probe::ObserveHitPartNamespace62({p.serial,p.sample,p.tick,p.key,p.generation,p.epoch,p.request,
            p.entityId,reinterpret_cast<std::uintptr_t>(target),reinterpret_cast<std::uintptr_t>(te),p.metadata59c7g.collider,p.part});'''
assert direct.count(observer)==1
# Diagnostic-only void leaf, independently exercised by part_namespace62's
# exact pre-submit caller fixture. Do not replace entry/claim/cleanup branches.
direct=direct.replace(observer,'        /* C62 diagnostic observer: explicit no-result fixture leaf, separately tested. */')
Path(str(out).replace('.inc','_direct.inc')).write_text(direct,encoding='utf-8',newline='\n')
