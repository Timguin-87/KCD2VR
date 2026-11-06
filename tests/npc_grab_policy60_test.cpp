// DS-F93: NPC grab admission (pure, host-tested).
#include "../src/subsystems/physics/npc_grab_policy60.h"
#include <cstdio>
namespace ng=kcd2vr::npc_grab60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
int main(){
    {ng::Admission a{};a.npc=true;a.alive=true;a.partKnown=true;a.limbPart=true;a.held=true;a.gripMs=200;
     CK(ng::qualify(a)==ng::Why::None);}
    {ng::Admission a{};a.npc=true;a.alive=true;a.partKnown=true;a.limbPart=true;a.held=true;a.gripMs=200;a.corpse=true;
     CK(ng::qualify(a)==ng::Why::Corpse);} // corpses belong to the impale
    {ng::Admission a{};a.npc=true;a.alive=true;a.partKnown=true;a.limbPart=true;a.held=true;a.gripMs=200;a.inventoryItem=true;
     CK(ng::qualify(a)==ng::Why::InventoryItem);} // inventory belongs to native pickup
    {ng::Admission a{};a.npc=true;a.alive=true;a.partKnown=true;a.limbPart=false;a.held=true;a.gripMs=200;
     CK(ng::qualify(a)==ng::Why::TorsoOrHead);}
    {ng::Admission a{};a.npc=true;a.alive=true;a.partKnown=true;a.limbPart=true;a.held=true;a.gripMs=200;a.fistActive=true;
     CK(ng::qualify(a)==ng::Why::FistActive);} // never steal a deliberate fist
    {ng::Admission a{};a.npc=true;a.alive=true;a.partKnown=true;a.limbPart=true;a.held=true;a.gripMs=50;
     CK(ng::qualify(a)==ng::Why::GripTooShort);}
    {ng::Admission a{};a.npc=true;a.alive=false;a.partKnown=true;a.limbPart=true;a.held=true;a.gripMs=200;
     CK(ng::qualify(a)==ng::Why::NotLiving);}
    {ng::Admission a{};a.npc=false;a.alive=true;a.partKnown=true;a.limbPart=true;a.held=true;a.gripMs=200;
     CK(ng::qualify(a)==ng::Why::NotNpc);}
    // Native part existence and anatomical class are separate witnesses.
    {ng::Admission a{};a.npc=a.alive=a.held=true;a.gripMs=200;a.partKnown=true;a.limbPart=false;
     CK(ng::qualify(a)==ng::Why::TorsoOrHead);a.partKnown=false;a.limbPart=true;CK(ng::qualify(a)==ng::Why::PartUnknown);}
    if(fails)return 1;std::puts("npc grab policy F93 OK");return 0;
}
