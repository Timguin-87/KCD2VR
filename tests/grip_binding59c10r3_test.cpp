#include "../src/vrik/grip_binding59c10r3.h"
#include <cstdint>
#include <cstdio>

struct Socket {
    std::uint64_t key=1,generation=2,tick=100,callback=4;
    void* character=reinterpret_cast<void*>(0x1000);int joint=208;
    float relative[7]{0,0,0,1,0,0,0},scale[3]{1,1,1};bool valid=true;
};
struct Grip {
    Socket captureBinding{};bool ready[2]{true,true};unsigned stable[2]{6,6};
    int candidate[2]{12,14};int mount=17,checkedCollisionSeed=23;
};
int main(){
    int checks=0,fail=0;
    const auto check=[&](bool ok,const char* name){++checks;if(!ok){++fail;std::printf("FAIL %s\n",name);}};
    using namespace kcd2vr::grip_binding59c10r3;
    const Socket original{};
    {Grip g{};Socket newer=original;newer.tick+=16;++newer.callback;
        check(!qualify(g,newer),"fresh callback preserves grip");
        check(g.ready[0]&&g.ready[1]&&g.stable[0]==6,"same binding preserves samples");}
    const auto changed=[&](Socket s,const char* reason){Grip g{};
        check(qualify(g,s),reason);
        check(!g.ready[0]&&!g.ready[1]&&!g.stable[0]&&!g.stable[1]&&
            !g.candidate[0]&&!g.candidate[1],"real change restarts both captures");
        check(g.mount==17&&g.checkedCollisionSeed==23,"mount and checked collision seed retained");
        g.ready[0]=true;g.stable[0]=6;
        check(!qualify(g,s)&&g.ready[0],"new binding retained after requalification");};
    {auto s=original;s.joint=242;changed(s,"hand to belt change");}
    {auto s=original;s.relative[6]=.61f;changed(s,"relative helper change");}
    {auto s=original;s.scale[2]=1.1f;changed(s,"model scale change");}
    {auto s=original;s.character=reinterpret_cast<void*>(0x2000);changed(s,"character instance change");}
    {auto s=original;++s.generation;changed(s,"item generation change");}
    {auto s=original;++s.key;changed(s,"item key change");}
    {auto s=original;s.valid=false;check(!same(original,s),"invalid is not stable binding");}
    std::printf("grip_binding59c10r3: %d checks, %d failures\n",checks,fail);return fail?1:0;
}
