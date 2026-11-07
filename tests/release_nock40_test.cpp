#include "vrik/staged_hold596c2.h"
#include <cassert>
using namespace kcd2vr::staged596c2;
Command step(Loader& l,unsigned t,double g,bool quiver,double gap,bool hand=true,bool attached=true,bool loaded=false){return l.update(t,true,false,hand,g,quiver,gap,attached,loaded,true);}
int main(){Loader l;assert(step(l,100,0,false,1,false)==Command::None);assert(step(l,111,.9,true,1,false)==Command::Retrieve);
 assert(step(l,122,.9,true,1)==Command::None);assert(step(l,133,.9,false,.10)==Command::None);
 assert(step(l,144,0,false,.10)==Command::Nock);assert(l.state==Reload::AwaitNock);
 assert(step(l,155,0,false,.10,false)==Command::None); // no replay while awaiting acknowledgment
 assert(l.update(166,true,true,false,0,false,.1,true,true,true)==Command::None);assert(l.state==Reload::Ready);
 Loader away;step(away,100,0,false,1,false);step(away,111,.9,true,1,false);step(away,122,.9,false,.3);assert(step(away,133,0,false,.3)==Command::RemoveHand);
 Loader pending;step(pending,100,0,false,1,false);step(pending,111,.9,true,1,false);step(pending,122,.9,false,.1,true,false);assert(step(pending,133,0,false,.1,true,false)==Command::RemoveHand);
 Loader stale;step(stale,100,0,false,1,false);step(stale,111,.9,true,1,false);step(stale,122,.9,false,.1);assert(step(stale,300,0,false,.1)==Command::RemoveHand); // loss not nock
}
