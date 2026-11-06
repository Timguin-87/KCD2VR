#include "../src/vrik/gore_material_d54.h"
#include "../src/vrik/gorepak50.h"
#include <unordered_map>
#include <string>
#include <cstdio>
#include <cstring>
#include <fstream>
using namespace kcd2vr::gore_material_d54;
static int passed=0,failed=0;
#define CHECK(x) do {if(x)++passed;else{++failed;std::printf("FAIL line %d: %s\n",__LINE__,#x);}}while(0)
struct Memory {std::unordered_map<std::uintptr_t,unsigned char> b;
 template<class T>void put(std::uintptr_t at,const T& value){auto* p=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof(value);++i)b[at+i]=p[i];}
 template<class T>bool read(std::uintptr_t at,T& value){auto* p=reinterpret_cast<unsigned char*>(&value);for(unsigned i=0;i<sizeof(value);++i){auto it=b.find(at+i);if(it==b.end())return false;p[i]=it->second;}return true;}
 void text(std::uintptr_t at,const char* s){for(unsigned i=0;i<=std::strlen(s);++i)b[at+i]=s[i];}
};
struct IO {bool valid=true;int retainAt=-1,setFailAt=-1,readsFailAt=-1,retains=0,sets=0;bool failAfterApply=false,rollbackFails=false;int held=0,ownerChangesAt=-1,currentCalls=0;std::uintptr_t m[6]{};
 bool current(){return valid&&(ownerChangesAt<0||currentCalls++<ownerChangesAt);}
 bool retain(std::uintptr_t){if(retains++==retainAt)return false;++held;return true;}
 void release(std::uintptr_t){--held;}
 bool set(unsigned lod,std::uintptr_t value){const int call=sets++;if(call==setFailAt){if(failAfterApply)m[lod]=value;return false;}if(rollbackFails&&call>setFailAt)return false;m[lod]=value;return true;}
 bool equals(unsigned lod,std::uintptr_t value){if(int(lod)==readsFailAt){readsFailAt=-1;return false;}return m[lod]==value;}
};
int main(int argc,char** argv){
 constexpr std::uintptr_t base=0x180000000,att=0x20000000,obj=0x20001000,skin=0x20002000,lods=0x20003000,mat=0x20004000;
 Memory mem;mem.put(att,base+0x3A65A08);mem.put(base+0x3A65A08+0xE0,base+0x113CE50);mem.put(att+0x28,obj);mem.put(att+0x78,skin);
 mem.put(att+8,base+0x3A65958);mem.put(base+0x3A65958+0x10,base+0x1A72CA0);mem.put(obj+8,att+8);
 mem.put(obj,base+0x3A3F158);mem.put(base+0x3A3F158+0x70,base+0xF564EC);mem.put(base+0x3A3F158+0x80,base+0x4A34E0);
 mem.put(skin,base+0x406C9F0);mem.put(base+0x406C9F0+0x28,base+0xF56528);mem.put(skin+0x10,lods);mem.put(lods-4,std::uint32_t(1));mem.put(lods+8,mat);
 mem.put(mat,base+0x40283F0);mem.put(base+0x40283F0+0x40,base+0x113CE50);mem.put(mat+0x28,mat+0x100);mem.text(mat+0x100,"Objects\\Beard06.MTL");
 for(unsigned i=0;i<6;++i)mem.put(obj+0x10+8*i,std::uintptr_t(0));
 const auto read=[&](auto a,auto& v){return mem.read(a,v);};Snapshot s{};
 CHECK(snapshot(read,true,base,att,s));CHECK(s.object==obj&&s.skin==skin&&s.effective[0]==mat);CHECK(s.effective[1]==0);
 CHECK(!snapshot(read,false,base,att,s));mem.put(obj+0x10,mat);CHECK(snapshot(read,true,base,att,s));CHECK(s.replacement[0]==mat);
 mem.put(lods-4,std::uint32_t(7));CHECK(!snapshot(read,true,base,att,s));mem.put(lods-4,std::uint32_t(1));
 mem.put(obj,base+123);CHECK(!snapshot(read,true,base,att,s));mem.put(obj,base+0x3A3F158);
 mem.put(base+0x406C9F0+0x28,base+123);CHECK(!snapshot(read,true,base,att,s));mem.put(base+0x406C9F0+0x28,base+0xF56528);
 mem.put(obj+8,att+9);CHECK(!snapshot(read,true,base,att,s));mem.put(obj+8,att+8);
 char path[260]{};CHECK(name(read,true,base,mat,path,sizeof(path)));CHECK(!std::strcmp(path,"objects/beard06.mtl"));
 mem.text(mat+0x100,"bad' material");CHECK(!name(read,true,base,mat,path,sizeof(path)));CHECK(path[0]==0);
 Snapshot src{},dst{};src.object=10;dst.object=20;src.effective[0]=100;src.effective[1]=101;dst.replacement[0]=200;dst.replacement[1]=201;
 unsigned changed=0;
 {IO io;io.m[0]=200;io.m[1]=201;CHECK(transfer(io,src,dst,changed)==Result::Applied);CHECK(changed==2&&io.m[0]==100&&io.m[1]==101&&io.held==0);}
 {IO io;io.valid=false;CHECK(transfer(io,src,dst,changed)==Result::OwnerChanged);CHECK(io.sets==0);}
 {IO io;io.retainAt=2;CHECK(transfer(io,src,dst,changed)==Result::RetainFailed);CHECK(io.held==0&&io.sets==0);}
 {IO io;io.m[0]=200;io.m[1]=201;io.setFailAt=1;CHECK(transfer(io,src,dst,changed)==Result::Restored);CHECK(io.held==0&&changed==0&&io.m[0]==200&&io.m[1]==201);}
 {IO io;io.m[0]=200;io.m[1]=201;io.setFailAt=1;io.failAfterApply=true;CHECK(transfer(io,src,dst,changed)==Result::Restored);CHECK(io.m[0]==200&&io.m[1]==201&&io.held==0);}
 {IO io;io.m[0]=200;io.m[1]=201;io.readsFailAt=1;CHECK(transfer(io,src,dst,changed)==Result::Restored);CHECK(io.m[0]==200&&io.m[1]==201&&io.held==0);}
 {IO io;io.m[0]=200;io.m[1]=201;io.setFailAt=1;io.rollbackFails=true;CHECK(transfer(io,src,dst,changed)==Result::RestoreFailed);CHECK(io.held==0&&changed==0);}
 {IO io;auto empty=src;empty.effective[0]=empty.effective[1]=0;CHECK(transfer(io,empty,dst,changed)==Result::Empty);CHECK(io.sets==0&&io.held==0);}
 {IO io;io.m[0]=200;io.m[1]=201;io.ownerChangesAt=1;CHECK(transfer(io,src,dst,changed)==Result::OwnerChanged);CHECK(io.sets==0&&io.held==0);}
 {IO io;io.m[0]=200;io.m[1]=201;io.ownerChangesAt=3;CHECK(transfer(io,src,dst,changed)==Result::RestoreFailed);CHECK(io.sets==1&&io.held==0);}
 // Full legal event with every attachment/material AND all limb fields must
 // serialize as one complete chunk; a small buffer must fail without executing.
 kcd2vr::gorepak50::Event e{};e.target=123;e.cut=kcd2vr::gorepak50::Cut::Head;e.nAtt=24;e.nSw43=16;e.level43=1;e.materialTokenD54=0xFEDCBA9876543210ULL;
 std::string nm(63,'n'),sk(199,'s'),mtl(259,'m');e.headSkin=sk.c_str();e.headMtl=mtl.c_str();
 for(int i=0;i<24;++i){e.attName[i]=nm.c_str();e.attSkin[i]=sk.c_str();e.attMtlD54[i]=mtl.c_str();}
 for(int i=0;i<16;++i){e.swOrig[i]=nm.c_str();e.swMade[i]=nm.c_str();e.swSkin[i]=sk.c_str();}
 char chunk[kcd2vr::gorepak50::kEventLuaCapacity61]{};const int n=kcd2vr::gorepak50::buildLua(e,chunk,sizeof(chunk));
 CHECK(n>19000&&n<int(sizeof(chunk)));CHECK(std::strstr(chunk,"materialTokenD54=KCD2VR_Gore.nativeMaterialD54 and 'fedcba9876543210' or nil")!=nullptr);CHECK(std::strstr(chunk,"iD54=KCD2VR_Gore.nativeMaterialD54 and 24 or nil")!=nullptr);
 if(argc>1){std::ofstream emitted(argv[1]);emitted.write(chunk,n);}
 CHECK(std::strstr(chunk+n-8,"end")!=nullptr);char small[512]{};CHECK(kcd2vr::gorepak50::buildLua(e,small,sizeof(small))==0);
 e.headMtl="bad'code";e.attMtlD54[0]="bad'code";CHECK(kcd2vr::gorepak50::buildLua(e,chunk,sizeof(chunk))>0);CHECK(std::strstr(chunk,"bad'code")==nullptr);
 std::printf("gore_material_d54: %d passed, %d failed\n",passed,failed);return failed?1:0;
}
