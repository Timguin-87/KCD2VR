#include "vrik/native_admission147.h"
#include "vrik/melee_stroke118.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr;
static void check(bool b,const char* s){if(!b){std::fprintf(stderr,"FAIL %s\n",s);std::exit(1);}}
int main(){
    check(native_admission147::predictor(1,1,0,0,1),"native admitted slash");
    check(native_admission147::predictor(1,1,2,0,0),"native admitted thrust");
    for(int i=0;i<4;++i)if(i!=1)check(!native_admission147::predictor(i,1,0,0,1),"invalid wrapper admission");
    check(!native_admission147::predictor(1,0,0,0,1),"disabled action");
    check(!native_admission147::predictor(1,1,1,0,1),"excluded native action mode");
    check(!native_admission147::predictor(1,1,3,0,1),"excluded native action mode 3");
    check(!native_admission147::predictor(1,1,0,1,1),"consumed hit");
    check(!native_admission147::predictor(1,1,0,0,5),"special action");
    native_combat104::Observed s{};s.phase=1;
    for(int type:{-1,0,1,2})for(int phase:{1,2,4,8,16,128}){
        s.type=type;s.phase=phase;s.requestCandidate=true;
        check(!native_admission147::preparationBlocked44c1(s),"ordinary action/candidate is not a blanket follow-up input veto");
        s.requested=true;check(native_admission147::preparationBlocked44c1(s),"held input request retains its owner");s.requested=false;
    }
    s.type=55;check(native_admission147::preparationBlocked44c1(s),"special action remains protected");
    s.type=1;s.phase=32;check(native_admission147::preparationBlocked44c1(s),"unknown action phase remains protected");
    s={};s.phase=1;
    check(native_admission147::guardWritable(s),"neutral guard direction allowed");
    s.phase=128;s.blocking=true;check(native_admission147::guardWritable(s),"normal blocking direction allowed");
    for(int phase:{4,8,16,32}){s.phase=phase;s.type=1;s.requestCandidate=false;
        check(!native_admission147::guardWritable(s),"cleared preparation bit never allows stance overwrite during action");}
    for(int type:{-1,0,1})for(int phase:{1,2,4,8,16,128}){
        s.type=type;s.phase=phase;s.requested=s.requestCandidate=true;
        check(native_admission147::guardInput(s),"ordinary native input remains available despite pending request");
        check(!native_admission147::guardWritable(s),"input permission cannot authorize a direction write");
    }
    s.type=10;check(!native_admission147::guardInput(s),"special actions remain excluded from input assistance");
    s.type=1;s.phase=32;check(!native_admission147::guardInput(s),"unclassified phase remains excluded");
    s.type=-1;s.phase=256;s.requested=s.requestCandidate=false;
    check(native_admission147::guardHeldInput44c2(s,true),"R46C1 phase 256 retains a previously owned guard");
    check(!native_admission147::guardHeldInput44c2(s,false),"R46C1 phase 256 cannot acquire a foreign guard");
    check(!native_admission147::guardInput(s)&&!native_admission147::guardWritable(s),"R46C1 phase 256 retention grants neither new press nor zone writes");
    s.type=1;check(native_admission147::guardHeldInput44c2(s,true)&&!native_admission147::guardHeldInput44c2(s,false),"R59.16C7E-DECAP1: phase 256 with the block ACTION (type 1) retains OUR held input (13 Oct log: free block without opponent), never a foreign one");s.type=-1;
    s.phase=512;check(!native_admission147::guardInput(s),"R45: other high phases stay excluded");
    for(int hz:{60,90,120}){
        melee118::State st{};st.ready=true;int prepare=0,commit=0;double dt=1.0/hz;
        for(int f=1;f<=hz/4;++f){melee118::Input in{};in.eligible=in.known=true;in.dt=dt;
            in.axis=in.modelAxis={0,0,1};in.tip=in.modelTip={3*dt,0,0};in.hand=in.modelHand={1.5*dt,0,0};
            const auto v=melee118::update(st,in,{true});if(v.prepare)prepare=f;if(v.commit)commit=f;
        }
        check(prepare>0&&prepare*dt<=.07,"prepare before 70ms at 3m/s");
        check(commit>prepare&&commit*dt>=.095,"retain deliberate 30 cm commitment (R45)");
    }
    {   // V66.178R46 (ChatGPT R45 review, section 5): a 0.6 m blade turned 90 degrees in 150 ms about a FIXED wrist.
        melee118::State st{};st.ready=true;int prepare=0,commit=0;const double dt=.010,r=.6,step=1.5707963/15;
        for(int f=1;f<=15;++f){melee118::Input in{};in.eligible=in.known=true;in.dt=dt;
            const double a0=step*(f-1),a1=step*f;
            in.tip=in.modelTip={r*(std::cos(a1)-std::cos(a0)),r*(std::sin(a1)-std::sin(a0)),0};in.hand=in.modelHand={};
            in.axis=in.modelAxis={std::cos(a1),std::sin(a1),0};
            const auto v=melee118::update(st,in,{true});if(v.prepare)prepare=f;if(v.commit)commit=f;
            check(v.observation.reason!=melee118::Reason::Slow||f<2,"R46: a 6.3 m/s wrist cut is not BELOW_SPEED");}
        check(prepare>0&&commit>prepare,"R46: a wrist-driven cut with no hand translation prepares and commits");
    }
    {   // V66.178R51: a fast wind-up (hand pulled straight back at 2 m/s, tip whipping) is not a cut candidate.
        melee118::State st{};st.ready=true;bool any=false;
        for(int f=1;f<=20;++f){melee118::Input in{};in.eligible=in.known=true;in.dt=.010;in.modelForward={0,1,0};
            in.tip=in.modelTip={-.03,-.02,0};in.hand=in.modelHand={0,-.02,0};in.axis=in.modelAxis={1,0,0};
            const auto v=melee118::update(st,in,{true});any|=v.prepare||v.commit;}
        check(!any,"R51: a backward wind-up does not prepare an attack");
    }
    {   // R46: a slow wave with a slow tip stays idle.
        melee118::State st{};st.ready=true;bool any=false;
        for(int f=1;f<=30;++f){melee118::Input in{};in.eligible=in.known=true;in.dt=.010;in.tip=in.modelTip={.01,0,0};in.hand=in.modelHand={.003,0,0};in.axis=in.modelAxis={0,1,0};
            const auto v=melee118::update(st,in,{true});any|=v.prepare||v.commit;}
        check(!any,"R46: a 1 m/s wave is not an attack");
        st={};st.ready=true;bool hit=false,aborted=false;
        for(int f=0;f<12;++f){melee118::Input in{};in.eligible=in.known=true;in.dt=.01;
            in.axis=in.modelAxis={0,0,1};const double d=f<5?.03:0;
            in.tip=in.modelTip={d,0,0};in.hand=in.modelHand={d*.5,0,0};
            auto v=melee118::update(st,in,{true});hit|=v.commit;aborted|=v.abort;}
        check(!hit&&aborted,"short prepared reposition cancels without attack");
    }
    for(bool prepared:{false,true}){
        melee118::State st{};st.ready=st.active=true;st.kind=melee118::Kind::Cut;st.prepared=prepared;st.age=.04;
        melee118::Input in{};in.dt=.01;in.eligible=in.known=true;in.modelForward={0,1,0};
        in.axis=in.modelAxis={1,0,0};in.tip=in.modelTip={.03,-.025,0};in.hand=in.modelHand={.01,-.02,0};
        auto o=melee118::update(st,in,{true});
        check(!o.prepare&&!o.commit&&!st.active&&o.abort==prepared,"R51C1 continuing wind-up cannot bank attack travel");
    }
    // C56: exhaustively constrain the extra INPUT-ready set. A pending candidate
    // is not a held request. No predicate here authorizes a native field write.
    for(int phase=0;phase<=0x7ff;++phase)for(int type=-1;type<=64;++type)
      for(bool requested:{false,true})for(bool candidate:{false,true})for(bool owned:{false,true}){
        native_combat104::Observed v{};v.phase=phase;v.type=type;
        v.requested=requested;v.requestCandidate=candidate;
        const bool oldBusy=native_admission147::preparationBlocked44c1(v);
        const bool newBusy=native_admission147::preparationBlockedOwnedGuard56(v,owned);
        const bool exception=owned&&!requested&&(type==-1||type==1)&&phase==256;
        check(newBusy==(oldBusy&&!exception),"C56 only an owned free-block input becomes ready");
        if(oldBusy&&!newBusy)check(!native_admission147::guardWritable(v),"readiness never authorizes zone writes");
      }
    for(unsigned field=0;field<6;++field){
        native_combat104::Observed v{};v.phase=256;v.type=1;
        switch(field){case 0:v.phase=-1;break;case 1:v.phase=0x800;break;case 2:v.type=65;break;
            case 3:v.input=256;break;case 4:v.zone=6;break;case 5:v.hand=3;break;}
        check(native_admission147::preparationBlockedOwnedGuard56(v,true),"invalid current state stays busy");
    }
    for(int phase:{128,256}){
        native_combat104::Observed v{};v.phase=phase;v.zone=7;v.hand=1;
        check(native_combat104::observedContextValidH14(v)&&!native_combat104::valid(v),"H14 native block metadata is observation only");
        check(native_admission147::preparationBlocked44c1(v)&&
            !native_admission147::preparationBlockedOwnedGuard56(v,true)&&
            native_admission147::preparationBlockedOwnedGuard56(v,false),"H14 only exact current held owner permits original input from native block metadata");
        check(!native_admission147::guardWritable(v)&&!native_admission147::guardInput(v)&&
            !native_admission147::guardHeldInput44c2(v,true),"H14 block metadata grants no direction write, LT acquisition or parry repress");
        v.requested=true;check(native_admission147::preparationBlockedOwnedGuard56(v,true),"H14 foreign pending input remains protected");
    }
    std::puts("PASS native admission, guard safety and early preparation; no game execution");
}
