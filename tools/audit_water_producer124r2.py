"""Exact-retail, read-only proof of water producer missing from the old hook scope."""
import argparse, hashlib, json, struct
from pathlib import Path
from native_research112 import PE
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

def main():
    ap=argparse.ArgumentParser(); ap.add_argument("dll"); ap.add_argument("out"); args=ap.parse_args()
    p=PE(args.dll); digest=hashlib.sha256(p.data).hexdigest()
    assert digest=="bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"
    assert p.stamp==0x6A350E20
    assert p.read(0x7C18B8,15)==bytes.fromhex("48895c24104889742418574883ec40")
    assert p.read(0x7C1987,8)==bytes.fromhex("488bcfe895fdffff")
    assert 0x7C198F+struct.unpack("<i",p.read(0x7C198B,4))[0]==0x7C1724
    md=Cs(CS_ARCH_X86,CS_MODE_64)
    ranges={name:p.function(rva) for name,rva in [("setup",0x7BBE70),("volumes",0x7C18B8),("producer",0x7C1724)]}
    assert not ranges["setup"][0]<=0x7C198A<ranges["setup"][1]
    out=Path(args.out);out.mkdir(parents=True,exist_ok=True)
    instructions={}
    for name,(a,b) in ranges.items():
        ins=list(md.disasm(p.read(a,b-a),a)); instructions[name]=ins
        (out/(name+".asm")).write_text("\n".join(f"{x.address:08X} {x.mnemonic} {x.op_str}" for x in ins)+"\n")
    prod=instructions["producer"]
    assert any(x.address==0x7C17A0 and x.mnemonic=="mov" and "0x288" in x.op_str for x in prod)
    assert any(x.address==0x7C17C3 and "0xd530" in x.op_str for x in prod)
    assert any(x.address==0x7C17FC and x.mnemonic=="mov" and "0xd530" in x.op_str for x in prod)
    assert any(x.address==0x7C1866 and x.op_str=="0x7729cc" for x in prod)
    assert any(x.address==0x7C1897 and x.op_str=="0x4ed0c0" for x in prod)
    report={"dll_sha256":digest,"timestamp":hex(p.stamp),"ranges":{k:[hex(v) for v in r] for k,r in ranges.items()},
        "old_setup_scope_excludes_producer":True,"producer_uses_registry_frame_parity_pair":True,
        "producer_updates_shared_last_frame":True,"native_generation_and_mip_calls_verified":True,
        "runtime_visual_result":"NOT_TESTED"}
    (out/"proof.json").write_text(json.dumps(report,indent=2)+"\n")
    print(json.dumps(report))
if __name__=="__main__": main()
