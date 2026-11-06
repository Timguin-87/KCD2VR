#include "temporal_pair_watchdog_d85.h"
#include "temporal_capture_transaction_d85.h"
#include <cstdio>
#include <cstring>
using namespace kcd2vr;
static unsigned checks=0,failures=0;
#define CHECK(x) do {++checks;if(!(x)){++failures;std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);}}while(0)
int main(){
 temporal_pair_d85::Watchdog w;w.Arm(100);
 CHECK(!w.Tick(5100)); // time alone / paused rendering is insufficient
 for(unsigned f=1;f<=45;++f)w.BeginAttempt(f);
 CHECK(!w.Tick(1099));CHECK(w.Tick(1100));CHECK(!w.Tick(1101));
 CHECK(w.FallbackActive());CHECK(w.ReportCheck()==temporal_pair_d85::Check::FinalMainBoundary);
 char text[192]{};CHECK(w.Report(1100,true,"DLSS",text,sizeof(text)));
 CHECK(std::strstr(text,"waiting for final-main submission")!=nullptr);
 CHECK(std::strstr(text,"trying eye boundary")==nullptr);
 CHECK(!w.Report(1101,false,"DLSS",text,sizeof(text)));
 CHECK(w.Report(11100,false,"DLSS",text,sizeof(text)));
 std::atomic<bool> used{false};bool via=true;
 temporal_compose_d82::Stamp stamp{11,22,1,7,45};
 // The old early RIGHT writer can have both SDK receipts while the final-main
 // output is still unfinished. Neither elapsed time nor even an old helper
 // stamp may consume the destination before the completed-output transaction.
 CHECK(!temporal_pair_d85::ClaimRightCopy(used,true,true,{},45,22,true,true,true,via));
 CHECK(!used.load()&&!via);
 CHECK(!temporal_pair_d85::ClaimRightCopy(used,true,true,stamp,45,22,true,true,true,via));
 CHECK(!used.load()&&!via);
 CHECK(!temporal_pair_d85::ClaimRightCopy(used,true,false,stamp,45,22,true,false,true,via));
 CHECK(!used.load());
 temporal_capture_d85::Owner owner;
 CHECK(owner.Begin(7,45));owner.NoteLeftBoundary(7,45,22,4,1001);
 CHECK(!owner.CanRecord(7,45,22,true)); // SDK ready alone, RIGHT still executing
 CHECK(owner.CompleteRight(7,45,true,1002));CHECK(owner.CanRecord(7,45,22,true));
 CHECK(owner.MarkLeft(owner.Current().generation,22,31,true,1001));
 CHECK(owner.Record(22,31,32,44));const auto receipt=owner.Incoming(44);
 CHECK(receipt.recorded);CHECK(owner.Publishable(receipt,true,true));
 CHECK(!owner.Publishable(receipt,false,true));CHECK(!owner.Publishable(receipt,true,false));
 auto stale=receipt;++stale.recording;CHECK(!owner.Publishable(stale,true,true));
 CHECK(owner.Finish(receipt));w.Published(false,11101);CHECK(!w.FallbackActive());CHECK(w.temporalPairs==1);
 CHECK(!w.Tick(12101)); // no attempts after the completed pair
 // Native0's existing first RIGHT writer semantics remain unchanged.
 used=false;via=true;CHECK(!temporal_pair_d85::ClaimRightCopy(used,false,false,{},0,0,false,true,true,via));
 CHECK(!used.load());CHECK(temporal_pair_d85::ClaimRightCopy(used,false,true,{},0,0,false,false,false,via));
 CHECK(used.load()&&!via);CHECK(!temporal_pair_d85::ClaimRightCopy(used,false,true,{},0,0,false,false,false,via));
 CHECK(temporal_pair_d85::ShouldRearmBootstrap(false,45,46,false,false));
 CHECK(!temporal_pair_d85::ShouldRearmBootstrap(true,45,46,false,false));
 CHECK(!temporal_pair_d85::ShouldRearmBootstrap(false,45,46,true,false));
 CHECK(!temporal_pair_d85::ShouldRearmBootstrap(false,45,46,false,true));
 std::printf("TEMPORAL_PUBLICATION_POLICY %u checks, %u failures\n",checks,failures);return failures?1:0;
}
