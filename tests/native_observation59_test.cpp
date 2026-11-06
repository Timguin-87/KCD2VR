#include "vrik/native_combat104.h"
#include <array>
#include <cstdio>
#include <cstring>
#include <vector>
using namespace kcd2vr::native_combat104;
static unsigned checks=0,failed=0;
static void ck(bool v,const char* text){++checks;if(!v){++failed;std::printf("FAIL %s\n",text);}}
struct Fixture59 {
    std::array<unsigned char,0x500> actor{};std::array<unsigned char,0xe00> state{};
    std::uintptr_t failAt=0;std::vector<std::pair<std::uintptr_t,std::size_t>> reads;
    template<class T>void put(unsigned off,T v){std::memcpy(state.data()+off,&v,sizeof(v));}
    Fixture59(){auto a=reinterpret_cast<std::uintptr_t>(actor.data()),s=reinterpret_cast<std::uintptr_t>(state.data());
        std::memcpy(actor.data()+0x210,&s,8);put(0xce0,a);put(0x38,1);put(0x218,-1);put(0x248,-1);put(0x278,2);put(0x1b8,1);}
    bool call(Observed& s,ReadReceipt59& receipt){return observe59(reinterpret_cast<std::uintptr_t>(actor.data()),reinterpret_cast<std::uintptr_t>(state.data()),s,
        [&](const void* at,auto& out){const auto address=reinterpret_cast<std::uintptr_t>(at);reads.push_back({address,sizeof(out)});
            if(address==failAt)return false;std::memcpy(&out,at,sizeof(out));return true;},receipt);}
};
int main(){
    {Fixture59 f;Observed s{};ReadReceipt59 r{};ck(f.call(s,r)&&s.phase==1&&s.type==-1&&s.zone==2,"actual helper accepts only native-valid copied fields");
     ck(f.reads.size()==10&&r.readMask==0x3ff&&r.offset==0&&!std::strcmp(r.why,"NONE"),"success uses exactly original10 reads with complete receipt");
     const unsigned offsets[]{0x210,0xce0,0x38,0x218,0x248,0x278,0x1b8,0x2a8,0x2d8,0x308};
     for(unsigned i=0;i<10;++i){const auto base=i?reinterpret_cast<std::uintptr_t>(f.state.data()):reinterpret_cast<std::uintptr_t>(f.actor.data());
        ck(f.reads[i].first==base+offsets[i]&&f.reads[i].second==(i<2?8:i<7?4:1),"same ordered native address and typed width, no larger snapshot read");}}
    {Observed s{};ReadReceipt59 r{};unsigned reads=0;
     ck(!observe59(0,0,s,[&](const void*,auto&){++reads;return false;},r)&&reads==0&&
        !std::strcmp(r.why,"STATE_ABSENT"),"absent state fails before any native memory read");}
    {Fixture59 f;for(unsigned off:{0x2a8u,0x2d8u,0x308u})f.put(off,static_cast<unsigned char>(1));Observed s{};ReadReceipt59 r{};
     ck(f.call(s,r)&&s.requested&&s.requestCandidate&&s.blocking&&r.requested==1&&r.candidate==1&&r.blocking==1,
        "valid native true booleans are copied with identical semantic meaning");}
    const unsigned offsets[]{0x210,0xce0,0x38,0x218,0x248,0x278,0x1b8,0x2a8,0x2d8,0x308};
    for(unsigned stop=0;stop<10;++stop){Fixture59 f;f.failAt=reinterpret_cast<std::uintptr_t>(stop?f.state.data():f.actor.data())+offsets[stop];
        Observed s{};ReadReceipt59 r{};ck(!f.call(s,r),"failed same read never becomes admission");
        ck(!std::strcmp(r.why,"READ_FAILED")&&r.offset==offsets[stop]&&r.readMask==((1u<<stop)-1)&&f.reads.size()==stop+1,
            "exact failed offset and completed-read mask; original short circuit prevents every later read");}
    for(unsigned off:{0x2a8u,0x2d8u,0x308u}){Fixture59 f;f.put(off,static_cast<unsigned char>(7));Observed s{};ReadReceipt59 r{};
        ck(!f.call(s,r)&&!std::strcmp(r.why,"BOOL_RANGE")&&r.offset==off&&r.readMask==0x3ff&&f.reads.size()==10,"nonboolean byte is identified but remains refused");
        ck((off==0x2a8?r.requested:off==0x2d8?r.candidate:r.blocking)==7,"receipt keeps actual invalid native byte without boolean coercion");}
    for(const auto& v:std::array<std::pair<unsigned,int>,5>{{{0x38,-1},{0x218,65},{0x248,256},{0x278,6},{0x1b8,3}}}){
        Fixture59 f;f.put(v.first,v.second);Observed s{};ReadReceipt59 r{};
        ck(!f.call(s,r)&&!std::strcmp(r.why,"TYPED_RANGE")&&r.offset==v.first&&f.reads.size()==10,"invalid typed field gets its own offset without weakening old range");}
    {Fixture59 f;std::uintptr_t wrong=0;std::memcpy(f.actor.data()+0x210,&wrong,8);Observed s{};ReadReceipt59 r{};
     ck(!f.call(s,r)&&!std::strcmp(r.why,"ACTOR_STATE_CHANGED")&&r.offset==0x210&&f.reads.size()==1,"changed actor link stops before state memory");}
    {Fixture59 f;f.put(0xce0,std::uintptr_t(0));Observed s{};ReadReceipt59 r{};
     ck(!f.call(s,r)&&!std::strcmp(r.why,"STATE_ACTOR_CHANGED")&&r.offset==0xce0&&f.reads.size()==2,"changed state backlink stops before fields");}
    {Fixture59 f;f.put(0x2a8,static_cast<unsigned char>(1));f.failAt=reinterpret_cast<std::uintptr_t>(f.state.data())+0x2d8;
     Observed s{};ReadReceipt59 r{};ck(!f.call(s,r)&&r.requested==1&&(r.readMask&(1u<<7))&&!(r.readMask&(1u<<8)),"partial receipt preserves an earlier bool read even when following read fails");}
    for(int phase:{128,256}){Fixture59 f;f.put(0x38,phase);f.put(0x278,7);Observed s{};ReadReceipt59 r{};
     ck(f.call(s,r)&&blockMetadataH14(s)&&!valid(s)&&observedContextValidH14(s),
        "H14 native block preparation/ready metadata is observable but never an attack-sector state");
     ck(f.reads.size()==10&&r.readMask==0x3ff&&!std::strcmp(r.why,"NONE"),
        "H14 classified native block context keeps identical ten reads and no polling");
     for(const auto& bad:std::array<std::pair<unsigned,int>,8>{{{0x38,64},{0x218,1},{0x248,7},{0x278,6},{0x278,8},{0x1b8,0},{0x1b8,2},{0x38,512}}}){
      Fixture59 b;b.put(0x38,phase);b.put(0x278,7);b.put(bad.first,bad.second);Observed got{};ReadReceipt59 receipt{};
      ck(!b.call(got,receipt)&&!blockMetadataH14(got),"H14 no other phase/type/input/zone/hand can borrow the block metadata observation");}
     for(unsigned flag:{0x2a8u,0x2d8u,0x308u}){Fixture59 b;b.put(0x38,phase);b.put(0x278,7);b.put(flag,std::uint8_t(1));Observed got{};ReadReceipt59 receipt{};
      ck(!b.call(got,receipt),"H14 changed request/candidate/blocking tuples stay unclassified rather than widening an enum");}
    }
    std::printf("native_observation59=%u/%u real_memory_reads=MOCKED read_order_and_count=ACTUAL_PRODUCTION_HELPER admission=UNCHANGED\n",checks-failed,checks);return failed?1:0;
}
