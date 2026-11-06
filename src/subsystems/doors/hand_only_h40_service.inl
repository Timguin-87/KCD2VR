// Included at file scope in mod.cpp. Native calls remain solely inside the
// existing verified original-first script update Context.
namespace kcd2vr::hand_only_door_h40 {
static std::uint64_t NativeClockR96() noexcept {return std::uint64_t(GetTickCount64());}
static bool WantedNativeH40() noexcept {return Wanted(std::uint64_t(GetTickCount64()));}
static void ServiceNativeH40(const native_script_t2::Context&ctx) noexcept {
 struct IO {const native_script_t2::Context&context;
  bool call(const char*in,char*out,std::size_t cap) noexcept {return context.CallPacket(kCallback,in,out,cap);}
  bool display(const char*in,char*out,std::size_t cap) noexcept {return context.CallPacket("kcd2vr_door_display_h40_t1",in,out,cap);}} io{ctx};
 service.tick(io,ctx.now,ctx.generation);
}
static long long pressAgeR9(int s,std::uint64_t now) noexcept {const auto t=door_press_why_r9::lastTick[s].load();return t&&now>=t?(long long)(now-t):-1;}
static void Diagnostic(std::uint64_t now) noexcept {
 namespace cap=diagnostics::capture168;
 const bool interaction=cap::active(cap::Domain::Interaction,now);
 if(!interaction&&!cap::active(cap::Domain::Bug,now))return;
 static std::uint64_t displaySession=0,displayDue=0;
 const auto domain=interaction?cap::Domain::Interaction:cap::Domain::Bug;
 const auto session=cap::gate.session[unsigned(domain)].load(std::memory_order_acquire);
 const auto captureKey=(session<<1)|(interaction?1u:0u);
 if(interaction&&(session!=displaySession||now>=displayDue)){
  displaySession=session;displayDue=now+500;Display d{};const bool copied=display.read(d);
  char point[1280]{};std::snprintf(point,sizeof(point),
   "H40 F5_DOOR_DISPLAY session=%llu enabled=%d copied=%d fresh=%d tick=%llu ageMs=%llu command=%llu player=%u body=%llu request=%llu tracking=%llu policy=%llu mask=%u R{door=%u claim=%llu point=(%.9g,%.9g,%.9g) phase=%.9g} L{door=%u claim=%llu point=(%.9g,%.9g,%.9g) phase=%.9g} source=ACTUAL_NATIVE_CLIP_READBACK grabbedPoint=NOT_MESH_HANDLE consumerApplied=UNCLAIMED writes=NONE",
   (unsigned long long)session,int(displayEnabled.load()),int(copied),int(copied&&Fresh(d.tick,now)),(unsigned long long)d.tick,
   (unsigned long long)(d.tick&&now>=d.tick?now-d.tick:UINT64_MAX),(unsigned long long)d.serial,d.source.player,
   (unsigned long long)d.source.body,(unsigned long long)d.source.request,(unsigned long long)d.source.tracking,(unsigned long long)d.source.policy,d.mask,
   d.door[0],(unsigned long long)d.token[0],d.point[0][0],d.point[0][1],d.point[0][2],d.phase[0],
   d.door[1],(unsigned long long)d.token[1],d.point[1][0],d.point[1][1],d.point[1][2],d.phase[1]);
  diagnostic_trace::Write(point);
 }
 static std::uint64_t lastSession=0;static unsigned lines=0;static Receipt last{};Receipt r{};if(!receipts.read(r))return;
 const bool newSession=captureKey!=lastSession;
 if(newSession){lastSession=captureKey;lines=0;}
 if(!newSession&&r.stage==last.stage&&r.reason==last.reason&&r.fault==last.fault&&r.reserved==last.reserved&&r.mayOwn==last.mayOwn&&r.acquired==last.acquired&&r.retiring==last.retiring&&r.resting==last.resting&&r.doors[0]==last.doors[0]&&r.doors[1]==last.doors[1])return;
 last=r;if(lines>=96)return;++lines;char line[1152]{};
 std::snprintf(line,sizeof(line),"H40 F5_HAND_ONLY_DOOR capture=%s session=%llu stage=%s reason=%s fault=%s player=%u originalBody=%llu originalRequest=%llu originalSample=%llu originalSourceTick=%llu command=%llu reservedMask=%u mayOwnMask=%u acquiredMask=%u retiringMask=%u restNativeCount=%u doorR=%u doorL=%u VMquarantine=%d semantic=SCRIPT_PENDING_OR_OWNED_NOT_VISUAL_SUCCESS handWhyR=%u handWhyL=%u pressR=%s pressL=%s pressAgeMsR=%lld pressAgeMsL=%lld claimedRL=%llu/%llu refusedRL=%llu/%llu budget=%u/96%s",
  interaction?"F5":"F2",(unsigned long long)session,Name(r.stage),Name(r.reason),Name(r.fault),r.player,(unsigned long long)r.body,(unsigned long long)r.request,(unsigned long long)r.sample,
  (unsigned long long)r.sourceTick,(unsigned long long)r.command,r.reserved,r.mayOwn,r.acquired,r.retiring,r.resting,r.doors[0],r.doors[1],int(vmQuarantine.load()),
  currentWhy[0].load(std::memory_order_relaxed),currentWhy[1].load(std::memory_order_relaxed),
  door_press_why_r9::name(door_press_why_r9::last[0].load()),door_press_why_r9::name(door_press_why_r9::last[1].load()),
  pressAgeR9(0,now),pressAgeR9(1,now),
  (unsigned long long)door_press_why_r9::claimedCount[0].load(),(unsigned long long)door_press_why_r9::claimedCount[1].load(),
  (unsigned long long)door_press_why_r9::refused[0].load(),(unsigned long long)door_press_why_r9::refused[1].load(),lines,lines==96?" detailBudgetExhausted=1":"");
 diagnostic_trace::Write(line);
}
bool Install() noexcept {clockR96.store(&NativeClockR96,std::memory_order_release);const bool ok=native_script_t2::Register(&ServiceNativeH40,&WantedNativeH40)&&native_script_t2::Install();transportInstalled.store(ok,std::memory_order_release);return ok;}
}
