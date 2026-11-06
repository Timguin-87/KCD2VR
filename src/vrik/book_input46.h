#pragma once
#include "../activity_input163.h"
#include "../core/book_pages45.h"

namespace kcd2vr::book_input46 {
struct Identity {
    int book=book_pages45::None;
    std::uint64_t lifetime=0;
    std::uintptr_t reading=0;
    activity_input163::AlchemyBook45 alchemy{};
};

// The native activity/read-enable callbacks may change between recognizing a
// stroke and publishing this pad frame. Rejoin the current identity immediately
// before copying an automatic shoulder pulse; a cached 80 ms pulse is no lease.
inline bool current(const Identity& id,std::uint64_t now) noexcept {
    namespace ai=activity_input163;
    const auto& w=ai::witness175;
    if(!id.lifetime||!w.ready.load(std::memory_order_acquire)||
       w.transitionDepth175.load(std::memory_order_acquire)||
       id.lifetime!=w.lifetime175.load(std::memory_order_acquire))return false;
    if(id.book==book_pages45::Reading){
        if(!id.reading||id.reading!=w.active[unsigned(ai::Kind175::Reading)].load(std::memory_order_acquire))return false;
    }else if(id.book==book_pages45::Alchemy){
        ai::AlchemyBook45 live{};
        if(!ai::AlchemyBookOpen45(now,live)||live.lifetime!=id.lifetime||
           live.instance!=id.alchemy.instance||live.action!=id.alchemy.action||
           live.lifetime!=id.alchemy.lifetime||live.generation!=id.alchemy.generation||
           live.entity!=id.alchemy.entity)return false;
    }else return false;
    return w.ready.load(std::memory_order_acquire)&&
        !w.transitionDepth175.load(std::memory_order_acquire)&&
        id.lifetime==w.lifetime175.load(std::memory_order_acquire);
}

template<class Pad>
inline bool apply(const Identity& id,int mode,const book_pages45::Result& result,
                  std::uint64_t now,Pad& pad) noexcept {
    if(!current(id,now)||mode!=book_pages45::clampMode(book_pages45::mode.load(std::memory_order_acquire)))return false;
    book_pages45::apply(id.book,mode,result,pad);
    return true;
}
}
