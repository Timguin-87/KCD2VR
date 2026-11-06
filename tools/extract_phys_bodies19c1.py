"""Extract current production proxy producer/cleanup into a host-only fixture."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import sys
root = Path(__file__).resolve().parents[1]
text = read_source(root / 'src/vrik/phys_bodies59c10r8.inl')
def extract(start, struct=False, source=text):
    first = source.index(start)
    brace = source.index('{', first)
    depth = 0
    for end in range(brace, len(source)):
        if source[end] == '{': depth += 1
        elif source[end] == '}':
            depth -= 1
            if not depth:
                return source[first:end + (2 if struct else 1)]
    raise ValueError('Unclosed production definition: ' + start)
out = Path(sys.argv[1])
out.parent.mkdir(parents=True, exist_ok=True)
body = [extract('struct Body {', True), extract('struct State {', True),
        'static State g_state; static std::atomic<int> g_mode59c10r8{0}; static std::atomic<bool> g_retiredT4{true}; static std::atomic<float> g_hand_mass{3.f},g_blade_mass{4.f};', extract('static void DestroyBody('),
        extract('static bool CreateBody('), extract('static bool StillOurs('),
        extract('static void Drive('),extract('static void PollContacts('),
        extract('struct BladeOwner19c2 {',True),extract('static bool TargetTime19c2('),
        extract('static bool TargetReady19c2('),extract('static bool Gameplay19c2('),
        extract('static bool BladeCurrent19c2('),extract('static bool BladeTarget('),
        extract('static bool HandTarget('),extract('void TickPhysBodies59c10r8(')]
out.write_text('// Generated from current production; never edit.\n' + '\n'.join(body) + '\n', encoding='utf-8')
native=read_source(root/'src/vrik/physical_native.inl')
frik=read_source(root/'src/frik_pose_probe.cpp')
types=[extract('struct QuatT28 {',True,frik)]
types += [extract('struct '+name+' {',True,native) for name in ['PhysEquipmentItem6698','PhysEquipment6698','PhysSocket101','PhysCorrelation126','PhysFrame6698']]
out.with_name('phys_bodies_adapter19c1_types.inc').write_text('// Actual producer types; no native layout guesses.\n'+'\n'.join(types)+'\n',encoding='utf-8')

# Native calling leaves are included by the MSVC-only adapter section.
out.with_name("phys_bodies_adapter19c4r3_leaves.inc").write_text("// Actual production native leaves.\n" + "\n".join(extract(x) for x in ["static int CallDestroy(","static int CallSetParams(","static int CallAction("]) + "\n",encoding="utf-8")
