#include <string_view>
// A3 REAL ANVIL: physics box selection, box lock/retire, swept box collision incl. rotation, no jump.
#define KCD2VR_ANVIL_A3_PURE
#include "subsystems/smithing/anvil_a2.h"
#include "subsystems/smithing/anvil_physics_a3.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
using namespace kcd2vr::subsystems::smithing;
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL %s line %d\n",#x,__LINE__);std::exit(1);}}while(0)
namespace av=anvil_a2;namespace ap=anvil_physics_a3;
static ap::Box box(double x0,double y0,double z0,double x1,double y1,double z1,std::uintptr_t e){ap::Box b{};b.mn[0]=x0;b.mn[1]=y0;b.mn[2]=z0;b.mx[0]=x1;b.mx[1]=y1;b.mx[2]=z1;b.entity=e;return b;}
static av::Blow blow(av::Identity id,double x,double y,double z,const ap::Box* b){
    av::Blow w{};w.id=id;w.point={x,y,z};w.rootPos={0,0,0};w.rootFwd={0,1,0};
    if(b){w.boxValid=true;w.boxMin={b->mn[0],b->mn[1],b->mn[2]};w.boxMax={b->mx[0],b->mx[1],b->mx[2]};w.phys=b->entity;}return w;}
int main(){
    // ---- selection: anvil (60x30x80 cm, top 2 cm under blow), floor slab, table, house, tall post
    const double hit[3]={10.0,20.0,0.82};
    ap::Box c[6]={box(9.0,19.0,-0.2,11.0,21.0,0.0,1)/*floor*/,box(9.7,19.85,0.0,10.3,20.15,0.80,2)/*anvil*/,
        box(9.5,19.5,0.0,10.6,20.6,0.79,3)/*bigger table-ish under*/,box(0,0,0,30,30,6,4)/*house*/,box(9.95,19.95,0,10.05,20.05,2.0,5)/*post*/,
        box(NAN,0,0,1,1,1,6)};
    ap::Box pick{};const char* why=nullptr;
    CHECK(ap::choose(hit,c,6,pick,why)&&pick.entity==2);
    CHECK(!ap::choose(hit,c,1,pick,why)&&std::string_view(why)=="NO_TOP_AT_BLOW_HEIGHT");
    const ap::Box onlyHouse[1]={box(0,0,0,30,30,0.82,4)};CHECK(!ap::choose(hit,onlyHouse,1,pick,why)&&std::string_view(why)=="NO_ANVIL_SIZED_STATIC");
    const ap::Box away[1]={box(11,21,0,11.5,21.5,0.8,7)};CHECK(!ap::choose(hit,away,1,pick,why)&&std::string_view(why)=="NO_STATIC_UNDER_BLOW");
    CHECK(!ap::choose(hit,c,0,pick,why));
    float mn[3],mx[3];ap::queryBox(hit,mn,mx);CHECK(mn[2]<0.3f&&mx[2]>0.82f&&mx[0]-mn[0]>0.69f);
    // ---- physics lock: first boxed blow locks, unboxed blows never teach in REAL mode
    const av::Identity id{0x1000,0x2000,7,3};const ap::Box an=box(9.7,19.85,0.0,10.3,20.15,0.80,2);
    av::Anvil a{};av::Ring r{};
    r.blow[0]=blow(id,10,20,.82,nullptr);r.blow[0].serial=1;r.serial=1;
    CHECK(av::integrate(a,r,id,11,-1,true)==av::Retire::None&&!a.locked&&a.lastSerial==1);
    r.blow[1]=blow(id,10,20,.82,&an);r.blow[1].serial=2;r.serial=2;
    av::integrate(a,r,id,11,-1,true);CHECK(a.locked&&a.box&&a.phys==2&&std::fabs(a.top-.80)<1e-9);
    // later blows inside never move it
    r.blow[2]=blow(id,10.25,20.1,.83,&an);r.blow[2].serial=3;r.serial=3;av::integrate(a,r,id,11,-1,true);CHECK(a.locked&&a.boxMax.x==10.3);
    // a blow far outside re-locks from that blow's own box
    const ap::Box an2=box(12,22,0,12.6,22.3,.8,9);
    r.blow[3]=blow(id,12.3,22.1,.82,&an2);r.blow[3].serial=4;r.serial=4;CHECK(av::integrate(a,r,id,11,-1,true)==av::Retire::OutsideFootprint&&a.locked&&a.phys==9);
    // identity (station/save) and pose epoch retire before any pose use
    CHECK(av::validate(a,{0x1000,0x2000,7,4},11,{0,0,0},{0,1,0})==av::Retire::Identity&&!a.locked);
    av::integrate(a,r,id,11,-1,true);CHECK(!a.locked); // old ring blows are already consumed (serial)
    r.blow[4]=blow(id,10,20,.82,&an);r.blow[4].serial=5;r.serial=5;av::integrate(a,r,id,11,-1,true);CHECK(a.locked);
    CHECK(av::validate(a,id,12,{0,0,0},{0,1,0})==av::Retire::PoseEpoch&&!a.locked);
    r.blow[5]=blow(id,10,20,.82,&an);r.blow[5].serial=6;r.serial=6;av::integrate(a,r,id,11,-1,true);CHECK(a.locked);
    CHECK(av::validate(a,id,11,{.3,0,0},{0,1,0})==av::Retire::RootMoved&&!a.locked);
    // foreign identity never joins
    av::Anvil f{};av::Ring rf{};rf.blow[0]=blow({0x9999,0x2000,7,3},10,20,.82,&an);rf.blow[0].serial=1;rf.serial=1;
    av::integrate(f,rf,id,11,-1,true);CHECK(!f.locked);
    // LEARNED mode ignores boxes (A2 unchanged)
    av::Anvil l{};av::Ring rl{};for(unsigned i=0;i<3;++i){rl.blow[i]=blow(id,10+.01*i,20,.82,&an);rl.blow[i].serial=i+1;}rl.serial=3;
    av::integrate(l,rl,id,11,-1,false);CHECK(l.locked&&!l.box);
    // ---- collision against the locked physics box
    av::Anvil k{};av::Ring rk{};rk.blow[0]=blow(id,10,20,.82,&an);rk.blow[0].serial=1;rk.serial=1;av::integrate(k,rk,id,11,-1,true);CHECK(k.locked&&k.box);
    const av::Probe head[1]={{{0,0,0},.03}};
    const av::Pose above{{10,20,1.0},{0,0,0,1}},inside{{10,20,.6},{0,0,0,1}},below{{10,20,-.5},{0,0,0,1}},beside{{11,20,1.0},{0,0,0,1}};
    CHECK(av::penetration(k,above,head,1)==0&&av::penetration(k,inside,head,1)>.2);
    auto s=av::solve(k,above,inside,head,1);CHECK(s.corrected&&std::fabs(s.pose.p.z-(.80+.03))<.003);
    // fast swing straight through the box with both endpoints clear (swept, not endpoint-only)
    s=av::solve(k,above,below,head,1);CHECK(s.corrected&&s.pose.p.z>.82);
    // sideways pass well above: no correction
    s=av::solve(k,above,beside,head,1);CHECK(!s.corrected);
    // start inside: never corrected (no jump)
    s=av::solve(k,inside,above,head,1);CHECK(s.startInside&&!s.corrected&&s.pose.p.z==1.0);
    // rotation-only penetration: a 40 cm handle pivoting down around a fixed wrist over the anvil
    const av::Probe tip[1]={{{.4,0,0},.02}};
    const double sh=std::sin(.3926990816987241),ch=std::cos(.3926990816987241);
    const av::Pose level{{9.65,20,1.0},{0,0,0,1}};         // tip at x=10.05, z=1.0 (clear)
    const av::Pose down{{9.65,20,1.0},{0,sh,0,ch}};         // +45 deg about y: tip at x=9.93, z=.72 (inside)
    CHECK(av::penetration(k,level,tip,1)==0&&av::penetration(k,down,tip,1)>0);
    s=av::solve(k,level,down,tip,1);CHECK(s.corrected&&s.fraction<1&&av::penetration(k,s.pose,tip,1)<=av::kClear);
    CHECK(std::fabs(s.pose.p.x-9.65)<1e-6&&std::fabs(s.pose.p.z-1.0)<1e-6); // wrist position unchanged, only the rotation stopped
    // D39: resting contact must permit a true upward arc or a level turn.
    // The old symmetric arc enclosure expands the top across t=0 and sticks
    // these clear motions at fraction 0 even though every actual probe clears.
    const av::Pose resting{{9.65,20,.8190001},{0,0,0,1}};
    const av::Pose lifting{{9.65,20,.8190001},{0,-sh,0,ch}};
    s=av::solve(k,resting,lifting,tip,1);CHECK(!s.startInside&&!s.corrected&&s.fraction==1);
    const av::Pose turning{{9.65,20,.8190001},{0,0,sh,ch}};
    s=av::solve(k,resting,turning,tip,1);CHECK(!s.startInside&&!s.corrected&&s.fraction==1);
    const av::Pose turningInto{{9.65,20,.8190001},{0,sh,0,ch}};
    s=av::solve(k,resting,turningInto,tip,1);CHECK(s.corrected&&s.fraction<1);
    const av::Probe twoEnds[2]={{{.4,0,0},.02},{{-.4,0,0},.02}};
    const av::Pose centered{{10,20,.8190001},{0,0,0,1}},oneEndDown{{10,20,.8190001},{0,-sh,0,ch}};
    av::Anvil wide=k;wide.boxMin.x=9.4;wide.boxMax.x=10.6;
    s=av::solve(wide,centered,oneEndDown,twoEnds,2);CHECK(s.corrected&&s.fraction<1); // another end still penetrates

    // Check the separating-plane proof against a densely sampled actual arc,
    // including non-axis-aligned wrists and the nlerp fast path. An optimistic
    // minimum would let the tool tunnel; all reported bounds must be lower.
    for(int u=0;u<11;++u)for(int v=0;v<9;++v){
        const auto qa=av::qnorm({.17*u-.7,.11*v-.4,.23,1.0});
        const auto qb=av::qnorm({.09*v+.1,-.12*u+.5,.37,1.0});
        const av::D3 point{.031*u-.1,.041*v-.2,.13};
        const double bound=av::minimumRotatedZD39(qa,qb,point);
        for(int j=0;j<=100;++j)CHECK(bound<=av::rot(av::slerp(qa,qb,j/100.0),point).z+1e-12);
    }
    // Live source switches revoke immediately even without a new blow. Old
    // queued strokes cannot teach a geometry choice made after that stroke.
    av::Anvil modeAnvil=k;CHECK(av::syncModeD39(modeAnvil,1,7)&&!modeAnvil.locked);
    av::Ring modeRing{};modeRing.serial=1;modeRing.blow[0]=blow(id,10,20,.82,&an);
    modeRing.blow[0].serial=1;modeRing.blow[0].modeRevisionD39=6;
    modeAnvil.lastSerial=0;av::integrate(modeAnvil,modeRing,id,11,-1,true,7);CHECK(!modeAnvil.locked);
    modeRing.serial=2;modeRing.blow[1]=blow(id,10,20,.82,&an);modeRing.blow[1].serial=2;modeRing.blow[1].modeRevisionD39=7;
    av::integrate(modeAnvil,modeRing,id,11,-1,true,7);CHECK(modeAnvil.locked&&modeAnvil.box);
    CHECK(av::syncModeD39(modeAnvil,2,8)&&!modeAnvil.locked&&modeAnvil.lastSerial==2);
    av::integrate(modeAnvil,modeRing,id,11,-1,false,8);CHECK(!modeAnvil.locked);
    CHECK(!av::syncModeD39(modeAnvil,2,8)&&modeAnvil.modeRevisionD39==8);
    // bounded recovery
    bool done=false;const auto p=av::recover(above,{{10,20,0.5},{0,0,0,1}},done);CHECK(!done&&std::fabs(p.p.z-(1.0-av::kMaxStep))<1e-9);
    // mode helpers
    const auto oldRevision=av::modeRevisionD39.load();av::setMode(1);CHECK(av::physicsMode()&&av::enabled.load());
    CHECK(av::modeRevisionD39.load()==oldRevision+1);av::setMode(1);CHECK(av::modeRevisionD39.load()==oldRevision+1);av::setMode(2);CHECK(!av::physicsMode()&&av::enabled.load());
    av::setMode(0);CHECK(!av::enabled.load());av::setMode(9);CHECK(av::physicsMode());
    std::printf("PASS %u anvil_a3 checks\n",checks);return 0;
}
