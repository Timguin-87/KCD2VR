#include "vrik/gore_skin_path_d52.h"
#include <map>
#include <cassert>
#include <cstdio>
#include <string>
using namespace kcd2vr::gore_skin_path_d52;
struct Memory {
    std::map<std::uintptr_t,unsigned char> bytes; unsigned reads=0;
    template<class T> void put(std::uintptr_t p,const T& v) { const auto* b=reinterpret_cast<const unsigned char*>(&v);for(unsigned i=0;i<sizeof(v);++i)bytes[p+i]=b[i]; }
    void string(std::uintptr_t p,const std::string& s) {for(unsigned i=0;i<=s.size();++i)bytes[p+i]=static_cast<unsigned char>(s.c_str()[i]);}
    template<class T> bool operator()(std::uintptr_t p,T& v) {++reads;auto* b=reinterpret_cast<unsigned char*>(&v);for(unsigned i=0;i<sizeof(v);++i){if(!bytes.count(p+i))return false;b[i]=bytes[p+i];}return true;}
};
int main() {
    constexpr std::uintptr_t base=0x180000000,att=0x10000,skin=0x20000,str=0x30000;
    Memory m;char out[260]{};
    m.put(att,base+0x3A65A08);m.put(att+0x78,skin);m.put(skin,base+0x406C9F0);
    m.put(base+0x406C9F0+0x20,base+0x113CE50);m.put(skin+0x28,str);
    m.string(str,"objects/characters/humans/male/hair/correct.skin");
    // Deliberate misleading first pointer used by the retired two-hop scan.
    m.put(att+0x18,std::uintptr_t(0x40000));m.string(0x40000,"objects/wrong_beard.skin");
    assert(path(m,true,base,att,out,sizeof(out))==Result::Found);
    assert(std::string(out).find("correct.skin")!=std::string::npos);
    const auto reads=m.reads;assert(path(m,false,base,att,out,sizeof(out))==Result::Unknown);assert(m.reads==reads);
    m.put(att,base+0x3A8F798);m.put(att+0x98,skin);assert(path(m,true,base,att,out,sizeof(out))==Result::Found);
    m.put(att+0x98,std::uintptr_t(0));assert(path(m,true,base,att,out,sizeof(out))==Result::Empty);
    m.put(att+0x98,skin);m.put(skin,std::uintptr_t(123));assert(path(m,true,base,att,out,sizeof(out))==Result::Unknown);assert(!out[0]);
    m.put(skin,base+0x406C9F0);m.bytes.erase(skin+0x28);assert(path(m,true,base,att,out,sizeof(out))==Result::Unknown);
    m.put(skin+0x28,str);m.string(str,"bad.skin.extra");assert(path(m,true,base,att,out,sizeof(out))==Result::InvalidPath);
    m.string(str,"objects/test.skin");assert(path(m,true,base,att,out,10)==Result::InvalidPath);assert(!out[0]);
    m.string(str,std::string(240,'a')+".skin");assert(path(m,true,base,att,out,sizeof(out))==Result::InvalidPath);
    assert(path(m,true,base,att,nullptr,0)==Result::InvalidPath);
    puts("gore_skin_path_d52: exact SKIN/VCLOTH, misleading neighbour, fingerprint, null, faults, truncation PASS");
}
