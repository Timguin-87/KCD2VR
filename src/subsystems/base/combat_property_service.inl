static int g_verified=-1;
static bool Verified() noexcept {
    if(g_verified<0)g_verified=(EntryIs596(kSetInt,kSetIntEntry,sizeof(kSetIntEntry))&&EntryIs596(kSetBool,kSetBoolEntry,sizeof(kSetBoolEntry))&&
        EntryIs596(kSetIntZone59c10,kSetIntZoneEntry59c10,sizeof(kSetIntZoneEntry59c10))&&
        EntryIs596(kSetFloat,kSetFloatEntry,sizeof(kSetFloatEntry))&&
        EntryIs596(kResolveDefense,kResolveDefenseEntry,sizeof(kResolveDefenseEntry))&&
        EntryIs596(kSetDefense,kSetDefenseEntry,sizeof(kSetDefenseEntry)))?1:0;
    return g_verified==1;
}
static std::atomic<bool> g_setter_exception{false}; // R59.16C7A: an exception inside a native setter = latched fault
enum class SetReadback { Success,ReadableMismatch,Unreadable,Exception,Rejected };
struct SetReceipt {SetReadback result=SetReadback::Rejected;std::uint32_t prop=0;double expected=0,observed=0;};
static SetReceipt g_last_set; // exact game-thread setter result; defaults are never interpreted as a native override
static bool SetInt(const void* hs,std::uint32_t prop,int v) noexcept {using Fn=void(__fastcall*)(const void*,int);
    g_last_set={SetReadback::Rejected,prop,double(v),0};
    const auto setter=(prop==kActionPhaseProp39||prop==kBlockingHand)?kSetInt:kSetIntZone59c10; // R59.16C10R4A: native setter per property (S31)
    __try{reinterpret_cast<Fn>(g_base+setter)(static_cast<const unsigned char*>(hs)+prop,v);int actual=0;
        if(!SehCopy146(static_cast<const unsigned char*>(hs)+prop+8,&actual,sizeof(actual))){g_last_set.result=SetReadback::Unreadable;return false;}
        g_last_set.observed=actual;g_last_set.result=actual==v?SetReadback::Success:SetReadback::ReadableMismatch;return actual==v;
    }__except(EXCEPTION_EXECUTE_HANDLER){g_setter_exception.store(true);g_last_set.result=SetReadback::Exception;return false;}}
static bool SetFloat(const void* hs,std::uint32_t prop,float v) noexcept {using Fn=void(__fastcall*)(const void*,float);
    g_last_set={SetReadback::Rejected,prop,double(v),0};
    __try{reinterpret_cast<Fn>(g_base+kSetFloat)(static_cast<const unsigned char*>(hs)+prop,v);float actual=0;
        if(!SehCopy146(static_cast<const unsigned char*>(hs)+prop+8,&actual,sizeof(actual))||!std::isfinite(actual)){g_last_set.result=SetReadback::Unreadable;return false;}
        g_last_set.observed=actual;g_last_set.result=actual==v?SetReadback::Success:SetReadback::ReadableMismatch;return actual==v;
    }__except(EXCEPTION_EXECUTE_HANDLER){g_setter_exception.store(true);g_last_set.result=SetReadback::Exception;return false;}}
// R59.16C10R17: the native bool property setter (72FA34: compares +8, stores, notifies) - used for Henry's hit flag +0x480 and the scoped admitted-intent input +0x5A0.
static bool SetBool(const void* hs,std::uint32_t prop,bool v) noexcept {using Fn=void(__fastcall*)(const void*,bool);
    g_last_set={SetReadback::Rejected,prop,double(v),0};
    if(!hs||(prop!=0x480&&prop!=0x5A0))return false;
    __try{reinterpret_cast<Fn>(g_base+kSetBool)(static_cast<const unsigned char*>(hs)+prop,v);unsigned char actual=0;
        if(!SehCopy146(static_cast<const unsigned char*>(hs)+prop+8,&actual,sizeof(actual))){g_last_set.result=SetReadback::Unreadable;return false;}
        g_last_set.observed=actual;g_last_set.result=(actual!=0)==v?SetReadback::Success:SetReadback::ReadableMismatch;return (actual!=0)==v;
    }__except(EXCEPTION_EXECUTE_HANDLER){g_setter_exception.store(true);g_last_set.result=SetReadback::Exception;return false;}}
