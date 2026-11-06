// DS-F38: bounded transactional lifecycle command FIFO (Codex F33 repro fixes).
#include "../src/vrik/constraint_op60.h"
#include <cstdio>
#include <cstring>
namespace co=kcd2vr::constraint_op60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static co::Op mk(co::Kind k,std::uint32_t id,std::uint32_t serial,std::uint64_t tick,std::uintptr_t body=1,std::uint64_t gen=1){
    co::Op o{};o.kind=k;o.key={id,0xBAD,0x777,body,gen};o.serial=serial;o.tick=tick;return o;
}
int main(){
    co::Op out{};
    // Codex repro 1 (exact): two DIFFERENT resources whose ids collided under %8 - both must drain
    CK(co::admit(mk(co::Kind::Remove,101,1,100)));
    CK(co::admit(mk(co::Kind::Remove,109,2,110)));
    CK(co::take(out)&&out.kind==co::Kind::Remove&&out.key.id==101&&out.serial==1);
    co::settle(out,true);
    CK(co::take(out)&&out.kind==co::Kind::Remove&&out.key.id==109&&out.serial==2);
    co::settle(out,true);
    // Codex repro 2 (exact): Create then Drive of the SAME resource - Drive must stay behind Create
    CK(co::admit(mk(co::Kind::Create,303,3,200)));
    CK(!co::admit(mk(co::Kind::Drive,303,4,210))); // refused while the Create is unexecuted
    CK(co::take(out)&&out.kind==co::Kind::Create&&out.key.id==303&&out.serial==3);
    co::settle(out,true);
    CK(co::admit(mk(co::Kind::Drive,303,5,220))); // admitted after the Create ACKed
    CK(co::take(out)&&out.kind==co::Kind::Drive&&out.key.id==303&&out.serial==5);
    co::settle(out,true);
    // Codex repro 3 (exact): recycled native id with a NEW generation is a different key
    CK(co::admit(mk(co::Kind::Remove,202,6,300,1,1)));
    CK(co::take(out)&&out.key.id==202&&out.key.sourceGeneration==1);co::settle(out,true);
    CK(co::admit(mk(co::Kind::Create,202,7,310,1,2))); // generation 2: admitted (different key)
    CK(co::take(out)&&out.kind==co::Kind::Create&&out.key.id==202&&out.key.sourceGeneration==2);
    co::settle(out,true);
    // Remove cancels an unsent Create of the SAME key; Create after pending Remove is dropped
    CK(co::admit(mk(co::Kind::Create,404,8,400)));
    CK(co::admit(mk(co::Kind::Remove,404,9,410)));
    CK(co::cancelCount.load()==1);
    CK(co::take(out)&&out.kind==co::Kind::Remove&&out.key.id==404&&out.serial==9);
    co::settle(out,true);
    CK(!co::take(out)); // the Create never executes
    CK(co::admit(mk(co::Kind::Remove,505,10,500)));
    CK(!co::admit(mk(co::Kind::Create,505,11,510))); // dropped
    CK(co::take(out)&&out.kind==co::Kind::Remove&&out.key.id==505&&out.serial==10);
    co::settle(out,true);
    // in-flight claim: a second take before settle cannot return the same op
    CK(co::admit(mk(co::Kind::Cleanup,606,12,600)));
    CK(co::take(out)&&out.serial==12);
    co::Op again{};CK(!co::take(again)); // claimed: nothing else pending
    co::settle(out,true);
    // two bodies sharing the same constraint id are different keys
    CK(co::admit(mk(co::Kind::Remove,707,13,700,0xA,1)));
    CK(co::admit(mk(co::Kind::Remove,707,14,710,0xB,1)));
    CK(co::take(out)&&out.serial==13&&out.key.sourceBody==0xA);co::settle(out,true);
    CK(co::take(out)&&out.serial==14&&out.key.sourceBody==0xB);co::settle(out,true);
    // invalid ops refused
    CK(!co::admit(mk(co::Kind::None,1,20,800)));
    CK(!co::admit(mk(co::Kind::Drive,0,21,810)));
    CK(!co::admit(mk(co::Kind::Create,2,0,820)));
    // full FIFO: explicit rejection, nothing lost
    {unsigned admitted=0;for(std::uint32_t s=100;s<200;++s)if(co::admit(mk(co::Kind::Remove,900+s,s,900+s)))++admitted;
     CK(admitted<=co::kFifo&&admitted>0); // bounded: the rest were BUSY (retry next tick), never silently evicted
     for(unsigned i=0;i<admitted;++i){CK(co::take(out));co::settle(out,true);}
     CK(!co::take(out));}
    if(fails)return 1;std::puts("constraint_op60 F38 FIFO OK");return 0;
}
