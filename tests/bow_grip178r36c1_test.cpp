#include "../src/vrik/bow_grip178r36c1.h"
#include <cstdio>
#include <limits>
namespace b=kcd2vr::bow_grip178r36c1;
namespace ph=kcd2vr::physical;
namespace m=kcd2vr::pose_core;
static int checks=0,failures=0;
static void ck(bool yes,const char* text){++checks;if(!yes)++failures;std::printf("%s %s\n",yes?"PASS":"FAIL",text);}
static bool same(ph::Pose a,ph::Pose c){return m::length(a.p-c.p)<1e-8&&m::angle(a.q,c.q)<1e-7;}
int main(){
    const double h=std::sqrt(.5);
    b::Identity id{1,2,3,4};b::Latch s{};ph::Pose result{};
    ph::Pose hand{{},{.1,-.2,.3}},string{{},{.13,-.24,.32}};
    ck(!b::follow(s,id,1,1000,.54,true,hand,string,result)&&!s.held,"Grip entry threshold does not attach early");
    ck(b::follow(s,id,1,1000,1,true,hand,string,result)&&same(result,string),"Grip captures full nock transform without a jump");
    const auto relative=ph::compose(ph::inverse(hand),string);
    hand.q={0,0,h,h};hand.p={.4,-.5,.6};
    ck(b::follow(s,id,2,1010,1,true,hand,string,result)&&m::length(ph::compose(ph::inverse(hand),result).p-relative.p)<1e-8&&m::angle(result.q,string.q)<1e-7,
        "finger-to-nock position follows the completed hand; arrow attitude stays bow-local");
    ph::Pose arrowLocal{{h,0,0,h},{.01,.08,0}};
    const auto arrow=ph::compose(result,arrowLocal);
    ck(same(ph::compose(ph::inverse(result),arrow),arrowLocal)&&m::angle(arrow.q,ph::compose(string,arrowLocal).q)<1e-7,"child arrow preserves the authored socket and bow-relative attitude");
    const auto prior=result;
    ck(b::follow(s,id,2,1010,.3,true,hand,ph::Pose{},result)&&same(result,prior),"duplicate sample cannot relearn from changed native animation; Grip hold has hysteresis");
    ck(!b::follow(s,id,3,1020,.2,true,hand,string,result)&&!s.held,"Grip release ends visual ownership");
    hand={{},{.1,-.2,.3}};
    ck(b::follow(s,id,4,1030,1,true,hand,string,result)&&same(result,string),"fresh Grip reacquires native presentation");
    ck(!b::follow(s,id,5,1040,1,false,hand,string,result)&&!s.held,"shot/cancel/menu or unowned hand cannot retain attachment");
    b::follow(s,id,6,1050,1,true,hand,string,result);
    ck(!b::follow(s,id,7,1131,1,true,hand,string,result)&&!s.held,"more than 80 ms cannot replay frozen nock");
    b::follow(s,id,8,1140,1,true,hand,string,result);
    ck(!b::follow(s,{1,9,3,4},9,1150,1,true,hand,string,result)&&!s.held,"replacement bow cannot inherit latch");
    b::follow(s,id,10,1160,1,true,hand,string,result);
    ck(!b::follow(s,{1,2,3,5},11,1170,1,true,hand,string,result)&&!s.held,"recenter epoch cannot inherit latch");
    b::follow(s,id,12,1180,1,true,hand,string,result);
    ck(!b::follow(s,{1,2,8,4},13,1190,1,true,hand,string,result)&&!s.held,"mode/request change cannot inherit latch");
    ck(!b::follow(s,id,14,1200,1,true,hand,ph::Pose{{},{4,5,6}},result),"remote string cannot be grabbed");
    ck(!b::follow(s,id,14,1200,std::numeric_limits<double>::quiet_NaN(),true,hand,string,result),"nonfinite Grip fails closed");
    b::follow(s,id,15,1210,1,true,hand,string,result);
    ck(!b::follow(s,id,14,1200,1,true,hand,string,result),"rewound sample/time releases the latch");
    ph::Pose body{{0,h,0,h},{10,20,30}},bow{{h,0,0,h},{11,19,31}},local{};
    ck(b::hand_in_bow(hand,body,1.7,bow,.8,local),"nonunit body and bow scales are supported");
    auto back=local;back.p=back.p*.8;back=ph::compose(bow,back);
    auto expected=hand;expected.p=expected.p*1.7;expected=ph::compose(body,expected);
    ck(same(back,expected),"skeleton-space nock conversion round-trips to exact completed world wrist");
    ph::Pose common{{0,0,h,h},{100,-50,4}},rotated{};
    ck(b::hand_in_bow(hand,ph::compose(common,body),1.7,ph::compose(common,bow),.8,rotated)&&same(local,rotated),
        "common body/joystick translation and rotation do not change the local nock");
    ck(!b::hand_in_bow(hand,body,1.7,bow,0,local),"invalid bow scale is not silently replaced by one");
    std::printf("bow_grip178r36c1=%d/%d native_execution=NONE\n",checks-failures,checks);return failures?1:0;
}
