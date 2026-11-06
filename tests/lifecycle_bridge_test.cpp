// Actual production observers with synthetic owned memory/register snapshots.
// No retail instruction or physics function is called by this fixture.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
static unsigned carryCalls59c7g=0;
static bool carryResult59c7g=false;
static bool __fastcall carryOriginal59c7g(void*) {++carryCalls59c7g;return carryResult59c7g;}
template<class T>void put165(unsigned char* p,T v){std::memcpy(p,&v,sizeof(v));}
int main(){
    hostTickCount64=10000;hostModuleHandleW=reinterpret_cast<void*>(0x180000000ull);g_wh_pinned104=0;
    const auto base=WhBase();kcd2vr::retail_build112::hostRetailVerified112=true;
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,true,GetTickCount64());
    g_lifecycle_ready165=true;g_lifecycle_session165=1;g_lifecycle_until165=20000;
    std::array<unsigned char,0x100> henry{},entity{},item{},pickable{};
    std::array<unsigned char,0x400> stack{};
    put165(henry.data(),base+kCEntityVftRva);put165(henry.data()+0xC,std::uint32_t(123));g_last_player_entity=henry.data();
    put165(entity.data(),base+kCEntityVftRva);put165(entity.data()+0xC,std::uint32_t(900));
    put165(item.data(),base+0x3A2DA88);put165(item.data()+0x30,std::uint64_t(77));put165(item.data()+0x38,std::uint64_t(88));
    put165(pickable.data(),base+0x3EC85E0);put165(pickable.data()+0x38,entity.data());put165(pickable.data()+0x58,item.data());
    const auto callerStack=reinterpret_cast<std::uintptr_t>(stack.data()+0x280);
    put165(stack.data()+0x278,base+0x2AC0087);
    LifecycleContext165 c{};
    auto reg=[&](LifecycleRegister165 r,std::uintptr_t v){DetourModKit::hook::gpr(c,r)=v;};
    auto resolve=[&]{c.stack=callerStack-8;reg(LifecycleRegister165::Rcx,reinterpret_cast<std::uintptr_t>(pickable.data()));};
    resolve();reg(LifecycleRegister165::Rdx,999);
    PickupRequest165(c);il165::Event e;assert(!g_lifecycle_queue165.pop(e)); // NPC excluded
    resolve();reg(LifecycleRegister165::Rdx,123);PickupRequest165(c);
    assert(g_lifecycle_queue165.pop(e)&&e.target==900&&e.entity==reinterpret_cast<std::uintptr_t>(entity.data())&&e.key==77&&e.meta==88);
    const auto call=e.call;assert(call&&LifecycleWatched165(e));
    // Mutation/removal during the original call cannot corrupt the return identity.
    item.fill(0);pickable.fill(0);c.stack=callerStack-0x120;reg(LifecycleRegister165::Rax,1);PickupReturn165(c);
    assert(g_lifecycle_queue165.pop(e)&&e.call==call&&e.key==77&&e.result==1);
    PickupReturn165(c);assert(!g_lifecycle_queue165.pop(e)); // no stale repeat
    std::array<unsigned char,0x700> controller{};
    put165(controller.data()+0x2A0,4);put165(controller.data()+0x2A4,2);
    c.stack=callerStack;reg(LifecycleRegister165::R8,123);AlchemyUseBegin165(c);
    c.stack=callerStack-8;put165(stack.data()+0x278,base+0xD5AE33);reg(LifecycleRegister165::Rcx,reinterpret_cast<std::uintptr_t>(controller.data()));AlchemyUseController165(c);
    c.stack=callerStack-0x40;reg(LifecycleRegister165::Rdi,reinterpret_cast<std::uintptr_t>(controller.data()));
    reg(LifecycleRegister165::Rax,reinterpret_cast<std::uintptr_t>(entity.data()));AlchemyTarget165(c);
    assert(g_lifecycle_queue165.pop(e)&&e.kind==il165::Kind::AlchemyTarget&&e.target==900);
    reg(LifecycleRegister165::Rbx,5);reg(LifecycleRegister165::Rax,1);AlchemyUse165(c);
    assert(g_lifecycle_queue165.pop(e)&&e.action==5&&e.state==4&&e.previousState==2&&e.result==1&&e.target==900);
    AlchemyUse165(c);assert(!g_lifecycle_queue165.pop(e)); // scope ended, direct unrelated call cannot borrow it
    // V168 distinct SetPhysicParams add/remove schemas, exact same-stack return.
    std::array<unsigned char,0x80> body{};put165(body.data(),base+0x407FC80);
    c.stack=callerStack;reg(LifecycleRegister165::Rsi,reinterpret_cast<std::uintptr_t>(entity.data()));
    reg(LifecycleRegister165::R8,reinterpret_cast<std::uintptr_t>(body.data()));reg(LifecycleRegister165::R9,10);
    const auto before=c;PhysicParamRequest168(c);assert(!std::memcmp(&before,&c,sizeof(c)));
    assert(g_lifecycle_queue165.pop(e)&&e.kind==il165::Kind::PhysicParamRequest168&&e.action==10&&e.target==900);
    reg(LifecycleRegister165::Rax,63);PhysicParamReturn168(c);assert(g_lifecycle_queue165.pop(e)&&e.result==63&&e.action==10);
    PhysicParamReturn168(c);assert(!g_lifecycle_queue165.pop(e));
    reg(LifecycleRegister165::R9,11);PhysicParamRequest168(c);assert(g_lifecycle_queue165.pop(e)&&e.action==11);
    reg(LifecycleRegister165::Rax,1);PhysicParamReturn168(c);assert(g_lifecycle_queue165.pop(e)&&e.action==11&&e.result==1);
    reg(LifecycleRegister165::R9,3);PhysicParamRequest168(c);assert(!g_lifecycle_queue165.pop(e));
    reg(LifecycleRegister165::R9,10);put165(entity.data()+0xC,std::uint32_t(901));PhysicParamRequest168(c);assert(!g_lifecycle_queue165.pop(e));
    put165(entity.data()+0xC,std::uint32_t(900));
    // Native parsed action and true return, not script wrapper result=1.
    std::array<unsigned char,0x2000> parserStack{};
    const auto wrapperSp=reinterpret_cast<std::uintptr_t>(parserStack.data()+0x1500);
    const auto parserRbp=wrapperSp-0xAD0;
    c.stack=wrapperSp;reg(LifecycleRegister165::R9,10);PhysicParamRequest168(c);
    assert(g_lifecycle_queue165.pop(e));const auto physicsCall=e.call;
    reg(LifecycleRegister165::Rbp,parserRbp);reg(LifecycleRegister165::R14,reinterpret_cast<std::uintptr_t>(body.data()));
    auto parserPut=[&](std::uintptr_t off,auto v){put165(reinterpret_cast<unsigned char*>(parserRbp+off),v);};
    parserPut(0x5F8,std::uintptr_t(0x2222));parserPut(0x618,7);parserPut(0x61C,8);
    PhysicAddSubmit36(c);assert(g_lifecycle_queue165.pop(e)&&e.kind==il165::Kind::PhysicSubmit&&e.call==physicsCall&&e.part0==7&&e.part1==8&&e.buddy==0x2222);
    reg(LifecycleRegister165::Rax,45);reg(LifecycleRegister165::R15,45);
    PhysicActionReturn36(c);assert(g_lifecycle_queue165.pop(e)&&e.constraintId==45&&e.result==45);
    PhysicActionReturn36(c);assert(!g_lifecycle_queue165.pop(e));
    PhysicParamReturn168(c);assert(g_lifecycle_queue165.pop(e)&&e.result==45);
    reg(LifecycleRegister165::R9,11);PhysicParamRequest168(c);assert(g_lifecycle_queue165.pop(e));
    parserPut(0x854,45);reg(LifecycleRegister165::Rdx,parserRbp+0x850);
    reg(LifecycleRegister165::R14,0x1234);PhysicRemoveSubmit36(c);assert(!g_lifecycle_queue165.pop(e)); // wrong body
    reg(LifecycleRegister165::R14,reinterpret_cast<std::uintptr_t>(body.data()));
    const auto registersBefore=c;PhysicRemoveSubmit36(c);assert(!std::memcmp(&c,&registersBefore,sizeof(c)));
    assert(g_lifecycle_queue165.pop(e)&&e.constraintId==45&&e.valid==1);
    reg(LifecycleRegister165::Rax,0);PhysicActionReturn36(c);assert(g_lifecycle_queue165.pop(e)&&e.result==0&&e.constraintId==45);
    reg(LifecycleRegister165::Rax,1);PhysicParamReturn168(c);assert(g_lifecycle_queue165.pop(e)&&e.result==1); // distinguish failed native action / script success
    // Same ID with a different CEntity address cannot borrow the watch.
    std::array<unsigned char,0x100> otherEntity{};put165(otherEntity.data(),base+kCEntityVftRva);put165(otherEntity.data()+0xC,std::uint32_t(900));
    reg(LifecycleRegister165::Rsi,reinterpret_cast<std::uintptr_t>(otherEntity.data()));PhysicParamRequest168(c);assert(!g_lifecycle_queue165.pop(e));
    // Input intent is observed independently of native hand/physics ownership.
    TrackingSnapshot tracking{};tracking.tick=10000;tracking.sequence=1;tracking.head_valid=tracking.right_valid=tracking.left_valid=true;
    LifecycleTracking36(tracking,true);assert(g_lifecycle_queue165.pop(e)&&e.side==0&&e.reason==0);assert(g_lifecycle_queue165.pop(e)&&e.side==1&&e.reason==0);
    tracking.right_grip=.8f;hostTickCount64=10010;tracking.tick=10010;LifecycleTracking36(tracking,true);
    assert(g_lifecycle_queue165.pop(e)&&e.side==0&&e.reason==1&&!e.key&&!e.entity);
    hostTickCount64=10100;tracking.tick=10100;LifecycleTracking36(tracking,true);assert(!g_lifecycle_queue165.pop(e));
    hostTickCount64=10600;tracking.tick=10600;LifecycleTracking36(tracking,true);assert(g_lifecycle_queue165.pop(e)&&e.reason==5);
    tracking.right_grip=.1f;hostTickCount64=10610;tracking.tick=10610;LifecycleTracking36(tracking,true);assert(g_lifecycle_queue165.pop(e)&&e.reason==2);
    tracking.right_grip=.8f;tracking.tick=10620;hostTickCount64=10620;LifecycleTracking36(tracking,true);assert(g_lifecycle_queue165.pop(e)&&e.reason==1);
    tracking.right_valid=false;tracking.tick=10630;hostTickCount64=10630;LifecycleTracking36(tracking,true);assert(g_lifecycle_queue165.pop(e)&&e.reason==3&&e.result==0);
    // Overflow and capture budget are explicit; no silent queue expansion.
    il165::Scopes<> full;il165::Event scopeEvent{};scopeEvent.session=1;
    for(unsigned i=0;i<8;++i)LifecycleScope36(full,0x100+i,scopeEvent);
    const auto lost=g_lifecycle_scope_lost36.load();LifecycleScope36(full,0x999,scopeEvent);assert(g_lifecycle_scope_lost36==lost+1);
    auto budgetEvent=LifecycleEvent165(il165::Kind::GripSample,0);g_lifecycle_budget36=4096;LifecycleEmit165(budgetEvent);
    assert(g_lifecycle_budget_lost36==1&&!g_lifecycle_queue165.pop(e));g_lifecycle_budget36=0;
    // Actual carry query adapter preserves the native return, excludes NPCs,
    // and does not mislabel the actor service's raw key as a CEntity id.
    std::array<unsigned char,0x200> carryPlayer{};
    std::array<unsigned char,0xe0> carryModule{};
    put165(carryPlayer.data(),base+0x3AA4B98);put165(carryPlayer.data()+0x38,henry.data());
    put165(carryPlayer.data()+0x188,carryModule.data());
    put165(carryModule.data(),base+0x3A2F438);put165(carryModule.data()+0xd8,carryPlayer.data());
    put165(carryModule.data()+0x68,5);put165(carryModule.data()+0x80,std::uint32_t(456));
    g_carry_original59c7g=&carryOriginal59c7g;carryResult59c7g=true;
    assert(CarryQueryDetour59c7g(carryPlayer.data())&&carryCalls59c7g==1);
    assert(g_lifecycle_queue165.pop(e)&&e.kind==il165::Kind::CarrySnapshot59c7g&&
        e.state==5&&e.result==1&&e.valid==1&&e.actor==123&&e.key==456&&!e.entity&&!e.target);
    assert(CarryQueryDetour59c7g(carryPlayer.data())&&carryCalls59c7g==2&&!g_lifecycle_queue165.pop(e));
    put165(carryModule.data()+0x68,6);carryResult59c7g=false;
    assert(!CarryQueryDetour59c7g(carryPlayer.data())&&carryCalls59c7g==3);
    assert(g_lifecycle_queue165.pop(e)&&e.state==6&&e.result==0&&e.valid==1);
    put165(carryPlayer.data()+0x38,entity.data());
    assert(!CarryQueryDetour59c7g(carryPlayer.data())&&carryCalls59c7g==4&&!g_lifecycle_queue165.pop(e));
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,GetTickCount64());
    put165(carryPlayer.data()+0x38,henry.data());
    assert(!CarryQueryDetour59c7g(carryPlayer.data())&&carryCalls59c7g==5&&!g_lifecycle_queue165.pop(e));
    g_carry_original59c7g=nullptr;
    PhysicParamRequest168(c);assert(!g_lifecycle_queue165.pop(e));
    g_lifecycle_until165=9999;resolve();PickupRequest165(c);assert(!g_lifecycle_queue165.pop(e));
    std::puts("PASS production lifecycle adapters: actor identity, stack joins, freed-item return, alchemy target/stage, timeout");
}
