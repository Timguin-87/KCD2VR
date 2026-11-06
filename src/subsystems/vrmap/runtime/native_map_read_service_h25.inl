void ServiceNativeDocumentH25(const native_script_t2::Context& context) noexcept {
    namespace n=native_map_open_h25;
    static std::uint64_t serial=0,vm=0,due=0;static unsigned attempts=0;
    n::Ticket t{};if(!n::Current(context.now,t))return;
    if(t.serial!=serial){serial=t.serial;vm=context.generation;due=0;attempts=0;}
    if(vm!=context.generation){n::Finish(t.serial);return;} // never replay across a VM replacement
    if(context.now<due)return;due=context.now+250;
    if(++attempts>3){n::Finish(t.serial);return;}
    char packet[80]{},reply[192]{},expected[96]{};
    std::snprintf(packet,sizeof(packet),"N5|%llu",static_cast<unsigned long long>(t.serial));
    const bool called=context.CallPacket("kcd2vr_map_native_read_h25",packet,reply,sizeof(reply));
    std::snprintf(expected,sizeof(expected),"N5|%llu|WAIT_FOCUS",static_cast<unsigned long long>(t.serial));
    // Only an explicit NO-USE focus wait may be retried. A lost reply or native
    // request submission is terminal; never double-click again on uncertainty.
    if(!called||std::strcmp(reply,expected)!=0||attempts==3)n::Finish(t.serial);
    char accepted[96]{};std::snprintf(accepted,sizeof(accepted),"N5|%llu|NATIVE_READ_REQUESTED",static_cast<unsigned long long>(t.serial));
    if(called&&std::strcmp(reply,accepted)==0)native_map_live_h28::Request(t.serial,context.now,context.generation);
    char log[384]{};std::snprintf(log,sizeof(log),"H25 NATIVE_MAP_READ serial=%llu attempt=%u callback=%d receipt=%.180s animation=GAME_NATIVE_ITEM_READING result=REQUEST_ONLY",
        static_cast<unsigned long long>(t.serial),attempts,called,called?reply:"MISSING_OR_FAULTED_CALLBACK");diagnostic_trace::Write(log);
}
