// I10 (R14-IM): KCD2 physics foreign data for an NPC body is type 2 (H50 C47 ROOT_OBSERVE 10:40:06 rootForeignKind=2).
#include "vrik/impale47.h"
#include <cstdio>
#include <cstring>
static int checks=0,fails=0;
static void ck(bool c,const char* n){++checks;if(!c){++fails;std::printf("FAIL %s\n",n);}}
int main(){
    namespace i=kcd2vr::impale47;
    const std::uintptr_t ent=0x1C3F8BBD800ull;
    ck(i::foreignIsEntity(2,ent,ent,true),"H50 ragdoll root: type 2 + same entity = ready");
    ck(i::foreignIsEntity(1,ent,ent,true),"type 1 still accepted");
    ck(!i::foreignIsEntity(2,ent,ent,false)&&i::foreignIsEntity(1,ent,ent,false),"I10 VrImpaleEntityRoot off = R25 rule (type 1 only)");
    ck(i::entityRoot.load(),"I10 VrImpaleEntityRoot on by default");
    ck(!i::foreignIsEntity(2,ent+8,ent,true),"other entity refused");
    ck(!i::foreignIsEntity(2,0,ent,true),"null data refused");
    ck(!i::foreignIsEntity(2,0,0,true),"null entity refused");
    ck(!i::foreignIsEntity(0,ent,ent,true),"type 0 refused");
    ck(!i::foreignIsEntity(3,ent,ent,true),"type 3 refused");
    ck(!i::foreignIsEntity(-1,ent,ent,true),"type -1 refused");
    ck(std::strstr(i::gaveUpText("BLADE_WITHDRAWN"),"pulled out")!=nullptr,"withdrawn text");
    ck(std::strstr(i::gaveUpText("WEAPON_RELEASED_OR_TRACKING_LOST"),"let go")!=nullptr,"released text");
    ck(std::strstr(i::gaveUpText("ORIGINAL_VICTIM_WAIT_EXPIRED"),"ragdoll")!=nullptr,"expired text");
    ck(!*i::gaveUpText("SUPERSEDED_BY_NEW_THRUST"),"superseded silent");
    ck(!*i::gaveUpText(""),"empty silent");ck(!*i::gaveUpText(nullptr),"null silent");
    ck(std::strstr(i::gaveUpText("VICTIM_ADMISSION_INVALID"),"C47")!=nullptr,"other reasons generic");
    ck(!std::strstr(i::gaveUpText("BLADE_WITHDRAWN_X"),"pulled"),"exact match only");
    std::printf("impale_root_r14 %d/%d\n",checks-fails,checks);return fails?1:0;}
