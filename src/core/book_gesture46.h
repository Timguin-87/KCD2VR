#pragma once
#include "book_pages45.h"
#include "book_surface46.h"
#include "../vrik/workstation_sample46.h"
#include "../vrik/alchemy_semi46.h"
#include <cmath>
namespace kcd2vr::book_gesture46 {
namespace bp=kcd2vr::book_pages45;
// C46 keeps the existing shoulder arbiter but replaces its unstamped moving-head
// flick source. Both hands use one semantic direction in a stable task frame.
struct Flick {
    struct Sample {std::uint64_t tick=0;float x=0,y=0,z=0,world[3]{};};
    struct HandState {Sample ring[32]{};unsigned count=0,head=0;bool inside=false,consumed=false;};
    HandState hand[2]{};std::uint64_t sequence=0,poseEpoch=0,sourceEpoch=0,timeNs=0,surfaceSequence=0;
    bool haveFrame=false,surface=false;float origin[3]{},right[3]{1,0,0},headOrigin[3]{};
    void reset() noexcept {*this=Flick{};}
    static float dot(const float* a,const float* b) noexcept {return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];}
    static bool vectorFinite(const float* p) noexcept {return std::isfinite(p[0])&&std::isfinite(p[1])&&std::isfinite(p[2]);}
    int local(unsigned side,std::uint64_t now,const float* p,float x,float y,float z,bool in) noexcept {
        auto& s=hand[side];if(!in){s=HandState{};return 0;}s.inside=true;
        Sample a{};a.tick=now;a.x=x;a.y=y;a.z=z;for(unsigned n=0;n<3;++n)a.world[n]=p[n];
        if(s.count){const auto& last=s.ring[(s.head+31)%32];float jump=0;
            for(unsigned n=0;n<3;++n){const float d=p[n]-last.world[n];jump+=d*d;}
            if(now<last.tick||jump>.0625f){s=HandState{};return 0;}}
        s.ring[s.head]=a;s.head=(s.head+1)%32;if(s.count<32)++s.count;
        if(s.consumed)return 0;
        for(unsigned i=1;i<s.count;++i){const auto& o=s.ring[(s.head+31-i)%32];if(now<o.tick||now-o.tick>400)break;
            const float dx=x-o.x,dy=std::fabs(y-o.y),dz=std::fabs(z-o.z);
            float absolute[3]{};for(unsigned n=0;n<3;++n)absolute[n]=p[n]-o.world[n];
            // The hand itself must move; an authored book/camera motion cannot
            // drag an otherwise stationary hand across the page detector.
            const float handTravel=dot(absolute,right);
            if(std::fabs(dx)>=.12f&&std::fabs(handTravel)>=.10f&&dx*handTravel>0&&dy<std::fabs(dx)*.8f&&dz<std::fabs(dx)*.8f){
                s.consumed=true;return dx>0?-1:1;
            }
        }
        return 0;
    }
    int step(const workstation_sample46::Frame& f,std::uint64_t now,const book_surface46::Surface* native) noexcept {
        if(!workstation_sample46::fresh(f,now)||!f.allowed||!f.headValid||(!f.hand[0].valid&&!f.hand[1].valid)){
            reset();return 0;}
        const bool sameEpoch=poseEpoch==f.poseEpoch&&sourceEpoch==f.sourceEpoch;
        if(haveFrame&&(!sameEpoch||f.sequence<sequence||(f.sequence!=sequence&&f.timeNs<=timeNs)||f.timeNs-timeNs>80000000ull)){reset();}
        if(sequence==f.sequence)return 0;
        sequence=f.sequence;timeNs=f.timeNs;poseEpoch=f.poseEpoch;sourceEpoch=f.sourceEpoch;
        bool ns=native&&native->valid&&native->entity&&native->tick&&now>=native->tick&&now-native->tick<=80&&
            native->sequence&&native->sequence<=f.sequence&&native->poseEpoch==f.poseEpoch&&native->sourceEpoch==f.sourceEpoch&&
            vectorFinite(native->origin)&&vectorFinite(native->right)&&vectorFinite(native->up)&&vectorFinite(native->normal)&&
            native->halfWidth>=.07f&&native->halfWidth<=.55f&&native->halfHeight>=.05f&&native->halfHeight<=.6f;
        if(ns){const float r=dot(native->right,native->right),u=dot(native->up,native->up),n=dot(native->normal,native->normal);
            ns=r>.98f&&r<1.02f&&u>.98f&&u<1.02f&&n>.98f&&n<1.02f&&std::fabs(dot(native->right,native->up))<.02f&&
                std::fabs(dot(native->right,native->normal))<.02f&&std::fabs(dot(native->up,native->normal))<.02f;}
        if(haveFrame&&surface!=ns){reset();sequence=f.sequence;timeNs=f.timeNs;poseEpoch=f.poseEpoch;sourceEpoch=f.sourceEpoch;}
        if(!haveFrame){haveFrame=true;surface=ns;for(unsigned n=0;n<3;++n)headOrigin[n]=f.head[n];
            if(ns){for(unsigned n=0;n<3;++n){origin[n]=native->origin[n];right[n]=native->right[n];}}
            else{
                const float x=f.headQ[0],y=f.headQ[1],z=f.headQ[2],w=f.headQ[3];
                const float fx=-2.f*(w*y+x*z),fz=-1.f+2.f*(x*x+y*y);const float len=std::sqrt(fx*fx+fz*fz);
                if(!std::isfinite(len)||len<.2f){reset();return 0;}
                right[0]=-fz/len;right[1]=0;right[2]=fx/len;
                for(unsigned n=0;n<3;++n)origin[n]=f.head[n];
            }
        }
        if(!ns){float movement=0;for(unsigned n=0;n<3;++n){const float d=f.head[n]-headOrigin[n];movement+=d*d;}
            // A changed reading posture requires a fresh approach. Head turning
            // alone never rotates the already armed gesture coordinate frame.
            if(movement>.0016f){reset();return 0;}}
        else if(dot(right,native->right)<.95f){reset();return 0;}
        if(ns&&surfaceSequence==native->sequence)return 0;
        if(ns)surfaceSequence=native->sequence;
        int result=0;
        for(unsigned side=0;side<2;++side){
            const bool valid=ns?native->handValid[side]:f.hand[side].valid;
            const float* p=ns?native->hand[side]:f.hand[side].position;
            if(!valid||!vectorFinite(p)){hand[side]=HandState{};continue;}
            float d[3]{};for(unsigned n=0;n<3;++n)d[n]=p[n]-(ns?native->origin[n]:origin[n]);
            const float x=dot(d,ns?native->right:right);
            const float y=ns?dot(d,native->up):d[1];
            const float z=ns?dot(d,native->normal):d[0]*right[2]-d[2]*right[0];
            bool in=false;
            if(ns){const float h=hand[side].inside?.025f:0.f;
                in=std::fabs(x)<=native->halfWidth+.025f+h&&std::fabs(y)<=native->halfHeight+.025f+h&&std::fabs(z)<=.055f+h;
            }else in=bp::FlickRecognizer::zone(x,y,z,hand[side].inside);
            const int dir=local(side,ns?native->tick:f.tick,p,x,y,z,in);if(!result&&dir)result=dir;
        }
        return result;
    }
};
struct Arbiter {
    bp::Arbiter native{};Flick flick{};int book=bp::None,mode=-1,lastSource=bp::NoSource;
    std::uint64_t owner=0;std::uintptr_t physicalInstance=0,physicalAction=0;std::uint64_t physicalLifetime=0;
    void reset() noexcept {*this=Arbiter{};}
    const char* zoneName() const noexcept {return flick.surface?"NATIVE_BOOK_PLANE":"COMFORT_READING_ZONE";}
    bp::Result step(int currentBook,std::uint64_t currentOwner,std::uint64_t now,int currentMode,bool left,bool right,
                    const workstation_sample46::Frame& f,float stickX,
                    std::uintptr_t instance=0,std::uintptr_t action=0,std::uint64_t lifetime=0) noexcept {
        if(currentBook==bp::None||!currentOwner){
            alchemy_semi46::BookEnable e{};alchemy_semi46::WorkWitness w{};
            const bool nativePageBusy=book==bp::Alchemy&&physicalInstance&&physicalAction&&physicalLifetime&&
                currentMode==mode&&alchemy_semi46::hooksReady.load(std::memory_order_acquire)&&
                alchemy_semi46::bookEnable.read(e)&&!e.enabled&&e.instance==physicalInstance&&e.action==physicalAction&&e.lifetime==physicalLifetime&&
                alchemy_semi46::work.read(w)&&w.kind==alchemy_semi46::Book&&w.instance==physicalInstance&&w.parent==physicalAction&&
                w.lifetime==physicalLifetime&&alchemy_semi46::fresh(w.tick,now);
            if(nativePageBusy){
                // Every authored page turn disables/re-enables the same native
                // read rows. Keep the consumed swipe until the hand leaves the
                // zone; a physical return stroke must not undo that page.
                native=bp::Arbiter{};lastSource=bp::NoSource;
                if(currentMode==bp::ThumbrestFlick)(void)flick.step(f,now,nullptr);
                return {};
            }
            reset();return {};
        }
        const bool samePhysical=currentBook==bp::Alchemy&&book==bp::Alchemy&&instance&&action&&lifetime&&
            instance==physicalInstance&&action==physicalAction&&lifetime==physicalLifetime;
        if(book!=currentBook||owner!=currentOwner||mode!=currentMode){
            if(samePhysical&&mode==currentMode){native=bp::Arbiter{};lastSource=bp::NoSource;}
            else reset();
            book=currentBook;owner=currentOwner;mode=currentMode;
        }
        physicalInstance=instance;physicalAction=action;physicalLifetime=lifetime;
        int gesture=0;
        if(currentMode==bp::ThumbrestFlick){
            book_surface46::Surface plane{};const bool exact=instance&&action&&lifetime&&book_surface46::current.read(plane)&&
                plane.instance==instance&&plane.action==action&&plane.lifetime==lifetime;
            gesture=flick.step(f,now,exact?&plane:nullptr);
            if(!flick.haveFrame&&lastSource==bp::Flick)native.until=0; // promptly release a gesture pulse on source loss
        }else flick.reset();
        const bool manualLeft=left,manualRight=right;
        if(!left&&!right){left=gesture<0;right=gesture>0;}
        bp::Head noHead{};bp::Hand noHand{};
        auto result=native.step(currentBook,currentOwner,now,currentMode==bp::Native?bp::Native:bp::ThumbrestOnly,
            left,right,noHead,noHand,noHand,stickX);
        if(result.requested&&gesture&&!manualLeft&&!manualRight)result.source=bp::Flick;
        if(result.accepted)lastSource=result.source;
        return result;
    }
};
}
