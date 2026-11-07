// Actual F9 baseline/legacy cleanup policy; no global amplification.
#include "../src/ragdoll_lease59.h"
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <limits>
static int checks=0,fails=0;
static void ck(bool b,const char*s){++checks;if(!b){++fails;std::printf("FAIL %s\n",s);}}
struct IO {
 std::map<std::string,float> vars;unsigned writes=0;bool readable=true;
 bool Read(const char*n,float&v){auto x=vars.find(n);if(!readable||x==vars.end())return false;v=x->second;return true;}
 bool Write(const char*n,float v){++writes;vars[n]=v;return true;}
};
int main(){using namespace kcd2vr::ragdoll59;IO io;
 for(const auto&e:kEntries)io.vars[e.name]=.37f;const auto original=io.vars;Lease lease;
 ck(lease.Apply(io,false,3)==0&&!lease.On()&&!io.writes,"initial OFF");
 ck(lease.Apply(io,true,3)==0&&lease.On()&&!io.writes&&io.vars==original,"old INI x3 never changes global CVars");
 for(std::size_t i=0;i<kCount;++i)ck(lease.slot(i).present&&!lease.slot(i).owned&&lease.slot(i).wanted==.37f,"each baseline observed only");
 ck(lease.Apply(io,true,10)==0&&!io.writes,"repeated ON no amplification");
 io.vars[kEntries[7].name]=1;ck(lease.Reassert(io)==0&&!io.writes&&io.vars[kEntries[7].name]==1,"load/foreign reset never reasserted");
 ck(lease.Apply(io,false,3)==0&&!lease.On()&&!io.writes,"OFF observation writes nothing");
 Slot owned{true,true,1,3};io.vars[kEntries[7].name]=3;
 ck(Lease::Restore(io,kEntries[7],owned)&&!owned.owned&&io.vars[kEntries[7].name]==1,"legacy exact owned restore");
 owned={true,true,1,3};io.vars[kEntries[7].name]=2;const auto writes=io.writes;
 ck(!Lease::Restore(io,kEntries[7],owned)&&io.writes==writes&&io.vars[kEntries[7].name]==2,"foreign value preserved");
 owned={true,true,1,3};io.readable=false;
 ck(!Lease::Restore(io,kEntries[7],owned)&&io.writes==writes,"unreadable never overwritten");
 io.readable=true;owned={true,true,1,3};io.vars[kEntries[7].name]=std::numeric_limits<float>::quiet_NaN();
 ck(!Lease::Restore(io,kEntries[7],owned)&&io.writes==writes,"nonfinite never restored");
 std::printf("%d/%d F9 baseline/cleanup checks passed\n",checks-fails,checks);return fails?1:0;
}
