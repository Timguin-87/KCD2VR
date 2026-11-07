// DS-F107: corpse-limb drag admission (pure, host-tested).
#include "../src/subsystems/physics/corpse_drag_policy60.h"
#include <cstdio>
#include <cstring>
namespace cd=kcd2vr::corpse_drag60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static cd::Evidence good(){cd::Evidence e{};e.entity=900;e.part=400;e.bodyKind=cd::kBodyArticulated;
    e.body=0xABCD;e.impaleActive=false;e.held=true;e.gripMs=300;e.press=1;e.tick=e.now=1000;e.nativeDead=e.currentBodyPart=e.limbClassified=true;return e;}
int main(){
    {auto r=cd::admit(good());CK(r.ok&&r.entity==900&&r.part==400&&r.body==0xABCD);}
    {auto e=good();e.bodyKind=cd::kBodyLiving;auto r=cd::admit(e);
     CK(!r.ok&&std::strcmp(r.why,"LIVING_NOT_DRAGGABLE")==0);} // living actors are never dragged
    {auto e=good();e.impaleActive=true;auto r=cd::admit(e);
     CK(!r.ok&&std::strcmp(r.why,"IMPALE_OWNS_THE_VICTIM")==0);} // the impale's victim stays its own
    {auto e=good();e.body=0;auto r=cd::admit(e);
     CK(!r.ok&&std::strcmp(r.why,"NO_RAGDOLL_BODY")==0);}
    {auto e=good();e.part=-1;auto r=cd::admit(e);
     CK(!r.ok&&std::strcmp(r.why,"PART_UNKNOWN")==0);}
    {auto e=good();e.part=5000;CK(cd::admit(e).ok);} // separately proven native part; not a skeleton array index
    {auto e=good();e.part=5;e.limbClassified=false;auto r=cd::admit(e);
     CK(!r.ok&&std::strcmp(r.why,"NOT_A_LIMB_PART")==0);} // small numeric IDs are not limb proof
    {auto e=good();e.bodyKind=int(kcd2vr::npc_contact172::Body::Physical);CK(!cd::admit(e).ok);}
    {auto e=good();e.bodyKind=int(kcd2vr::npc_contact172::Body::Rigid);CK(!cd::admit(e).ok);}
    static_assert(cd::kBodyArticulated==int(kcd2vr::npc_contact172::Body::Articulated));
    {auto e=good();e.held=false;auto r=cd::admit(e);
     CK(!r.ok&&std::strcmp(r.why,"GRIP_TOO_SHORT")==0);}
    {auto e=good();e.gripMs=50;auto r=cd::admit(e);
     CK(!r.ok&&std::strcmp(r.why,"GRIP_TOO_SHORT")==0);}
    {auto e=good();e.bodyKind=0;auto r=cd::admit(e);
     CK(!r.ok&&std::strcmp(r.why,"NOT_A_CORPSE")==0);}
    {auto e=good();e.nativeDead=false;CK(!cd::admit(e).ok);} // living articulated is not dead proof
    {auto e=good();e.limbClassified=false;CK(!cd::admit(e).ok);} // head/torso in valid numeric range
    {auto e=good();e.currentBodyPart=false;CK(!cd::admit(e).ok);}
    {auto e=good();e.press=0;CK(!cd::admit(e).ok);}
    {auto e=good();e.now=e.tick+81;CK(!cd::admit(e).ok);}
    if(fails)return 1;std::puts("corpse drag F107 OK");return 0;
}
