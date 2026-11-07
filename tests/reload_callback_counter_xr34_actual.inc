// Exact current native typedefs, actual OwnerTransaction and complete three callback wrappers; explicit original/post leaves only.
static std::atomic_flag g_owner_transaction19c4=ATOMIC_FLAG_INIT;
static thread_local unsigned g_owner_transaction_depth19c4=0;
struct OwnerTransaction19c4 {
    bool acquired=false;
    OwnerTransaction19c4() noexcept {
        if(g_owner_transaction_depth19c4){++g_owner_transaction_depth19c4;acquired=true;}
        else if(!g_owner_transaction19c4.test_and_set(std::memory_order_acquire)){g_owner_transaction_depth19c4=1;acquired=true;}
    }
    ~OwnerTransaction19c4(){if(acquired&&!--g_owner_transaction_depth19c4)g_owner_transaction19c4.clear(std::memory_order_release);}
    explicit operator bool() const noexcept {return acquired;}
};

using XR34StartFn=bool(__fastcall*)(void*,void*);
using XR34MetadataFn=void(__fastcall*)(void*,void*,int,int);
using XR34StopFn=void(__fastcall*)(void*);
static XR34StartFn g_crossbow_start_xr34=nullptr;
static XR34MetadataFn g_crossbow_metadata_xr34=nullptr;
static XR34StopFn g_crossbow_stop_xr34=nullptr;

static std::atomic<unsigned> g_crossbow_inflight_xr34{0};
#if defined(_MSC_VER)
#pragma push_macro("__try")
#undef __try
#endif
// C++ transaction locals live in small bodies, outside the SEH-finally wrapper.
// The same existing counter MUST be released even if an original native entry
// raises an SEH exception that the game catches farther up under /EHsc.
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
static void XR34MetadataBody(void* parent,void* child,int animation,int scope) {
    if(g_crossbow_metadata_xr34)g_crossbow_metadata_xr34(parent,child,animation,scope);
    OwnerTransaction19c4 tx;if(tx)XR34PostMetadataNative(parent,child,animation,scope);
}
static void __fastcall XR34MetadataDetour(void* parent,void* child,int animation,int scope) {
    g_crossbow_inflight_xr34.fetch_add(1,std::memory_order_acq_rel);
#if defined(_MSC_VER)
    __try{XR34MetadataBody(parent,child,animation,scope);}
    __finally{g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);}
#else
    try{XR34MetadataBody(parent,child,animation,scope);}
    catch(...){g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);throw;}
    g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);
#endif
}
static void XR34StopBody(void* parent) {
    {OwnerTransaction19c4 tx;if(tx)XR34BeforeStopNative(parent);else xr34::RevokeNativeParent(parent);}
    if(g_crossbow_stop_xr34)g_crossbow_stop_xr34(parent);
}
static void __fastcall XR34StopDetour(void* parent) {
    g_crossbow_inflight_xr34.fetch_add(1,std::memory_order_acq_rel);
#if defined(_MSC_VER)
    __try{XR34StopBody(parent);}
    __finally{g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);}
#else
    try{XR34StopBody(parent);}
    catch(...){g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);throw;}
    g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);
#endif
}
#if defined(_MSC_VER)
#pragma pop_macro("__try")
#endif
