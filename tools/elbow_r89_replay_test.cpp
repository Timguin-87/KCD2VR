// R89 elbow replay test. Replays logged F7 elbow inputs (tools/elbow_r89_export.py) through
//   LOGGED  - the pole the game really used (what he saw)
//   D85R5   - condition_elbow_preference WITH Codex's raised-arm outward floor + FRIK lab / continuous filter
//             (negative control: must reproduce the logged flicker, proves the harness is faithful)
//   R88     - same without the floor (current master before R89)
//   R89     - kcd2vr::elbow_r89::elbow (the shipped header, not a model of it)
//   R2      - R90: ChatGPT/Codex D85R5R2 path = elbow_destination_d86::choose (prior = committed bend) feeding the
//             same filter the log row used (FRIK lab with R2's rate cap, or continuous_elbow; bow arms continuous).
//             head is estimated (shoulder midpoint + 0.20 up); R2NOHEAD = same without the face term.
// and counts swivel jumps >15 deg per sample (elbow circle >3 cm), wrist bend (forearm vs displayed hand) and
// elbows inside the chest capsule, split by bow/free and hand above/below the shoulder.
// build: g++ -std=c++20 -O2 -I src -I src/vrik tools/elbow_r89_replay_test.cpp -o elbow_r89_replay
// run:   elbow_r89_replay nov3.csv [--expect-negative]   (exit 0 = all checks pass)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "vrik/elbow_r89.h"
#include "vrik/elbow_destination_d86.h"
#include "vrik/frik_elbow_lab.h"
namespace pc=kcd2vr::pose_core;
struct Row {
    int session,side,bow,frikLab;long long sample;std::uint64_t tick,timeNs,epoch;
    pc::Vec3 sh,goal,raw,back,pref,ta,tb,filtered;double upper,fore,radius;pc::Quat q;
};
static pc::Vec3 V(const std::vector<double>& v,int i){return {v[i],v[i+1],v[i+2]};}
static std::vector<Row> load(const char* path){
    std::ifstream in(path);std::string line;std::vector<Row> rows;std::getline(in,line);
    while(std::getline(in,line)){
        std::vector<double> v;std::stringstream ss(line);std::string c;
        while(std::getline(ss,c,','))v.push_back(std::strtod(c.c_str(),nullptr));
        if(v.size()<39)continue;
        Row r{};r.session=int(v[0]);r.side=int(v[1]);r.sample=(long long)v[2];r.tick=std::uint64_t(v[3]);r.timeNs=std::uint64_t(v[4]);
        r.epoch=std::uint64_t(v[5]);r.bow=int(v[6]);r.frikLab=int(v[7]);r.sh=V(v,8);r.goal=V(v,11);r.upper=v[14];r.fore=v[15];
        r.raw=V(v,16);r.back=V(v,19);r.pref=V(v,22);r.ta=V(v,25);r.tb=V(v,28);r.radius=v[31];r.q={v[32],v[33],v[34],v[35]};r.filtered=V(v,36);
        rows.push_back(r);
    }
    // the export writes the 4 row kinds of one sample at once; keep stable order per session/side/sample
    return rows;
}
// hand-bone local axes of this rig (F7 FINGER rows: fingers along -X right / +X left, thumb root (-.0157,.0056,-.0197) right)
static pc::Vec3 forwardLocal(int side){return side?pc::Vec3{1,0,0}:pc::Vec3{-1,0,0};}
static pc::Vec3 thumbLocal(int side){return side?pc::Vec3{0,-.2721,.9623}:pc::Vec3{0,.2721,-.9623};}
// chest right/up from the logged W1 pole: raw*|(.2,.45,1)| = +-.2 right + .45 backward - up
static void chest(pc::Vec3 raw,pc::Vec3 back,int side,pc::Vec3& right,pc::Vec3& up){
    const pc::Vec3 w=raw*std::sqrt(.04+.2025+1.0)-back*.45;const double s=side?-1:1;
    up=pc::unit(w*-1).value_or(pc::Vec3{0,0,1});
    for(int i=0;i<6;++i){right=pc::unit(pc::cross(back*-1,up)).value_or(right);up=pc::unit((w-right*(s*.2))*-1).value_or(up);}
}
enum Mode{LOGGED,D85R5,R88,R89,R89RAW,R2,R2NOHEAD,MODES};
static const char* kName[]={"LOGGED (seen)","D85R5 floor (neg. control)","R88 master","R89 hand-guided","R89 target, no filter (ctrl)","R2 D86 destination","R2 D86, no head term"};
struct Stat{int n=0,jumps=0,flips=0,intrude=0,bendOver70=0;double seconds=0;std::vector<double> bend;};
static double deg(pc::Vec3 a,pc::Vec3 b){return std::acos(std::clamp(pc::dot(a,b),-1.0,1.0))*57.29577951308232;}
int main(int argc,char** argv){
    if(argc<2){std::fprintf(stderr,"usage: %s replay.csv [--expect-negative]\n",argv[0]);return 2;}
    const bool expectNegative=argc>2&&!std::strcmp(argv[2],"--expect-negative");
    auto rows=load(argv[1]);
    std::map<std::tuple<int,long long,int>,const Row*> bySample; // R90: other hand / both shoulders for R2's D86 inputs
    for(const auto& r:rows)bySample[{r.session,r.sample,r.side}]=&r;
    std::fprintf(stdout,"rows %zu from %s\n",rows.size(),argv[1]);
    std::map<std::string,Stat> stats[MODES];
    for(int mode=0;mode<MODES;++mode){
        std::map<std::pair<int,int>,pc::ElbowContinuity> state;
        std::map<std::pair<int,int>,std::tuple<std::uint64_t,pc::Vec3,pc::Vec3>> prev;
        for(const auto& r:rows){
            const auto key=std::make_pair(r.session,r.side);auto& s=state[key];
            const auto axis=pc::unit(r.goal-r.sh);if(!axis)continue;
            std::optional<pc::Vec3> bend;
            if(mode==LOGGED)bend=r.filtered;
            else if(mode==R89||mode==R89RAW){
                kcd2vr::elbow_r89::Input in{};in.shoulder=r.sh;in.wrist=r.goal;in.upper=r.upper;in.fore=r.fore;
                in.body=r.bow?r.pref:r.raw+r.back*.6;in.forward=pc::rotate(r.q,forwardLocal(r.side));in.thumb=pc::rotate(r.q,thumbLocal(r.side));
                in.torsoA=r.ta;in.torsoB=r.tb;in.torsoRadius=r.radius;
                double strength=0;
                bend=mode==R89?kcd2vr::elbow_r89::elbow(s,in,r.epoch,std::uint64_t(r.sample),r.tick,r.timeNs)
                              :kcd2vr::elbow_r89::target(in,*axis,strength);
            }else if(mode==R2||mode==R2NOHEAD){
                namespace d86=kcd2vr::elbow_destination_d86;
                pc::Vec3 right{},up{};chest(r.raw,r.back,r.side,right,up);
                d86::Input in{};in.shoulder=r.sh;in.hand=r.goal;in.upper=r.upper;in.fore=r.fore;in.relaxed=r.raw;
                in.up=up;in.outward=right*(r.side?-1.:1.);in.backward=pc::cross(in.up,in.outward)*(r.side?1.:-1.);
                in.torsoA=r.ta;in.torsoB=r.tb;in.torsoRadius=r.radius;
                const auto other=bySample.find({r.session,r.sample,1-r.side});
                if(mode==R2&&other!=bySample.end()){in.head=(r.sh+other->second->sh)*.5+up*.20;in.haveHead=true;}
                in.forearmDirection=pc::rotate(r.q,forwardLocal(r.side));in.haveForearm=true;
                if(pc::elbow_history_current(s,r.epoch,std::uint64_t(r.sample),r.tick))in.prior=pc::transported_elbow_bend(s,*axis);
                if(r.bow&&r.side==0&&other!=bySample.end()){in.bowAway=r.goal-other->second->goal;in.drawWeight=1;}
                const auto dest=d86::choose(in);
                const pc::Vec3 anat=dest.valid?dest.pole:r.pref,fallback=dest.valid?dest.pole:r.raw;
                const double reach=pc::length(r.goal-r.sh)/(r.upper+r.fore);
                bend=(r.frikLab&&!r.bow)?kcd2vr::frik_elbow_lab::continuous_elbow_lab(s,r.goal-r.sh,anat,fallback,reach,r.epoch,std::uint64_t(r.sample),r.tick,r.timeNs)
                              :pc::continuous_elbow(s,r.goal-r.sh,anat,fallback,reach,r.epoch,std::uint64_t(r.sample),r.tick,r.timeNs);
            }else{
                pc::Vec3 anat=r.pref,fallback=r.raw;
                if(!r.bow){
                    pc::Vec3 floor{};
                    if(mode==D85R5){pc::Vec3 right{},up{};chest(r.raw,r.back,r.side,right,up);
                        floor=right*((r.side?-1.:1.)*.2*pc::smooth01((pc::dot(r.goal-r.sh,up)+.25)/.30));}
                    const auto c=pc::condition_elbow_preference(s,r.goal-r.sh,r.pref,r.raw,r.back,r.epoch,std::uint64_t(r.sample),r.tick,floor);
                    if(!c)continue;
                    anat=c->anatomical;fallback=c->fallback;
                }
                const double reach=pc::length(r.goal-r.sh)/(r.upper+r.fore);
                bend=r.frikLab?kcd2vr::frik_elbow_lab::continuous_elbow_lab(s,r.goal-r.sh,anat,r.raw,reach,r.epoch,std::uint64_t(r.sample),r.tick,r.timeNs)
                              :pc::continuous_elbow(s,r.goal-r.sh,anat,r.raw,reach,r.epoch,std::uint64_t(r.sample),r.tick,r.timeNs);
            }
            if(!bend)continue;
            const auto arm=pc::solve_arm(r.sh,r.goal,r.upper,r.fore,*bend,*bend);if(!arm)continue;
            const double circle=pc::length(pc::perpendicular(arm->elbow-r.sh,*axis));
            const std::string ctx=std::string(r.bow?"bow":"free")+(r.goal.z-r.sh.z>0?"_above":"_low");
            auto& st=stats[mode][ctx];
            auto it=prev.find(key);
            if(it!=prev.end()&&r.tick>std::get<0>(it->second)&&r.tick-std::get<0>(it->second)<=40){
                const auto carried=pc::unit(pc::perpendicular(pc::rotate(pc::arm_from_to(std::get<1>(it->second),*axis,std::get<2>(it->second)),std::get<2>(it->second)),*axis));
                const auto now=pc::unit(pc::perpendicular(*bend,*axis));
                if(carried&&now){const double step=deg(*carried,*now);++st.n;st.seconds+=double(r.tick-std::get<0>(it->second))*.001;
                    if(circle>.03&&step>15)++st.jumps;
                    if(circle>.03&&step>60)++st.flips;}
            }
            prev[key]={r.tick,*axis,*bend};
            if(const auto fa=pc::unit(r.goal-arm->elbow)){const double b=deg(*fa,pc::rotate(r.q,forwardLocal(r.side)));st.bend.push_back(b);if(b>70)++st.bendOver70;}
            const pc::Vec3 ab=r.tb-r.ta;const double t=std::clamp(pc::dot(arm->elbow-r.ta,ab)/std::max(1e-12,pc::dot(ab,ab)),0.0,1.0);
            if(pc::length(arm->elbow-(r.ta+ab*t))<r.radius)++st.intrude;
        }
    }
    const char* ctxs[]={"free_above","bow_above","free_low","bow_low"};
    for(const char* c:ctxs){
        std::printf("\n%s\n",c);
        for(int m=0;m<MODES;++m){auto& st=stats[m][c];auto b=st.bend;std::sort(b.begin(),b.end());
            const double p95=b.empty()?0:b[size_t(.95*(b.size()-1))],p50=b.empty()?0:b[b.size()/2];
            std::printf("  %-28s samples=%6d seconds=%6.1f jumps>15=%4d (%.2f/s) flips>60=%3d wristBend p50=%5.1f p95=%5.1f >70=%5d inChest=%4d\n",
                kName[m],st.n,st.seconds,st.jumps,st.seconds>0?st.jumps/st.seconds:0.,st.flips,p50,p95,st.bendOver70,st.intrude);}
    }
    int fail=0;
    const auto check=[&](bool ok,const char* what){std::printf("%s %s\n",ok?"PASS":"FAIL",what);if(!ok)++fail;};
    std::printf("\n");
    check(stats[R89]["free_above"].jumps==0&&stats[R89]["bow_above"].jumps==0,"R89: no >15 deg swivel jump with the hand above the shoulder (free and bow)");
    check(stats[R89]["free_low"].jumps==0&&stats[R89]["bow_low"].jumps==0,"R89: no >15 deg swivel jump below the shoulder");
    const auto p95=[&](int m,const char* c){auto b=stats[m][c].bend;if(b.empty())return 0.0;std::sort(b.begin(),b.end());return b[size_t(.95*(b.size()-1))];};
    check(stats[R89]["free_above"].bend.empty()||p95(R89,"free_above")<=p95(R88,"free_above")+1.0,"R89: wrist bend p95 with hands up not worse than R88");
    if(expectNegative)check(stats[R89RAW]["free_above"].jumps+stats[R89RAW]["free_low"].jumps+stats[R89RAW]["bow_above"].jumps>0,"negative control: the R89 target WITHOUT its angle-space filter still jumps (the filter is needed)");
    // R90: built against R2 headers the D85R5/R88 rows use R2's rate-capped FRIK lab (0 jumps), so only the log is the
    // flicker control here; the D85R5-header faithfulness check (105 vs 112) is in REVIEW89_elbow.md.
    if(expectNegative)check(stats[LOGGED]["free_above"].jumps>=50,"negative control: the log shows the raised-hand flicker (>=50 jumps)");
    // R90: R2 comparison (report only, decides the INI default; see REVIEW90_a.md)
    std::printf("R2 vs R89 hands up: jumps %d vs %d, wrist p95 %.1f vs %.1f\n",stats[R2]["free_above"].jumps+stats[R2]["bow_above"].jumps,
        stats[R89]["free_above"].jumps+stats[R89]["bow_above"].jumps,p95(R2,"free_above"),p95(R89,"free_above"));
    std::printf("%s (%d failed)\n",fail?"FAILED":"ALL PASS",fail);
    return fail?1:0;
}
