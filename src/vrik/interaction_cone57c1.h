#pragma once
#include <algorithm>
#include <cmath>
#include <cstring>
#include "interaction_ray112.h"

namespace kcd2vr::interaction57c1 {
constexpr float HandReach=2.f, HandRadius=.20f, HandHalfAngle=18.f;
enum class Kind { Unknown, Object, Actor, NativeContext };
// Exact script entity classes from retail Scripts.pak. Never inspect entity names.
inline Kind Classify(const char* name) noexcept {
    if(!name||!name[0])return Kind::Unknown;
    // This native contextual activity owns its approach/distance/quest checks.
    if(std::strcmp(name,"ForgeBuilderTrigger")==0)return Kind::NativeContext;
    constexpr const char* actors[]{"NPC","NPC_Female","NPC_NAI","Player","PlayerFemale",
        "Horse","Dog","WildDog","InventoryDummyDog","InventoryDummyHorse",
        "DeadBody_Human","DeadBody_Horse","DeadBody_WolfDog","SO_DeadBody_Human",
        "SO_DeadBody_Human_Hanged","SO_DeadBody_Human_Interactable","SO_DeadBody_Horse","SO_DeadBody_WolfDog"};
    for(const auto* actor:actors)if(std::strcmp(name,actor)==0)return Kind::Actor;
    return Kind::Object;
}
inline int HoverOwner(int previous,const bool eligible[2],const float grip[2]) noexcept {
    if(previous>=0&&previous<2&&eligible[previous]){
        const int other=1-previous;
        if(eligible[other]&&grip[other]>.35f&&grip[previous]<.30f)return other;
        return previous;
    }
    return eligible[0]?0:(eligible[1]?1:-1);
}
struct Projection { bool inside=false;float x=50,y=50,distance=0; };
inline Projection Project(const interaction112::Ray& ray,float px,float py,float pz,
                          float reach,float baseRadius,float halfDegrees) noexcept {
    Projection result{};
    const float x=px-ray.ox,y=py-ray.oy,z=pz-ray.oz;
    const float d2=x*x+y*y+z*z,t=x*ray.x+y*ray.y+z*ray.z;
    if(!std::isfinite(d2)||!std::isfinite(t)||!std::isfinite(reach)||reach<=0||
       t<=.01f||d2>reach*reach)return result; // radial cap, no 5 cm grace beyond 2 m
    const float r=std::min(ray.hand?.65f:.50f,baseRadius+t*std::tan(halfDegrees*.017453292519943295f));
    const float perpendicular2=std::max(0.f,d2-t*t);
    if(!(r>0)||perpendicular2>r*r)return result;
    // Native candidate ranking uses squared distance of this percentage pair
    // from (50,50). Do not give every candidate the same perfect-center score.
    float rx=ray.y,ry=-ray.x,rz=0.f;
    const float horizontal=std::sqrt(rx*rx+ry*ry);
    if(horizontal>.001f){rx/=horizontal;ry/=horizontal;}else{rx=1;ry=0;}
    const float ux=ry*ray.z-rz*ray.y,uy=rz*ray.x-rx*ray.z,uz=rx*ray.y-ry*ray.x;
    result.inside=true;result.distance=std::sqrt(d2);
    result.x=50.f+20.f*(x*rx+y*ry+z*rz)/r;
    result.y=50.f-20.f*(x*ux+y*uy+z*uz)/r;
    return result;
}
}
