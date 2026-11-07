#include "../src/diagnostics/geometry_scope40c1.h"
#include <cassert>
#include <cstring>
#include <cstddef>
#include <initializer_list>
#ifndef _MSC_VER
#define __fastcall
#define __try
#define __finally
#endif
static std::uint64_t GetTickCount64(){return 100;}
template<class T>T Rd(const void* p,std::uintptr_t off,T fallback){T value{};if(!p)return fallback;std::memcpy(&value,static_cast<const char*>(p)+off,sizeof(value));return value;}
namespace geom27 {
#include "../src/diagnostics/native_hit_result50c1.inl"
// NR1: the zone borrow is owned by native_state_context.inl; this fixture only sees that it is asked.
namespace vrstate596 {static int nr1Begins=0;static const void* nr1Victim=nullptr;
bool ReactionZoneBeginNR1(const void*,int,const void*& hs,int& restore,int& written,const void* victim) noexcept {++nr1Begins;nr1Victim=victim;hs=nullptr;restore=written=-1;return false;}
void ReactionZoneEndNR1(const void*,const void*,int,int) noexcept {}}
}
#include "../src/diagnostics/native_hit_receipt50c1.inl"
static int builds=0,dispatches=0;static const void* expectedReceiver;static const void* expectedTarget;
static void* expectedOut;static const void* expectedHit;static bool dispatchResult=false;static int action;
static void* __fastcall Build(const void* receiver,void* out,const void* target,int kind,const void* hit){
    ++builds;assert(receiver==expectedReceiver&&out==expectedOut&&target==expectedTarget&&kind==9&&hit==expectedHit);
    const void* value=&action;std::memcpy(out,&value,sizeof(value));return out;
}
static bool __fastcall Dispatch(const void* target,void* out){++dispatches;assert(target==expectedTarget&&out==expectedOut);return dispatchResult;}
int main(){
    int actor,target,other,collision,hit;const void* receiver[3]{nullptr,&actor,nullptr};const void* out=nullptr;
    expectedReceiver=receiver;expectedTarget=&target;expectedOut=&out;expectedHit=&hit;
    g_reaction_build_original50=Build;g_reaction_dispatch_original50=Dispatch;
    using geom27::g_inject_result178r35;
    assert(ReactionBuildDetour50(receiver,&out,&target,9,&hit)==&out);
    assert(!ReactionDispatchDetour50(&target,&out));assert(builds==1&&dispatches==1);
    assert(g_inject_result178r35.reactionCreated==-1&&g_inject_result178r35.dispatch==-1);
    {
      kcd2vr::geometry40c1::Scope scope(&actor,&collision,&target);
      assert(ReactionBuildDetour50(receiver,&out,&target,9,&hit)==&out);
      assert(!ReactionDispatchDetour50(&target,&out));
      assert(g_inject_result178r35.reactionCreated==1&&g_inject_result178r35.reactionKind==9&&g_inject_result178r35.dispatch==0);
      dispatchResult=true;assert(ReactionDispatchDetour50(&target,&out));assert(g_inject_result178r35.dispatch==1);
      {kcd2vr::geometry40c1::Scope otherScope(&actor,&collision,&other);g_inject_result178r35={};
       assert(ReactionBuildDetour50(receiver,&out,&target,9,&hit)==&out);assert(ReactionDispatchDetour50(&target,&out));
       assert(g_inject_result178r35.reactionCreated==-1&&g_inject_result178r35.dispatch==-1);}
    }
    assert(builds==3&&dispatches==4); // exactly one native call, unchanged arguments/result
    unsigned char details[16]{},info[0xa0]{};
    for(unsigned offset:{0u,1u,3u,7u}){
      std::memset(details,0,sizeof(details));details[offset]=1;g_inject_result178r35={};
      geom27::NoteInjectedSubmit178r35(info,details,true,true);
      const auto& r=g_inject_result178r35;
      assert(r.fields&&r.parry==(offset==0)&&r.blocked==(offset==1)&&r.perfect==(offset==3)&&r.master==(offset==7));
    }
    geom27::NoteInjectedSubmit178r35(info,details,false,false);
    assert(!g_inject_result178r35.fields&&g_inject_result178r35.perfect==-1&&g_inject_result178r35.master==-1);
}
