// DS-F95: NPC grab bridge (pure, host-tested).
#include "../src/subsystems/physics/npc_grab_bridge60.h"
#include <cstdio>
#include <cstring>
namespace nb=kcd2vr::npc_grab_bridge60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static nb::Evidence good(){nb::Evidence e{};e.entity=100;e.side=0;e.part=5;e.bodyKind=nb::kBodyLiving;
    e.alive=true;e.fistActive=false;e.held=true;e.gripMs=300;e.press=1;e.tick=e.now=1000;
    e.identity=0x1000;e.body=0x2000;e.nativeActorCurrent=e.currentBodyPart=e.limbClassified=true;return e;}
int main(){
    {auto r=nb::bridge(good());CK(r.ok&&r.entity==100&&r.part==5&&r.bodyKind==nb::kBodyLiving);}
    {auto e=good();e.alive=false;e.bodyKind=nb::kBodyArticulated;auto r=nb::bridge(e);
     CK(!r.ok&&std::strcmp(r.why,"CORPSE_NOT_GRABBABLE")==0);} // corpses are the impale's
    {auto e=good();e.fistActive=true;auto r=nb::bridge(e);
     CK(!r.ok&&std::strcmp(r.why,"FIST_OWNER_ACTIVE")==0);} // never steal a deliberate fist
    {auto e=good();e.part=9999;e.limbClassified=false;auto r=nb::bridge(e);CK(!r.ok);} // number cannot classify a limb
    {auto e=good();e.part=9999;CK(nb::bridge(e).ok);} // a separately proven raw native part is not a joint-array index
    {auto e=good();e.part=5;e.limbClassified=false;CK(!nb::bridge(e).ok);} // a low numeric ID may be head/torso
    {auto e=good();e.currentBodyPart=false;CK(!nb::bridge(e).ok);}
    {auto e=good();e.nativeActorCurrent=false;CK(!nb::bridge(e).ok);}
    {auto e=good();e.now=1081;CK(!nb::bridge(e).ok);}
    {auto e=good();e.side=2;CK(!nb::bridge(e).ok);}
    {auto e=good();e.bodyKind=int(kcd2vr::npc_contact172::Body::Physical);CK(!nb::bridge(e).ok);}
    {auto e=good();e.bodyKind=int(kcd2vr::npc_contact172::Body::Rigid);CK(!nb::bridge(e).ok);}
    static_assert(nb::kBodyLiving==int(kcd2vr::npc_contact172::Body::Living));
    static_assert(nb::kBodyArticulated==int(kcd2vr::npc_contact172::Body::Articulated));
    {auto e=good();e.held=false;auto r=nb::bridge(e);
     CK(!r.ok&&std::strcmp(r.why,"NOT_HELD")==0);}
    {auto e=good();e.gripMs=40;auto r=nb::bridge(e);
     CK(!r.ok&&std::strcmp(r.why,"GRIP_TOO_SHORT")==0);}
    {auto e=good();e.bodyKind=0;auto r=nb::bridge(e);
     CK(!r.ok&&std::strcmp(r.why,"NOT_NPC")==0);}
    if(fails)return 1;std::puts("npc grab bridge F95 OK");return 0;
}
