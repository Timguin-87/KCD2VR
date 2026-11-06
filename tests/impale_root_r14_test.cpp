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
    ck(std::strstr(i::gaveUpText("VICTIM_ADMISSION_INVALID"),"gave up")!=nullptr,"other reasons generic");
    {char b[96]{};ck(!std::strcmp(i::gaveUpLine("VICTIM_ADMISSION_INVALID",b,sizeof(b)),"impale: gave up (VICTIM_ADMISSION_INVALID)"),"R128 reason in the line");
     ck(std::strstr(i::gaveUpLine("BLADE_WITHDRAWN",b,sizeof(b)),"pulled out")!=nullptr,"R128 specific text kept");
     ck(!*i::gaveUpLine("SUPERSEDED_BY_NEW_THRUST",b,sizeof(b)),"R128 superseded silent");
     ck(std::strlen(i::gaveUpLine("NO_EXACT_RAGDOLL_OR_NEW_CORPSE_CONTACT_IN_ORIGINAL_WAIT",b,sizeof(b)))<=96,"R128 fits the status line");}
    ck(!std::strstr(i::gaveUpText("BLADE_WITHDRAWN_X"),"pulled"),"exact match only");
    std::printf("impale_root_r14 %d/%d\n",checks-fails,checks);return fails?1:0;}
