#include "ui_adaptive_hud57c1.h"
#include <cassert>
struct IO {int value=1;unsigned calls=0;bool ready=true,writeOk=true;
    bool Read(int& v){++calls;v=value;return ready;}
    bool Write(int v){++calls;if(!writeOk)return false;value=v;return true;}};
int main(){
    kcd2vr::ui_adaptive_hud57c1::Lease lease;IO io;
    assert(lease.Update(io,true,1)&&io.value==0&&lease.Owned());
    auto calls=io.calls;for(int i=0;i<10000;++i)assert(!lease.Update(io,true,1));assert(io.calls==calls);
    assert(lease.Update(io,false,1)&&io.value==1&&!lease.Owned());
    io.value=0;lease.Update(io,true,2);assert(!lease.Owned());lease.Update(io,false,2);assert(io.value==0);
    io.value=1;lease.Update(io,true,3);io.value=2;lease.Update(io,false,3);assert(io.value==2&&!lease.Owned());
    io.ready=false;calls=io.calls;lease.Update(io,true,4);assert(!lease.Owned()&&io.calls==calls+1);
    io.ready=true;lease.Update(io,true,5);assert(io.value==0&&lease.Owned());lease.Update(io,false,5);assert(io.value==2);
    io.writeOk=false;lease.Update(io,true,6);assert(!lease.Owned()&&io.value==2);
}
