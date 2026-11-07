#include <limits>
// C44 GORE: pure loose-neck part. Retail pe_params_joint layout (offsets read off WHGame 6A350E20 SetParams 6F4957 /
// GetParams 21E2E1C), the engine's "unused" markers, the loose build (unlock, wide limits, no spring), the head impulse
// direction and the bounded ragdoll wait. No native call, no game.
#include "../src/vrik/decap_joint44.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace kcd2vr::decap44;
namespace g=kcd2vr::gore32;
static unsigned checks=0;
static void ck(bool b,const char* what){++checks;if(!b){std::printf("FAIL %s\n",what);assert(b);}}
static std::uint32_t bits(const float& f){std::uint32_t b=0;std::memcpy(&b,&f,4);return b;}
int main(){
    // markers exactly as the disassembly tests them
    const JointParams marker=query(0);
    ck(isUnused(marker.pivot[0])&&bits(marker.pivot[0])==0xFFBFFFFFu,"actual native float marker bytes = 0xFFBFFFFF");
    const auto* rawMarker=reinterpret_cast<const unsigned char*>(&marker);
    for(std::size_t off=0xC;off<0xA8;off+=4){if(off>=0x28&&off<0x30)continue;
        std::uint32_t raw=0;std::memcpy(&raw,rawMarker+off,4);ck(raw==0xFFBFFFFFu,"every native vector marker retains exact raw payload");}
    ck(bits(marker.animationTimeScale)==0xFFBFFFFFu&&bits(marker.ranimationTimeScale)==0xFFBFFFFFu,"native time-scale markers retain exact raw payload");
    ck(!isUnused(0.f)&&!isUnused(-3.f)&&!isUnused(1e30f),"ordinary floats are used");
    ck(isUnused(kUnusedInt)&&!isUnused(0)&&!isUnused(-1)&&!isUnused(458),"int unused = 0x80000000 only");
    // query: type 5, op[1] = the part, every tunable unused, pointer marker -1, bNoUpdate set (no refresh on a read)
    JointParams q=query(458);
    ck(q.type==5&&q.op[1]==458&&q.op[0]==kUnusedInt,"query names the part in op[1], parent unused");
    ck(isUnused(int(q.flags))&&q.flagsPivot==0,"query flags unused");
    for(int i=0;i<3;++i)ck(isUnused(q.pivot[i])&&isUnused(q.limits[0][i])&&isUnused(q.limits[1][i])&&isUnused(q.bounciness[i])&&
        isUnused(q.ks[i])&&isUnused(q.kd[i])&&isUnused(q.qdashpot[i])&&isUnused(q.kdashpot[i])&&isUnused(q.q[i])&&isUnused(q.qext[i])&&isUnused(q.qtarget[i]),"query vectors unused");
    for(int i=0;i<4;++i)ck(isUnused(q.q0[i]),"query q0 unused");
    ck(q.pMtx0==reinterpret_cast<void*>(~std::uintptr_t(0))&&q.pSelfCollidingParts==nullptr&&isUnused(q.nSelfCollidingParts),"query pointers/self-collision unused");
    ck(q.bNoUpdate==1&&isUnused(q.animationTimeScale)&&isUnused(q.ranimationTimeScale),"query no refresh, time scales unused");
    // raw byte view: the offsets the engine reads
    const unsigned char* b=reinterpret_cast<const unsigned char*>(&q);int iv=0;
    std::memcpy(&iv,b+0xAC,4);ck(iv==458,"op[1] at +0xAC");std::memcpy(&iv,b+0xA8,4);ck(iv==kUnusedInt,"op[0] at +0xA8");
    std::memcpy(&iv,b+0xC0,4);ck(iv==1,"bNoUpdate at +0xC0");std::memcpy(&iv,b,4);ck(iv==5,"type 5 at +0");
    // loose: what the ragdoll gave back, minus the angle locks, limits wide, ks 0, kd small, bounce 0, refresh on
    JointParams before=query(458);before.flags=0x200007u;before.op[0]=457;
    for(int i=0;i<3;++i){before.limits[0][i]=-.3f;before.limits[1][i]=.3f;before.ks[i]=50;before.kd[i]=2;before.bounciness[i]=.1f;before.q[i]=.01f;}
    JointParams l=loose(before,458);
    ck(l.type==5&&l.op[1]==458&&l.op[0]==kUnusedInt,"loose keeps part, parent unused");
    ck(l.flags==0x200000u,"loose clears angle0..2 locked (7) and keeps the other joint flags");
    for(int i=0;i<3;++i){ck(l.limits[0][i]==-3.f&&l.limits[1][i]==3.f,"loose limits +-3 rad");ck(l.ks[i]==0.f&&l.kd[i]==.05f&&l.bounciness[i]==0.f,"loose no spring, small damping, no bounce");}
    for(int i=0;i<3;++i)ck(isUnused(l.pivot[i])&&isUnused(l.q[i])&&isUnused(l.qext[i])&&isUnused(l.qdashpot[i])&&isUnused(l.kdashpot[i]),"loose leaves pivot/angles/dashpot alone");
    for(int i=0;i<4;++i)ck(isUnused(l.q0[i]),"loose leaves q0 alone");
    ck(l.pMtx0==reinterpret_cast<void*>(~std::uintptr_t(0))&&l.bNoUpdate==0&&isUnused(l.nSelfCollidingParts),"loose: engine refreshes the joint, matrix/self-collision untouched");
    JointParams u=query(5);JointParams lu=loose(u,5);ck(isUnused(int(lu.flags)),"flags stay unused when the read had none");
    JointParams lt=loose(before,458,{9.0,-1.0});ck(lt.limits[1][0]==3.1f&&lt.kd[0]==0.f,"tuning clamped (limit <= 3.1 rad, kd >= 0)");
    JointParams ln=loose(before,458,{std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN()});ck(ln.limits[1][0]==3.f&&ln.kd[0]==.05f,"NaN tuning = defaults");
    // head impulse: away from the hit side across the neck axis, 0.6 up, |v| = magnitude
    g::V3 hit{1,0,1.6},neck{0,0,1.6},up{0,0,1};
    g::V3 v=headImpulse(hit,neck,up,{},20);
    ck(std::fabs(g::len(v)-20)<1e-6&&v.x<0&&std::fabs(v.y)<1e-9&&v.z>0,"impulse to the far side of the neck, upward, magnitude kept");
    ck(std::fabs(v.z/(-v.x)-.6)<1e-6,"0.6 up per 1 sideways");
    g::V3 d=headImpulse(neck,neck,up,{0,2,0},10);ck(d.y>0&&d.x==0&&std::fabs(g::len(d)-10)<1e-6,"hit on the axis: away from Henry");
    g::V3 s=headImpulse(neck,neck,up,{},10);ck(s.x==0&&s.y==0&&std::fabs(s.z-10)<1e-6,"nothing to go by: straight up");
    g::V3 z=headImpulse(hit,neck,up,{},0);ck(g::len(z)==0,"zero magnitude = no impulse");
    g::V3 n=headImpulse({std::numeric_limits<double>::quiet_NaN(),0,0},neck,up,{},10);ck(g::len(n)==0,"NaN hit = no impulse");
    g::V3 t=headImpulse({1,0,1.6},{0,0,1.6},{0,1,0},{},10);ck(std::fabs(t.z)<1e-9&&t.y>0&&t.x<0,"up follows the body axis (lying body)");
    // wait: 20 Hz polls inside the window, expiry after it, no poll before the next slot
    Wait w{};ck(!w.due(100)&&!w.expired(100)&&w.waitedMs(100)==0,"inactive wait never due");
    w.begin(1000,3000);ck(w.active&&w.due(1000)&&!w.due(1001)&&!w.due(1049)&&w.due(1050)&&w.polls==2,"50 ms poll slots");
    ck(!w.expired(4000)&&w.expired(4001)&&w.waitedMs(4001)==3001,"window 3000 ms");
    ck(w.due(4001)&&w.polls==3,"a poll is still allowed on the expiry tick (last chance)");
    ck(style(0)==Style::Hide&&style(1)==Style::LooseNeck&&style(2)==Style::StumpHead&&style(7)==Style::Hide&&style(-1)==Style::Hide,"unknown style = hide");
    ck(std::strcmp(name(Style::LooseNeck),"LOOSE_NECK")==0&&std::strcmp(name(Style::Hide),"HIDE")==0&&std::strcmp(name(Style::StumpHead),"STUMP_HEAD")==0,"names");
    ck(hidesHead(Style::Hide)&&!hidesHead(Style::LooseNeck)&&hidesHead(Style::StumpHead),"C46: HIDE and STUMP_HEAD hide the head parts, LOOSE NECK keeps them");
    std::printf("decap_joint44_test OK (%u checks)\n",checks);return 0;
}
