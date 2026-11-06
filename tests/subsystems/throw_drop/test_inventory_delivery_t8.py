"""T8 actual packaged Lua 5.1: post-load owner, finite durable stock and revocation."""
from pathlib import Path
import argparse, json, zipfile, sys
sys.path.insert(0,str(Path(__file__).resolve().parent))
from test_inventory_delivery_t6 import HARNESS, load_lua, DEFAULT_DEPS, ROOT, MEMBER

def run(source):
    LuaRuntime=load_lua(DEFAULT_DEPS);checks=[]
    def fixture(config=""):
        l=LuaRuntime(unpack_returned_tuples=True);l.execute(HARNESS);l.execute(config);l.execute(source);return l
    def ck(ok,name):
        assert ok,name
        checks.append(name)
    def call(l,load=2,serial=1,recovery=0):return l.globals().kcd2vr_inventory_native_t8(f"{serial}|1|{load}|{recovery}")
    l=fixture();g=l.globals();g.t6_emit("OnGameplayStarted");g.t6_pump(650)
    ck(len(g.state.calls)==0 and g.t6_reads()==0,"gameplay event cannot grant without post-load native scope")
    ck(g.kcd2vr_inventory_native_t7("1|1")=="1|WAIT_NATIVE_LOAD_OWNER" and len(g.state.calls)==0,"old actor fallback cannot bypass load scope")
    ck(call(l)=="1|DONE|5|3" and len(g.state.calls)==8,"native post-load call delivers exactly five daggers and three axes")
    ck(call(l,2,2)=="2|DONE|5|3" and len(g.state.calls)==8,"same lease cannot duplicate starter")
    l.execute("state.counts={};state.perks={}")
    ck(call(l,4,3)=="3|DONE|5|3" and len(g.state.calls)==16,"second save same actor address rereads its fresh saved inventory")
    l.execute("state.counts={}")
    ck(call(l,6,4)=="4|ALREADY_CLAIMED|0|0" and len(g.state.calls)==16,"depleted saved stock is not automatically replenished")
    for packet in ("1|2|2","0|1|2","1|0|2","1|1|0","1|1|2|extra","-1|1|2"):
        before=len(g.state.calls);ck("DONE" not in g.kcd2vr_inventory_native_t8(packet) and len(g.state.calls)==before,"reject malformed/foreign "+packet)
    l=fixture("UIAction=nil;Script.SetTimer=nil");ck(call(l)=="1|DONE|5|3","lost gameplay events/timers do not lose delivery")
    for mode in ("false","nil","throw","false_commit","throw_commit"):
        l=fixture(f"state.failCreateAt=3;state.createMode='{mode}'")
        ck("PARTIAL_UNCERTAIN" in call(l),"truthful uncertain create "+mode)
        before=l.globals().t6_callsFor(l.globals().gd)
        call(l,4,2)
        ck(l.globals().t6_callsFor(l.globals().gd)==before,"durable claim prevents uncertain replay "+mode)
    for leaf,at in (("hasPerk",1),("hasPerk",2),("count",1),("count",2),("claim",1),("create",1)):
        for action in ("end","swap"):
            l=fixture(f"state.mutateLeaf='{leaf}';state.mutateAt={at};state.mutateAction='{action}'")
            ck("DONE" not in call(l) and len(l.globals().state.calls)<=1,f"reentrant {action} at {leaf} revokes outgoing grant")
    l=fixture("state.perks[dr]=true;state.perks[ar]=true")
    ck(call(l)=="1|ALREADY_CLAIMED|0|0" and len(l.globals().state.calls)==0,"legacy consumed/claimed stock stays finite")
    l=fixture();call(l);g=l.globals();g.KCD2VRThrowingAssets1.WorldEnded()
    ck("WAIT_WORLD" in call(l,2,2),"same outgoing load cannot reopen after explicit gameplay end")
    ck("DONE" in call(l,4,3) or "ALREADY_CLAIMED" in call(l,4,4),"next qualified load can enter after old world end")
    ck("REFUSED" in call(l,2,5),"old load cannot revive after a newer accepted load")
    l=fixture("state.perks[dr]=true;state.perks[ar]=true")
    ck(call(l,2,1,1)=="1|DONE|5|3","explicit recovery restores absent legacy starter without clearing old receipt")
    l.execute("state.counts={}")
    ck(call(l,4,2,1)=="2|ALREADY_CLAIMED|0|0" and len(l.globals().state.calls)==8,"recovery is once per save, not infinite refill")
    l=fixture("state.counts[gd]=4;state.counts[ga]=2")
    ck(call(l,2,1,1)=="1|DONE|5|3" and len(l.globals().state.calls)==2,"explicit recovery preserves existing stock and adds only missing units")
    return {"checks":checks,"passed":len(checks),"lua":"5.1","engine_executed":False}
if __name__=="__main__":
    ap=argparse.ArgumentParser();ap.add_argument("--pak",type=Path);ap.add_argument("--out",type=Path);a=ap.parse_args()
    source=zipfile.ZipFile(a.pak).read(MEMBER).decode("utf-8-sig") if a.pak else (ROOT/"src/subsystems/throw_drop/resources/inventory_delivery_t6.lua").read_text()
    report=run(source)
    if a.out:a.out.write_text(json.dumps(report,indent=2)+"\n")
    print(json.dumps(report,indent=2))
