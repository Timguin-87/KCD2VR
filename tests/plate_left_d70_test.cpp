#define KCD2VR_INPUT67_NO_MAIN
#include "subsystems/alchemy/input67_test.cpp"
int main(){Fixture q;q.in.proximityCraft1=q.in.handsOnlyD53=true;q.in.nativeFrame.table.actionKnown=true;
 q.admit(0,1);q.tool(1,0);q.in.tool[1].binding={1,2,888,777,100,1,0,0,0,666,3};q.step();q.advance(90);
 auto& o=q.in.nativeFrame.offers[6];o={{6,106},{.05,0,0},q.in.now,true};o.boundsValid=true;
 for(int j=0;j<3;++j){o.bounds.local[j]=-.01f;o.bounds.local[j+3]=.01f;o.bounds.world[j*4+j]=1;}o.bounds.world[3]=.05f;
 q.selected(6);q.step();q.advance();q.f.hand[1].rotation[0]=std::sin(.6f);q.f.hand[1].rotation[3]=std::cos(.6f);
 const auto result=q.step();std::printf("leftPlateUse=%d leftPlateReturn=%d\n",result.use,result.ordinaryReturnRequested);
 return result.use&&result.ordinaryReturnRequested?0:1;
}
