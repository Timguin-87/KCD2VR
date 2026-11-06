#define main priorBridge67Main
#include "subsystems/alchemy/bridge67_test.cpp"
#undef main
int main(){reset();kcd2vr::minigame_hands45::alchemyMode67.store(kcd2vr::minigame_hands45::SixDofVrik);
 auto t=table(1002);t.sequence=2;b::finish(t);offer(t,14,.15,1002);b::finish(t);b::NativeFrame nf{};check(b::completed.read(nf));
 auto c=copy(nf);auto ni=input(1002);ni.proximityCraft1=ni.handsOnlyD53=true;n::State state{};auto d=state.step(c,ni);
 check(d.ref.entity==114);d.palmRayD70=true;d.aimH21={{1,0,0},d.palm.tick,4,5,6,1,true};b::nomination.publish({ni.context,d,ni.now});
 unsigned id=0;check(b::selectionNear68(t,id,1002)&&id==114);
 auto sideways=bounds(.05);sideways.world[7]=.10f;b::selectionBounds68(t,t.refs[14],sideways,1002);
 check(b::selectionNear68(t,id,1002)&&id==0); // current bound defeats old nominated distance
 offer(t,14,.15,1002);d.aimH21.tick--;b::nomination.publish({ni.context,d,ni.now});
 check(b::selectionNear68(t,id,1002)&&id==0);
 d.aimH21.tick++;d.aimH21.sourceEpoch++;b::nomination.publish({ni.context,d,ni.now});
 check(b::selectionNear68(t,id,1002)&&id==0);
 std::printf("D70 actual native selector: %u assertions PASS; copied geometry and provenance rechecked\n",checks);
}
