"""Verify exact retail native touch/shape boundaries and optional parent RED input.

No game code is executed. The parent fixture replaces only the native AddImpact
leaf and otherwise compiles the original touch body and original call block.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
from native_research112 import PE

ap = argparse.ArgumentParser()
ap.add_argument("binary", type=Path)
ap.add_argument("--out", type=Path, required=True)
ap.add_argument("--parent-root", type=Path)
ap.add_argument("--extract-parent", type=Path)
ap.add_argument("--disassemble", action="store_true")
a = ap.parse_args()
root = Path(__file__).resolve().parents[1]
pe = PE(a.binary)
sha = lambda data: hashlib.sha256(data).hexdigest()
assert sha(pe.data) == "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"
assert pe.base == 0x180000000 and pe.stamp == 0x6A350E20
pins = {
    0x37CBB1A: "488db900010000",  # native pose -> embedded physics
    0x37C61CF: "3bf0",             # AddImpact input is joint-domain
    0x37C61F7: "4a8b0cc1",         # joint-name read via joint*48
    0x37C65F3: "8975c8",           # same joint enters native action part
    0xB34428: "4a8b4c2020",        # joint physicalInfo, indexed by R12
    0xB3442D: "488b11",            # primary phys_geometry passed to AddGeometry
    0xB344EE: "4189440c1c",        # actual primary returned part ID
    0xB344CA: "8d83c8000000",      # secondary requested joint+200
    0xB34505: "4a8b442120",        # current same default joint physicalInfo
    0xB3450A: "488b08",            # its primary phys_geometry
    0xB3450D: "48397950",          # secondary geometry at primary+50
    0xB34513: "448b4d80",          # secondary requested ID, not base+ID
    0xB34543: "488b5250",          # actual secondary phys_geometry arg
    0x8A67E5: "8b441148",          # GetStatus obtains current actual part ID
    0x8A67F0: "894704",            # actual part ID returned in status+4
    0x8A6824: "488b4c0138",        # actual part primary phys_geometry
    0x8A6829: "488b09",            # actual primary IGeometry
    0x8A682C: "48894f68",          # status.geometry
    0x8A6837: "488b540240",        # actual part proxy phys_geometry
    0x8A683C: "488b02",            # actual proxy IGeometry
    0x8A683F: "48894770",          # status.proxyGeometry
    0x37C6181: "498bd9",           # R9 is the impulse pointer
    0x37C6188: "4d8be8",           # R8 is the point pointer
    0x37C618B: "4863f2",           # EDX is signed joint input
    0x37C65AD: "f20f1003",         # native-scaled impulse enters packet+4
    0x37C65C4: "f2410f104500",     # original point, no model transform
    0x37C65D1: "f20f1145bc",       # point copied to packet+1C
    0x37C65FF: "b001",             # AddImpact TRUE is submission, not Action return
    0x37C66AD: "4881c100010000",   # skeleton pose thunk receiver+100
    0x37C66DF: "e874faffff",       # exact AddImpact body call
    0xA9A956: "45392f",            # articulated native action type2
    0x226FC0A: "418b5728",         # action.partid, not array index
    0x226FC12: "3910",             # native search compares actual body part ID
    0xA9B492: "f30f108402a0000000",# native world position in shape centre
    0xA9B49B: "f30f5800",          # world centre = local transformed centre + body position
    0xA9B5D8: "f30f1044021c",     # supplied point in action+1C
    0xA9B5DE: "f30f5c00",          # supplied point - world shape centre
    0x845B90: "488bc448895810488970185557415441", # living Action entry
    0x845D5A: "41833e020f840f010000", # exact native type2 branch
    0x845EDE: "41837e3402f2410f104604418b460c", # source+34, impulse+4/+C
    0x845EFB: "f30f108fe0010000",  # native living inverse mass
    0x845F03: "41395e30",          # native applyTime+30 compared with EBX=0
    0x224B7D4: "8975e7895dfb",     # native move type1 (ESI) and auxiliary0 (EBX)
    0x224B810: "c745f702000000ff5038", # native iJump2 and original Action
    0x224B6BE: "498bc6492bc7f3420f10443804f3410f5807f3410f1107", # native velocity += request delta
    0x1466831: "c744246002000000c7458c0000008048c7459002000000897588", # original AddImpulse type2/sentinels/applyTime2
    0x146687C: "4533c08944246c488bcb488b03f20f11442464ff5038", # original Action(...,0)
}
checks = []
for rva, encoded in pins.items():
    expected = bytes.fromhex(encoded)
    actual = pe.read(rva, len(expected))
    checks.append(dict(rva=hex(rva), expected=encoded, actual=actual.hex(), match=actual == expected))
for table, slot, target in ((0x3A71028,0x58,0x37C66A4),(0x3A48B30,0x30,0x8A9348),(0x3A82570,0x38,0x845B90)):
    actual = struct.unpack("<Q", pe.read(table+slot,8))[0]-pe.base
    checks.append(dict(table=hex(table), slot=hex(slot), expected=hex(target), actual=hex(actual), match=target == actual))
inputs = ["src/vrik/touch_react43.inl", "src/subsystems/physics/touch_react_service60.h", "src/frik_pose_probe.cpp",
          "src/vrik/part_namespace62.h", "src/subsystems/gore/native_part_anchor.inl", "tests/native_touch_h9_test.cpp",
          "tools/verify_native_touch_h9.py", "tests/CMakeLists.txt", "src/vrik/passive_hand_evidence169r1.inl",
          "src/subsystems/physics/pose_free_hand_contact.inl", "src/vrik/coherent_pose.inl", "src/vrik/npc_contact172.h",
          "src/vrik/part_anchor59c7g.h", "src/vrik/passive_contact169r1.h", "src/core/collab_test_h21.h"]
body = (root/inputs[0]).read_text()
caller = (root/inputs[2]).read_text()
assert caller.count("touch43::HandContactsH9(std::uint64_t(GetTickCount64()));") == 1
assert "(!g_on.load(std::memory_order_relaxed)&&" not in body
assert "a.mainPhysics!=e.collider" in body and "PartStatus47(a.mainPhysics,status)" in body
assert "found.joint!=candidate.joint" in body
assert "if(!primaryShape&&!secondaryShape)continue;" in body
assert "s.tick,&owner" in body and "last.sequence=s.sequence" in body
assert "CurrentF9H9(owner)" in body and "collab_h21::current(x.physicsEpoch)" in body
assert "MapJointH9(e,owner,joint)" in body and "OnContact(side,e.entity,joint" in body
producer = (root/"src/vrik/passive_hand_evidence169r1.inl").read_text()
assert "x.request=g_contact_control_epoch6695.load();" in producer
assert "x.request==g_contact_control_epoch6695.load(std::memory_order_acquire)" in body
assert "g_committed_character6689.store(s.character,std::memory_order_release)" in (root/inputs[10]).read_text()
assert "x.actor=reinterpret_cast<std::uintptr_t>(s.character)" in producer
assert "target.contact=plane.native172" in producer
assert "bodies?npc172::BodiesAndWorld" in caller and "out.native172=witness" in caller
assert "witness.body=npc172::classify(witness.vft)" in caller
# The direct query and the later deferred completion each borrow/restore the
# same pass-local query context. Qualify BOTH concrete placements: direct
# contact owns it before fresh/model reads; completion owns it only after
# consuming a ready pending query and before its saved context/extra rays.
scope_call="kcd2vr::touch_react_service60::ScopedNpcQueryH9 npcHandH9(g_npc_query172,diagnostics::lab::NativeReference());"
assert caller.count(scope_call)==2
contact_entry=caller.split("static bool ContactHand6695(",1)[1].split("c.fresh169r1=false;",1)[0].split("{",1)[1]
complete_entry=caller.split("static bool CompleteHandContact6695(",1)[1].split("const auto& before=pending.before;",1)[0].split("{",1)[1]
assert re.sub(r"\s+","",contact_entry)==re.sub(r"\s+","",scope_call)
assert re.sub(r"\s+","",complete_entry)==re.sub(r"\s+","","if(!pending.ready)return false;pending.ready=false;"+scope_call)
publisher = (root/inputs[1]).read_text()
assert "e.sample.motion=p::update(nativeMotionH9[s.side],s).motion" in publisher
assert "nativeMotionEpochH9[s.side]!=e.physicsEpoch" in publisher
assert "if(found.joint==e.part)nativeJoint=status" in body
assert "PartStatus47(a.mainPhysics,nativeJoint)" in body
result = dict(retail_sha256=sha(pe.data), timestamp=hex(pe.stamp), checks=checks,
              all_match=all(c["match"] for c in checks), native_calls=0, native_writes=0,
              source_inputs={p:sha((root/p).read_bytes()) for p in inputs},
              query_scope_contract={"callsites":2,"argument":"g_npc_query172; current NativeReference()",
                                    "direct":"ContactHand6695 entry before fresh/model reads",
                                    "deferred":"CompleteHandContact6695 after ready consumption, before saved-context/extra-ray reads"},
              capsule_route={"class":"current PE_LIVING entity proxy body","Action":"845B90 type2","packet_size":56,
                             "scheduling":"native applyTime2/source0; Action argument0; native own queue/lock policy unchanged",
                             "motion":"same committed source, point/normal and measured energy; native inverse mass and velocity integration",
                             "skeleton_part":"NONE","damage_or_RPG_writes":"NONE"},
              limits=["Static native ABI/shape proof; native movement and headset touch remain NOT_RUN",
                      "Only unique current articulated hit-skeleton shapes map. Living capsules use their own current native whole-body Action; no ambiguous/replaced shape becomes a guessed joint or capsule fallback",
                      "No arm/head holds, attractive pull, living ragdoll or drag are implemented by this touch extension"])
if a.parent_root or a.extract_parent:
    assert a.parent_root and a.extract_parent
    parent_body = (a.parent_root/inputs[0]).read_text()
    parent_caller = (a.parent_root/inputs[2]).read_text()
    assert caller.count("WorldRayDistance6692(")==parent_caller.count("WorldRayDistance6692("),"No additional ray call site"
    def npc_selector(text):return text.split("static bool NpcQuerySelected172() noexcept {",1)[1].split("\n}",1)[0]
    assert npc_selector(caller)==npc_selector(parent_caller),"Weapon/global query policy unchanged"
    start = parent_body.index("static int CallAddImpact(")
    opening = parent_body.index("{", start)
    closing = parent_body.index("\n}", opening)+2
    head = parent_body[:opening+1]
    tail = parent_body[closing-1:]
    parent_test_body = head+"\n    return HostTouchAddImpact(pose,fn,part,point,impulse);\n"+tail
    start = parent_caller.index("    { // DS-2026-10-23: the completed FREE-HAND living contact")
    end = parent_caller.index("    {static kcd2vr::impact59c6::Seen lastLimb60", start)
    fragment = parent_caller[start:end]
    assert "touch43::OnContact(ht.side,ht.entity,ht.part" in fragment
    a.extract_parent.mkdir(parents=True,exist_ok=True)
    (a.extract_parent/"touch_parent_body.inl").write_text(parent_test_body)
    (a.extract_parent/"touch_parent_dispatch.inc").write_text(fragment)
    result["parent_red_input"] = dict(root=str(a.parent_root), native_leaf_only_replaced=True,
        source_body_sha256=sha(parent_body.encode()), source_caller_sha256=sha(parent_caller.encode()),
        source_body_raw_sha256=sha((a.parent_root/inputs[0]).read_bytes()),source_caller_raw_sha256=sha((a.parent_root/inputs[2]).read_bytes()),
        generated_body_sha256=sha(parent_test_body.encode()), generated_dispatch_sha256=sha(fragment.encode()))
if a.disassemble:
    dumpbin=Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/dumpbin.exe")
    ranges={"ADDIMPACT_INPUT":(0x37C6158,0x37C6201), "ADDIMPACT_SUBMIT":(0x37C65AD,0x37C66E9),
            "CURRENT_PART_GEOMETRY":(0x8A67D8,0x8A6849), "PRIMARY_SECONDARY_SHAPES":(0xB3441D,0xB3454A),
            "NATIVE_ACTION_TYPE2":(0xA9A950,0xA9AB51), "NATIVE_ACTION_ID_LOOKUP":(0x226FBED,0x226FC32),
            "NATIVE_WORLD_POINT":(0xA9B478,0xA9B5F1)}
    outputs={}
    for label,(start,end) in ranges.items():
        r=subprocess.run([str(dumpbin),"/DISASM:NOBYTES",f"/RANGE:{pe.base+start:#x},{pe.base+end:#x}",str(a.binary)],capture_output=True,check=True)
        path=a.out.parent/(label+".asm");path.write_bytes(r.stdout);outputs[path.name]=dict(start=hex(start),end=hex(end),sha256=sha(r.stdout))
    result["disassembly"]=outputs
a.out.parent.mkdir(parents=True,exist_ok=True)
a.out.write_text(json.dumps(result,indent=2)+"\n")
print(f"native touch retail/source: {sum(c['match'] for c in checks)}/{len(checks)} PASS")
raise SystemExit(0 if result["all_match"] else 1)
