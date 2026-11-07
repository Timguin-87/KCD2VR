#include "subsystems/vrmap/runtime/held_map_m1.h"
#include "diagnostics/capture168.h"
#include <cstdio>
int n=0,f=0;void ck(bool b,const char*s){++n;f+=!b;printf("%s %s\n",b?"PASS":"FAIL",s);}
int main(){namespace m=kcd2vr::held_map_m1;namespace c=kcd2vr::diagnostics::capture168;
m::Enable(true);ck(!m::Selected()&&!m::Requested(),"old saved held-map setting cannot replace native map");m::Select(true);ck(!m::Selected()&&!m::Requested(),"diagnostic selection cannot restore retired parchment");m::SetRequested31(true,1000);ck(!m::Requested(),"stale direct request cannot open parchment");
int nativeMapKeys=0;bool comfort=false;auto press=[&](){if(m::Selected())m::SetRequested31(true,2000);else{++nativeMapKeys;comfort=true;}};press();ck(nativeMapKeys==1&&comfort,"existing native map fallback remains selected");
c::Gate gate;ck(gate.cycleVrik(1000)==1,"F7 starts diagnostics");ck(gate.cycleVrik(1010)==2,"F7 detailed rung remains distinguishable");ck(gate.cycleVrik(1020)==0,"F7 can stop without changing gameplay");gate.select(c::Domain::HandReference,true,1030);ck(gate.active(c::Domain::HandReference,1040)&&!gate.active(c::Domain::Vrik,1040),"ShiftF10 recorder remains independent");gate.select(c::Domain::HandReference,false,1050);ck(!gate.active(c::Domain::HandReference,1060),"ShiftF10 stops only its recorder");printf("native map/capture %d/%d\n",n-f,n);return f?1:0;}
