#pragma once
#include "subsystems/dialogues/lively_npc_d73.h"
#include <array>
namespace kcd2vr::lively_npc_d73 {
struct MembersD76 {std::array<std::uint64_t,16> ids{};unsigned count=0;bool known=false;};
// Same exact native scene participant layout as dialogue_membership19c2.
// No entity search/call. Only used in the existing 10 Hz admitted script callback.
template<class Read> MembersD76 Members(std::uintptr_t base,const Frame& f,Read&& read) noexcept {
 MembersD76 out{};if(!base||!f.dialogue||!f.scene||!f.dialogueInstance||!f.dialogueManager||!f.playerSoul)return out;
 std::uintptr_t vf=0,current=0,begin=0,end=0;std::uint32_t state=0,flags=0;
 if(!read(f.dialogueManager,vf)||vf!=base+0x3E64C90||!read(f.dialogueManager+0x90,current)||current!=f.dialogueInstance||
    !read(f.dialogueInstance,vf)||vf!=base+0x3E63A40||!read(f.dialogueInstance+0x18,vf)||vf!=base+0x3E63B60||
    !read(f.dialogueInstance+0x158,current)||current!=f.scene||!read(f.dialogueInstance+0x278,state)||state==0x17||
    !read(f.scene+0xBC,flags)||(flags&8)||!read(f.scene,begin)||!read(f.scene+8,end)||
    end<begin||(end-begin)%0x20||(end-begin)/0x20>16||(!begin&&end))return {};
 bool henry=false;std::array<unsigned char,16> participating{};std::array<std::uint64_t,16> all{};
 const unsigned count=unsigned((end-begin)/0x20);
 for(unsigned i=0;i<count;++i){
    if(!read(begin+0x20*i,all[i])||!read(begin+0x20*i+0x18,participating[i]))return {};
    if(participating[i]&&all[i]){if(all[i]==f.playerSoul)henry=true;else out.ids[out.count++]=all[i];}
 }
 if(!henry)return {};
 for(unsigned i=0;i<count;++i){std::uint64_t id=0;unsigned char active=0;
    if(!read(begin+0x20*i,id)||id!=all[i]||!read(begin+0x20*i+0x18,active)||active!=participating[i])return {};}
 std::uint32_t nstate=0,nflags=0;
 if(!read(f.dialogueManager+0x90,current)||current!=f.dialogueInstance||
    !read(f.dialogueInstance+0x158,current)||current!=f.scene||!read(f.dialogueInstance+0x278,nstate)||nstate!=state||
    !read(f.scene+0xBC,nflags)||nflags!=flags||!read(f.scene,current)||current!=begin||!read(f.scene+8,current)||current!=end)return {};
 out.known=true;return out;
}
}
