#include "subsystems/vrmap/runtime/held_map_m1.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::held_map_m1;
int main() {
 assert(!NativePaperSelectedM43());
 const auto initial=activation.load();
 SetNativePaperM43(false);assert(activation.load()==initial&&!Requested()&&!CloseTicket31());
 SetNativePaperM43(true);assert(NativePaperSelectedM43()&&!Requested()&&!Selected()&&!CloseTicket31());
 SetNativePaperM43(false);assert(!NativePaperSelectedM43()&&!Requested());
 Enable(true);PublishAllowed(true,100);SetRequested31(true,100);
 const auto oldOpen=activation.load();
 SetNativePaperM43(true);const auto nativeClose=activation.load();
 assert(nativeClose==oldOpen+1&&Selected()&&!Requested()&&NativePaperSelectedM43());
 assert(CloseTicket31()==nativeClose&&Allowed(100));
 SetNativePaperM43(true);assert(activation.load()==nativeClose&&CloseTicket31()==nativeClose);
 AckClosed31(oldOpen);assert(CloseTicket31()==nativeClose);
 assert(!FinishClose31(oldOpen,true,true)&&CloseTicket31()==nativeClose);
 SetNativePaperM43(false);const auto oldClose=activation.load();
 assert(!NativePaperSelectedM43()&&!Requested()&&Selected()&&oldClose==nativeClose+1);
 assert(CloseTicket31()==oldClose);
 AckClosed31(nativeClose);assert(CloseTicket31()==oldClose);
 assert(FinishClose31(oldClose,true,true)&&!CloseTicket31()&&closedAt31.load()==oldClose);
 SetRequested31(true,120);const auto nextOpen=activation.load();
 assert(Requested()&&!NativePaperSelectedM43()&&nextOpen>oldClose);
 AckClosed31(oldClose);assert(Requested()&&activation.load()==nextOpen);
 SetNativePaperM43(false);assert(Requested()&&activation.load()==nextOpen);
 std::puts("PASS map renderer preference: default OFF, request-only close, no auto-open, no-op reapply, stale ACK isolation, cleanup ticket retention, existing selection/context preserved");
}