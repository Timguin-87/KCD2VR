#pragma once
#include <cstdint>
#include <initializer_list>
namespace kcd2vr::dialogue_original43 {
struct Pair {std::uintptr_t original=0,twin=0;};
// Called synchronously AFTER the stock creator returns its owned actor. Never
// put Henry into a twin ownership/deletion list or replace a gameplay actor.
template<class Read> bool pair(std::uintptr_t b,std::uintptr_t caller,bool immersive,
    bool created,std::uintptr_t actor,std::uintptr_t twin,Read&& read,Pair& out)noexcept {
    out={};if(!b||!immersive||!created||!actor||!twin||actor==twin||
        (caller!=b+0xC89C1B&&caller!=b+0xC89C47))return false;
    std::uintptr_t vf=0,combat=0,back=0,original=0,copy=0,hide=0;
    if(!read(actor,vf)||vf!=b+0x3AA4B98||!read(actor+0x40,vf)||vf!=b+0x3A81028||
       !read(actor+0x278,combat)||!combat||!read(combat,vf)||vf!=b+0x3A80610||
       !read(combat+0x1F8,back)||back!=actor||!read(actor+0x38,original)||!original||
       !read(twin+0x38,copy)||!copy||copy==original)return false;
    for(auto entity:{original,copy}){
        if(!read(entity,vf)||vf!=b+0x4072C78||!read(vf+0x208,hide)||hide!=b+0x9DFC5C)return false;
    }
    out={original,copy};return true;
}
}
