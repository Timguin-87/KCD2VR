namespace services59c5 {
void Install() noexcept {
    constexpr unsigned char raisedEntry[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57};
    const bool raisedOk=Hook(raisedHook,raised,0xC5BA24,raisedEntry,&Raised,"kcd2vr_r59c5_raised_observe");
    constexpr unsigned char commitEntry[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7C,0x24,0x20,0x55};
    const bool commitOk=Hook(commitHook,commit,0xC64B18,commitEntry,&Commit,"kcd2vr_r59c5_commit_observe");
    constexpr unsigned char effectEntry[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x48,0x08,0x57};
    const bool effectOk=Hook(effectHook,effect,0xC66334,effectEntry,&Effect,"kcd2vr_r59c5_effect_observe");
    constexpr unsigned char counterEntry[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x55};
    const bool counterOk=Hook(counterHook,counter,0x1483580,counterEntry,&Counter,"kcd2vr_r59c5_counter_observe");
    char line[240]{};std::snprintf(line,sizeof(line),"R59C5 NATIVE_SERVICE_INSTALL raised=%d commit=%d effect=%d counter=%d exactRetail=1 observationOnly=1",raisedOk,commitOk,effectOk,counterOk);diagnostic_trace::Write(line);
}
void Uninstall() noexcept {
    for(auto* h:{&raisedHook,&commitHook,&effectHook,&counterHook})if(*h){(void)(*h)->disable();h->reset();}
    raised=nullptr;commit=nullptr;effect=nullptr;counter=nullptr;
}
}
