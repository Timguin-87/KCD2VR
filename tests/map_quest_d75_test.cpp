#include "subsystems/vrmap/quest_packet_d75.h"
#include <cassert>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <limits>
using namespace kcd2vr::held_map_feed_t2;
struct Memory {
 std::map<std::uintptr_t,std::vector<unsigned char>> b;
 bool operator()(std::uintptr_t p,void* q,std::size_t n){auto i=b.upper_bound(p);if(i==b.begin())return false;--i;
  if(p-i->first+n>i->second.size())return false;std::memcpy(q,i->second.data()+p-i->first,n);return true;}
 template<class T>void put(std::uintptr_t p,T v){auto i=b.upper_bound(p);assert(i!=b.begin());--i;assert(p-i->first+sizeof v<=i->second.size());std::memcpy(i->second.data()+p-i->first,&v,sizeof v);}
};
static unsigned checks=0;static void check(bool b){++checks;assert(b);}
int main(){
 const std::uintptr_t base=0x180000000ull;Memory m;
 for(auto [p,n]:std::initializer_list<std::pair<std::uintptr_t,std::size_t>>{{base+0x549D388,8},{0x100000,256},{0x200000,256},{0x300000,32},{0x400000,256},{0x500000,32},{0x600000,256},{0x700000,256}})m.b[p].resize(n);
 m.put(base+0x549D388,std::uintptr_t(0x100000));m.put(0x1000e8,std::uintptr_t(0x200000));m.put(0x200040,Span{0x300000,0x300010});
 m.put(0x300000,std::uintptr_t(0x400000));m.put(0x400000,base+0x3c118b8);m.put(0x400060,Span{0x500000,0x500020});
 for(unsigned i=0;i<2;++i){auto p=std::uintptr_t(0x600000+i*0x100000);m.put(0x500000+i*16,p);m.put(p,base+0x3a6b4c8);m.put(p+0x10,base+0x3a6b4f0);
  m.put(p+0x4c,i?86:2);m.put(p+0x50,1234.f+i);m.put(p+0x54,2345.f);m.put(p+0x58,12.f);m.put(p+0x68,i?0:2);m.put(p+0x6c,int(4+i));m.put(p+0x74,1);}
 NativeSnapshot s;check(Capture(m,base,s)&&s.compassKnown&&s.count==2);
 check(s.markers[0].tracking==2&&s.markers[0].objective==4&&s.markers[0].state==1);
 check(s.markers[1].kind==Kind::Quest&&s.markers[1].nativeType==86&&s.markers[1].tracking==0);
 char out[4096]{};float pin[2]{1200,2300};
 check(EncodeQuestD75(s,true,true,pin,5,7,42,100,out));
 check(std::string(out)=="Q75|5|7|42|100|1|1200.000,2300.000|2,2,4,1234.000,2345.000;86,0,5,1235.000,2345.000;");
 check(EncodeQuestD75(s,false,false,pin,5,7,42,100,out)&&std::string(out)=="Q75|5|7|42|100|0||");
 check(!EncodeQuestD75(s,true,false,pin,0,7,42,100,out));
 s.count=97;check(!EncodeQuestD75(s,true,false,pin,5,7,42,100,out));
 s.count=96;for(auto& q:s.markers){q=s.markers[0];q.id=0xfeedbeef;}
 check(EncodeQuestD75(s,true,false,pin,5,7,42,100,out));
 unsigned n=0;for(const auto* p=out;*p;++p)n+=*p==';';check(n==16&&std::string(out).find("feedbeef")==std::string::npos);
 s.markers[0].x=std::numeric_limits<float>::quiet_NaN();s.count=1;
 check(EncodeQuestD75(s,true,false,pin,5,7,42,100,out)&&std::string(out)=="Q75|5|7|42|100|1||");
 m.put(0x600010,std::uintptr_t(0));check(Capture(m,base,s)&&s.count==1&&s.markers[0].nativeType==86);
 m.put(0x400068,std::uintptr_t(0x500021));check(!Capture(m,base,s));
 std::printf("D75 current native marker offsets / bounded value packet: %u checks PASS\n",checks);
}
