static bool ArmFistReceiverH12(std::uintptr_t base) noexcept {
    if(g_fist_receiver_hook_h12)return FistReceiverOriginalH12()!=nullptr;
    if(!base||base!=g_base||!retail_build112::Ready(base)||g_fist_receiver_uncertain_h12)return false;
    unsigned char entry[sizeof(kFistReceiverEntryH12)]{};
    if(!SehCopy146(reinterpret_cast<const void*>(base+kFistReceiverH12),entry,sizeof(entry))||
       std::memcmp(entry,kFistReceiverEntryH12,sizeof(entry)))return false;
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    auto hook=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{"kcd2vr_h12_fist_receiver_ancestry",
        DetourModKit::hook::Target{DetourModKit::Address{base+kFistReceiverH12}},options},&FistReceiverDetourH12);
    if(!hook)return false;
    g_fist_receiver_hook_h12=std::move(*hook);
    g_fist_receiver_original_h12=g_fist_receiver_hook_h12->original<FistReceiverFnH12>();
    g_fist_receiver_base_h12=base;
    if(!g_fist_receiver_original_h12){(void)ReleaseFistReceiverH12();return false;}
    const bool enabled=bool(g_fist_receiver_hook_h12->enable());
    if(!enabled||!g_fist_receiver_hook_h12->is_enabled()||
       !SehCopy146(reinterpret_cast<const void*>(base+kFistReceiverH12),g_fist_receiver_owned_h12.data(),g_fist_receiver_owned_h12.size())){
        (void)ReleaseFistReceiverH12();return false;
    }
    return FistReceiverOriginalH12()!=nullptr;
}
