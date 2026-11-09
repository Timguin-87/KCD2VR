#pragma once
#include <array>
#include <cstdint>

namespace kcd2vr::dialogue_input_owner19c3 {
enum class Reason : unsigned { Unavailable, Missing, Disabled, Enabled };
inline const char* name(Reason r) noexcept {
    constexpr const char* names[]{"UNAVAILABLE","MISSING","DISABLED","ENABLED"};return names[unsigned(r)];
}
struct Result {bool known=false,enabled=false;Reason reason=Reason::Unavailable;
    std::uintptr_t manager=0,map=0;};
// Exact retail A10AF4/4535A4/3F7C40: manager+50 is the named-map
// RB tree, node+20 is its char* key, node+28 its map, nil byte+19.
// Enabled1A88910 reads map+9; ctorA1436C proves map+10 manager and
// map+58 name. No SDK virtual call, pooled-string construction, global
// Enable hook, worker polling or pointer retained beyond this callback.
// R156: the same verified walk for any named action map (the hold-to-look prompt: "infinite_hold_interaction_init" is
// enabled only while the game offers the prompt - lb_prompt_x_r153.h). wanted = the exact lowercase map name, <= 63 chars.
template<class Read> Result inspectMap(std::uintptr_t base,const char* wantedName,Read&& read) noexcept {
    Result out{};
    unsigned char wanted[64]{};unsigned wantedLen=0;
    if(!wantedName)return out;
    for(;wantedName[wantedLen]&&wantedLen<63;++wantedLen)wanted[wantedLen]=static_cast<unsigned char>(wantedName[wantedLen]);
    if(!wantedLen||wantedName[wantedLen])return out; // empty or over-long name: unknown
    wanted[wantedLen]=0;const unsigned wantedSize=wantedLen+1;
    std::uintptr_t cry=0,manager=0,vf=0,head=0,root=0,count=0;
    if(!base||!read(base+0x549D328,cry)||!cry||!read(cry,vf)||vf!=base+0x40472D0||
       !read(cry+0x530,manager)||!manager||!read(manager,vf)||vf!=base+0x4048018||
       !read(base+0x4048018+0x98,vf)||vf!=base+0xA10AF4||
       !read(base+0x4048530+0xD0,vf)||vf!=base+0x1A88910||
       !read(base+0x4048530+0xC8,vf)||vf!=base+0x85ADF4||
       !read(manager+0x50,head)||!head||!read(manager+0x58,count)||count>4096||
       !read(head+8,root)||!root)return out;
    unsigned char nil=0;if(!read(head+0x19,nil)||nil!=1)return out;
    struct Hop {std::uintptr_t node=0,parent=0,key=0,next=0;unsigned edge=0;};
    std::array<Hop,32> path{};unsigned used=0;std::uintptr_t node=root,parent=head,map=0,name=0;
    unsigned char enabled=0;
    const auto compare=[&](std::uintptr_t key,int& order) noexcept {
        if(!key)return false;order=0;
        for(unsigned i=0;i<wantedSize;++i){unsigned char ch=0;if(!read(key+i,ch))return false;
            if(ch!=wanted[i]){order=ch<wanted[i]?-1:1;return true;}
            if(!ch)return true;}
        return false;
    };
    while(node!=head){
        if(used>=path.size()||!count)return out;
        Hop h{};h.node=node;h.parent=parent;std::uintptr_t actualParent=0;
        if(!read(node+0x19,nil)||nil||!read(node+8,actualParent)||actualParent!=parent||
           !read(node+0x20,h.key))return out;
        int order=0;if(!compare(h.key,order))return out;
        if(!order){path[used++]=h;
            if(!read(node+0x28,map)||!map||!read(map,vf)||vf!=base+0x4048530||
               !read(map+0x10,actualParent)||actualParent!=manager||!read(map+0x58,name)||
               !compare(name,order)||order||!read(map+9,enabled)||enabled>1)return out;
            break;
        }
        h.edge=order<0?0x10u:0u;
        if(!read(node+h.edge,h.next)||!h.next||h.next==node)return out;
        path[used++]=h;parent=node;node=h.next;
    }
    // Revalidate the exact traversed ownership path, selected object and bytes.
    // A concurrent removal/replacement is UNKNOWN, never an old ENABLED lease.
    std::uintptr_t current=0,currentManager=0,currentHead=0,currentRoot=0,currentCount=0;
    if(!read(base+0x549D328,current)||current!=cry||!read(cry,vf)||vf!=base+0x40472D0||
       !read(cry+0x530,currentManager)||currentManager!=manager||!read(manager,vf)||vf!=base+0x4048018||
       !read(manager+0x50,currentHead)||currentHead!=head||!read(manager+0x58,currentCount)||currentCount!=count||
       !read(head+8,currentRoot)||currentRoot!=root||!read(head+0x19,nil)||nil!=1)return out;
    for(unsigned i=0;i<used;++i){const auto& h=path[i];std::uintptr_t key=0;
        if(!read(h.node+0x19,nil)||nil||!read(h.node+8,current)||current!=h.parent||
           !read(h.node+0x20,key)||key!=h.key)return out;
        int order=0;if(!compare(key,order))return out;
        if(h.next){if((order<0?0x10u:0u)!=h.edge||!order||!read(h.node+h.edge,current)||current!=h.next)return out;}
        else if(order||!read(h.node+0x28,current)||current!=map)return out;
    }
    if(map){unsigned char currentEnabled=0;int order=0;
        if(!read(map,vf)||vf!=base+0x4048530||!read(map+0x10,current)||current!=manager||
           !read(map+0x58,current)||current!=name||!compare(current,order)||order||
           !read(map+9,currentEnabled)||currentEnabled!=enabled)return out;
    }
    out.known=true;out.enabled=map&&enabled;out.manager=manager;out.map=map;
    out.reason=!map?Reason::Missing:enabled?Reason::Enabled:Reason::Disabled;return out;
}
// The exact native "dialog" input map (unchanged behaviour; R156 made the name a parameter above).
template<class Read> Result inspect(std::uintptr_t base,Read&& read) noexcept {return inspectMap(base,"dialog",read);}
}
