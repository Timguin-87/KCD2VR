"""Run the canonical/final-packed T6 delivery Lua in real Lua 5.1.

Native inventory/soul/event leaves are explicit spies. No game, DLL, inventory,
save, installed PAK or user INI is touched. This proves Lua policy, not retail
save serialization or independently wieldable quantity-one native WUIDs.
"""
from pathlib import Path
import argparse
import hashlib
import json
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[3]
DEFAULT_DEPS = Path(r"C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps")
MEMBER = "Scripts/Startup/zzkcd2vr_throwing_assets1.lua"

HARNESS = r'''
gd='43286e47-cde0-50bd-ba4c-9747b777b16d'
ga='c0760688-f426-5341-95b9-482b87dbcb9b'
dr='70ceb681-04b8-5c5d-bb49-f80f03c9dc40'
ar='86c6952f-0382-5a68-9435-aebcd31a4a39'
state={counts={},perks={},calls={},logs={},timers={},listeners={},timerSerial=0,
 countReads=0,perkReads=0,claimCalls=0,entityReads=0,failCreateAt=0,
 createMode='false',claimMode='ok',claimAckMode='ok',countAckMode='ok',
 actorJoined=true,createsAtEnd=0,leafCounts={}}
function t6_mutate(leaf)
 state.leafCounts[leaf]=(state.leafCounts[leaf] or 0)+1
 if state.mutateLeaf==leaf and state.mutateAt==state.leafCounts[leaf] then
  if state.mutateAction=='end' then KCD2VRThrowingAssets1.WorldEnded()
  elseif state.mutateAction=='swap' then g_localActor=t6_newActor(2)
  else error('unknown fixture mutation') end
 end
end
function t6_newActor(id)
 local p={id=id,inventory={},soul={}}
 function p.inventory:GetCountOfClass(guid)
  state.countReads=state.countReads+1
  t6_mutate('count')
  if state.initialCountMode=='nil' then return nil end
  if #state.calls>0 and state.countAckMode~='ok' then
   if state.countAckMode=='throw' then error('native count unavailable') end
   return nil
  end
  return state.counts[guid] or 0
 end
 function p.inventory:CreateItem(guid,health,amount)
  assert(health==1 and amount==1,'only real native quantity-one requests allowed')
  state.calls[#state.calls+1]={guid=guid,health=health,amount=amount}
  local fail=state.failCreateAt==#state.calls
  local commit=not fail or state.createMode=='false_commit' or state.createMode=='throw_commit'
  if commit then state.counts[guid]=(state.counts[guid] or 0)+amount end
  t6_mutate('create')
  if state.endAtCreate==#state.calls then KCD2VRThrowingAssets1.WorldEnded() end
  if state.swapAtCreate==#state.calls then g_localActor=t6_newActor(2) end
  if fail then
   if state.createMode=='throw' or state.createMode=='throw_commit' then error('native create uncertain') end
   if state.createMode=='nil' then return nil end
   return false
  end
  return true
 end
 function p.soul:HasPerk(guid,include)
  state.perkReads=state.perkReads+1
  t6_mutate('hasPerk')
  if state.initialPerkMode=='nil' then return nil end
  if state.claimCalls>0 and state.claimAckMode~='ok' then
   if state.claimAckMode=='throw' then error('native claim read unavailable') end
   if state.claimAckMode=='nil' then return nil end
   return false
  end
  return state.perks[guid]==true
 end
 function p.soul:AddPerk(guid)
  state.claimCalls=state.claimCalls+1
  if state.claimMode=='throw_before' then error('native claim uncertain') end
  if state.claimMode~='noeffect' then state.perks[guid]=true end
  t6_mutate('claim')
  if state.claimMode=='throw_after' then error('native claim uncertain after mutation') end
 end
 return p
end
g_localActor=t6_newActor(1)
System={LogAlways=function(s) state.logs[#state.logs+1]=s end,
 GetEntity=function(id)
  state.entityReads=state.entityReads+1
  if state.actorJoined and g_localActor and g_localActor.id==id then return g_localActor end
  return nil
 end}
Script={SetTimer=function(ms,f)
 assert(type(ms)=='number' and ms>0 and type(f)=='function','bounded deferred timer required')
 state.timerSerial=state.timerSerial+1
 state.timers[state.timerSerial]={ms=ms,f=f}
 return state.timerSerial
end,KillTimer=function(id) state.timers[id]=nil end}
UIAction={RegisterEventSystemListener=function(obj,system,event,method)
 state.listeners[#state.listeners+1]={obj=obj,system=system,event=event,method=method}
end}
function t6_emit(event)
 local copy={} for i,v in ipairs(state.listeners) do copy[i]=v end
 for _,l in ipairs(copy) do if l.system=='System' and l.event==event then
  assert(type(l.obj[l.method])=='function','actual named event handler required')
  l.obj[l.method](l.obj)
 end end
end
function t6_tick()
 local key=nil
 for k in pairs(state.timers) do if key==nil or k<key then key=k end end
 if not key then return false end
 local q=state.timers[key] state.timers[key]=nil q.f() return true
end
function t6_pump(limit)
 local n=0 while n<limit and t6_tick() do n=n+1 end return n
end
function t6_timerCount() local n=0 for _ in pairs(state.timers) do n=n+1 end return n end
function t6_count(guid) return state.counts[guid] or 0 end
function t6_callsFor(guid)
 local n=0 for _,call in ipairs(state.calls) do if call.guid==guid then n=n+1 end end return n
end
function t6_native(body,request)
 return KCD2VRThrowingAssets1.NativeReady(g_localActor,body or 10,request or 20)
end
function t6_reads() return state.countReads+state.perkReads end
function t6_noRenderApis()
 return g_localActor.human==nil and g_localActor.CreateBoneAttachment==nil and
 g_localActor.GetAttachmentBone==nil and ItemManager==nil and
 g_localActor.GetWorldPos==nil and g_localActor.GetCharacter==nil
end
'''


def load_lua(deps):
    sys.path.insert(0, str(deps))
    from lupa.lua51 import LuaRuntime
    return LuaRuntime


def run(source, LuaRuntime):
    checks = []

    def fixture(config=""):
        lua = LuaRuntime(unpack_returned_tuples=True)
        lua.execute(HARNESS)
        if config:
            lua.execute(config)
        lua.execute(source)
        return lua

    def ck(ok, name):
        assert ok, name
        checks.append(name)

    def boot_quiet(lua):
        g = lua.globals()
        ck(len(g.state.calls) == 0 and g.t6_reads() == 0 and g.state.claimCalls == 0,
           "startup performs no inventory, perk or grant work before gameplay event")

    def start(lua):
        lua.globals().t6_emit("OnGameplayStarted")
        return lua.globals().t6_pump(650)

    def terminal_quiet(lua, label):
        g = lua.globals()
        reads, creates, claims = g.t6_reads(), len(g.state.calls), g.state.claimCalls
        g.t6_pump(650)
        ck(g.t6_reads() == reads and len(g.state.calls) == creates and g.state.claimCalls == claims,
           label + ": no steady inventory/perk polling or create replay")

    l = fixture()
    g = l.globals()
    boot_quiet(l)
    ck(len(g.state.listeners) == 2 and all(g.state.listeners[i].system == "System" for i in (1, 2)),
       "actual event registration uses the game's System gameplay-event namespace")
    ck(g.t6_noRenderApis(), "fixture has no VRIK, pelvis, hand, renderer, hip or item-manager API")
    start(l)
    ck(g.t6_count(g.gd) == 5 and g.t6_count(g.ga) == 3 and len(g.state.calls) == 8,
       "actual gameplay event grants exactly five daggers and three axes without hip/VRIK APIs")
    ck(all(g.state.calls[i].amount == 1 and g.state.calls[i].health == 1 for i in range(1, 9)),
       "every native create uses one independently requested quantity-one unit")
    terminal_quiet(l, "completed grant")
    start(l)
    ck(len(g.state.calls) == 8, "duplicate gameplay events preserve durable receipts")
    l.execute("state.counts={}")
    start(l)
    ck(len(g.state.calls) == 8 and g.t6_count(g.gd) == 0 and g.t6_count(g.ga) == 0,
       "depleted stock never automatically replenishes on another gameplay event")
    l.execute("KCD2VRThrowingAssets1=nil")
    l.execute(source)
    start(l)
    ck(len(g.state.calls) == 8 and g.t6_count(g.gd) == 0 and g.t6_count(g.ga) == 0,
       "new Lua instance plus persistent save receipts never replenishes consumed inventory")

    l = fixture("state.counts[gd]=3;state.counts[ga]=1")
    start(l)
    g = l.globals()
    ck(g.t6_count(g.gd) == 5 and g.t6_count(g.ga) == 3 and len(g.state.calls) == 4,
       "existing supply is preserved and only missing starter units are requested")

    l = fixture("state.perks[dr]=true;state.perks[ar]=true")
    start(l)
    g = l.globals()
    ck(len(g.state.calls) == 0, "legacy claimed receipts stay terminal even when inventory is empty")

    l = fixture("g_localActor.id={opaqueNativeHandle=true}")
    start(l)
    g = l.globals()
    ck(len(g.state.calls) == 8, "current actor join supports native opaque entity handles without tonumber coercion")

    l = fixture("state.actorJoined=false")
    start(l)
    g = l.globals()
    ck(len(g.state.calls) == 0 and g.state.claimCalls == 0,
       "System.GetEntity mismatch cannot claim receipts or create inventory")
    ck(g.t6_timerCount() == 0, "wrong-actor readiness retry is bounded")
    ck(g.KCD2VRThrowingAssets1.TryStarter() != "DONE", "wrong-actor exhaustion never becomes false completion")

    l = fixture("outgoing=g_localActor;g_localActor=nil;player=outgoing")
    start(l)
    g = l.globals()
    ck(len(g.state.calls) == 0 and g.state.claimCalls == 0,
       "stale fallback player never substitutes for absent current g_localActor")
    ck(g.t6_timerCount() == 0, "absent-current-actor readiness retry is bounded")
    ck(g.KCD2VRThrowingAssets1.TryStarter() != "DONE", "missing-actor exhaustion never becomes false completion")

    for mode in ("initialPerkMode", "initialCountMode"):
        l = fixture("state." + mode + "='nil'")
        start(l)
        g = l.globals()
        ck(len(g.state.calls) == 0 and g.state.claimCalls == 0 and g.t6_timerCount() == 0,
           mode + " unavailable has bounded retries and no mutation")
        ck(g.KCD2VRThrowingAssets1.TryStarter() != "DONE",
           mode + " retry exhaustion never falsely completes delivery")

    l = fixture("UIAction=nil")
    g = l.globals()
    boot_quiet(l)
    g.t6_pump(650)
    ck(g.t6_timerCount() == 0 and g.t6_reads() == 0 and len(g.state.calls) == 0,
       "missing event subsystem only retries registration boundedly without boot inventory polling")

    l = fixture()
    g = l.globals()
    g.t6_emit("OnGameplayStarted")
    stale_tick = g.state.timers[g.state.timerSerial].f
    g.t6_emit("OnGameplayStarted")
    stale_tick()
    ck(g.t6_reads() == 0 and len(g.state.calls) == 0,
       "stale gameplay-start timer cannot run after a newer start generation")
    g.t6_pump(650)
    ck(len(g.state.calls) == 8, "only newest gameplay-start generation executes the finite delivery")

    l = fixture("g_localActor=nil")
    g = l.globals()
    g.t6_emit("OnGameplayStarted")
    g.t6_tick()
    ck(len(g.state.calls) == 0, "event retry does not grant until an exact current actor exists")
    l.execute("g_localActor=t6_newActor(2)")
    g.t6_pump(650)
    ck(len(g.state.calls) == 8, "bounded event retry recovers when the exact current actor becomes available")

    l = fixture()
    g = l.globals()
    g.t6_emit("OnGameplayStarted")
    g.t6_emit("OnGameplayEnded")
    g.t6_pump(650)
    ck(len(g.state.calls) == 0 and g.state.claimCalls == 0,
       "gameplay-end revokes a queued starter before any outgoing-world grant")
    start(l)
    ck(len(g.state.calls) == 8, "fresh gameplay-start permits event-owned delivery after outgoing timer cancellation")

    l = fixture("state.endAtCreate=1")
    start(l)
    g = l.globals()
    ck(len(g.state.calls) == 1 and g.t6_count(g.gd) == 1 and g.t6_count(g.ga) == 0,
       "world-ended event reentered from native create immediately stops subsequent item writes")
    terminal_quiet(l, "mid-create world revocation")

    l = fixture("state.swapAtCreate=1")
    start(l)
    g = l.globals()
    ck(len(g.state.calls) == 1 and g.t6_count(g.gd) == 1 and g.t6_count(g.ga) == 0,
       "current actor replacement after a native create prevents continuation on outgoing inventory")
    terminal_quiet(l, "mid-create current actor replacement")

    for leaf, at, expected in (("hasPerk", 1, 0), ("hasPerk", 2, 0),
                               ("count", 1, 0), ("count", 2, 1),
                               ("claim", 1, 0), ("create", 1, 1)):
        for action in ("end", "swap"):
            l = fixture(f"state.mutateLeaf='{leaf}';state.mutateAt={at};state.mutateAction='{action}'")
            start(l)
            g = l.globals()
            ck(len(g.state.calls) == expected,
               f"{action} reentered at native {leaf}#{at} prevents every later outgoing/foreign create")
            terminal_quiet(l, f"{action}/{leaf}#{at} revocation")

    for mode in ("noeffect", "throw_before", "throw_after"):
        l = fixture("state.claimMode='" + mode + "'")
        start(l)
        g = l.globals()
        ck(len(g.state.calls) == 0, "receipt " + mode + " never admits an unacknowledged item create")
        status = g.t6_native()
        ck(status == "CLAIM_REFUSED", "receipt " + mode + " returns truthful CLAIM_REFUSED")
        terminal_quiet(l, "receipt " + mode)

    for mode in ("false", "nil", "throw", "false_commit", "throw_commit"):
        l = fixture("state.failCreateAt=3;state.createMode='" + mode + "'")
        start(l)
        g = l.globals()
        expected = 3 if mode.endswith("_commit") else 2
        ck(g.t6_count(g.gd) == expected and len(g.state.calls) <= 6,
           "create " + mode + " stops uncertain family without creating remaining units")
        ck(g.t6_native() == "PARTIAL_UNCERTAIN", "create " + mode + " never reports completed delivery")
        before = g.t6_callsFor(g.gd)
        l.execute("state.failCreateAt=0;KCD2VRThrowingAssets1=nil")
        l.execute(source)
        start(l)
        ck(g.t6_callsFor(g.gd) == before, "create " + mode + " never replays a durable family claim in a new VM")
        terminal_quiet(l, "create " + mode)

    for fail_at in (1, 5, 6, 8):
        l = fixture(f"state.failCreateAt={fail_at};state.createMode='nil'")
        start(l)
        g = l.globals()
        ck(g.t6_native() == "PARTIAL_UNCERTAIN" and len(g.state.calls) <= 8,
           f"unit {fail_at} refusal is a bounded honest terminal result")
        failed_guid = g.gd if fail_at <= 5 else g.ga
        before = g.t6_callsFor(failed_guid)
        l.execute("state.failCreateAt=0;KCD2VRThrowingAssets1=nil")
        l.execute(source)
        start(l)
        ck(g.t6_callsFor(failed_guid) == before,
           f"unit {fail_at} uncertain-family receipt prevents later VM replay")

    for mode in ("nil", "throw"):
        l = fixture("state.countAckMode='" + mode + "'")
        start(l)
        g = l.globals()
        ck(len(g.state.calls) <= 1, "failed post-create count " + mode + " cannot continue uncertain writes")
        ck(g.t6_native() == "PARTIAL_UNCERTAIN", "failed post-create count " + mode + " reports uncertainty")
        terminal_quiet(l, "post-create count " + mode)

    l = fixture()
    g = l.globals()
    boot_quiet(l)
    l.execute("state.timers={};UIAction=nil")
    ck(g.t6_native() == "DONE" and len(g.state.calls) == 8,
       "qualified NativeReady fallback survives lost startup listeners/timers")
    ck(g.t6_native(10, 21) == "DONE" and len(g.state.calls) == 8,
       "fallback request change cannot duplicate the body-owned starter")
    terminal_quiet(l, "native fallback")
    before = len(g.state.calls)
    foreign = l.table_from({"id": 1})
    ck(g.KCD2VRThrowingAssets1.NativeReady(foreign, 11, 22) == "REFUSED" and len(g.state.calls) == before,
       "foreign actor table cannot enter qualified fallback")
    for body in (0, -1, 10.5, 9007199254740992):
        ck(g.t6_native(body, 23) == "REFUSED" and len(g.state.calls) == before,
           f"fallback rejects invalid native body {body}")
    g.KCD2VRThrowingAssets1.WorldEnded()
    ck(g.t6_native(10, 24) in ("ENDED_BODY", "ENDED_ACTOR") and len(g.state.calls) == before,
       "known outgoing native body cannot reopen inventory delivery")

    l = fixture()
    g = l.globals()
    ck(g.t6_native(10, 20) == "DONE" and len(g.state.calls) == 8,
       "initial native fallback establishes body-owned delivery result")
    l.execute("state.counts={};state.perks={}")
    ck(g.t6_native(11, 21) == "DONE" and len(g.state.calls) == 16,
       "new qualified native body with same actor and restored fresh soul state rereads save receipts without events")
    l.execute("state.counts={}")
    ck(g.t6_native(12, 22) == "ALREADY_CLAIMED" and len(g.state.calls) == 16,
       "another native body retains saved receipts and cannot replenish consumed inventory")
    l = fixture("g_localActor.id={opaqueNativeHandle=true};local prior=System.GetEntity;System.GetEntity=function(id) if id==1 then return g_localActor end return prior(id) end")
    g = l.globals()
    ck(g.kcd2vr_inventory_native_t7("1|1") == "1|DONE" and len(g.state.calls) == 8,
       "native numeric lookup and opaque actor-id lookup join the same current actor without coercion")
    # T7 catches the missed startup event through a separately qualified native
    # actor packet. No body, pelvis, hand, mode checkbox or render APIs exist.
    l = fixture("UIAction=nil;Script.SetTimer=nil")
    g = l.globals()
    boot_quiet(l)
    ck(g.kcd2vr_inventory_native_t7("1|1") == "1|DONE" and len(g.state.calls) == 8,
       "native actor catch-up grants finite stock after event/timer loss without hip API")
    l.execute("state.counts={}")
    ck(g.kcd2vr_inventory_native_t7("2|1") in ("2|DONE", "2|ALREADY_CLAIMED") and len(g.state.calls) == 8,
       "native actor catch-up never replenishes consumed units")
    for packet in ("3|2", "0|1", "3|0", "3|1|extra", "-1|1", "3|4294967296"):
        before = len(g.state.calls)
        ck("DONE" not in g.kcd2vr_inventory_native_t7(packet) and len(g.state.calls) == before,
           "native actor catch-up rejects foreign or malformed packet " + packet)
    g.KCD2VRThrowingAssets1.WorldEnded()
    ck(g.kcd2vr_inventory_native_t7("4|1") == "4|WAIT_WORLD" and len(g.state.calls) == 8,
       "native actor catch-up cannot reopen explicit gameplay end")
    l.execute(source) # Hot reload must not erase the explicit old-world end.
    ck(g.kcd2vr_inventory_native_t7("5|1") == "5|WAIT_WORLD" and len(g.state.calls) == 8,
       "hot reload retains explicit native end fence instead of fabricating a world start")
    g.KCD2VRThrowingAssets1.WorldStarted()
    ck(g.kcd2vr_inventory_native_t7("6|1") == "6|ALREADY_CLAIMED" and len(g.state.calls) == 8,
       "authentic next gameplay event reopens lifecycle but never replenishes saved stock")
    l = fixture("state.failCreateAt=1;state.createMode='false_commit';UIAction=nil")
    g = l.globals()
    ck(g.kcd2vr_inventory_native_t7("1|1") == "1|PARTIAL_UNCERTAIN",
       "catch-up preserves native create uncertainty")
    before = len(g.state.calls)
    ck(g.kcd2vr_inventory_native_t7("2|1") == "2|PARTIAL_UNCERTAIN" and len(g.state.calls) == before,
       "uncertain native catch-up is not retried")
    return checks, l.eval("_VERSION")


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--script", type=Path,
                    default=ROOT / "src/subsystems/throw_drop/resources/inventory_delivery_t6.lua")
    ap.add_argument("--pak", type=Path, help="Read the actual final unified PAK member and require source equality")
    ap.add_argument("--lupa-dir", type=Path, default=DEFAULT_DEPS)
    ap.add_argument("--report", type=Path,
                    default=ROOT / "review/subsystems/throw_drop/INVENTORY_DELIVERY_T6_LUA51.json")
    args = ap.parse_args()
    raw = args.script.read_bytes()
    pak_hash = None
    if args.pak:
        with zipfile.ZipFile(args.pak) as z:
            assert z.testzip() is None, "final PAK CRC failure"
            assert len({n.casefold() for n in z.namelist()}) == len(z.namelist()), "duplicate package member"
            actual = z.read(MEMBER)
        assert actual == raw, "final packaged delivery differs from canonical source bytes"
        raw = actual
        pak_hash = hashlib.sha256(args.pak.read_bytes()).hexdigest()
    if b"function kcd2vr_inventory_native_t8" in raw:
        # The prior event-only contract is intentionally replaced by T8 load admission.
        from test_inventory_delivery_t8 import run as run_t8
        t8=run_t8(raw.decode("utf-8"));checks=t8["checks"];version=t8["lua"]
    else:
        checks, version = run(raw.decode("utf-8"), load_lua(args.lupa_dir))
    report = {"result": "PASS", "checks": len(checks), "details": checks, "lua": version,
              "script": str(args.script), "script_sha256": hashlib.sha256(raw).hexdigest(),
              "actual_pak": str(args.pak) if args.pak else None, "pak_sha256": pak_hash,
              "native_leaves": "explicit native inventory/soul/event substitutions",
              "retail_save_serialization": "NOT_EXECUTED", "native_wuid_stack_behavior": "NOT_EXECUTED",
              "headset": "NOT_EXECUTED", "installed_files_changed": False}
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"PASS: actual T6 inventory delivery Lua5.1 {len(checks)} checks; report={args.report}")


if __name__ == "__main__":
    main()
