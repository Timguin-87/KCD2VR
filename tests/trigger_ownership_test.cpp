#include "../src/core/trigger_ownership.h"
#include <cstdio>
#include <limits>
using kcd2vr::core::TriggerOwnership;
int main(){int n=0,p=0;auto check=[&](bool a,const char*t){++n;p+=a;std::printf("%s %s\n",a?"PASS":"FAIL",t);};TriggerOwnership l{},r{};
 check(r.sample(false,true,.8f)==.8f,"ordinary gameplay trigger is unchanged");
 check(r.sample(true,true,.9f)==0&&r.blocked,"menu/activation owns trigger immediately");
 check(r.sample(false,true,.9f)==0,"menu close while held cannot leak punch or index curl");
 check(r.sample(false,false,0)==0&&r.blocked,"tracking loss with zero value cannot rearm");
 check(r.sample(false,true,.9f)==0,"tracking recovery while held is still consumed");
 check(r.sample(false,true,.2f)==0,"partial trigger relaxation does not rearm");
 check(r.sample(false,true,.1f)==.1f&&!r.blocked,"tracked physical release rearms");
 check(r.sample(false,true,.9f)==.9f,"next deliberate pull owns normal gameplay");
 check(l.sample(false,true,.9f)==.9f,"other hand ownership is independent");
 r.sample(true,true,0);check(r.blocked,"released trigger while menu remains open is not transferred early");
 check(r.sample(false,true,std::numeric_limits<float>::quiet_NaN())==0&&r.blocked,"invalid sample cannot clear ownership");
 check(r.sample(false,true,0)==0&&!r.blocked,"closing with physical trigger released is immediately ready");
 std::printf("trigger_ownership=%d/%d\n",p,n);return p!=n;}
