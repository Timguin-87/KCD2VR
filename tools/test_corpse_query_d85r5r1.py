#!/usr/bin/env python3
"""Actual corpse Lua + actual native parser/F9 gates, with explicit API spies.
Runs system Lua5.4 (not retail Lua5.1). No game, ragdoll or headset execution.
"""
import argparse,ctypes,ctypes.util,json,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
LUA=r'''
local checks=0
local function ck(x)checks=checks+1;assert(x,'corpse query Lua check '..checks)end
local outputs={}
local names={'entries','tables','boneMethods','velocityMethods','current','deathMissing','deathErrors','dead','idUnreadable','eligible','scanned','boneCalls','boneErrors','finitePoints','nearest','nearestId','nearestBone'}
local function decode(s)
 local t={};for x in s:gmatch('[^|]+')do t[#t+1]=x end
 local a={};if t[10]=='F9'then for i,name in ipairs(names)do a[name]=tonumber(t[10+i])end end
 return t,a
end
local function query(op)
 local s=kcd2vr_corpse_d72('D72|'..(op or 'q')..'|1_1_1|0|1|0|0|0|0|0')
 outputs[#outputs+1]=s
 return decode(s)
end
local function setup(kind,pos)
 scans,bones,lookups,writes=0,0,0,0;entities={};list={};player={id=1};g_localActor=player
 System={GetEntity=function(id)lookups=lookups+1;return entities[id]end,
  GetEntitiesInSphere=function(p,r)scans=scans+1;ck(r==1.6);return list end}
 local e={id=42,pos={x=pos or .1,y=0,z=0},actor={}}
 function e.actor:IsDead()if kind=='death-error'then error('death unavailable')end;return kind~='living'end
 function e:GetBonePos(...)
  ck(select('#',...)==1);bones=bones+1
  if kind=='bone-error'then error('joint unavailable')end
  if kind=='no-joint'or (...)~='RightHand'then return nil end
  return self.pos
 end
 function e:SetVelocity(v)writes=writes+1;self.velocity=v end
 if kind=='death-missing'then e.actor={}end
 if kind=='unreadable-id'then e.id='corpse42'end
 entities[e.id]=e;list={e};return e
end
setup();list={};local t,a=query();ck(t[9]=='NO_NEAR_DEAD_BONE'and a.entries==0 and a.finitePoints==0 and a.nearest==-1 and scans==1 and bones==0)
local e=setup();e.GetBonePos=nil;t,a=query();ck(a.entries==1 and a.tables==1 and a.boneMethods==0 and a.velocityMethods==1 and a.current==0 and a.dead==0)
e=setup();e.SetVelocity=nil;t,a=query();ck(a.boneMethods==1 and a.velocityMethods==0 and a.current==0 and writes==0)
setup('living');t,a=query();ck(a.current==1 and a.dead==0 and a.boneCalls==0 and a.nearest==-1)
setup('death-missing');t,a=query();ck(a.deathMissing==1 and a.dead==0 and a.scanned==0)
setup('death-error');t,a=query();ck(a.deathErrors==1 and a.dead==0 and a.scanned==0)
setup('unreadable-id');t,a=query();ck(t[9]=='DEAD_ENTITY_ID_UNREADABLE'and a.dead==1 and a.idUnreadable==1 and a.eligible==0)
setup('no-joint');t,a=query();ck(a.dead==1 and a.eligible==1 and a.scanned==1 and a.boneCalls==31 and a.finitePoints==0 and a.boneErrors==0 and scans==1)
setup('bone-error');t,a=query();ck(a.boneCalls==31 and a.boneErrors==31 and a.finitePoints==0 and a.nearest==-1 and writes==0)
setup(nil,2);t,a=query();ck(t[9]=='NO_NEAR_DEAD_BONE'and a.finitePoints==1 and a.nearest==2 and a.nearestId==42 and a.nearestBone==1 and scans==1)
e=setup();t,a=query();ck(t[9]=='CANDIDATE'and a.nearest==.1 and a.finitePoints==1 and writes==0)
local id,token=t[3],t[4]
ck(kcd2vr_corpse_d72('D72|A|1_1_1|0|2|'..id..'|'..token..'|0|0|0'):match('ACQUIRED$')and writes==0)
ck(kcd2vr_corpse_d72('D72|D|1_1_1|0|3|'..id..'|'..token..'|.2|0|0'):match('VELOCITY_RETURNED$')and writes==1)
ck(kcd2vr_corpse_d72('D72|R|1_1_1|0|4|0|0|0|0|0'):match('RELEASED$')and writes==1)
setup();local plain=query('Q');local plainCounts={scans,bones,lookups,writes};ck(plain[10]==nil)
setup();local traced=query('q');ck(scans==plainCounts[1]and bones==plainCounts[2]and lookups==plainCounts[3]and writes==plainCounts[4])
for i=1,9 do if i~=4 then ck(plain[i]==traced[i])end end -- monotonically increasing lease token intentionally differs
setup();System.GetEntitiesInSphere=nil;t,a=query();ck(t[9]=='NO_QUERY'and a.entries==0 and scans==0)
e=setup();list={};for i=1,4097 do list[i]=e end;t,a=query();ck(t[9]=='QUERY_TOO_DENSE'and a.entries==4097 and a.boneCalls==0 and scans==1 and writes==0)
RESULTS=table.concat(outputs,'\n');CHECKS=tostring(checks)
print('actual corpse Lua: '..checks..' checks PASS; Lua5.4; native leaves modeled')
'''
def execute_lua():
 lib=ctypes.CDLL(ctypes.util.find_library('lua5.4'))
 lib.luaL_newstate.restype=ctypes.c_void_p
 lib.luaL_openlibs.argtypes=[ctypes.c_void_p]
 lib.luaL_loadbufferx.argtypes=[ctypes.c_void_p,ctypes.c_char_p,ctypes.c_size_t,ctypes.c_char_p,ctypes.c_char_p]
 lib.lua_pcallk.argtypes=[ctypes.c_void_p,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_longlong,ctypes.c_void_p]
 lib.lua_getglobal.argtypes=[ctypes.c_void_p,ctypes.c_char_p]
 lib.lua_tolstring.argtypes=[ctypes.c_void_p,ctypes.c_int,ctypes.c_void_p];lib.lua_tolstring.restype=ctypes.c_char_p
 lib.lua_close.argtypes=[ctypes.c_void_p]
 state=lib.luaL_newstate();lib.luaL_openlibs(state)
 try:
  for source in [(ROOT/'companion/source/zzkcd2vr_corpse_d72.lua').read_bytes(),LUA.encode()]:
   code=lib.luaL_loadbufferx(state,source,len(source),b'corpse-query-test',None)
   if not code:code=lib.lua_pcallk(state,0,0,0,0,None)
   if code:raise RuntimeError(lib.lua_tolstring(state,-1,None).decode())
  lib.lua_getglobal(state,b'RESULTS');wire=lib.lua_tolstring(state,-1,None).decode()
  lib.lua_getglobal(state,b'CHECKS');checks=int(lib.lua_tolstring(state,-1,None).decode())
  return wire,checks
 finally:lib.lua_close(state)

def native(out,wire):
 source=(ROOT/'src/subsystems/physics/saved_corpse_drag_d72.inl').read_text()
 start=source.index('static bool SavedQueryAuditDueD85R5R1(');end=source.index('static void SavedRetireD72(',start)
 actual=source[start:end]
 cpp=r'''
#include "subsystems/physics/saved_corpse_drag_d72.h"
#include <fstream>
#include <string>
#include <vector>
namespace sd72=kcd2vr::saved_corpse_d72;
static bool recording=false;static std::uint64_t g_savedQueryReportD85R5R1[2]{};
static std::vector<std::string> lines;
namespace kcd2vr::diagnostics::capture168 {enum class Domain{Physics};bool active(Domain,std::uint64_t){return recording;}}
namespace diagnostic_trace {void WriteFor(kcd2vr::diagnostics::capture168::Domain,const char* s){lines.emplace_back(s);}}
'''+actual+r'''
int main(int argc,char** argv){
 unsigned checks=0;const auto ck=[&](bool x){++checks;if(!x)std::fprintf(stderr,"native check %u failed\n",checks);return x;};
 #define CK(x) do{if(!ck(x))return 1;}while(0)
 std::ifstream in(argv[1]);std::string line;unsigned traced=0,ordinary=0;sd72::Reply last{};
 while(std::getline(in,line)){sd72::Reply r{};CK(sd72::parse(line.c_str(),1,r));const bool extended=line.find("|F9|")!=std::string::npos;
  CK(r.audit.valid==extended);if(extended){++traced;last=r;}else ++ordinary;
  if(!extended)continue;
  const auto end=line.find("|F9|");auto plain=line.substr(0,end);sd72::Reply p{};CK(sd72::parse(plain.c_str(),1,p));
  CK(r.id==p.id&&r.token==p.token&&r.bone==p.bone&&!std::strcmp(r.result,p.result)&&r.point.x==p.point.x);
 }
 CK(traced==14&&ordinary==1);
 CK(!SavedQueryAuditDueD85R5R1(0,100));SavedQueryAuditD85R5R1(0,last,{},100);CK(lines.empty());
 recording=true;CK(SavedQueryAuditDueD85R5R1(0,100));SavedQueryAuditD85R5R1(0,last,{1,2,3},100);CK(lines.size()==1);
 CK(!SavedQueryAuditDueD85R5R1(0,100)&&!SavedQueryAuditDueD85R5R1(0,2099));CK(SavedQueryAuditDueD85R5R1(1,100));
 CK(SavedQueryAuditDueD85R5R1(0,2100));SavedQueryAuditD85R5R1(0,last,{},2100);CK(lines.size()==2);
 recording=false;SavedQueryAuditD85R5R1(0,last,{},4100);CK(lines.size()==2&&!SavedQueryAuditDueD85R5R1(0,4100));
 sd72::Reply r{};CK(!sd72::parse("D72|1|0|0|0|nan|0|0|NO_NEAR_DEAD_BONE",1,r));
 CK(!sd72::parse("D72|2|0|0|0|0|0|0|NO_NEAR_DEAD_BONE",1,r));
 CK(!sd72::parse("D72|1|0|0|0|0|0|0|NO_NEAR_DEAD_BONE|UNKNOWN",1,r));
 CK(sd72::parse("D72|1|0|0|0|0|0|0|NO_NEAR_DEAD_BONE|F9|bad",1,r)&&!r.audit.valid);
 CK(sd72::parse("D72|1|0|0|0|0|0|0|NO_NEAR_DEAD_BONE|F9|0|0|0|0|0|0|0|0|0|0|0|0|0|0|nan|0|0",1,r)&&!r.audit.valid);
 std::printf("actual corpse native parser/F9 gate: %u checks PASS; traced=%u ordinary=%u\n",checks,traced,ordinary);return 0;
}
'''
 (out/'corpse_query_actual.cpp').write_text(cpp);(out/'corpse_query_packets.txt').write_text(wire+'\n')
 exe=out/'corpse_query_actual'
 build=subprocess.run(['g++','-std=c++17','-O2','-I'+str(ROOT/'src'),str(out/'corpse_query_actual.cpp'),'-o',str(exe)],capture_output=True,text=True)
 if build.returncode:raise RuntimeError(build.stderr)
 result=subprocess.run([str(exe),str(out/'corpse_query_packets.txt')],capture_output=True,text=True)
 return {'passed':result.returncode==0,'output':result.stdout+result.stderr}

def main():
 p=argparse.ArgumentParser();p.add_argument('--out',type=Path);args=p.parse_args()
 out=args.out or Path(tempfile.mkdtemp(prefix='kcd2vr_corpse_query_'));out.mkdir(parents=True,exist_ok=True)
 wire,checks=execute_lua();report={'lua':{'passed':True,'checks':checks,'runtime':'Lua5.4','native_leaves':'modeled'},'native':native(out,wire)}
 (out/'corpse_query_result.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
 if not report['native']['passed']:raise SystemExit(1)
if __name__=='__main__':main()
