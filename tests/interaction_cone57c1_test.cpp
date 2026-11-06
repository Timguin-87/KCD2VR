#include "vrik/interaction_cone57c1.h"
#include "vrik/physical_interaction.h"
#include <cassert>
#include <limits>
#include <cstdio>
using namespace kcd2vr;
int main(){
    using namespace interaction57c1;
    for(const auto* actor:{"NPC","NPC_Female","NPC_NAI","Dog","Horse","WildDog","SO_DeadBody_Human"})assert(Classify(actor)==Kind::Actor);
    for(const auto* object:{"PickableItem","Door","Apple","DogMarkingSpot","AnimalSpawner"})assert(Classify(object)==Kind::Object);
    assert(Classify("ForgeBuilderTrigger")==Kind::NativeContext);
    assert(Classify("UIForgeBuilderLinkNode")==Kind::Object);
    assert(Classify("forgebuildertrigger")==Kind::Object);
    assert(Classify(nullptr)==Kind::Unknown&&Classify("")==Kind::Unknown);
    bool eligible[]{true,true};float grips[]{0,0};
    assert(HoverOwner(-1,eligible,grips)==0);
    assert(HoverOwner(1,eligible,grips)==1); // no right-hand theft
    for(float g:{.2f,.34f,.36f,.57f,.59f,.9f}){grips[1]=g;assert(HoverOwner(1,eligible,grips)==1);} // squeeze gap closed
    grips[0]=0;grips[1]=.4f;assert(HoverOwner(0,eligible,grips)==1);
    eligible[1]=false;assert(HoverOwner(1,eligible,grips)==0);
    eligible[0]=false;assert(HoverOwner(0,eligible,grips)==-1);
    interaction112::Ray ray{};ray.hand=true;ray.limit=HandReach;ray.radius=HandRadius;
    auto project=[&](float x,float y,float z){return Project(ray,x,y,z,2,HandRadius,HandHalfAngle);};
    assert(project(0,1,0).inside&&project(0,2,0).inside);
    assert(project(.45f,1,0).inside); // wider than the old 14 cm + 10 degree cone
    assert(!project(.8f,1,0).inside&&!project(0,-1,0).inside);
    assert(!project(0,2.01f,0).inside&&!project(.2f,2,0).inside); // strict spherical 2 m
    assert(!project(std::numeric_limits<float>::quiet_NaN(),1,0).inside);
    assert(project(.1f,1,0).x>project(0,1,0).x); // distinguish competing candidates
    assert(project(0,1,.1f).y<50);
    ray.ox=10;ray.oy=20;ray.oz=30;assert(project(10,21,30).inside);
    ray={};assert(Project(ray,0,15,0,20,.21f,10).inside); // head/dog range is not the hand limit
    assert(physical_interaction106::EffectiveReach(20,true,2)==2);
    assert(physical_interaction106::EffectiveReach(.8f,true,2)==.8f);
    assert(physical_interaction106::EffectiveReach(20,false,2)==20);
    std::puts("PASS split-category, hover squeeze/ownership, wide cone, strict reach, native range and ranking");
}
