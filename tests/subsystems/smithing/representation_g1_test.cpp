// Runs the real smith reader AND its production native-lifetime wrapper with
// sparse fake memory. Fake byte witnesses exercise admission, not retail ABI;
// verify_smith_tool36.py separately checks those pins against the actual DLL.
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <unordered_map>
#include <vector>
#include "vrik/smith_tool36.h"
#include "activity_input163.h"
namespace st=kcd2vr::smith_tool36;
static constexpr std::uintptr_t base=0x180000000ull;
static constexpr std::uintptr_t instance=0x100000,player=0x200000,model=0x300000,
    slots=0x400000,character=0x500000,attachment=0x600000,charManager=0x700000,
    object=0x800000,holder=0x900000,itemManager=0xA00000,registry=0xB00000,
    item=0xC00000,itemLink=0xD00000,name=0xE00000;
static constexpr std::uint32_t sourceId=100,actualId=200;
static constexpr std::uint64_t key=0x0200000000040003ull; // index3, generation1
static std::unordered_map<std::uintptr_t,unsigned char> image;
static std::function<void(std::uintptr_t)> beforeRead;
static bool retail=true;
template<class T>static void put(std::uintptr_t at,const T& value){
    auto p=reinterpret_cast<const unsigned char*>(&value);
    for(std::size_t i=0;i<sizeof(value);++i)image[at+i]=p[i];
}
static void blob(std::uintptr_t at,std::initializer_list<unsigned char> bytes){for(auto b:bytes)image[at++]=b;}
template<class T>static bool Read(const void* ptr,T& value){
    const auto at=reinterpret_cast<std::uintptr_t>(ptr);if(beforeRead)beforeRead(at);
    auto p=reinterpret_cast<unsigned char*>(&value);
    for(std::size_t i=0;i<sizeof(value);++i){auto it=image.find(at+i);if(it==image.end())return false;p[i]=it->second;}
    return true;
}
static const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
static std::uintptr_t WhBase(){return base;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail;}}
static bool PhysVft6698(void* p,std::uintptr_t want){std::uintptr_t v=0;return Read(p,v)&&v==base+want;}
static std::atomic<void*>g_henry_item_attach_manager{reinterpret_cast<void*>(itemManager)};
#include "vrik/smith_tool36.inl"
#include "native_pins_fixture_g1.h"
static std::uintptr_t entryFor(std::uint64_t value){return registry+0x168+(std::uint32_t(value)&0x3FFFFu)*24;}
static void reset(int side=0,bool workpiece=false){
    image.clear();beforeRead={};retail=true;seedNativePinsG1();
    put(instance,base+st::kInstanceVft);put(instance+0x18,player);put(instance+0x2C,std::uint8_t(1));put(instance+st::kModel,model);
    put(model,base+st::kModelVft);put(player,base+st::kPlayerVft);put(player+0x30,std::uint32_t(900));put(player+0x990,slots);
    put(model+(workpiece?st::kWorkpieceSlot:side?st::kTongsSlot:st::kHammerSlot),holder);
    put(holder,base+(workpiece?st::kWorkpieceVft:st::kSlotToolVft));put(holder+st::kHand,side);
    put(holder+st::kHost,std::uint32_t(900));put(holder+st::kAttached,std::uint8_t(1));put(holder+st::kEntity,sourceId);
    put(holder+st::kKey,workpiece?std::uint64_t(0):key);put(holder+st::kAssetSlot,std::uint8_t(0));
    put(slots+st::kHandKeys+8*side,key);put(attachment,base+st::kAttachmentVft);put(attachment+0x28,charManager);
    put(charManager+0x70,character);put(attachment+0x10,name);put(attachment+0x20,object);
    put(object,base+st::kObjectVft);put(object+8,actualId);
    const char* text=workpiece?"entity_left":side?"hand_left":"hand_right";
    for(std::size_t i=0;i<=std::strlen(text);++i)put(name+i,text[i]);
    put(player+0x9D0,itemManager);put(itemManager,base+st::kItemManagerVft);put(itemManager+0x38,player);
    put(base+st::kRegistryGlobal,registry);put(entryFor(key)+0x28,std::uint16_t(1));put(entryFor(key)+0x38,item);
    put(item,base+st::kItemVft);put(item+0x30,key);put(item+0x88,itemLink);put(itemLink+0x30,actualId);
    namespace ai=kcd2vr::activity_input163;
    ai::witness175.Clear();ai::witness175.ready=true;ai::witness175.transitionDepth175=0;
    ai::witness175.active[unsigned(ai::Kind175::Smithing)]=instance;
    ai::witness175.smithSampleC1=(ai::witness175.smithEpochC1.load()<<8)|11;
    g_henry_item_attach_manager=reinterpret_cast<void*>(itemManager);
}
static unsigned checks=0;
static void check(bool value,const char* label){++checks;if(!value){std::fprintf(stderr,"FAIL %u %s\n",checks,label);std::abort();}}
static bool pure(st::Binding& out,st::ReadEvidenceI1* e=nullptr){return st::read(rd,base,instance,player,character,attachment,actualId,11,out,e);}
static bool wrapped(st::Binding& out){return ReadSmithTool36(reinterpret_cast<void*>(attachment),actualId,character,out);}
static void refuses(void(*change)(),const char* label){reset();change();st::Binding out{};check(!pure(out)&&!out,label);}
int main(){
    st::Binding out{},first{};st::ReadEvidenceI1 e{};
    reset();check(pure(out,&e)&&out.entity==actualId&&out.sourceEntity==sourceId&&out.representation.item==item,"source/runtime IDs join through the exact native current item");
    check(out.representation.registry==registry&&out.representation.entry==entryFor(key)&&out.representation.generation==1,"bounded native registry identity is retained");
    check(wrapped(out)&&out.lifetime==kcd2vr::activity_input163::witness175.lifetime175.load(),"production wrapper qualifies same current native lifecycle");
    first=out;check(wrapped(out)&&st::same(first,out),"stable exact binding equality");
    out.representation.link+=8;check(!st::same(first,out),"replacement link cannot reuse tool socket");
    out=first;out.representation.registry+=8;check(!st::same(first,out),"replacement world registry cannot reuse tool socket");
    reset(1);check(pure(out)&&out.side==1&&out.tool==1,"tongs current native representation uses left hand membership");
    reset(1,true);check(!pure(out)&&!out,"workpiece does not gain an inventory alternate-entity escape");
    put(holder+st::kEntity,actualId);check(pure(out)&&out.tool==2&&!out.representation.item,"direct sword workpiece retains strict native entity path");
    reset();put(holder+st::kEntity,actualId);image.erase(base+st::kRegistryGlobal);check(pure(out)&&!out.representation.item,"unchanged exact source entity path needs no registry lookup");
    refuses([]{put(base+st::kRegistryGlobal,std::uintptr_t(0));},"missing registry refuses");
    refuses([]{put(entryFor(key)+0x28,std::uint16_t(2));},"recycled registry slot generation refuses");
    refuses([]{put(entryFor(key)+0x38,std::uintptr_t(0));},"absent current representation refuses");
    refuses([]{put(item+0x30,key+1);},"full current item key mismatch refuses");
    refuses([]{put(item,base+0x1234);},"non C_Item representation refuses");
    refuses([]{put(item+0x88,std::uintptr_t(0));},"missing link record refuses");
    refuses([]{put(itemLink+0x30,actualId+1);},"another current entity cannot borrow same tool key");
    refuses([]{put(slots+st::kHandKeys,key+1);},"current hand key replaced refuses");
    refuses([]{put(holder+st::kKey,key+1);},"different station tool key refuses");
    refuses([]{put(holder+st::kHost,std::uint32_t(901));},"foreign holder host refuses");
    refuses([]{put(holder+st::kHand,1);},"wrong hand refuses");
    refuses([]{put(holder+st::kAttached,std::uint8_t(0));},"not attached refuses");
    refuses([]{put(itemManager+0x38,player+0x100);},"foreign item-manager player refuses");
    refuses([]{put(player+0x9D0,itemManager+0x100);},"player manager backlink mismatch refuses");
    // Interrupt between the two samples of a native registry/item field.
    for(int which=0;which<8;++which){
        reset();unsigned seen=0;const auto target=which==0?base+st::kRegistryGlobal:which==1?entryFor(key)+0x28:which==2?entryFor(key)+0x38:
            which==3?item+0x30:which==4?item+0x88:which==5?itemLink+0x30:which==6?model+st::kHammerSlot:slots+st::kHandKeys;
        beforeRead=[&,target](std::uintptr_t at){if(at!=target||++seen!=2)return;
            if(which==1)put(at,std::uint16_t(2));else if(which==5)put(at,actualId+1);else if(which==3||which==7)put(at,key+1);
            else put(at,std::uintptr_t(0));};
        check(!pure(out)&&!out,"mid-read registry/item/holder/hand churn refuses");
    }
    reset();retail=false;check(!wrapped(out)&&!out,"retail mismatch revokes before pointer admission");
    reset();kcd2vr::activity_input163::witness175.transitionDepth175=1;check(!wrapped(out)&&!out,"native transition refuses");
    reset();kcd2vr::activity_input163::witness175.active[unsigned(kcd2vr::activity_input163::Kind175::Smithing)]=0;check(!wrapped(out)&&!out,"activity ended refuses");
    reset();bool changed=false;beforeRead=[&](std::uintptr_t at){if(at==item+0x88&&!changed){changed=true;kcd2vr::activity_input163::witness175.lifetime175.fetch_add(1);}};
    check(!wrapped(out)&&!out,"actual wrapper rejects lifetime change during representation read");
    reset();changed=false;beforeRead=[&](std::uintptr_t at){if(at==item+0x88&&!changed){changed=true;g_henry_item_attach_manager=reinterpret_cast<void*>(itemManager+8);}};
    check(!wrapped(out)&&!out,"actual wrapper rejects replaced Henry manager during read");
    reset();kcd2vr::activity_input163::witness175.smithSampleC1=0;check(!wrapped(out)&&!out,"unknown current native phase refuses");
    reset();changed=false;beforeRead=[&](std::uintptr_t at){if(at==item+0x88&&!changed){changed=true;
        auto& w=kcd2vr::activity_input163::witness175;w.smithSampleC1=(w.smithEpochC1.load()<<8)|14;}};
    check(!wrapped(out)&&!out,"actual wrapper rejects current native phase change during read");
    reset();unsigned finalRead=0;beforeRead=[&](std::uintptr_t at){if(at==base+st::kRegistryGlobal&&++finalRead==4)put(slots+st::kHandKeys,key+1);};
    check(!pure(out)&&!out,"hand cannot change during final registry revalidation");
    reset();put(holder+st::kEntity,std::uint32_t(0));check(!pure(out)&&!out,"unknown station-linked source never gains representation permission");
    reset();{const std::uint64_t last=0x020000000003D090ull;put(holder+st::kKey,last);put(slots+st::kHandKeys,last);put(item+0x30,last);
        put(entryFor(last)+0x28,std::uint16_t(0));put(entryFor(last)+0x38,item);
        check(pure(out)&&out.key==last,"native maximum valid registry index is admitted");}
    // Exercise the exact native key boundaries independently of the body reader.
    const std::uint64_t badKeys[]{0,0x0100000000040003ull,0x0200000000040000ull,0x020000000003D091ull};
    for(auto value:badKeys){
        reset();st::InventoryRepresentation r{};const char* why=nullptr;
        check(!st::inventoryRepresentation(rd,base,player,value,actualId,r,why)&&!r.item,"invalid native key type/index refuses");
    }
    for(unsigned i=1;i<=128;++i){
        reset();const std::uint64_t live=0x0200000000000000ull|(std::uint64_t(i)<<18)|i;
        put(holder+st::kKey,live);put(slots+st::kHandKeys,live);put(item+0x30,live);
        put(entryFor(live)+0x28,std::uint16_t(i));put(entryFor(live)+0x38,item);
        check(pure(out)&&out.key==live&&out.representation.generation==i,"current exact indexed generation join accepts");
        put(entryFor(live)+0x28,std::uint16_t(i+1));check(!pure(out)&&!out,"same index with next generation refuses");
    }
    std::printf("G1 smith native representation: %u assertions passed; fake memory/native leaves, no headset claim\n",checks);
}
