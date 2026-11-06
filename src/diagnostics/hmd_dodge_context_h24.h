#pragma once
#include <cstdint>
#include "../core/snapshot_mailbox.h"
namespace kcd2vr::hmd_dodge_h24 {
struct Context {std::uintptr_t entity=0,actor=0,state=0,opponent=0,opponentState=0;std::uint64_t tick=0,epoch=0;bool incoming=false;};
inline core::SnapshotMailbox<Context> published;
inline bool ordinary(int phase,int type) noexcept {
    return (type==-1||type==0||type==1||type==5)&&
        (phase==1||phase==2||phase==4||phase==8||phase==16||phase==128||phase==256);
}
template<class Read> Context observe(std::uintptr_t base,std::uintptr_t actor,std::uintptr_t entity,
                                    std::uintptr_t state,std::uint64_t epoch,std::uint64_t now,Read read) noexcept {
    Context c{};if(!base||!actor||!entity||!state||!epoch||!now)return c;
    std::uintptr_t vf=0,back=0,player=0,playerEntity=0,opponent=0,os=0;
    int phase=-99,type=-99,enemyPhase=-99,enemyType=-99;
    if(!read(actor,vf)||vf!=base+0x3A80610||!read(actor+0x210,back)||back!=state||
       !read(state+0xCE0,back)||back!=actor||!read(actor+0x1F8,player)||!player||
       !read(player,vf)||vf!=base+0x3AA4B98||!read(player+0x278,back)||back!=actor||
       !read(player+0x38,playerEntity)||playerEntity!=entity||!read(entity,vf)||vf!=base+0x4072C78||
       !read(state+0x38,phase)||!read(state+0x218,type)||!ordinary(phase,type)||
       !read(state+0xCF8,opponent)||!opponent||opponent==actor||
       !read(opponent,vf)||vf!=base+0x3E3FAC8||!read(opponent+0x210,os)||!os||
       !read(os+0xCE0,back)||back!=opponent||!read(os+0xCF8,back)||back!=actor||
       !read(os+0x38,enemyPhase)||!read(os+0x218,enemyType)||
       !((enemyType>=0&&enemyType<=9&&(enemyPhase==4||enemyPhase==8))||ordinary(enemyPhase,enemyType)))return c;
    // One original-first callback, bounded reads; retire a pair changed mid-copy.
    if(!read(actor+0x210,back)||back!=state||!read(state+0xCF8,back)||back!=opponent||
       !read(opponent+0x210,back)||back!=os||!read(os+0xCF8,back)||back!=actor||
       !read(player+0x38,back)||back!=entity)return {};
    c={entity,actor,state,opponent,os,now,epoch,enemyType>=0&&enemyType<=9&&(enemyPhase==4||enemyPhase==8)};return c;
}
// D74: a dodge INPUT needs an engaged ordinary reciprocal pair, not an
// already-running incoming strike. Perfect-dodge success stays native. This
// still excludes paired/special actor ownership and never sends jump outside
// an exact current combat pair.
inline bool currentEngaged(const Context& c,std::uintptr_t entity,std::uint64_t epoch,std::uint64_t now) noexcept {
    return c.entity&&c.entity==entity&&c.actor&&c.state&&c.opponent&&c.opponentState&&
        c.epoch&&c.epoch==epoch&&c.tick&&now>=c.tick&&now-c.tick<=60;
}
inline bool current(const Context& c,std::uintptr_t entity,std::uint64_t epoch,std::uint64_t now) noexcept {
    return c.incoming&&currentEngaged(c,entity,epoch,now);
}
// Native block-context indicator. The original MAX-property payload is the
// authority; neither physical posture nor our held LT lease supplies this bit.
struct BlockContext {std::uintptr_t entity=0,actor=0,state=0;std::uint64_t tick=0,epoch=0;int maximum=-1;};
inline core::SnapshotMailbox<BlockContext> blockPublished;
template<class Read> BlockContext observeBlock(std::uintptr_t base,std::uintptr_t actor,std::uintptr_t entity,
                                              std::uintptr_t state,std::uint64_t epoch,std::uint64_t now,Read read) noexcept {
    if(!base||!actor||!entity||!state||!epoch||!now)return {};
    std::uintptr_t vf=0,back=0;unsigned char maximum=255;
    if(!read(actor+0x210,back)||back!=state||!read(state+0xCE0,back)||back!=actor||
       !read(state+0x630,vf)||vf!=base+0x3B0A968||!read(state+0x64D,maximum)||maximum>2||
       !read(actor+0x210,back)||back!=state||!read(state+0xCE0,back)||back!=actor)return {};
    return {entity,actor,state,now,epoch,int(maximum)};
}
inline bool blockCurrent(const BlockContext& c,std::uintptr_t entity,std::uint64_t epoch,std::uint64_t now) noexcept {
    return c.entity&&c.entity==entity&&c.actor&&c.state&&c.epoch&&c.epoch==epoch&&c.tick&&
        now>=c.tick&&now-c.tick<=60&&c.maximum>=0&&c.maximum<=2;
}
// One bounded input handoff. No token constitutes native dodge acceptance.
struct Guard {std::uintptr_t entity=0,actor=0,state=0;std::uint64_t epoch=0,tick=0;bool held=false;};
struct Request {Context owner{};std::uint64_t serial=0,started=0,poseTick=0,until=0;bool needsRelease=false;};
inline core::SnapshotMailbox<Guard> guardPublished;
inline core::SnapshotMailbox<Request> requestPublished;
inline std::atomic<std::uint64_t> nextRequest{0},releaseAck{0};
inline void publishGuard(std::uintptr_t entity,std::uintptr_t actor,std::uintptr_t state,
                         std::uint64_t epoch,std::uint64_t now,bool held) noexcept {
    (void)guardPublished.publish({entity,actor,state,epoch,now,held});
}
inline bool currentRequest(std::uintptr_t entity,std::uintptr_t actor,std::uintptr_t state,
                           std::uint64_t epoch,std::uint64_t now,Request& out) noexcept {
    Request r{};Context c{};
    if(!requestPublished.read(r)||!r.serial||!r.started||now<r.started||now-r.started>120||
       !r.poseTick||now<r.poseTick||now-r.poseTick>60||!r.until||now>=r.until||
       r.until-r.started>120||r.owner.entity!=entity||r.owner.actor!=actor||r.owner.state!=state||r.owner.epoch!=epoch||
       !published.read(c)||!currentEngaged(c,entity,epoch,now)||c.actor!=actor||c.state!=state||
       c.opponent!=r.owner.opponent||c.opponentState!=r.owner.opponentState)return false;
    out=r;return true;
}
inline bool beginRequest(std::uintptr_t entity,std::uint64_t epoch,std::uint64_t now,
                         std::uint64_t poseTick,Request& out) noexcept {
    Context c{};
    if(!published.read(c)||!currentEngaged(c,entity,epoch,now)||!poseTick||now<poseTick||now-poseTick>60)return false;
    const auto serial=nextRequest.fetch_add(1,std::memory_order_relaxed)+1;if(!serial)return false;
    out={c,serial,now,poseTick,now+120,false};
    Guard g{};out.needsRelease=guardPublished.read(g)&&g.held&&g.entity==c.entity&&g.actor==c.actor&&g.state==c.state&&g.epoch==c.epoch;
    return requestPublished.publish(out);
}
inline bool releaseRequired(const Request& r) noexcept {
    Guard g{};
    // A known owned held guard must acknowledge release. Its stale publication
    // cannot silently turn into permission; pending expires instead.
    return guardPublished.read(g)&&g.held&&g.entity==r.owner.entity&&g.actor==r.owner.actor&&
        g.state==r.owner.state&&g.epoch==r.owner.epoch;
}
inline bool releaseReady(const Request& r) noexcept {return !(r.needsRelease||releaseRequired(r))||releaseAck.load(std::memory_order_acquire)==r.serial;}
inline void acknowledgeRelease(std::uint64_t serial) noexcept {if(serial)releaseAck.store(serial,std::memory_order_release);}
inline void cancelRequest() noexcept {(void)requestPublished.publish({});}

}
