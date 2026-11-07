// D67C lane 1: elbow destination continuity (REVIEW_LANE1 section 4, ChatGPT VRIK_REVIEW V1).
// Replays the recorded counterexample (left hand, session 2, samples 302666 -> 302670, 11:54:47.419,
// both hands empty) through the UNCHANGED D66 search (reproduces the 60.9 deg destination jump) and
// through the D67C hysteretic search (the jump is gone), with the recorded prior history per row, the
// chained history across the recorded sequence, and the 10,000-step linear input sweep ChatGPT used.
// Optional argv[1]: ChatGPT's full current_elbow_input.txt (7,309 rows) for the full reproduction check.
#include "vrik/elbow_hysteresis_d69.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
using namespace kcd2vr::pose_core;
struct Row {
    double session,side,sample,epoch,tick,ns,ready,priorEpoch,priorSample,priorTick,priorNs;
    double priorAxis[3],priorBend[3],shoulder[3],goal[3],raw[3],torsoA[3],torsoB[3],backward[3],recordedAxis[3];
    double upper,fore,radius,reach;double recordedTorso[3],recordedConditioned[3],recordedFiltered[3];
};
static const Row kRows[]{
#include "data/elbow_branch_d67c_recorded.inl"
};
static int checks=0,failed=0;
static void ck(bool b,const char* name){++checks;failed+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",name);}
static Vec3 V(const double* v){return {v[0],v[1],v[2]};}
static double deg(Vec3 a,Vec3 b){return std::acos(std::clamp(dot(*unit(a),*unit(b)),-1.,1.))*180/armPi;}
// Step of a pole between two frames, measured after transporting the previous pole along the moving
// arm axis (the same transport continuous_elbow uses): a 2 cm wrist move must not count as elbow motion.
static Vec3 Axis(const Row& r){return *unit(V(r.goal)-V(r.shoulder));}
static double stepDeg(Vec3 prevAxis,Vec3 prevPole,Vec3 axis,Vec3 pole){
    const auto moved=unit(perpendicular(rotate(arm_from_to(prevAxis,axis,prevPole),prevPole),axis));return moved?deg(*moved,pole):deg(prevPole,pole);}
static ElbowContinuity Prior(const Row& r){ElbowContinuity s{};s.ready=r.ready>0;s.epoch=std::uint64_t(r.priorEpoch);s.sample=std::uint64_t(r.priorSample);s.tick=std::uint64_t(r.priorTick);s.timeNs=std::uint64_t(r.priorNs);s.axis=V(r.priorAxis);s.bend=V(r.priorBend);return s;}
static Vec3 d66(const Row& r){return torso_elbow_preference(V(r.shoulder),V(r.goal),r.upper,r.fore,V(r.raw),V(r.torsoA),V(r.torsoB),r.radius,std::nullopt,false,false);}
static Vec3 d67c(ElbowContinuity& s,const Row& r){return torso_elbow_destination_d67c(s,V(r.shoulder),V(r.goal),r.upper,r.fore,V(r.raw),V(r.torsoA),V(r.torsoB),r.radius,std::uint64_t(r.epoch),std::uint64_t(r.sample),std::uint64_t(r.tick),std::uint64_t(r.ns));}
static Row mix(const Row& a,const Row& b,double t){Row x=a;
    for(int i=0;i<3;++i){x.shoulder[i]+= (b.shoulder[i]-a.shoulder[i])*t;x.goal[i]+=(b.goal[i]-a.goal[i])*t;x.raw[i]+=(b.raw[i]-a.raw[i])*t;x.torsoA[i]+=(b.torsoA[i]-a.torsoA[i])*t;x.torsoB[i]+=(b.torsoB[i]-a.torsoB[i])*t;}
    x.radius+=(b.radius-a.radius)*t;return x;}
int main(int argc,char** argv){
    std::vector<Row> rows(std::begin(kRows),std::end(kRows));
    // 1. Reproduction: the D66 search in this tree still returns exactly the recorded torso vectors.
    double torsoError=0;for(const auto& r:rows)torsoError=std::max(torsoError,length(d66(r)-V(r.recordedTorso)));
    std::printf("embedded_rows=%zu d66_max_torso_vector_error=%.3g\n",rows.size(),torsoError);
    ck(torsoError<1e-10,"historical D66 search reproduces every recorded torso target");
    if(argc>1){std::ifstream in(argv[1]);std::string line;unsigned n=0;double err=0;
        while(std::getline(in,line)){std::istringstream ss(line);Row r{};double* p=reinterpret_cast<double*>(&r);
            for(unsigned i=0;i<sizeof(Row)/sizeof(double);++i)ss>>p[i];err=std::max(err,length(d66(r)-V(r.recordedTorso)));++n;}
        std::printf("full_rows=%u d66_max_torso_vector_error=%.3g\n",n,err);ck(n>7000&&err<1e-10,"full 7,309-row ChatGPT replay reproduces with the OFF path");}
    // D69 OFF calls its actual continuous-baseline helper, not the D66 donor path.
    for(bool continuous:{false,true})for(bool lab:{false,true})for(const auto& r:rows){
        auto h=Prior(r);const auto before=h;
        const auto old=kcd2vr::elbow_motion_d67::preference(V(r.shoulder),V(r.goal),r.upper,r.fore,V(r.raw),V(r.torsoA),V(r.torsoB),r.radius,h,std::uint64_t(r.epoch),std::uint64_t(r.sample),std::uint64_t(r.tick),continuous,lab);
        const auto actual=kcd2vr::elbow_motion_d69::preference(false,V(r.shoulder),V(r.goal),r.upper,r.fore,V(r.raw),V(r.torsoA),V(r.torsoB),r.radius,h,std::uint64_t(r.epoch),std::uint64_t(r.sample),std::uint64_t(r.tick),std::uint64_t(r.ns),continuous,lab);
        ck(length(old-actual)==0&&h.sample==before.sample&&h.timeNs==before.timeNs,"D69 OFF exact current preference and no temporal advance");
        for(bool on:{false,true}){auto k=Prior(r);const auto pole=kcd2vr::elbow_motion_d69::preference(on,V(r.shoulder),V(r.goal),r.upper,r.fore,V(r.raw),V(r.torsoA),V(r.torsoB),r.radius,k,std::uint64_t(r.epoch),std::uint64_t(r.sample),std::uint64_t(r.tick),std::uint64_t(r.ns),continuous,lab);
            const auto solved=solve_arm(V(r.shoulder),V(r.goal),r.upper,r.fore,pole,pole);
            const auto reference=solve_arm(V(r.shoulder),V(r.goal),r.upper,r.fore,old,old);
            ck(solved&&reference&&length(solved->hand-reference->hand)<1e-12&&std::abs(length(solved->elbow-V(r.shoulder))-r.upper)<1e-9&&std::abs(length(solved->hand-solved->elbow)-r.fore)<1e-9,"both trial states preserve exact reachable wrist and hard bone lengths");
        }
    }
    // 2. The recorded counterexample: consecutive left-hand rows 302666 -> 302670 (session 2).
    const Row *a=nullptr,*b=nullptr;
    for(const auto& r:rows){if(r.session==2&&r.side==1&&r.sample==302666)a=&r;if(r.session==2&&r.side==1&&r.sample==302670)b=&r;}
    ck(a&&b,"counterexample rows 302666 and 302670 present");
    if(a&&b){
        const double jumpD66=stepDeg(Axis(*a),d66(*a),Axis(*b),d66(*b));
        ElbowContinuity s=Prior(*a);s.destinationReadyD67C=false;
        const Vec3 first=d67c(s,*a),second=d67c(s,*b);
        const double jumpD67C=stepDeg(Axis(*a),first,Axis(*b),second);
        std::printf("counterexample wristMotionMm=%.4f D66 targetJumpDeg=%.3f D67C destinationStepDeg=%.3f\n",length(V(b->goal)-V(a->goal))*1000,jumpD66,jumpD67C);
        ck(jumpD66>50,"D66 reproduces the ~60 deg torso target jump between the two recorded samples");
        ck(jumpD67C<=180.0*.013+1e-6,"D67C destination turns at most 180 deg/s between the two recorded samples (12 ms apart)");
        // 3. ChatGPT's 10,000-step linear sweep of the same inputs: no step above the rate limit, no branch flip.
        double maxD66=0,maxD67C=0;Vec3 prevD66=d66(*a);ElbowContinuity sw=Prior(*a);Vec3 prevD67C=d67c(sw,*a);Vec3 prevAxis=Axis(*a);
        for(unsigned k=1;k<=10000;++k){Row x=mix(*a,*b,k/10000.);x.sample=a->sample+k;x.tick=a->tick+k/834;x.ns=a->ns+k*1200;
            const Vec3 now=d66(x);maxD66=std::max(maxD66,stepDeg(prevAxis,prevD66,Axis(x),now));prevD66=now;
            const Vec3 dest=d67c(sw,x);maxD67C=std::max(maxD67C,stepDeg(prevAxis,prevD67C,Axis(x),dest));prevD67C=dest;prevAxis=Axis(x);}
        std::printf("linear_sweep D66 maxTargetStepDeg=%.4f D67C maxDestinationStepDeg=%.4f\n",maxD66,maxD67C);
        ck(maxD66>50,"D66: one 0.002 mm sweep step still flips the branch by ~60 deg");
        ck(maxD67C<1.0,"D67C: no sweep step above 1 deg (hysteresis holds the branch; dwell never reached in 12 ms)");
    }
    // 4. Chained replay of the whole recorded left-hand sequence of session 2 and right-hand of session 3:
    //    per-frame destination step vs the D66 target step, hanging-arm frames included.
    for(int which=0;which<2;++which){
        const double session=which?3:2,side=which?0:1;
        std::vector<const Row*> seq;for(const auto& r:rows)if(r.session==session&&r.side==side)seq.push_back(&r);
        double maxStepD66=0,maxStepD67C=0,sumD66=0,sumD67C=0;unsigned steps=0,over15D66=0,over15D67C=0;
        ElbowContinuity s{};const Row* prev=nullptr;Vec3 prevD66{},prevD67C{};
        for(const Row* r:seq){
            const Vec3 t66=d66(*r);
            if(!prev||r->priorSample!=prev->sample){s=Prior(*r);s.destinationReadyD67C=false;}
            const Vec3 t67=d67c(s,*r);
            if(prev&&r->priorSample==prev->sample){
                const double a66=stepDeg(Axis(*prev),prevD66,Axis(*r),t66),a67=stepDeg(Axis(*prev),prevD67C,Axis(*r),t67);
                maxStepD66=std::max(maxStepD66,a66);maxStepD67C=std::max(maxStepD67C,a67);sumD66+=a66;sumD67C+=a67;++steps;
                over15D66+=a66>15;over15D67C+=a67>15;
            }
            prev=r;prevD66=t66;prevD67C=t67;
        }
        std::printf("sequence session=%g side=%g rows=%zu linkedSteps=%u D66 maxStepDeg=%.3f meanStepDeg=%.3f over15=%u | D67C maxStepDeg=%.3f meanStepDeg=%.3f over15=%u\n",
            session,side,seq.size(),steps,maxStepD66,steps?sumD66/steps:0,over15D66,maxStepD67C,steps?sumD67C/steps:0,over15D67C);
        ck(steps>0,"recorded sequence has linked consecutive rows");
        ck(maxStepD67C<=maxStepD66+1e-9,"D67C never steps the destination more than D66 did");
        ck(over15D67C==0,"D67C: no destination step above 15 deg in the recorded sequence");
    }
    // 5. Duplicate consumer (same sample) does not advance or change the destination.
    if(a){ElbowContinuity s=Prior(*a);const Vec3 x=d67c(s,*a);const auto frozen=s;const Vec3 y=d67c(s,*a);
        ck(length(x-y)<1e-12&&s.destSampleD67C==frozen.destSampleD67C&&s.destTickD67C==frozen.destTickD67C,"duplicate sample returns the same destination without advancing history");}
    // 6. Synthetic: hysteresis switches to a clearly better branch only after the 100 ms dwell.
    {   // hand in front of the chest, symmetric geometry: both +/- routes cost the same, then the torso moves.
        const Vec3 shoulder{.18,0,1.4},torsoA{0,0,1.0},torsoB{0,0,1.5};const double upper=.3,fore=.28,radius=.14;
        ElbowContinuity s{};std::uint64_t tick=1000,ns=1000000000ull;unsigned sample=1;
        Vec3 hand{.05,.30,1.25};
        const Vec3 start=torso_elbow_destination_d67c(s,shoulder,hand,upper,fore,{0,-.2,-1},torsoA,torsoB,radius,1,sample,tick,ns);
        // shove the torso so the kept branch intrudes and the other one is clearly better
        const Vec3 torsoA2{.10,.02,1.0},torsoB2{.10,.02,1.5};
        Vec3 d=start;unsigned switchedAt=0;
        for(unsigned n=1;n<=30;++n){tick+=11;ns+=11000000ull;++sample;
            const Vec3 next=torso_elbow_destination_d67c(s,shoulder,hand,upper,fore,{0,-.2,-1},torsoA2,torsoB2,radius,1,sample,tick,ns);
            if(!switchedAt&&deg(next,start)>20)switchedAt=n;d=next;}
        std::printf("synthetic hysteresis: switchedAtFrame=%u (0 = never) finalDeg=%.2f\n",switchedAt,deg(d,start));
        ck(switchedAt==0||switchedAt>=9,"a clearly better branch is adopted only after >= 100 ms of dwell (9 frames at 11 ms)");
    }
    std::printf("%d checks, %d failures\n",checks,failed);return failed?1:0;
}
