#pragma once
#include "../dialogue_state59c10r2.h"
#include <array>
#include <cstdint>
#include <cmath>

namespace kcd2vr::dialogue_native_head28 {
namespace ds=dialogue_state59c10r2;
// Existing native View callback only. The caller owns exact-retail qualification
// and a nonblocking pose-mutex scope; no skinning hint/cache can supply identity.
// Native94D030 reads this same exact original actor/entity/character, and its
// CSkeletonPose::GetAbsJoint641250 reaches CPoseData+50 ->53D830 below.
struct Binding {
    std::uintptr_t player=0,actor=0,entity=0,animated=0,gameObject=0;
    std::uintptr_t game=0,module=0,manager=0,instance=0,scene=0,cameras=0;
    std::uintptr_t inputManager=0,inputMap=0,getCharacter=0;
    friend bool operator==(const Binding& a,const Binding& z) noexcept {
        return a.player==z.player&&a.actor==z.actor&&a.entity==z.entity&&
            a.animated==z.animated&&a.gameObject==z.gameObject&&a.game==z.game&&
            a.module==z.module&&a.manager==z.manager&&a.instance==z.instance&&
            a.scene==z.scene&&a.cameras==z.cameras&&a.inputManager==z.inputManager&&
            a.inputMap==z.inputMap&&a.getCharacter==z.getCharacter;
    }
};
template<class Read> bool binding(std::uintptr_t b,const ds::State& s,Read& read,Binding& out) noexcept {
    Binding p{};p.player=s.player;p.entity=s.entity;p.instance=s.instance;
    p.inputManager=s.inputOwner.manager;p.inputMap=s.inputOwner.map;
    std::uintptr_t vf=0,back=0,active=0;unsigned char bit=0;std::uint32_t phase=0,flags=0;
    if(!p.player||!p.entity||!p.instance||!p.inputManager||!p.inputMap||
       !read(p.player,vf)||vf!=b+0x3AA4B98||!read(p.player+0x40,vf)||vf!=b+0x3A81028||
       !read(p.player+0x278,p.actor)||!p.actor||!read(p.actor,vf)||vf!=b+0x3A80610||
       !read(p.actor+0x1F8,back)||back!=p.player||!read(p.player+0x38,back)||back!=p.entity||
       !read(p.entity,vf)||vf!=b+0x4072C78||!read(vf+0x328,p.getCharacter)||p.getCharacter!=b+0x6411AC||
       !read(p.player+0x268,p.animated)||!p.animated||!read(p.animated,vf)||vf!=b+0x3A49468||
       !read(p.animated+0x18,p.gameObject)||!p.gameObject||!read(p.gameObject+0x18,back)||back!=p.entity||
       !read(b+0x549D388,p.game)||!p.game||!read(p.game+0x150,p.module)||!p.module||
       !read(p.module,vf)||vf!=b+0x3E64C38||!read(p.module+0x10,p.manager)||!p.manager||
       !read(p.manager,vf)||vf!=b+0x3E64C90||!read(p.manager+0x90,back)||back!=p.instance||
       !read(p.instance,vf)||vf!=b+0x3E63A40||!read(p.instance+0x18,vf)||vf!=b+0x3E63B60||
       !read(p.instance+0x278,phase)||phase!=s.membership.state||phase==0x17||
       !read(p.instance+0x158,p.scene)||!p.scene||!read(p.scene+0xBC,flags)||flags!=s.membership.flags||(flags&8)||
       !read(p.game+0x38,p.cameras)||!p.cameras||!read(p.cameras,vf)||vf!=b+0x408E560||
       !read(p.cameras+0x38,bit)||bit||!read(p.cameras+0x3B,bit)||bit||
       !read(p.cameras+0x30,active)||active!=p.cameras+0x1E8||!read(active,vf)||vf!=b+0x3A5A0F8||
       !read(p.inputManager,vf)||vf!=b+0x4048018||!read(p.inputMap,vf)||vf!=b+0x4048530||
       !read(p.inputMap+0x10,back)||back!=p.inputManager||!read(p.inputMap+9,bit)||bit!=1)return false;
    out=p;return true;
}
struct Body {
    std::uintptr_t character=0,pose=0,owner=0,joints=0,poseGet=0;
    std::uint32_t count=0;std::uint16_t head=0;
    friend bool operator==(const Body& a,const Body& z) noexcept {
        return a.character==z.character&&a.pose==z.pose&&a.owner==z.owner&&
            a.joints==z.joints&&a.poseGet==z.poseGet&&a.count==z.count&&a.head==z.head;
    }
};
template<class Read> bool body(std::uintptr_t b,std::uintptr_t character,Read& read,Body& out) noexcept {
    Body p{};p.character=character;p.pose=character+0x8E0;
    std::uintptr_t vf=0,owner=0,method=0;std::uint32_t count=0;
    if(!character||!read(character,vf)||vf!=b+0x40602F8||
       !read(vf+0x38,method)||method!=b+0x1AA6BC0||
       !read(character+0x110,vf)||vf!=b+0x3A711E0||
       !read(character+0x698,vf)||vf!=b+0x3A71028||!read(vf+0xD8,method)||method!=b+0x641250||
       !read(p.pose,vf)||vf!=b+0x3A71510||!read(vf+8,method)||method!=b+0x1A74B40||
       !read(vf+0x10,method)||method!=b+0x53F550||!read(vf+0x50,p.poseGet)||p.poseGet!=b+0x53D830||
       !read(p.pose+0x60,p.count)||p.count<8||p.count>2048||
       !read(p.pose+0x190,p.owner)||!p.owner||!read(character+0xA70,owner)||owner!=p.owner||
       !read(p.owner,vf)||vf!=b+0x406B930||!read(p.owner+8,p.joints)||p.joints<4||
       !read(p.joints-4,count)||count!=p.count||
       !read(p.owner+0x17C+2*10,p.head)||p.head==0xFFFF||p.head>=p.count)return false;
    out=p;return true;
}
template<class Read,class GetCharacter,class ReadPose,class CurrentEpoch,class ReadOrigin,class OriginCurrent>
bool copy(std::uintptr_t base,const ds::State& state,std::uint64_t now,std::uint64_t epoch,
    bool qualifiedScope,Read&& read,GetCharacter&& getCharacter,ReadPose&& readPose,
    CurrentEpoch&& currentEpoch,ReadOrigin&& readOrigin,OriginCurrent&& originCurrent,ds::NativeHead& out) noexcept {
    if(!qualifiedScope||!base||!epoch||currentEpoch()!=epoch||!ds::fresh(state,now)||!state.active||
       state.camera!=ds::Camera::FirstPerson||!state.membership.known||!state.membership.active||
       !state.membership.participant||!state.membership.listed||!state.inputOwner.known||!state.inputOwner.enabled)return false;
    Binding first{},last{};Body a{},z{};std::uintptr_t character=0,again=0;
    if(!binding(base,state,read,first)||!getCharacter(first.getCharacter,first.entity,character)||
       !body(base,character,read,a))return false;
    native_roomscale_d76::Origin roomOrigin{};
    if(!readOrigin(character,roomOrigin))return false;
    ds::V modelHead{};std::array<float,8> world{},worldAgain{};
    if(!readPose(a.poseGet,a.pose,int(a.head),modelHead)||!ds::finite(modelHead)||
       !read(character+0xA08,world))return false;
    for(float v:world)if(!std::isfinite(v))return false;
    const float norm=world[0]*world[0]+world[1]*world[1]+world[2]*world[2]+world[3]*world[3];
    if(norm<.8f||norm>1.2f||world[7]<.1f||world[7]>10.f)return false;
    const float inverse=1.f/std::sqrt(norm);
    ds::NativeHead next{};next.tick=now;next.epoch=epoch;next.entity=first.entity;next.character=character;
    next.body={world[0]*inverse,world[1]*inverse,world[2]*inverse,world[3]*inverse};next.scale=world[7];
    next.modelOrigin={world[4],world[5],world[6]};next.modelOriginKnown=true;next.roomOriginD76=roomOrigin;
    next.world=ds::add(next.modelOrigin,ds::rotate(next.body,ds::mul(modelHead,next.scale)));
    // Re-read the original actor binding and character, not an old skinning hint.
    // A changing native model/pose refuses rather than issuing a fresh timestamp.
    ds::V headAgain{};
    if(!binding(base,state,read,last)||!(last==first)||
       !getCharacter(last.getCharacter,last.entity,again)||again!=character||
       !body(base,again,read,z)||!(z==a)||!read(character+0xA08,worldAgain)||worldAgain!=world||
       !readPose(z.poseGet,z.pose,int(z.head),headAgain)||headAgain.x!=modelHead.x||headAgain.y!=modelHead.y||headAgain.z!=modelHead.z||
       currentEpoch()!=epoch||!originCurrent(character,roomOrigin.serial)||!ds::fresh(next,now))return false;
    out=next;return true;
}
}
