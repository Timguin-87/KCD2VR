#include "diagnostics/capture_ticket_d84.h"
#include <cstdio>

namespace cap=kcd2vr::diagnostics::capture168;
namespace ticket=kcd2vr::diagnostics::capture_ticket_d84;
using D=cap::Domain;
static unsigned checks=0,clocks=0;
static std::uint64_t now=10000;
static std::uint64_t clockLeaf() noexcept {++clocks;return now;}
static auto handling(){return ticket::select<D::Physics,D::Vrik,D::Combat,D::Interaction,D::Reload>(clockLeaf);}
static void stopAll(){for(auto d:{D::Physics,D::Vrik,D::Combat,D::Interaction,D::Reload,D::Bug})cap::gate.select(d,false,now);}
#define CK(x) do {++checks;if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
    for(unsigned i=0;i<10000;++i)CK(!handling());
    CK(clocks==0);
    cap::gate.select(D::Bug,true,now);CK(!handling()&&clocks==0);stopAll();
    for(auto d:{D::Physics,D::Vrik,D::Combat,D::Interaction,D::Reload}){
        cap::gate.select(d,true,now);const auto before=clocks;
        const auto t=handling();CK(t&&t.domain==d&&t.now==now&&clocks==before+1);
        CK(t.session==cap::gate.session[unsigned(d)].load()&&cap::gate.receipt(d,t.session,now));
        cap::gate.select(d,false,now);CK(!handling());
        CK(cap::gate.receipt(d,t.session,now)); // original bounded drain only
        now+=5001;CK(!cap::gate.receipt(d,t.session,now));
        cap::gate.select(d,true,now);CK(!cap::gate.receipt(d,t.session,now));
        const auto replacement=handling();CK(replacement&&replacement.session!=t.session);
        stopAll();now+=10;
    }
    // Aliases preserve the canonical original session.
    for(auto alias:{D::Hands,D::HandReference}){
        cap::gate.select(alias,true,now);const auto t=handling();
        CK(t&&t.domain==cap::canonical(alias));
        CK(t.session==cap::gate.session[unsigned(alias)].load());stopAll();
    }
    for(auto d:{D::Physics,D::Vrik,D::Combat,D::Interaction,D::Reload})cap::gate.select(d,true,now);
    for(auto d:{D::Physics,D::Vrik,D::Combat,D::Interaction,D::Reload}){
        const auto t=handling();CK(t&&t.domain==d);cap::gate.select(d,false,now);
    }
    const auto quiet=clocks;CK(!handling()&&clocks==quiet);
    // An expired but not yet polled bit may read time, but cannot issue a ticket.
    cap::gate.select(D::Physics,true,now);now+=cap::Duration;
    CK(!handling());cap::gate.poll(now);const auto afterPoll=clocks;
    CK(!handling()&&clocks==afterPoll);
    cap::gate.select(D::Physics,true,now);const auto original=handling();
    cap::gate.select(D::Vrik,true,now);cap::gate.select(D::Physics,false,now);
    now+=5001;CK(!cap::gate.receipt(original.domain,original.session,now));
    CK(handling().domain==D::Vrik); // active F7 cannot renew an old F9 observation
    std::printf("capture_ticket_d84: %u checks; direct production header, quiet clock, route priority, aliases, expiry and original-session restart PASS\n",checks);
}
