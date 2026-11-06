#define main d61_prior_main
#include "subsystems/alchemy/unacquired_admission_d61_test.cpp"
#undef main
int main(){
 for(int fault=0;fault<6;++fault){Fixture q;waitingD61(q);q.tool(0,4);switch(fault){case 0:q.in.tool[0].tick=q.in.now-81;break;case 1:++q.in.tool[0].lifetime;break;case 2:++q.in.tool[0].poseEpoch;break;case 3:++q.in.tool[0].instance;break;case 4:q.in.tool[0].side=1;break;case 5:q.in.work.parent=999;q.in.tool[0].joined=false;break;}auto o=q.step();check(o.unacquiredRetiredD61&&!o.admitted&&!o.use);}
 {Fixture q;waitingD61(q);q.tool(0,4);auto o=q.step();check(!o.unacquiredRetiredD61&&o.admitted&&!o.use);}
 recoveredPlateD61();retainCounterexamplesD61();std::printf("D67 stale binding %u checks PASS\n",checks);
}
