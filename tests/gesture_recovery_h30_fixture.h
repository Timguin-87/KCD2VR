#pragma once
// Actual physical kernel: ordinary deliberate forward strokes out of held guard.
#include "vrik/physical_weapon.h"
#include "vrik/user_modes175.h"
#include <cstdio>
#include <vector>
#include <cstring>
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
static unsigned checksH30=0,failuresH30=0;
static void ckH30(bool yes,const char* text){++checksH30;failuresH30+=!yes;std::printf("%s %s\n",yes?"PASS":"FAIL",text);}
struct RigH30 {
    ph::Input in{};ph::State state{};std::array<ph::Profile,2> profiles{};ph::Output out{};std::vector<ph::Command> commands;
    pm::Quat model{},tracking{};bool known=true,blocked=false,actor=false;unsigned targetId=13;
    ph::Vec3 contactNormal{0,-1,0};
    RigH30(bool direct,ph::Vec3 axis={1,0,0},double yaw=0,double trackingYaw=0){
        model={0,0,std::sin(yaw*.5),std::cos(yaw*.5)};tracking={0,0,std::sin(trackingYaw*.5),std::cos(trackingYaw*.5)};
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.epoch=in.bodyEpoch=1;in.tick=1000;in.timeNs=1000000000;in.sample=1;
        in.requirePrimaryGrip114=in.heldMotionAttacks=in.controllerAnchor119=true;in.grip[0]=1;in.reach[0]=in.reach[1]=1.9;
        in.naturalCombat117=in.coherentMelee119=in.positionGuard176=in.attacks=true;
        const auto selectedModeH31=direct?kcd2vr::user175::Mode::CombatContactLab:
            kcd2vr::user175::default59(kcd2vr::user175::Mode::Calibrated,true);
        const auto mode36=kcd2vr::user175::configuration(selectedModeH31);
        const bool synchronizedH31=selectedModeH31==kcd2vr::user175::Mode::CombatSynchronized;
        in.physicalCombat177=mode36.physical177;ph::select_forward_motion36(in,direct,synchronizedH31);
        // Match actual public HYBRID: H30 producer is independent of the old
        // R16 direct-contact route, which remains OFF in this fixture.
        in.intent59c16=false;
        in.dynamics=false;in.contact=true;in.strikeMinScaleGate59c14=false;
        in.windupHold57=false;in.guardEnterMaxSpeed178r29=2.5;in.guardEnterDwellMs178r29=0;in.guardExitSpeed178r45=3.5;
        in.shoulders[0]=pm::rotate(model,{.15,0,1.3});in.shoulders[1]=pm::rotate(model,{-.15,0,1.3});
        const double gripX=axis.x<-.9?.20:-.20;
        in.hands[0]={model,pm::rotate(model,{gripX,.35,1.45})};in.intentHands[0]={tracking,pm::rotate(tracking,{gripX,.35,1.45})};
        auto& p=profiles[0];p.key=77;p.generation=1;p.side=0;p.classId=4;p.primary=p.geometry=true;p.axis=axis;p.tip=axis*.6;
        // Explicit blade broad face in this synthetic profile; normal is perpendicular to blade.
        p.guardFaceReadyH24=true;p.guardFaceNormalH24={-axis.y,axis.x==0&&axis.y==0?1.:axis.x,0};
        for(unsigned i=0;i<30;++i)step();commands.clear();
    }
    void step(ph::Vec3 bodyDelta={},bool moveModel=true,bool moveTracking=true){
        ++in.sample;in.tick+=10;in.timeNs+=10000000;
        if(moveModel)in.hands[0].p=in.hands[0].p+pm::rotate(model,bodyDelta);
        if(moveTracking)in.intentHands[0].p=in.intentHands[0].p+pm::rotate(tracking,bodyDelta);
        out=ph::step(state,in,profiles,[&](const ph::Profile&,ph::Pose from,ph::Pose,bool){ph::Contact103 c{};c.known=known;
            if(blocked){c.fraction=actor?.5:0;c.plane=true;c.normal=pm::rotate(model,contactNormal);c.point=from.p;
                if(actor){auto& n=c.native172;n.ownerRead=true;n.owner=1;n.collider=2;n.entityId=targetId;n.part=1;n.mask=kcd2vr::npc_contact172::BodiesAndWorld;n.body=kcd2vr::npc_contact172::Body::Living;}}
            return c;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);
    }
    unsigned count(ph::CommandKind k)const{unsigned n=0;for(auto c:commands)n+=c.kind==k;return n;}
    unsigned hits()const{return count(ph::CommandKind::Slash)+count(ph::CommandKind::Thrust)+count(ph::CommandKind::Pommel);}
    std::uint64_t first(ph::CommandKind k)const{for(auto c:commands)if(c.kind==k)return c.tick;return 0;}
};

// Production-connected regression: temporary collision/reach recovery must not
// destroy a fresh tracked normal attack. Callback geometry is deterministic;
// no game code/native acceptance/damage is executed or claimed.
static const ph::Command* firstH30(const RigH30& r,ph::CommandKind kind){for(const auto& c:r.commands)if(c.kind==kind)return &c;return nullptr;}
static void moveH30(RigH30& r,unsigned count){for(unsigned i=0;i<count;++i)r.step({0,.025,0});}
static void gapH30(RigH30& r,unsigned count=1){r.known=false;r.in.contactSuspended59c1=true;moveH30(r,count);}
static void restoreH30(RigH30& r,unsigned count){r.known=true;r.in.contactSuspended59c1=false;moveH30(r,count);}
static bool sameImpactH30(const ph::ImpactRecord178r30& a,const ph::ImpactRecord178r30& b){return
 a.valid==b.valid&&a.tick==b.tick&&a.sample==b.sample&&a.gesture59c3==b.gesture59c3&&a.key==b.key&&a.generation==b.generation&&a.epoch==b.epoch&&a.request==b.request&&
 a.entityId==b.entityId&&a.collider==b.collider&&a.part==b.part&&a.kind==b.kind&&a.originZone47==b.originZone47&&pm::length(a.point-b.point)<1e-12&&pm::length(a.normal-b.normal)<1e-12;}
