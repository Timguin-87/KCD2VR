#include "vrik/tracking_grace111.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::tracking_grace111;
static int n=0;static void ck(bool v,const char*m){++n;if(!v){std::fprintf(stderr,"FAIL %s\n",m);std::exit(1);}}
int main(){
 ck(KeepLastGood(true,1000,0),"live always valid");
 ck(KeepLastGood(false,1000,950,80),"short dropout held");
 ck(KeepLastGood(false,1000,920,80),"grace boundary");
 ck(!KeepLastGood(false,1001,920,80),"grace expires");
 ck(!KeepLastGood(false,1000,0,80),"no prior sample");
 ck(!KeepLastGood(false,900,950,80),"clock reversal fails closed");
 std::printf("PASS %d tracking grace checks\n",n);return 0;
}