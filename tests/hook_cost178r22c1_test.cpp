#include "diagnostics/hook_cost178r22.h"
#include <cassert>
#include <cstring>
// V66.178R28: R22C1's three extra slots on the R22b/R23 counters (microseconds, own-vs-original split).
int main(){
    using namespace kcd2vr::hookcost178r22;
    char text[2048]{};
    assert(summary(text,sizeof(text),5000)==0);
    add(Alignment,500);add(AlignmentNative,100);add(AlignmentPolicy,400);
    assert(summary(text,sizeof(text),5000)>0);
    assert(std::strstr(text,"HOOK_COST windowMs=5000 modOwnMs=1.0"));
    assert(std::strstr(text,"ALIGNMENT_QUERY calls=1 ms=0.5 maxUs=500"));
    assert(std::strstr(text,"ALIGNMENT_NATIVE calls=1")&&std::strstr(text,"ALIGNMENT_POLICY calls=1"));
    assert(summary(text,sizeof(text),5000)==0);
    {Scope measured(AnimationBody);}
    assert(counters[AnimationBody].calls.load()==1);
    {Original game(AnimationBody);}
    assert(summary(text,sizeof(text),5000)>0&&std::strstr(text,"ANIMATION_BODY_POLICY calls=1"));
    assert(counters[AnimationBody].us.load()==0&&counters[AnimationBody].origUs.load()==0);
}
