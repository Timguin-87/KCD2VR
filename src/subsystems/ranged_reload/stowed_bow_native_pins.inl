static bool BowAttachPins59c15() noexcept {
    static int pinned=-1; if(pinned>=0)return pinned==1; const auto b=WhBase(); if(!b||!retail_build112::Ready(b))return false;
    constexpr unsigned char find[16]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x48};
    constexpr unsigned char hide[16]{0x44,0x8B,0x41,0x08,0x41,0x8B,0xC0,0x25,0xFF,0xFF,0xF8,0xFF,0x41,0x81,0xC8,0x00};
    unsigned char a[16]{},c[16]{};
    pinned=(Read(reinterpret_cast<const void*>(b+0x971B2C),a)&&!std::memcmp(a,find,16)&&Read(reinterpret_cast<const void*>(b+0x5AF5E8),c)&&!std::memcmp(c,hide,16))?1:0;
    diagnostic_trace::Write(pinned?"R59.16C10R15 STOWED_BOW attachment API pinned (GetInterfaceByName 0x971B2C, HideAttachment 0x5AF5E8)":"R59.16C10R15 STOWED_BOW attachment API pin MISMATCH: bow hide off");
    return pinned==1;
}
