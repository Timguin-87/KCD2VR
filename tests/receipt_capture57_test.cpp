#include <cstdint>
#include <cassert>
#include <cstdio>
#include "diagnostics/receipt_budget56.h"
static std::uint64_t now57=5000;
static std::uint64_t GetTickCount64(){return now57;}
namespace kcd2vr::diagnostics::capture168 {
enum class Domain {Combat};
static bool enabled57=false;
static bool active(Domain,std::uint64_t){return enabled57;}
}
#include "receipt_capture57.inc"
static unsigned formatted57=0;
static void RefusalSite57(){if(Allow(4,4))++formatted57;}
static void ChattySite57(){if(Allow(4,30))++formatted57;}
int main(){
 using namespace kcd2vr::diagnostics::capture168;
 for(int i=0;i<100;++i){RefusalSite57();ChattySite57();}
 assert(formatted57==0); // Actual macro short-circuits before formatting/budget.
 enabled57=true;
 for(int i=0;i<35;++i)ChattySite57();assert(formatted57==30);
 for(int i=0;i<8;++i)RefusalSite57();assert(formatted57==34);
 enabled57=false;now57=6000;
 for(int i=0;i<20;++i)RefusalSite57();assert(formatted57==34);
 enabled57=true;
 for(int i=0;i<8;++i)RefusalSite57();assert(formatted57==38);
 std::puts("actual Allow macro: capture-off silent, independent refusal cap PASS");
}
#undef Allow
