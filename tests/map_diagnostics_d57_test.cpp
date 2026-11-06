#include "subsystems/vrmap/runtime/map_diagnostics_d57.h"
#include <cstdio>
using kcd2vr::map_diagnostics_d57::Budget;
int main(){
    unsigned checks=0,failures=0;
    auto check=[&](bool pass,const char* why){++checks;if(!pass){++failures;std::printf("FAIL %s\n",why);}};
    Budget b;
    check(!b.admit(true,100,0,0,1),"OFF has no observer call");
    check(!b.admit(false,100,1,0,1),"capture without active map has no observer call");
    check(!b.admit(true,100,1,0,0),"invalid VM cannot observe");
    check(b.admit(true,100,1,0,1)&&b.used==1,"one current F2 capture can observe");
    check(!b.admit(true,599,1,0,1)&&b.used==1,"minimum 500ms interval");
    for(unsigned n=1;n<120;++n)check(b.admit(true,100+500ull*n,1,0,1),"within finite capture budget");
    check(!b.admit(true,100000,1,0,1)&&b.used==120,"budget cannot replenish itself");
    check(!b.admit(false,101000,1,0,1)&&b.used==120,"closing map cannot replenish budget");
    check(b.admit(true,102000,2,0,1)&&b.used==1,"new actual capture resets budget");
    b.unavailable=true;
    check(!b.admit(true,103000,2,0,1),"missing callback produces only one attempt");
    check(b.admit(true,104000,2,0,2)&&!b.unavailable,"new VM allows a new callback capability");
    check(b.admit(true,105000,0,3,2),"F5 independently observes");
    check(!b.admit(true,106000,0,0,2),"stop removes permission");
    check(!b.admit(true,UINT64_MAX,1,0,2),"clock overflow is rejected");
    std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;
}
