"""Exact-retail and source-contract check for the native movement read view.

This verifies static boundaries only. It does not execute game AI, demonstrate
headset spacing, or turn constructor defaults into observed runtime numbers.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
from native_research112 import PE

parser = argparse.ArgumentParser()
parser.add_argument("binary", type=Path)
parser.add_argument("--out", type=Path, required=True)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
pe = PE(args.binary)
sha = lambda data: hashlib.sha256(data).hexdigest()
assert pe.base == 0x180000000
assert pe.stamp == 0x6A350E20
assert sha(pe.data) == "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"
source_path = root / "src/subsystems/direct/npc_tuning.cpp"
source = source_path.read_text()
unit = source.split("namespace kcd2vr::npc_unarmed_spacing {", 1)[1]
pins = []
for rva, encoded, size in re.findall(r'\{(0x[0-9A-Fa-f]+),"([^"\n]+)",(\d+)\}', unit):
    raw = encoded.encode().decode("unicode_escape").encode("latin1")
    assert len(raw) == int(size)
    at = int(rva, 16)
    assert pe.read(at, len(raw)) == raw, rva
    pins.append({"rva": rva, "bytes": len(raw), "sha256": sha(raw)})
assert len(pins) == 7
# The only changed source register is dead after the third load: no native
# vtable/identity/pointer consumer can receive this small read view.
assert pe.read(0x207967C, 33) == bytes.fromhex(
    "f3440f10402c440f294c2460f3440f104828440f29542450f3440f105030488b01")
assert struct.unpack("<Q", pe.read(0x3FF1B18 + 0x250, 8))[0] == pe.base + 0x730B30
assert struct.unpack("<Q", pe.read(0x3FE3F70 + 0x10, 8))[0] == pe.base + 0x207C4A0
for name, vft in ((b".?AVC_NPC@xgenaimodule@wh@@", 0x3FF1B18),
                  (b".?AVC_CombatMove@xgenaimodule@wh@@", 0x3A836A0)):
    locator = struct.unpack("<Q", pe.read(vft - 8, 8))[0] - pe.base
    td = struct.unpack("<I", pe.read(locator + 12, 4))[0]
    actual = pe.read(td + 16, len(name) + 1).split(b"\0")[0]
    assert actual == name, actual
# Source must leave stored native profile alone and change only source RAX.
sample = unit.split("void Sample(", 1)[1].split("struct Pin", 1)[0]
assert "auto& source=gpr(context,Gpr::Rax)" in sample
assert "source=ReadView(source,gpr(context,Gpr::Rdi),gpr(context,Gpr::Rcx))" in sample
assert sample.count("gpr(") == 3
assert "Copy(move+0x28,&view.activity,3*sizeof(float))" in unit
assert "view.inner=inner;view.outer=outer;" in unit
assert "if(factor==1.f||!ready.load" in unit
assert "!Equal(actor,base+0x3E3FAC8)" in unit
assert "!Read(row+8,id)||id!=12" in unit
assert "!Read(owner+0x640,current)||current!=move" in unit
host = (root / "src/subsystems/direct/host.inl").read_text()
mod = (root / "src/mod.cpp").read_text()
menu = (root / "src/subsystems/direct/menu.inl").read_text()
menu_cpp = (root / "src/vr_menu.cpp").read_text()
assert 'std::clamp(state.npc_unarmed_spacing_percent,25.f,100.f)*.01f' in host
assert 'wf("VrNpcUnarmedSpacingPercent",npc_unarmed_spacing::scale.load()*100.f)' in host
assert '"VrNpcUnarmedSpacingPercent",npc_unarmed_spacing::defaultPercent' in host
assert '"VrNpcUnarmedSpacingVersion",npc_unarmed_spacing::settingsVersion' in host
assert 'saved==100.f?npc_unarmed_spacing::defaultPercent:saved' in host
assert 'defaultPercent=60.f' in (root / "src/subsystems/direct/npc_tuning.h").read_text()
assert 'out.npc_unarmed_spacing_failure = npc_unarmed_spacing::Failure();' in host
assert '#define KCD2VR_M2_HOST_SLOT 9010\n#include "subsystems/direct/host.inl"' in mod
assert 'npc_unarmed_spacing::Shutdown();' in mod
assert 'float npcSpacingAttempt=1.f;' in mod and 'static float npcSpacingAttempt' not in host
assert 'std::uintptr_t npcSpacingModule=0;' in mod
assert 'if(spacing!=npcSpacingAttempt||spacingModule!=npcSpacingModule)' in host
assert 'if(spacing<1.f&&!npc_unarmed_spacing::Ready())' in host
assert 'NPC unarmed preferred-distance scale' in menu
assert 'Native tactical distance overrides' in menu
assert 's.npc_unarmed_spacing_failure' in menu
assert 'std::round(25.f+t(kNpcUnarmedSpacing)*75.f)' in menu_cpp
assert 'released==DragControl::NpcUnarmedSpacing){ApplyState(s);save();return;}' in menu_cpp
inputs = ["src/subsystems/direct/npc_tuning.cpp", "src/subsystems/direct/npc_tuning.h",
          "src/subsystems/direct/host.inl", "src/subsystems/direct/menu.inl",
          "src/subsystems/hybrid/menu.inl", "src/mod.cpp", "src/vr_menu.cpp", "src/vr_menu.h",
          "tests/npc_unarmed_spacing_test.cpp", "tests/npc_settings_test.cpp", "tests/CMakeLists.txt"]
ranges = {
    "profile_refresh": (0x731E70, 0x731F20),
    "rpg_profile_builder": (0x731F48, 0x7321E3),
    "range_callback": (0x2079630, 0x2079950),
    "range_callback_register": (0xB1D6A0, 0xB1D750),
    "range_sampler_dispatch": (0x733254, 0x7333D0),
    "range_sampler_update": (0x733048, 0x7330CC),
    "native_cached_smoothers": (0x734814, 0x7349A0),
    "inner_getter": (0x733C98, 0x733CBB),
    "outer_getter": (0x7343F0, 0x734413),
    "native_world_goal": (0x733AFC, 0x733C96),
    "native_contains_ring": (0x734B3C, 0x734D00),
    "npc_move_ctor": (0xBDCD10, 0xBDCDD0),
    "npc_owner_ctor": (0xBDC5F8, 0xBDCA8F),
    "native_effective_class": (0x9D33F8, 0x9D3421),
    "native_class_getter": (0x9D3574, 0x9D35DF),
}
report = {
    "status": "PASS_STATIC_EXACT_RETAIL_AND_PRODUCTION_SOURCE",
    "native": {"sha256": sha(pe.data), "timestamp": hex(pe.stamp), "base": hex(pe.base)},
    "pins": pins,
    "ranges": [{"name": name, "start": hex(a), "end_exclusive": hex(b),
                "sha256": sha(pe.read(a, b-a))} for name, (a, b) in ranges.items()],
    "source": {"path": "src/subsystems/direct/npc_tuning.cpp", "sha256": sha(source_path.read_bytes())},
    "source_inputs": {path: sha((root/path).read_bytes()) for path in inputs},
    "control": "Existing NPC settings page;25..100percent;100native/no initial hook; actual state/apply/save/load and once-per-value lifecycle; concrete activation failure is displayed.",
    "boundary": "Scale fresh native inner/outer base through three immediate scalar reads; native profile never written; subsequent target/tactical adjustments and coherent native cache untouched.",
    "important_correction": "731F48 can return without rewriting old profile bounds: post-refresh field multiplication would compound. This candidate does not write those fields.",
    "scope": "Current exact XGenNPC/C_CombatMove owner and native CombatActor; native effective selected class12 only. Henry and armed NPC passthrough.",
    "limits": ["Not headset execution or actual current-NPC distance evidence.",
               "Native tactical/keep-distance overrides may retain a farther final goal.",
               "Does not alter punch input, kicks, contact, damage, max attack reach, Henry, or movement eligibility."]
}
args.out.parent.mkdir(parents=True, exist_ok=True)
args.out.write_text(json.dumps(report, indent=2) + "\n")
print("PASS exactRetail7InstructionPins2VftJoins2RTTI15NativeRanges sourceReadOnlyBoundary actualSettingsLifecycleLinked headset=NOT_RUN")
