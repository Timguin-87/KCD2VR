// Generated EXACT native start typedefs, inflight scope and original-first wrapper.
using XR34StartFn=bool(__fastcall*)(void*,void*);
using XR34MetadataFn=void(__fastcall*)(void*,void*,int,int);
using XR34StopFn=void(__fastcall*)(void*);

static XR34StartFn g_crossbow_start_xr34=nullptr;
static std::atomic<unsigned> g_crossbow_inflight_xr34{0};
static bool XR34StartBody(void* parent,void* originalStartInput) {
    const bool result=g_crossbow_start_xr34?g_crossbow_start_xr34(parent,originalStartInput):false;
    {OwnerTransaction19c4 tx;if(tx)XR34PostStartNative(parent,result);}return result;
}
static bool __fastcall XR34StartDetour(void* parent,void* originalStartInput) {
    g_crossbow_inflight_xr34.fetch_add(1,std::memory_order_acq_rel);bool result=false;
#if defined(_MSC_VER)
    __try{result=XR34StartBody(parent,originalStartInput);}
    __finally{g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);}
#else
    try{result=XR34StartBody(parent,originalStartInput);}
    catch(...){g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);throw;}
    g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);
#endif
    return result;
}
