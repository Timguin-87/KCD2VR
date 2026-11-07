#include "../src/vrik/physical_handoff132.h"
#include <cstdio>
using kcd2vr::physical_handoff132::allow_bridge;
int main(){unsigned f=0;auto c=[&](bool x,const char*n){std::printf("%s %s\n",x?"PASS":"FAIL",n);if(!x)++f;};
 c(allow_bridge(4,true,true,true,true,true,0),"handling rung can bridge qualified same attachment without grip requirement");
 c(allow_bridge(6,true,true,true,true,true,.8),"full rung bridges while primary Grip is intentionally held");
 c(!allow_bridge(6,true,true,true,true,true,.1),"full rung release cannot keep old writer alive");
 c(!allow_bridge(6,false,true,true,true,true,.8),"different attachment cannot inherit bridge");
 c(!allow_bridge(6,true,false,true,true,true,.8),"unqualified historical grip cannot bridge");
 c(!allow_bridge(6,true,true,true,true,false,.8),"tracking loss ends bridge");return f?1:0;}
