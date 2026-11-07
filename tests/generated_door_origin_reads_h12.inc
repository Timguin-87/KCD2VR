// Generated exact bounded production reader/policy helpers.
static bool SelectionCopy68(std::uintptr_t address,void* out,std::size_t n) noexcept {
    if(!address||!out||!n)return false;
    __try {std::memcpy(out,reinterpret_cast<const void*>(address),n);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T>static bool SelectionRead68(std::uintptr_t address,T& out) noexcept {
    return SelectionCopy68(address,&out,sizeof(out));
}
static bool SelectionVft68(std::uintptr_t p,std::uintptr_t rva) noexcept {
    std::uintptr_t vf=0;return g_wh_base&&SelectionRead68(p,vf)&&vf==g_wh_base+rva;
}
// Map layout is the native 16-byte key/pointer vector read by63D504:
// interactor+D8=data, +E0=count. This never calls the lookup or inserts a key.
static bool SelectionPolicy68(std::uintptr_t interactor,std::uintptr_t player,
                              std::uintptr_t& policy) noexcept {
    policy=0;std::uintptr_t back=0,data=0,count=0;
    if(!SelectionVft68(interactor,0x3A6A0C8)||!SelectionRead68(interactor+8,back)||back!=player||
       !SelectionVft68(player,0x3AA4B98)||!SelectionRead68(player+0xB08,back)||back!=interactor||
       !SelectionRead68(interactor+0xD8,data)||!SelectionRead68(interactor+0xE0,count)||!data||!count||count>16)return false;
    // Supported retail constructor installs exactly object1/dog2/chat4. The
    // bounded scan tolerates additional native entries but never reads a world.
    for(std::uintptr_t i=0;i<count;++i){unsigned key=0;std::uintptr_t p=0;
        if(!SelectionRead68(data+16*i,key))return false;
        if(key!=1)continue;
        if(policy||!SelectionRead68(data+16*i+8,p)||!SelectionVft68(p,0x3A69D90)||
           !SelectionRead68(p+8,back)||back!=player)return false;
        policy=p;
    }
    return policy!=0;
}
