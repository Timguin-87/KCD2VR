"""Exact retail native keep-world vs retire-world pickup-transfer branches.

Read-only evidence for the existing transfer's third bool. No function executes,
no virtual slot is transplanted, and no engine-side render/delete is simulated.
"""
import argparse, hashlib, json, struct
from pathlib import Path
from verify_native_grasp13 import PE, EXPECTED_SHA, EXPECTED_TS

def verify(path):
    p=PE(path);checks=[]
    def check(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    check("exact retail SHA",hashlib.sha256(p.data).hexdigest(),EXPECTED_SHA)
    check("exact retail timestamp",p.timestamp,EXPECTED_TS)
    for name,at,h in [
        ("third bool copied from R8B to R14B",0x8CF717,"458af0"),
        ("request animate and third bool select retire-world branch",0x8CF798,"807b1000740f4584f6750a41b601e8319fbaffeb034533f6"),
        ("native world-retire helper clears item world link+88",0x479745,"48c7838800000000000000"),
        ("native stored-container equality checked before success",0x8CF814,"488bcfe8c46db9ff483bc60f85d1000000"),
        ("R14B selects skip-reattach branch",0x8CF85B,"4584f6754032c9"),
        ("animated request skips reattach on that branch",0x8CF8A0,"84c07513"),
        ("ordinary hand-detach separately calls world-link boolean false",0x8D0337,"488b114c8b82b002000033d241ffd0"),
        ("alchemy return reads type-qualified prop entity",0x6C5331,"8bbc8150010000"),
        ("alchemy return reads authored table entity",0x6C533F,"418b5028"),
        ("native table-helper return loads prop worldTM setter",0xD59349,"4c8b88f0000000"),
        ("native table-helper return invokes same setter",0xD594AC,"41ffd1"),
    ]:check(name,p.at(at,len(bytes.fromhex(h))).hex(),h)
    for name,at,target in [
        ("transfer calls world-link retirement",0x8CF7A6,0x4796DC),
        ("world-link retirement queues native entity removal event",0x479725,0x1E3BC40),
        ("transfer retains native inventory storage",0x8CF80C,0x8D534C),
        ("keep-world branch calls native attach-only",0x8CF899,0x8D0404),
        ("alchemy detach event continues through prop return",0x6C2A89,0x6C531C),
        ("alchemy helper resolves table and prop then returns",0x6C5362,0xD5925C),
        ("prop return reads its native item type and slot name",0xD592E6,0xD59568),
        ("prop return resolves actual authored table helper transform",0xD5932B,0xB309DC),
    ]:
        raw=p.at(at,5)
        actual=at+5+struct.unpack("<i",raw[1:])[0] if raw[0]==0xE8 else 0
        check(name,hex(actual),hex(target))
    check("native prop return uses existing CEntity worldTM setter",hex(p.q(0x4072C78+0xF0)-p.base),hex(0x7F0B88))
    check("native prop type field name",p.cstr(0x3F61040),"alchemy_type")
    check("native prop authored table helper field",p.cstr(0x3F61050),"slot_name")
    return dict(passed=all(c["passed"] for c in checks),checks=checks,
                contract="Only post-native-cleanup STOW uses third=false; original accepted request/native storage result preserved",
                native_execution=False,headset_test=False)

if __name__=="__main__":
    a=argparse.ArgumentParser();a.add_argument("--dll",type=Path,required=True);a.add_argument("--out",type=Path,required=True);v=a.parse_args()
    r=verify(v.dll);v.out.parent.mkdir(parents=True,exist_ok=True);v.out.write_text(json.dumps(r,indent=2)+"\n")
    print(str(sum(c["passed"] for c in r["checks"]))+"/"+str(len(r["checks"]))+" exact-retail F5 cleanup checks "+("PASS" if r["passed"] else "FAIL"))
    raise SystemExit(0 if r["passed"] else 1)
