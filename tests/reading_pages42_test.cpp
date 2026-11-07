#include "core/reading_pages42.h"
#include <cassert>
using namespace kcd2vr::reading_pages42;
int main(){State s;
 bool lb=true,rb=true;float lx=1,rx=1;
 apply(false,{},lb,rb,lx,rx);assert(lb&&rb&&lx==1&&rx==1);
 apply(true,{},lb,rb,lx,rx);assert(!lb&&!rb&&lx==0&&rx==0);
 apply(true,{true,false},lb,rb,lx,rx);assert(lb&&!rb);

 assert(!s.step(true,1,1000,1).right); // held on entry is not a page turn
 s.step(true,1,1010,0);
 assert(s.step(true,1,1020,1).right);
 assert(s.step(true,1,1099,1).right);
 assert(!s.step(true,1,1100,1).right); // no repeated page spam
 s.step(true,1,1110,0);assert(s.step(true,1,1120,-1).left);
 assert(!s.step(false,1,1130,-1).left);
 assert(!s.step(true,2,1140,-1).left); // new book needs neutral
 s.step(true,2,1150,0);assert(s.step(true,2,1160,-1).left);
 assert(!s.step(true,2,1100,-1).left); // reverse clock retires input
}
