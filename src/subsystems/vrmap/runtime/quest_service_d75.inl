// Bounded read-only native compass -> existing fast world paper. It does not
// open GeneralBook, refresh a native UI, or impersonate a Reading action lease.
void ServiceQuestsD75(const native_script_t2::Context& context) noexcept {
 static std::uint64_t due=0,serial=0,vm=0,capturedAck=0;
 const auto now=context.now;const auto intent=held_map_m1::RendererIntent43();
 if(intent.native||!held_map_m1::Requested()||!held_map_m1::Allowed(now)||!ready.load()||!context.Current())return;
 if(serial!=intent.serial||vm!=context.generation){serial=intent.serial;vm=context.generation;due=0;}
 if(now<due)return;due=now+500;
 ReadinessAck ack{};
 if(!readinessAck.read(ack)||ack.native||ack.activation!=serial||ack.vm!=vm||!ack.actor||!ack.helper||!ack.epoch||
    ack.serial<=missingSerial.load()||!held_map_m1::RendererIntentMatches43(intent))return;
 // A readiness receipt changes on reopen, VM/body/helper recovery and artwork change.
 // Keep ink static for that receipt; failed/hidden callbacks still retry.
 if(capturedAck==ack.serial)return;
 NativeSnapshot native{};Reader read;const bool valid=Capture(read,base,native);
 float xy[2]{};const bool waypoint=valid&&Flag(native.apse,xy)&&(xy[0]!=0||xy[1]!=0);
 // Read-only leaves may fail; explicitly clear current objectives instead of
 // retaining the previous quest/level. Current ownership is checked by Lua too.
 if(!context.Current()||!held_map_m1::RendererIntentMatches43(intent))return;
 char packet[4096]{},reply[128]{};
 const bool called=EncodeQuestD75(native,valid,waypoint,xy,serial,ack.epoch,ack.actor,ack.helper,packet)&&
  context.CallPacket("kcd2vr_map_quests_d75",packet,reply,sizeof(reply));
 ReadinessAck after{};
 if(called&&valid&&native.compassKnown&&std::strncmp(reply,"Q75|LIVE|",9)==0&&
    context.Current()&&held_map_m1::RendererIntentMatches43(intent)&&
    readinessAck.read(after)&&after.serial==ack.serial&&ack.serial>missingSerial.load())capturedAck=ack.serial;
 if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
  static std::uint64_t logDue=0;if(now>=logDue){logDue=now+2000;char line[256]{};
   std::snprintf(line,sizeof(line),"D75 MAP_QUESTS actor=%u helper=%u nativeKnown=%d entries=%u waypoint=%d reply=%.96s retryHzMax=2 refresh=ON_OPEN source=COMPASS_NOT_ALL_WORLD_QUESTS",
    ack.actor,ack.helper,int(valid&&native.compassKnown),valid?native.count:0,int(waypoint),called?reply:"UNAVAILABLE");diagnostic_trace::Write(line);
  }
 }
}
