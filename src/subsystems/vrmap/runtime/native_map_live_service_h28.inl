void ServiceNativeLiveH28(const native_script_t2::Context& context) noexcept {
    namespace live=native_map_live_h28;
    static std::uint64_t due=0,serial=0,vm=0,first=0,lastLog=0;static bool succeeded=false;
    live::Lease lease{};
    const auto& w=activity_input163::witness175;
    const bool current=live::Current(context.now,context.generation,lease)&&w.ready.load(std::memory_order_acquire)&&
        !w.transitionDepth175.load(std::memory_order_acquire)&&w.lifetime175.load(std::memory_order_acquire)==lease.lifetime&&
        w.active[unsigned(activity_input163::Kind175::Reading)].load(std::memory_order_acquire)==lease.reading;
    if(!current){
        if(live::until.exchange(0)||live::clear.exchange(false)){
            live::clear.store(false);char reply[80]{};(void)context.CallPacket("kcd2vr_map_native_live_h28","OFF",reply,sizeof(reply));
        }return;
    }
    if(serial!=lease.serial||vm!=context.generation){serial=lease.serial;vm=context.generation;due=0;first=context.now;succeeded=false;}
    if(context.now<due)return;due=context.now+500;live::clear.store(false);
    NativeSnapshot native{};Reader read;const bool valid=Capture(read,base,native);
    float waypoint[2]{};const bool waypointKnown=valid&&Flag(native.apse,waypoint)&&(waypoint[0]!=0||waypoint[1]!=0);
    char packet[16384]{},reply[1024]{};Snapshot result{};
    const bool called=Encode(native,valid,waypointKnown,waypoint,lease.lifetime,packet)&&
        context.CallPacket("kcd2vr_map_native_live_h28",packet,reply,sizeof(reply));
    const bool player=called&&PlayerPacket(reply,result);
    succeeded=succeeded||player;
    if(!succeeded&&context.now>=first&&context.now-first>=5000){live::Finish(lease.serial);live::until.store(0);live::clear.store(true);}
    if(!lastLog||context.now-lastLog>=5000){lastLog=context.now;char m[320]{};
        std::snprintf(m,sizeof(m),"H28 NATIVE_MAP_LIVE callback=%d player=%d nativeObjectives=%u waypoint=%d receipt=%.120s renderer=NATIVE_GENERALBOOK_INSTANCE_EVENT hzMax=2 minimapDLL=NONE headset=UNVERIFIED",
            int(called),int(player),valid?native.count:0,int(waypointKnown),called?reply:"MISSING_OR_FAULTED_LIVE_MODULE");diagnostic_trace::Write(m);}
}
