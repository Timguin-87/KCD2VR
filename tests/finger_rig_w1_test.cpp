// W1: finger chains by name crc on any joint numbering, captured fist keyed by crc, elbow pole + symmetric torso search.
#include "vrik/finger_rig_w1.h"
#include "vrik/elbow_pole_w1.h"
#include "vrik/arm_constraints.h"
#include <cstdio>
#include <cmath>
#include <vector>
namespace fr=kcd2vr::finger_rig_w1;namespace ep=kcd2vr::elbow_pole_w1;namespace pc=kcd2vr::pose_core;
static int total=0,failed=0;static void ck(bool v,const char* n){++total;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
struct Rig {std::vector<int> parent;std::vector<std::uint32_t> crc;int hand=-1;};
// renumber: original id -> new id via map function; filler joints in between.
static Rig MakeRig(bool left,int hand,int base,bool interleave){
    Rig r;const int count=base+200;r.parent.assign(count,0);r.crc.assign(count,0x1234u);r.hand=hand;
    for(int j=0;j<count;++j)r.parent[j]=j?0:-1;
    r.parent[hand]=0;r.crc[hand]=0xAAAA0000u;
    const auto& ref=fr::kPunch[left?1:0];int n=0,next=base;
    for(int f=0;f<5;++f){int prev=hand;for(int k=0;k<fr::kChainLength[f];++k,++n){const int id=next;next+=interleave?2:1;
        r.parent[id]=prev;r.crc[id]=ref[n].crc;prev=id;}}
    return r;
}
int main(){
    for(int side=0;side<2;++side)for(int layout=0;layout<2;++layout){
        const bool left=side!=0;const int hand=layout?(left?198:163):(left?443:408);
        Rig r=MakeRig(left,hand,layout?20:500,layout!=0);
        std::array<fr::Chain,5> c{};
        const bool ok=fr::DiscoverByCrc(int(r.parent.size()),hand,left,[&](int j){return r.crc[j];},[&](int j){return r.parent[j];},c);
        char name[96];std::snprintf(name,sizeof(name),"%s hand=%d: 5 finger chains found by name crc",left?"L":"R",hand);ck(ok,name);
        bool lengths=ok;for(int f=0;f<5&&ok;++f)lengths=lengths&&c[f].count==fr::kChainLength[f]&&r.parent[c[f].id[0]]==hand;
        ck(lengths,"chain lengths thumb..pinky 4/4/4/5/5, roots under the hand");
        int n=0;std::uint32_t crcAt[22]{};for(int f=0;f<5;++f)for(int k=0;k<c[f].count;++k)crcAt[n++]=r.crc[c[f].id[k]];
        ck(ok&&fr::PunchMatches(c,hand,left,[&](int id){return r.parent[id];},[&](int i){return crcAt[i];}),"captured fist matches the renumbered rig by crc");
    }
    {Rig r=MakeRig(false,163,20,true);const int dup=219;r.parent[dup]=163;r.crc[dup]=fr::kPunch[0][0].crc; // second thumb root
     std::array<fr::Chain,5> c{};ck(!fr::DiscoverByCrc(int(r.parent.size()),163,false,[&](int j){return r.crc[j];},[&](int j){return r.parent[j];},c),"two thumbs under one hand -> refused (no guess)");}
    {Rig r=MakeRig(false,163,20,false);for(auto& x:r.crc)x=0; // names unreadable
     std::array<fr::Chain,5> c{};const bool ok=fr::DiscoverByTopology(int(r.parent.size()),163,-1,[&](int j){return r.parent[j];},c);
     ck(ok&&c[0].id[0]>c[4].id[0],"topology fallback: 5 single chains, higher id = thumb");}
    {Rig r=MakeRig(false,408,500,false);std::array<fr::Chain,5> c{};
     fr::DiscoverByCrc(int(r.parent.size()),408,false,[&](int j){return r.crc[j];},[&](int j){return r.parent[j];},c);
     std::uint32_t crcAt[22]{};int n=0;for(int f=0;f<5;++f)for(int k=0;k<c[f].count;++k)crcAt[n++]=r.crc[c[f].id[k]];crcAt[5]^=1u;
     ck(!fr::PunchMatches(c,408,false,[&](int id){return r.parent[id];},[&](int i){return crcAt[i];}),"one wrong finger name -> captured fist not applied");}
    // elbow pole: hanging arm -> mostly BACK, forward reach -> mostly DOWN, mirrored sides.
    for(int side=0;side<2;++side){
        const auto p=ep::preference(side,ep::outward.load(),ep::backward.load());
        const auto hang=ep::perpendicular(p,{0,0,-1}),fwd=ep::perpendicular(p,{0,1,0});
        ck(std::fabs(hang.y)>std::fabs(hang.x)&&hang.y<0,side?"L hanging arm: elbow back more than out":"R hanging arm: elbow back more than out");
        ck(fwd.z< -.85,side?"L forward reach: elbow down":"R forward reach: elbow down");
        const auto old=ep::perpendicular(ep::preference(side,.35,.25),{0,0,-1});
        ck(std::fabs(old.x)>std::fabs(old.y),"old .35/.25 pole was sideways on a hanging arm (the reported wing)");
    }
    // torso search: mirrored arms around a symmetric torso give mirrored elbows; hanging arm turns <= 30 deg.
    {const pc::Vec3 pelvis{0,0,.9},spine{0,0,1.3};
     const pc::Vec3 shR{.18,0,1.36},shL{-.18,0,1.36},handR{.05,.12,1.08},handL{-.05,.12,1.08};
     const auto pr=pc::torso_elbow_preference(shR,handR,.2663,.2615,*pc::unit(pc::Vec3{.35,-.25,-1}),pelvis,spine,.12);
     const auto pl=pc::torso_elbow_preference(shL,handL,.2663,.2615,*pc::unit(pc::Vec3{-.35,-.25,-1}),pelvis,spine,.12);
     ck(std::fabs(pr.x+pl.x)<1e-6&&std::fabs(pr.y-pl.y)<1e-6&&std::fabs(pr.z-pl.z)<1e-6,"mirrored arms -> mirrored elbow branch");
     const pc::Vec3 hang{.08,.05,.98};const auto raw=*pc::unit(pc::Vec3{.35,-.25,-1}); // hand in front of the hip: old search turned 60 deg
     const auto ph=pc::torso_elbow_preference(shR,hang,.2663,.2615,raw,pelvis,spine,.16);
     auto arm=pc::solve_arm(shR,hang,.2663,.2615,raw,raw);
     const double turn=arm?std::acos(std::clamp(pc::dot(*pc::unit(ph),*pc::unit(arm->bend)),-1.,1.))*57.2958:999;
     ck(turn<=30.5,"hanging arm torso clearance turn <= 30 deg");}
    std::printf("finger_rig_w1 %d/%d\n",total-failed,total);return failed?1:0;
}
