#!/usr/bin/env python3
"""Compile and replay the actual candidate CraftAttachmentReceipt36b body.

Native leaves use counters/fake memory. This tests the diagnostic filter,
bounded gate and refusal formatting; it is not a WHGame or headset test.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/frik_pose_probe.cpp"
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--out", type=Path, required=True, help="Directory for temporary generated fixture, executable and results")
OUT = parser.parse_args().out.resolve()
source = read_source(SOURCE)
start = source.index("static void CraftAttachmentReceipt36b(")
end = source.index('\n#include "vrik/tool_attachment59c10.inl"', start)
body = source[start:end]
OUT.mkdir(parents=True, exist_ok=True)

prefix = r'''
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "diagnostics/capture168.h"
#include "diagnostics/interaction_receipt59c10.h"
#include "vrik/smith_tool36.h"
#define __try try
#define __except(x) catch(...)
#define EXCEPTION_EXECUTE_HANDLER 1
namespace diagnostics { namespace capture168=kcd2vr::diagnostics::capture168; }
namespace kcd2vr::activity_input163 {
enum class Kind175 { Smithing, Alchemy };
struct Witness {
    std::atomic<std::uintptr_t> active[2]{};
    std::atomic<std::uint64_t> lifetime175{1};
    int phase=11;
    int SmithPhaseC1()const{return phase;}
} witness175;
}
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
static std::array<unsigned char,0x100> manager{},attachment{},object{},character{},entity{};
static std::atomic<void*> g_henry_char_attachment_manager{nullptr};
static int nativeReads=0,smithReads=0,helperReads=0;
static bool accepted=false;
template<class T>static bool Read(const void* p,T& v){++nativeReads;if(!p)return false;std::memcpy(&v,p,sizeof(v));return true;}
template<class M,class T>static void Put(M& m,unsigned offset,T v){assert(offset+sizeof(v)<=m.size());std::memcpy(m.data()+offset,&v,sizeof(v));}
static bool PhysVft6698(void* p,std::uintptr_t v){std::uintptr_t actual=0;return Read(p,actual)&&v==actual;}
static void* EntityById6694(std::uint32_t id){return id?entity.data():nullptr;}
static void Rtti(void*,char* s,unsigned n){std::snprintf(s,n,"FakeEntity");}
static bool Helper(void*,const char*,QuatT28* out,bool){++helperReads;*out={};return true;}
static auto g_attachment_helper_transform_original=&Helper;
static bool ReadSmithTool36(void*,std::uint32_t id,std::uintptr_t,
    kcd2vr::smith_tool36::Binding& out,kcd2vr::smith_tool36::ReadEvidenceI1* e){
    ++smithReads;out={};*e={};e->boundEntity=id;e->linkedEntity=99;
    e->holder=0xABC0;e->holderFields=true;e->holderHand=0;e->attached=1;
    e->keysRead=true;e->holderKey=e->handKey=0x11223344;
    e->refusal=accepted?"ACCEPTED_CURRENT_BINDING":"SMITH_LINKED_ENTITY_DIFFERS_FROM_BOUND_ENTITY";
    if(accepted){out.instance=0x1234;out.holder=e->holder;out.entity=id;out.side=0;out.tool=0;}
    return accepted;
}
static std::vector<std::string> records;
namespace diagnostic_trace {static void Write(const char* s){records.emplace_back(s);}}
'''
suffix = r'''
static unsigned checks=0;
static void ck(bool v,const char* why){++checks;if(!v)std::fprintf(stderr,"FAIL %s\n",why);assert(v);}
static void Name(const char* text){Put(attachment,0x10,text);}
static void Run(std::uint64_t tick){CraftAttachmentReceipt36b(object.data(),attachment.data(),tick);}
int main(){
    using namespace kcd2vr::activity_input163;
    namespace cap=diagnostics::capture168;
    Put(attachment,0,std::uintptr_t(0x3A657C0));Put(attachment,0x28,manager.data());
    Put(manager,0x70,character.data());Put(object,8,std::uint32_t(42));
    g_henry_char_attachment_manager=manager.data();witness175.active[0]=0x1234;
    Name("hand_right");Run(5000);
    ck(nativeReads==0&&smithReads==0&&helperReads==0&&records.empty(),"F5 off does no native diagnostic reads");
    cap::gate.select(cap::Domain::Interaction,true,5000);
    for(int i=0;i<1000;++i)for(const char* name:{"weapon_holster_left","weapon_bow","weapon_quiver","weapon_quiver_ammo"}){Name(name);Run(5000+i);}
    ck(smithReads==0&&helperReads==0&&records.empty(),"4000 unrelated callbacks never invoke tool readers/helpers or log");
    Name("hand_right");Run(6000);
    ck(smithReads==1&&helperReads==2&&records.size()==1,"relevant first observation is retained");
    ck(records.back().find("C37I1 F5_CRAFT_ATTACH")!=std::string::npos&&
       records.back().find("smithRefusal=SMITH_LINKED_ENTITY_DIFFERS_FROM_BOUND_ENTITY")!=std::string::npos&&
       records.back().find("linkedEntity=99 boundEntity=42")!=std::string::npos&&
       records.back().find("holderKey=0000000011223344 handKey=0000000011223344")!=std::string::npos,
       "actual formatter retains exact refusal and both IDs/keys");
    for(std::uint64_t t=6001;t<7000;++t)Run(t);
    ck(smithReads==1&&helperReads==2&&records.size()==1,"stable callback coalesces before costly native reads");
    accepted=true;Run(7000);
    ck(records.size()==2&&smithReads==2&&records.back().find("smithTool36=HAMMER")!=std::string::npos,
       "heartbeat re-observes a native attach that completed after the first sample");
    witness175.phase=14;Run(7100);Run(7350);
    ck(records.size()==3&&records.back().find("smithPhase=14")!=std::string::npos,"phase edge survives spacing coalescence");
    Name("entity_left");Run(7350);
    ck(records.size()==4&&records.back().find("attachment=entity_left")!=std::string::npos,"separate relevant attachment has its own gate");
    const int before=nativeReads;cap::gate.select(cap::Domain::Interaction,false,8000);Run(8000);
    ck(nativeReads==before&&records.size()==4,"stopping F5 ends all diagnostic reads immediately");
    cap::gate.select(cap::Domain::Interaction,true,8100);Run(8100);
    ck(records.size()==5,"new recorder session retains unchanged first binding");
    const auto initial=records.size();
    for(unsigned i=0;i<1000;++i){witness175.phase=i%2?11:14;Run(8350+i*250);}
    ck(records.size()==initial+255,"256-line per-slot budget remains finite across phase churn");
    const int after=nativeReads;Run(300000);
    ck(nativeReads==after,"expired finite lease performs no native diagnostic work");
    cap::gate.select(cap::Domain::Interaction,true,300001);
    witness175.active[0]=0;witness175.active[1]=0x5678;Name("entity_right");Run(300001);
    ck(records.back().find("kind=ALCHEMY")!=std::string::npos&&
       records.back().find("smithRefusal=NOT_SMITHING")!=std::string::npos,
       "shared receipt still observes alchemy without smith eligibility");
    std::printf("PASS %u checks of actual CraftAttachmentReceipt36b body; native leaves doubled\n",checks);
}
'''
test = OUT / "craft_receipt_extracted.cpp"
test.write_text(prefix + body + suffix)
binary = OUT / "craft_receipt_extracted"
command = ["g++", "-std=c++17", "-O0", "-Wall", "-Wextra", "-I" + str(ROOT / "src"), str(test), "-o", str(binary)]
compiled = subprocess.run(command, capture_output=True, text=True)
(OUT / "compile.log").write_text(compiled.stdout + compiled.stderr)
ran = subprocess.run([str(binary)], capture_output=True, text=True) if compiled.returncode == 0 else None
if ran:
    (OUT / "run.log").write_text(ran.stdout + ran.stderr)
result = {"source": str(SOURCE), "source_body_sha256": hashlib.sha256(body.encode()).hexdigest(),
          "compile_exit": compiled.returncode, "run_exit": ran.returncode if ran else None,
          "native_execution": False, "headset_test": False,
          "passed": compiled.returncode == 0 and ran is not None and ran.returncode == 0}
(OUT / "results.json").write_text(json.dumps(result, indent=2) + "\n")
print(json.dumps(result, indent=2))
if ran:
    print(ran.stdout + ran.stderr)
else:
    print(compiled.stdout + compiled.stderr)
raise SystemExit(0 if result["passed"] else 1)
