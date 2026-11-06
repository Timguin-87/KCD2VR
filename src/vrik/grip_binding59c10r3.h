#pragma once
#include <cstring>
#include <cmath>
#include <cstdint>

namespace kcd2vr::grip_binding59c10r3 {
// A binding is immutable geometry/topology, not callback freshness. A newer
// callback for the same binding must preserve learned grips. The native adapter
// already validates these copied fields against the exact retail contract.
template<class Socket>
inline bool same(const Socket& a,const Socket& b) noexcept {
    return a.valid&&b.valid&&a.key==b.key&&a.generation==b.generation&&
        a.character==b.character&&a.joint==b.joint&&
        !std::memcmp(&a.relative,&b.relative,sizeof(a.relative))&&
        !std::memcmp(a.scale,b.scale,sizeof(a.scale));
}

// Called only for a held item with a qualified coherent socket. Mount sampling
// while stowed does not replace the binding from which a grip was learned.
template<class Socket,class Grip>
inline bool qualify(Grip& grip,const Socket& current) noexcept {
    if(same(grip.captureBinding,current))return false;
    grip.captureBinding=current;
    for(int side=0;side<2;++side){
        grip.ready[side]=false;grip.stable[side]=0;grip.candidate[side]={};
    }
    return true;
}
// Exact accepted bow redraw relation. Callback freshness does not grant reuse;
// current native wrist/socket relation must still equal the accepted local grip.
template<class Socket,class Grip>
inline bool reuseBowD70(const Grip& grip,const Socket& current,std::uint64_t epoch,
    const float* scale,int side,double positionError,double angleError) noexcept {
    return side>=0&&side<2&&scale&&epoch&&grip.heldReadyR27[side]&&
        grip.heldBindingEpochR27==epoch&&same(grip.heldBindingR27,current)&&
        !std::memcmp(grip.heldScaleR27,scale,sizeof(grip.heldScaleR27))&&
        std::isfinite(positionError)&&positionError>=0&&positionError<1e-5&&
        std::isfinite(angleError)&&angleError>=0&&angleError<1e-4;
}

}
