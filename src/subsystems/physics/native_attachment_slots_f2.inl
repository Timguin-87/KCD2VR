// Included in the existing private native-held owner namespace; no executor.
// Retail1085234 indexes model+2C+i*24 and the six names at40AC288+i*8 with
// the SAME native slot index. It is not the VR hand/weapon-class side index.
struct NativeHeldSlotF2 {
    unsigned index=6;
    std::uintptr_t name=0;
    friend bool operator==(const NativeHeldSlotF2& a,const NativeHeldSlotF2& b) noexcept {
        return a.index==b.index&&a.name==b.name;
    }
};
template<class ReadSlot> static bool ReadNativeHeldSlotF2(std::uintptr_t base,std::uintptr_t model,
        std::uint32_t entity,ReadSlot&& read,NativeHeldSlotF2& out) noexcept {
    out={};
    if(!base||!model||!entity||model>~std::uintptr_t(0)-0xE4||base>~std::uintptr_t(0)-0x40AC2B8)return false;
    unsigned found=6;
    for(unsigned slot=0;slot<6;++slot){
        std::uint32_t id=0;
        if(!read(model+0x2C+slot*0x24,id))return false;
        if(id==entity){if(found!=6)return false;found=slot;}
    }
    std::uintptr_t name=0;
    if(found==6||!read(base+0x40AC288+found*8,name)||!name)return false;
    out={found,name};return true;
}
