#include "../src/pause_chord58c2.h"
#include <cassert>
int main(){
    kcd2vr::pause_chord58c2::State s;
    assert(!s.update(true,true,true,true).fire); // no phantom edge on startup
    s.update(true,true,false,false);
    auto a=s.update(true,true,true,true);assert(a.fire&&a.consume);
    for(int i=0;i<100;++i){a=s.update(true,true,true,i%2);assert(!a.fire&&a.consume);}
    s.update(true,true,false,false);
    a=s.update(true,true,true,false);assert(!a.fire&&!a.consume); // plain inventory untouched
    a=s.update(true,true,true,true);assert(a.fire&&a.consume); // late chord
    s.update(false,true,true,true);assert(!s.update(true,true,true,true).fire);
    s.update(true,true,false,false);assert(s.update(true,true,true,true).fire);
    s.update(true,false,true,true);assert(!s.update(true,true,true,true).fire);
    s.update(true,false,false,false);assert(!s.update(true,false,true,true).fire);
    assert(!s.update(true,true,true,true).fire); // VR menu close cannot leak pause
    s.update(true,true,false,false);assert(s.update(true,true,true,true).fire);
}
