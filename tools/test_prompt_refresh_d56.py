"""Execute the D56 production refresh functions and the packaged Lua callback.

Native VM/Flash leaves are explicit substitutes. This does not claim visible
headset glyphs or execute the retail game.
"""
from pathlib import Path
import argparse, json, os, subprocess, tempfile

ROOT=Path(__file__).resolve().parents[1]

def body(text, signature):
    start=text.index(signature); p=text.index('{', start); depth=1; end=p+1
    while depth:
        depth+=(text[end]=='{')-(text[end]=='}'); end+=1
    return text[start:end]

def main():
    ap=argparse.ArgumentParser(); ap.add_argument('--out',type=Path,required=True)
    args=ap.parse_args(); out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
    src=(ROOT/'src/subsystems/hud_ui_controls/runtime/prompt_device_lock.cpp').read_text()
    production='\n'.join(body(src,s) for s in ('bool FlashWantedD56()',
        'void FlashServiceD56(', 'bool RequestFlashStyleD56('))
    scheduler=(ROOT/'src/native_script_t2.cpp').read_text()
    capacity='\n'.join(line for line in scheduler.splitlines() if
        line.startswith('std::array<std::atomic<Client>') or line.startswith('std::array<std::atomic<Wanted>'))
    capacity+='\n'+body(scheduler,'bool Register(Client client,Wanted want)')
    code=r'''
#include "subsystems/hud_ui_controls/runtime/prompt_refresh_d56.h"
#include "native_script_t2.h"
#include <atomic>
#include <array>
#include <cstdio>
#include <cstring>
#include <string>
#include <cassert>
namespace kcd2vr::diagnostic_trace {void Write(const char*){}}
namespace kcd2vr::native_script_t2 {
bool installed=true,registered=true,transport=true;unsigned calls=0;std::string reply,input;
bool replaceInCall=false;void replacement();
bool Register(Client,Wanted)noexcept{return registered;}
bool Install()noexcept{return installed;}
bool Context::CallPacket(const char* cb,const char* packet,char*out,std::size_t cap)const noexcept {
 assert(std::string(cb)=="kcd2vr_prompt_d56");++calls;input=packet;
 if(replaceInCall){replaceInCall=false;replacement();}
 if(transport)std::snprintf(out,cap,"%s",reply.c_str());return transport;
}
}
namespace kcd2vr::prompt_device_lock {
std::atomic<std::uint64_t> g_flash_request_d56{0},g_flash_done_d56{0};
std::atomic<bool>g_flash_ready_d56{false},g_suspended{false};
prompt_refresh_d56::State g_flash_state_d56;
bool PromptCaptureActiveD50(){return false;}
@PRODUCTION@
}
namespace kcd2vr::native_script_t2 {void replacement(){kcd2vr::prompt_device_lock::RequestFlashStyleD56(4);}}
namespace capacity_fixture {
using namespace kcd2vr::native_script_t2;
@CAPACITY@
template<int N>void Leaf(const Context&)noexcept{}
bool WantedLeaf()noexcept{return true;}
}
int main(){using namespace kcd2vr::prompt_device_lock;namespace vm=kcd2vr::native_script_t2;
 unsigned n=0;auto ck=[&](bool ok){++n;if(!ok){std::printf("FAIL check %u\n",n);std::abort();}};
 vm::Context c{1,2,1000,1};
 vm::registered=false;ck(!RequestFlashStyleD56(2)&&!FlashWantedD56());vm::registered=true;
 vm::installed=false;ck(!RequestFlashStyleD56(2)&&!FlashWantedD56());vm::installed=true;
 ck(RequestFlashStyleD56(1)&&FlashWantedD56());vm::reply="P56|1|2|8";
 FlashServiceD56(c);ck(vm::calls==1&&vm::input=="P56|1|2"&&!FlashWantedD56());
 FlashServiceD56(c);ck(vm::calls==1); // No perpetual refresh after receipt.
 RequestFlashStyleD56(4);vm::reply="P56|1|2|8";FlashServiceD56(c);
 ck(vm::calls==2&&FlashWantedD56()); // Old serial and wrong family cannot acknowledge.
 c.now=1099;FlashServiceD56(c);ck(vm::calls==2);
 vm::reply="P56|2|4|0";c.now=1100;FlashServiceD56(c);ck(vm::calls==3&&FlashWantedD56());
 vm::reply="P56|2|4|8junk";c.now=1200;FlashServiceD56(c);ck(vm::calls==4&&FlashWantedD56());
 vm::transport=false;c.now=1300;FlashServiceD56(c);ck(vm::calls==5&&!FlashWantedD56());
 c.now=5000;FlashServiceD56(c);ck(vm::calls==5); // Bounded retries, no timer keepalive.
 vm::transport=true;RequestFlashStyleD56(4);g_suspended=true;
 FlashServiceD56(c);ck(vm::calls==5);g_suspended=false;
 vm::reply="P56|3|4|1";FlashServiceD56(c);ck(vm::calls==6&&!FlashWantedD56());
 RequestFlashStyleD56(2);vm::reply="P56|4|2|0";FlashServiceD56(c);ck(vm::calls==7);
 c.generation=2;vm::reply="P56|4|2|8";FlashServiceD56(c);ck(vm::calls==8&&!FlashWantedD56());
 RequestFlashStyleD56(2);vm::reply="P56|5|2|8";vm::replaceInCall=true;FlashServiceD56(c);
 ck(vm::calls==9&&FlashWantedD56()); // Old completion never consumes the new UI/style event.
 vm::reply="P56|6|4|8";FlashServiceD56(c);ck(vm::calls==10&&!FlashWantedD56());
 kcd2vr::prompt_refresh_d56::State s;ck(!s.begin(0,1,1)&&!s.begin(1,0,1));
 namespace cap=capacity_fixture;
 std::array<vm::Client,9> leaves{cap::Leaf<0>,cap::Leaf<1>,cap::Leaf<2>,cap::Leaf<3>,cap::Leaf<4>,cap::Leaf<5>,cap::Leaf<6>,cap::Leaf<7>,cap::Leaf<8>};
 ck(!cap::Register(nullptr,nullptr));
 for(unsigned i=0;i<8;++i)ck(cap::Register(leaves[i],nullptr));
 ck(!cap::Register(leaves[8],nullptr));
 ck(cap::Register(leaves[6],&cap::WantedLeaf)&&cap::wanted[6]==&cap::WantedLeaf);
 std::printf("D56 prompt real request/service %u/%u checks PASS; native VM/Flash leaves substituted\n",n,n);
}
'''.replace('@PRODUCTION@',production).replace('@CAPACITY@',capacity)
    (out/'prompt_refresh.cpp').write_text(code)
    subprocess.run([os.environ.get('CXX','g++'),'-std=c++20','-O1','-I'+str(ROOT/'src'),
        str(out/'prompt_refresh.cpp'),'-o',str(out/'prompt_refresh')],check=True)
    cxx=subprocess.run([str(out/'prompt_refresh')],capture_output=True,text=True,check=True)
    from lupa.lua51 import LuaRuntime
    lua=LuaRuntime(unpack_returned_tuples=True)
    lua.execute((ROOT/'companion/source/Scripts/Startup/zzkcd2vr_prompt_d56.lua').read_text())
    lua.execute(r'''
checks=0
local function ck(x) checks=checks+1;assert(x)end
ck(kcd2vr_prompt_d56("P56|1|2")=="P56|UNAVAILABLE")
local calls={}
UIAction={CallFunction=function(...)calls[#calls+1]={...};return true end}
for _,p in ipairs({"","P56|0|2","P56|1|1","P56|1|4junk","P56|9007199254740992|2",
 "P56|1|2\n","P56|x|2",string.rep("a",65)})do ck(kcd2vr_prompt_d56(p)=="P56|REFUSED")end
ck(kcd2vr_prompt_d56({})=="P56|REFUSED" and #calls==0)
for _,style in ipairs({2,4})do
 calls={};ck(kcd2vr_prompt_d56("P56|12|"..style)=="P56|12|"..style.."|8")
 for _,c in ipairs(calls)do ck(c[2]==-1 and c[3]=="SetKcd2vrGlyphStyleD68" and c[4]==style and #c==4)end
end
UIAction.CallFunction=function()return false end
ck(kcd2vr_prompt_d56("P56|13|2")=="P56|13|2|0")
UIAction.CallFunction=function()error("unavailable UI")end
ck(kcd2vr_prompt_d56("P56|14|4")=="P56|14|4|0")
UIAction.CallFunction=function()end
ck(kcd2vr_prompt_d56("P56|15|2")=="P56|15|2|8")
''')
    result={'cxx':cxx.stdout.strip(),'lua_checks':lua.globals().checks,'passed':True,
        'native_vm_executed':False,'visible_headset_glyphs_observed':False}
    (out/'RESULTS.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))

if __name__=='__main__':main()
