#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::door19c4 {
struct Point {float x=0,y=0,z=0;};
inline Point sub(Point a,Point b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline float dot(Point a,Point b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline bool finite(Point p) noexcept {return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
struct Request {
    std::uint64_t serial=0,tick=0,epoch=0;std::uintptr_t player=0,entity=0,action=0;
    std::uint32_t id=0;int side=-1;Point start{};std::uint64_t sourceGeneration=0;
    std::uint64_t inputToken46=0,trackingEpoch46=0,inputSettingsEpoch46=0; // explicit semi-door squeeze, otherwise legacy local admission
    std::uint64_t inputExperimentEpoch65=0,inputPress65=0;
    Point surface66{}; // exact same-door physical witness, never the generic ray centre
    int nativeSide66=-1;
    int mode69=2; // F5=2 resident spatial curve; F5=3 reviewed legacy gesture pace
};
struct Binding {
    std::uintptr_t entity=0,character=0,anim=0,entries=0;std::uint32_t id=0;int animation=-1;
    float initialTime=0,savedRate=1;
};
// The runtime and fixture execute this same owner. IO substitutes only exact
// native leaves and completed tracking samples. No synthetic door state change.
struct Owner {
    Request request{};Binding binding{};Point axis{};bool active=false,haveAxis=false,pending=false,restorePending=false;
    std::uint64_t seen=0;float lastTime=0;
    template<class IO> void release(IO& io,const char* reason) noexcept {
        if(active){
            // Never restore into a replacement character, or overwrite
            // a different native speed writer. Native AnimDoor owns completion.
            // Rate belongs to the layer: release it even if the FIFO clip was
            // replaced, but only on the same exact character and owned rate0.
            const int result=io.restore(binding);
            if(result<0){if(!restorePending)io.receipt(request,"RESTORE_PENDING",lastTime,false);
                restorePending=true;return;}
            io.receipt(request,reason,lastTime,result==1);
        }
        active=false;haveAxis=false;pending=false;restorePending=false;binding={};
    }
    template<class IO> void tick(IO& io,const Request& incoming,std::uint64_t now) noexcept {
        if(restorePending){release(io,"RESTORE_RETRY");if(restorePending)return;}
        if(incoming.serial&&incoming.serial!=seen){
            release(io,"REPLACED");if(restorePending)return;
            seen=incoming.serial;request=incoming;pending=true;
        }
        if(!active&&pending){
            Point hand{};
            if(!request.tick||now<request.tick||now-request.tick>250||!io.allowed(request)||
               !io.hand(request,hand)||!finite(hand)||!finite(request.start)){pending=false;io.receipt(request,"START_NOT_CONFIRMED_OR_OWNER_LOST",0,false);return;}
            Binding candidate{};
            if(!io.acquire(request,candidate)||!std::isfinite(candidate.initialTime)||
               candidate.initialTime<0.f||candidate.initialTime>=.9995f||candidate.savedRate!=1.f)return;
            binding=candidate;lastTime=binding.initialTime;
            // Claim cleanup before a setter: false can mean a write succeeded
            // but its readback faulted. Never discard a possibly applied pause.
            active=true;pending=false;haveAxis=false;
            if(!io.speed(binding,0.f)){release(io,"NATIVE_SPEED_WRITE_FAILED");return;}
            io.receipt(request,"ACQUIRED",lastTime,true);
        }
        if(!active)return;
        Point hand{};
        if(now<request.tick||now-request.tick>15000||!io.allowed(request)||!io.hand(request,hand)||!finite(hand)){
            release(io,"RELEASE_OR_OWNER_LOST");return;
        }
        float currentTime=0,rate=0;
        if(!io.current(binding,currentTime,rate)||rate!=0.f||std::fabs(currentTime-lastTime)>.04f){release(io,"NATIVE_OWNER_CHANGED");return;}
        const auto delta=sub(hand,request.start);const float length2=dot(delta,delta);
        // A teleport/recenter or leaving this door is not a giant opening gesture.
        if(!std::isfinite(length2)||length2>1.44f){release(io,"HAND_LEFT_DOOR");return;}
        if(!haveAxis&&length2>=.0009f){
            const float scale=1.f/std::sqrt(length2);axis={delta.x*scale,delta.y*scale,delta.z*scale};haveAxis=true;
        }
        if(!haveAxis)return;
        // Experimental travel adapter, NOT a solved hinge constraint: the first
        // deliberate 3 cm establishes the pull/push axis; 55 cm traverses the
        // accepted authored clip. Reverse hand travel reverses that same clip.
        const float wanted=std::clamp(binding.initialTime+dot(delta,axis)/.55f,0.f,.9995f);
        if(std::fabs(wanted-lastTime)<.0005f)return;
        if(!io.time(binding,wanted)){release(io,"NATIVE_TIME_WRITE_FAILED");return;}
        lastTime=wanted;
    }
};
}
