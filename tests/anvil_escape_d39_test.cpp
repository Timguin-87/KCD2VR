// Uses only the pre-D39 solver API, so the same behavioral regression fixture
// can execute against the supplied D38 source and the repaired source.
#include "subsystems/smithing/anvil_a2.h"
#include <cstdio>
namespace av39=kcd2vr::subsystems::smithing::anvil_a2;
int main(){
 av39::Anvil a{};a.locked=a.box=true;a.boxMin={9.7,19.85,0};a.boxMax={10.3,20.15,.8};a.top=.8;
 const av39::Probe tip[]={{{.4,0,0},.02}};
 const av39::Pose from{{9.65,20,.8190001},{0,0,0,1}},to{{9.65,20,.8190001},{0,-.382683432365,0,.923879532511}};
 const auto s=av39::solve(a,from,to,tip,1);
 std::printf("depthFrom=%.12f depthTo=%.12f corrected=%d fraction=%.9f startInside=%d\n",
   av39::penetration(a,from,tip,1),av39::penetration(a,to,tip,1),s.corrected,s.fraction,s.startInside);
 if(s.corrected){std::puts("FAIL: resting hammer cannot lift clear without sticking");return 1;}
 std::puts("anvil_escape_d39: PASS");
}
