#pragma once
#include <array>
#include <cstdint>
#include <cstring>
// Read-only attribution of our own committed x64 detour, never a patch writer.
namespace kcd2vr::owned_inline_d65 {
struct Node {std::uintptr_t address=0;std::array<unsigned char,16> bytes{};unsigned size=0;};
struct Route {
    std::array<Node,4> nodes{};unsigned count=0;bool valid=false;
    template<class Read> bool capture(std::uintptr_t target,std::uintptr_t detour,Read&& read) noexcept {
        *this={};if(!target||!detour)return false;
        auto add=[&](std::uintptr_t p,unsigned n)->const Node* {
            if(count>=nodes.size()||n>16)return nullptr;
            auto& x=nodes[count];x.address=p;x.size=n;
            if(!read(p,x.bytes.data(),n))return nullptr;++count;return &x;
        };
        auto pc=target;
        for(unsigned depth=0;depth<2;++depth){
            // Witness only the decoded jump's own bytes. A SafetyHook relay is
            // FF25 + disp32 + pointer (14 bytes); recording16 also captures the
            // next allocation, which later hooks may legitimately populate.
            unsigned char opcode[2]{};if(!read(pc,opcode,sizeof(opcode)))return false;
            const unsigned size=opcode[0]==0xe9?5:
                opcode[0]==0xff&&opcode[1]==0x25?6:
                opcode[0]==0x48&&opcode[1]==0xb8?12:0;
            if(!size)return false;
            const auto* n=add(pc,size);if(!n)return false;const auto& b=n->bytes;
            std::uintptr_t next=0;
            if(b[0]==0xe9){std::int32_t d=0;std::memcpy(&d,b.data()+1,4);next=pc+5+d;}
            else if(b[0]==0xff&&b[1]==0x25){std::int32_t d=0;std::memcpy(&d,b.data()+2,4);
                const auto* ptr=add(pc+6+d,8);if(!ptr)return false;std::memcpy(&next,ptr->bytes.data(),8);}
            else if(b[0]==0x48&&b[1]==0xb8&&b[10]==0xff&&b[11]==0xe0)std::memcpy(&next,b.data()+2,8);
            else return false;
            if(next==detour){valid=true;return true;}if(!next||next==pc)return false;pc=next;
        }
        return false;
    }
    template<class Read> bool current(Read&& read) const noexcept {
        if(!valid||!count)return false;
        for(unsigned i=0;i<count;++i){unsigned char b[16]{};const auto& n=nodes[i];
            if(!read(n.address,b,n.size)||std::memcmp(b,n.bytes.data(),n.size))return false;}
        return true;
    }
};
}
