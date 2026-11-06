#!/usr/bin/env python3
"""Run the shipped Lua Use/preparation and native note getter with explicit spies.

Lua 5.4, not the game's Lua 5.1; game physics and native script leaves are mocked.
The original OnUsed call must survive every refused/throwing preparation exactly
once. A refused preparation must never report native physicalization success.
"""
import argparse
import ctypes
import ctypes.util
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
LUA = r'''
local checks=0
local function ck(v) checks=checks+1;assert(v,'pickup preparation Lua check '..checks) end
local user,e,item,definition,calls,mass,grams,lastNative,capture,notes
local function setup()
 calls={physical=0,used=0,admission=0};mass=0;grams=1000;lastNative=nil;capture=true;notes={}
 user={id=42};g_localActor=user
 item={CanUse=function()return true end,CanSteal=function()return false end,
       BelongsToDeadBody=function()return false end,GetId=function()return 7 end}
 function item:OnUsed(id) ck(id==42);calls.used=calls.used+1;return 'NATIVE_USED' end
 e={id=51,class='PickableItem',npcOnly=false,user=user,item=item,Properties={},Use=PickableItem.Use}
 function e:GetSlotCount() lastNative='slots';return 1 end
 function e:IsSlotValid(slot) ck(slot==0);lastNative='slotvalid';return true end
 function e:GetMass() lastNative='mass';return mass end
 function e:Physicalize(slot,kind,p)
  ck(lastNative=='admission' and slot==0 and kind==1 and p.density==-1 and p.mass==grams/1000)
  calls.physical=calls.physical+1;mass=p.mass;lastNative='physicalize'
 end
 definition={id=7,class='51555071-7c55-4da1-9b61-ee3c14fde18b'};PE_RIGID=1
 Framework={WUIDToString=function(id)lastNative='token';return '[Item-'..id..']' end}
 ItemManager={GetItem=function(id)ck(id==7);return definition end}
 System={GetEntity=function(id)lastNative='entity';if id==42 then return user elseif id==51 then return e end end,
  GetCVar=function(key)
   if key=='kcd2vr_door_capture_f5' then return capture and 1 or 0 end
   local n=key:match('^kcd2vr_pickup_prepare_note_v1:(%d+)$')
   if n then notes[#notes+1]=tonumber(n);return 0 end
   ck(key=='kcd2vr_small_pickup_v1:[Item-7]');calls.admission=calls.admission+1;lastNative='admission';return grams
  end}
end
setup();ck(PickableItem.OnUsed(e,user)=='NATIVE_USED');ck(calls.physical==1 and calls.used==1 and notes[1]==0)
ck(PickableItem.Use(e,user)=='NATIVE_USED');ck(calls.physical==1 and calls.used==2 and notes[2]==13)
setup();capture=false;ck(PickableItem.Use(e,user)=='NATIVE_USED');ck(calls.physical==1 and #notes==0)
local faults={
 {4,function()g_localActor={}end}, {4,function()e.class='BasicEntity'end},
 {4,function()e.Properties.bQuestItem=true end}, {4,function()e.npcOnly=true end},
 {4,function()e.user={}end}, {6,function()item.CanUse=function()return false end end},
 {7,function()item.CanSteal=function()return true end end},
 {8,function()item.BelongsToDeadBody=function()return true end end},
 {5,function()System.GetEntity=function()return {}end end},
 {10,function()definition.id=8 end}, {12,function()e.GetSlotCount=function()return 2 end end},
 {12,function()e.IsSlotValid=function()return false end end}, {13,function()mass=1 end},
 {13,function()mass=0/0 end}, {11,function()e.Physicalize=nil end},
 {9,function()Framework.WUIDToString=function()return 'bad'end end},
 {15,function()grams=0 end}, {15,function()grams=3001 end}, {15,function()grams=1.5 end},
 {2,function()item.CanUse=function()error('mock native error')end end},
 {16,function()local old=System.GetCVar;System.GetCVar=function(k)
   local v=old(k);if k=='kcd2vr_small_pickup_v1:[Item-7]'then e.Properties.bQuestItem=true end;return v end end}
}
for _,fault in ipairs(faults)do
 setup();fault[2]();ck(PickableItem.Use(e,user)=='NATIVE_USED')
 ck(calls.physical==0 and calls.used==1 and notes[1]==fault[1])
end
setup();e.Physicalize=function()calls.physical=calls.physical+1 end
ck(PickableItem.Use(e,user)=='NATIVE_USED');ck(calls.physical==1 and calls.used==1 and notes[1]==18)
setup();local original=KCD2VR_Lab.TryPreparePickupR24;KCD2VR_Lab.TryPreparePickupR24=nil
ck(PickableItem.Use(e,user)=='NATIVE_USED');ck(calls.physical==0 and notes[1]==1)
KCD2VR_Lab.TryPreparePickupR24=original
setup();ck(PickableItem.Use(e,nil)==false and calls.physical==0 and calls.used==0 and #notes==0)
print('actual shipped Lua: '..checks..' checks PASS; Lua5.4; native physics mocked')
'''


def lua_run(root):
    lib = ctypes.CDLL(ctypes.util.find_library("lua5.4"))
    lib.luaL_newstate.restype = ctypes.c_void_p
    lib.luaL_openlibs.argtypes = [ctypes.c_void_p]
    lib.luaL_loadbufferx.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t, ctypes.c_char_p, ctypes.c_char_p]
    lib.lua_pcallk.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_longlong, ctypes.c_void_p]
    lib.lua_tolstring.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
    lib.lua_tolstring.restype = ctypes.c_char_p
    lib.lua_close.argtypes = [ctypes.c_void_p]
    state = lib.luaL_newstate()
    lib.luaL_openlibs(state)
    try:
        scripts = [(root / "companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua").read_bytes(),
                   (root / "companion/source/Scripts/Entities/Items/PickableItem.lua").read_bytes(), LUA.encode()]
        for script in scripts:
            code = lib.luaL_loadbufferx(state, script, len(script), b"pickup-test", None)
            if not code:
                code = lib.lua_pcallk(state, 0, 0, 0, 0, None)
            if code:
                return {"passed": False, "error": lib.lua_tolstring(state, -1, None).decode()}
        return {"passed": True, "vm": "Lua5.4", "native_physics": "mocked"}
    finally:
        lib.lua_close(state)


def function(text, marker):
    start = text.index(marker)
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]


def native_run(build):
    note = function((ROOT / "src/door_signal19c4r4.cpp").read_text(), "static void DoorPickupNoteD85R5R1(")
    getter = (ROOT / "src/core/door_signal_hook19c4r4.inl").read_text()
    prelude = r'''
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <initializer_list>
#include "diagnostics/pickup_prepare_d85r5r1.h"
#include "diagnostics/interaction_receipt59c10.h"
#define __fastcall
#define KCD2VR_PICKUP_PREPARE_NOTE_D85R5R1 1
namespace kcd2vr::diagnostics::capture168 {
enum class Domain {Interaction};
struct Gate{std::atomic<unsigned> mask{0};std::atomic<std::uint64_t> session[1]{{1}};};
static Gate gate;static bool active(Domain,std::uint64_t){return gate.mask.load()!=0;}
}
static unsigned lines=0,originals=0;static std::uint64_t now=100;
namespace diagnostic_trace {static void Write(const char*){++lines;}}
static std::uint64_t DoorClock19c4r4(){return now;}
struct Channel {bool enabled(){return false;}bool pending(){return false;}
 bool field(const char*,std::uint64_t,std::uint32_t&){return false;}};
static Channel g_channel19c4r4;
static bool DoorNativeName19c4r4(void* h,char* out,std::size_t cap){if(std::strlen(static_cast<char*>(h))>=cap)return false;std::strcpy(out,static_cast<char*>(h));return true;}
static int DoorNativeEndInteger19c4r4(void*,std::uint32_t value){return int(value);}
static int original(void*,void*){++originals;return 99;}
static auto g_get_cvar_original19c4r4=&original;
'''
    tail = r'''
int main(){
 using namespace kcd2vr::diagnostics::pickup_prepare_d85r5r1;
 unsigned code=99,checks=0;
 const auto ck=[&](bool value){++checks;if(!value){std::fprintf(stderr,"native getter check%u failed\n",checks);return false;}return true;};
 #define CK(v) do{if(!ck(v))return 1;}while(0)
 CK(parse("kcd2vr_pickup_prepare_note_v1:0",code)&&code==0);
 CK(parse("kcd2vr_pickup_prepare_note_v1:18",code)&&code==18);
 for(const char* bad:{"kcd2vr_pickup_prepare_note_v1:19","kcd2vr_pickup_prepare_note_v1:-1","kcd2vr_pickup_prepare_note_v1:1x","kcd2vr_pickup_prepare_note_v1:","kcd2vr_pickup_prepare_note_v1:000"})CK(!parse(bad,code));
 DoorPickupNoteD85R5R1(0);CK(lines==0);
 auto get=[](const char* name){return DoorGetCVarDetour19c4r4(nullptr,const_cast<char*>(name));};
 CK(get("kcd2vr_pickup_prepare_note_v1:0")==99&&lines==0);
 kcd2vr::diagnostics::capture168::gate.mask=1;
 CK(get("kcd2vr_pickup_prepare_note_v1:0")==0&&lines==1);
 CK(get("kcd2vr_pickup_prepare_note_v1:19")==0&&lines==1);
 CK(get("r_ordinary_game_cvar")==99&&lines==1);
 for(unsigned i=0;i<200;++i)DoorPickupNoteD85R5R1(1);
 CK(lines==1);now+=101;DoorPickupNoteD85R5R1(2);CK(lines==2);
 for(unsigned i=0;i<200;++i){now+=101;DoorPickupNoteD85R5R1(i%19);}
 CK(lines<=129);
 const auto before=lines;kcd2vr::diagnostics::capture168::gate.mask=0;now+=20000;DoorPickupNoteD85R5R1(3);CK(lines==before);
 kcd2vr::diagnostics::capture168::gate.mask=1;kcd2vr::diagnostics::capture168::gate.session[0]=2;DoorPickupNoteD85R5R1(3);CK(lines==before+1);
 std::printf("actual native getter/note: %u checks PASS\n",checks);return 0;
}
'''
    cpp = build / "pickup_note.cpp"
    cpp.write_text(prelude + note + getter + tail)
    exe = build / "pickup_note"
    result = subprocess.run(["g++", "-std=c++17", "-O2", "-I" + str(ROOT / "src"), str(cpp), "-o", str(exe)], capture_output=True, text=True)
    if result.returncode:
        raise RuntimeError(result.stderr)
    result = subprocess.run([str(exe)], capture_output=True, text=True)
    return {"passed": result.returncode == 0, "output": result.stdout + result.stderr}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", type=Path)
    parser.add_argument("--report", type=Path)
    args = parser.parse_args()
    report = {"lua": lua_run(ROOT)}
    with tempfile.TemporaryDirectory(prefix="kcd2vr_pickup_notes_") as temp:
        report["native_note_getter"] = native_run(Path(temp))
    passed = all(r["passed"] for r in report.values())
    if args.baseline:
        report["baseline_lua"] = lua_run(args.baseline)
        report["baseline_missing_outcome_caught"] = not report["baseline_lua"]["passed"]
        passed &= report["baseline_missing_outcome_caught"]
    report["all_passed"] = bool(passed)
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
