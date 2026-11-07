#include "../src/vrik/palm_selection59c9.h"
#include <cassert>
#include "../src/vrik/held_interaction114.h"
#include "../src/vrik/interaction_ownership111.h"
#include <limits>
int main(){using namespace kcd2vr; palm59c9::Pair p{};
for(int s=0;s<2;++s){p.eligible[s]=true;p.ray[s].hand=true;p.ray[s].tick=100;p.ray[s].y=1;}
p.ray[1].oy=.85f;
assert(palm59c9::choose(p,0,1,0,110).side==1);
p.locked=0;assert(palm59c9::choose(p,0,1,0,110).side==0);
p.locked=-1;p.eligible[1]=false;assert(palm59c9::choose(p,0,1,0,110).side==0);
assert(palm59c9::choose(p,0,3,0,110).side==-1);
assert(palm59c9::choose(p,0,1,0,181).side==-1);
assert(palm59c9::choose(p,std::numeric_limits<float>::quiet_NaN(),1,0,110).side==-1);
p.eligible[1]=true;p.ray[1].oy=.01f;
assert(palm59c9::choose(p,0,1,0,110).side==0); // right wins near tie
assert(palm59c9::choose(p,0,1,0,110,1).side==1); // existing left remains stable
palm59c9::Ranking r{};assert(r.offer(100,7,r.evaluate(1,100,7,.1f)));
assert(!r.offer(100,8,r.evaluate(1,100,8,.4f)));assert(r.id==7);
assert(r.evaluate(2,116,7,.1f)<r.evaluate(2,116,8,.09f));
assert(r.offer(200,9,r.evaluate(2,200,9,.4f))); // same native pass cannot retain stale winner
held_interaction114::State held{};physical_interaction106::Input in{};
in.enabled=interaction_ownership111::Comparison114(0,2,false,true);in.now_ms=100;in.hand[0]={true,true,true,0,false};
assert(!held_interaction114::update(held,in).hold);
in.now_ms=116;in.hand[0].grip=.9f;assert(held_interaction114::update(held,in).ray);
in.now_ms=140;in.hand[0].neutral=false;assert(held_interaction114::update(held,in).hold);
in.now_ms=150;in.hand[0].tracked=false;assert(!held_interaction114::update(held,in).hold);
in.now_ms=160;in.hand[0].tracked=true;in.hand[0].neutral=true;assert(!held_interaction114::update(held,in).hold);
return 0;}
