// Regression crosses BOTH production gates: the D52 visual observation and
// PrepareNpcQuery172's actual self-exclusion builder. No engine calls execute.
#define main observer_fixture_main_d52
#include "contact_occupancy_d52_test.cpp"
#undef main
#include "../src/vrik/npc_contact172.h"
namespace npc172=kcd2vr::npc_contact172;
using kcd2vr::frik_pose_probe::ExactHandItemState;
struct NpcQuery172 {bool requested=false,prepared=false,ready=false;npc172::Gate gate=npc172::Gate::Off;npc172::Exclusions skip{};};
static NpcQuery172 g_npc_query172;
struct PhysEquipment6698 {
    struct Item {void* entity=nullptr;struct {std::uint32_t entity=0;} profile;};
    bool valid=false;std::uint64_t epoch=0,request=0,tick=0;int count=0;void* player=nullptr;std::array<Item,4> items{};
};
static struct EquipmentFixture {bool read(PhysEquipment6698&) {return false;}} g_phys_equipment6698;
static bool NpcContract172(std::uintptr_t){return retail_build112::ready;}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t vf=0;return Read(p,vf)&&vf==WhBase()+rva;}
static bool PhysEntityIdentity135(void* p,std::uint32_t id){std::uint32_t got=0;return PhysVft6698(p,0x4072c78)&&Read(static_cast<unsigned char*>(p)+0xc,got)&&got==id;}
static std::array<unsigned char,0x200> extraEntities[2]{};
static std::array<unsigned char,0x100> trees[3]{},nodes[3]{},proxies[3]{},bodies[3]{},objects[2]{};
static std::uint32_t heldIds[2]{};
static void* EntityById6694(std::uint32_t id){return id==10?g_last_player_entity.load():id==20?extraEntities[0].data():id==30?extraEntities[1].data():nullptr;}
static bool NativeHeldItemId6694(int side,std::uint32_t& id){id=heldIds[side];return keyKnown[side];}
#include "../src/vrik/npc_contact172.inl"
static void queryReset(){
    reset();g_npc_query172={};g_npc_query172.requested=true;heldIds[0]=heldIds[1]=0;
    for(auto& e:extraEntities)e.fill(0);for(auto& a:trees)a.fill(0);for(auto& a:nodes)a.fill(0);
    for(auto& a:proxies)a.fill(0);for(auto& a:bodies)a.fill(0);for(auto& a:objects)a.fill(0);
    put(player.data()+0x38,reinterpret_cast<std::uintptr_t>(g_last_player_entity.load()));
    for(unsigned i=0;i<3;++i){auto* entity=static_cast<unsigned char*>(EntityById6694(10+10*i));
        put(entity,WhBase()+0x4072c78);put(entity+0xc,std::uint32_t(10+10*i));
        put(entity+0xa8,reinterpret_cast<std::uintptr_t>(trees[i].data()));
        put(trees[i].data()+8,reinterpret_cast<std::uintptr_t>(nodes[i].data()));
        put(nodes[i].data()+0x20,1u);put(nodes[i].data()+0x28,reinterpret_cast<std::uintptr_t>(proxies[i].data()));
        put(proxies[i].data(),WhBase()+0x3ac3478);put(proxies[i].data()+0x38,reinterpret_cast<std::uintptr_t>(bodies[i].data()));
        put(bodies[i].data(),WhBase()+(i?0x3a48dc0:0x3a82570));
    }
    for(int side=0;side<2;++side){put(objects[side].data(),WhBase()+0x3A68F68);put(objects[side].data()+8,30u);}
}
int main(){
    for(auto& a:extraEntities)regions.push_back({a.data(),a.size()});
    for(auto* group:{&trees,&nodes,&proxies,&bodies})for(auto& a:*group)regions.push_back({a.data(),a.size()});
    for(auto& a:objects)regions.push_back({a.data(),a.size()});
    queryReset();auto o=sample();
    {co::QueryScopeD53 owner(o);
        check(PrepareNpcQuery172()&&g_npc_query172.ready&&g_npc_query172.skip.bodyCount==1&&legacyReads==0,
              "current pose-only empty hands reach inner query without stale gameplay equipment");}
    check(!co::queryObservationD53,"scope cannot leak observation into later pose");
    queryReset();o=sample();check(!PrepareNpcQuery172()&&legacyReads==1,"no observation cannot turn unknown gameplay cache into clearance");
    queryReset();put(attachments[1].data()+0x20,reinterpret_cast<std::uintptr_t>(objects[1].data()));o=sample();
    {co::QueryScopeD53 owner(o);check(PrepareNpcQuery172()&&g_npc_query172.skip.bodyCount==2&&
        g_npc_query172.skip.excludes(extraEntities[1].data(),bodies[2].data()),"exact entity-held tool is excluded for the other free hand");}
    queryReset();keys[0]=77;heldIds[0]=20;put(attachments[0].data()+0x20,reinterpret_cast<std::uintptr_t>(objects[0].data()));o=sample();
    {co::QueryScopeD53 owner(o);check(PrepareNpcQuery172()&&g_npc_query172.skip.bodyCount==3&&
        g_npc_query172.skip.excludes(extraEntities[0].data(),bodies[1].data())&&
        g_npc_query172.skip.excludes(extraEntities[1].data(),bodies[2].data()),"coexisting weapon and station prop both excluded");}
    for(unsigned bad=0;bad<8;++bad){queryReset();o=sample();
        switch(bad){case 0:++g_pose_epoch6689;break;case 1:++g_weapon_request_epoch6694;break;
          case 2:++kcd2vr::tracking59c6::epoch;break;case 3:++kcd2vr::activity_input163::witness175.lifetime175;break;
          case 4:keys[1]=77;break;case 5:put(attachments[1].data()+0x20,reinterpret_cast<std::uintptr_t>(objects[1].data()));break;
          case 6:keyKnown[0]=false;break;case 7:put(player.data()+0x38,std::uintptr_t(0x777));break;}
        co::QueryScopeD53 owner(o);check(!PrepareNpcQuery172(),"changed body/tracking/scene/key/binding/player cannot reuse old exclusions");}
    queryReset();put(attachments[0].data()+0x20,reinterpret_cast<std::uintptr_t>(objects[0].data()));o=sample();
    {co::QueryScopeD53 owner(o);mutateAt=reinterpret_cast<std::uintptr_t>(objects[0].data()+8);mutation=4;
        check(!PrepareNpcQuery172(),"owner changes during tool-body exclusion retire query");}
    queryReset();put(attachments[0].data()+0x20,reinterpret_cast<std::uintptr_t>(objects[0].data()));o=sample();
    {co::QueryScopeD53 owner(o);put(objects[0].data(),WhBase()+0x123);
        check(!PrepareNpcQuery172(),"unknown prop binding class never treated as empty");}
    queryReset();o=sample();{co::QueryScopeD53 outer(o);auto inner=o;inner.owner.scene=123;
        {co::QueryScopeD53 nested(inner);check(co::queryObservationD53==&inner,"nested scope uses current observation");}
        check(co::queryObservationD53==&o,"nested scope restores original pass");}
    std::printf("contact_query_d53: %u checks passed; engine/headset=NOT_EXECUTED\n",count);return 0;
}
