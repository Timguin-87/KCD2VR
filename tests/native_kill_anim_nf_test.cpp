// NF/NF2: native kill animations switch - pure rules (src/subsystems/masterstrike_combos/native_kill_anim_nf.h)
// Pure baseline gate predicates remain unchanged; D51 hook-level bypass is tested in native_master_backups_d51.
// NF2 ports I5 occurrence counterexamples: kill seen behind a clinch, no stale stamp on
// OFF->ON, stand-down only for the current Henry anim's live occurrence.
#include <thread>
#include <atomic>
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h"
#include "vrik/master_gate47.h"
#include <cstdio>
#include <vector>
#include <string>
static int checks=0,fails=0;
static void ck(bool c,const char* w){++checks;if(!c){++fails;std::printf("FAIL %s\n",w);}}
namespace nk=kcd2vr::native_kill_nf;
// mirror of animation_filter ObserveKillNF2: every live protected entry is scanned, first kill alias wins,
// independent of which protected entry the body inspector returns first.
struct Entry {int layer,id;bool live,protectedBody;std::string name;};
static void scan(nk::State& s,bool on,std::uintptr_t anim,const std::vector<Entry>& fifo,std::uint64_t now){
    for(const auto& e:fifo){if(!e.live)continue;
        if(nk::KillAlias(e.name.c_str())&&e.protectedBody){nk::observe(s,on,anim,true,e.layer,e.id,now);return;}}
    nk::observe(s,on,anim,false,-1,-1,now);
}
int main(){
    // aliases (unchanged)
    ck(nk::KillAlias("combat_rg_sz1_dz2_masterstrikedeath_lngsw_master"),"masterstrike death master");
    ck(nk::KillAlias("combat_combodeath_c05_left_lngsw_master"),"combo death master");
    ck(nk::KillAlias("combat_combo_c07_left_hlbrd_lngsw_master"),"combo finisher master");
    ck(nk::KillAlias("combat_takedown_front_left_s"),"takedown");
    ck(nk::KillAlias("combat_execution_01"),"execution");
    ck(!nk::KillAlias("combat_clinch1_sz0_clinchtransition_step0_lngsw_master"),"clinch transition master");
    ck(!nk::KillAlias("combat_clinch1_sz0_dz0_blockperfectsync_step0_lngsw_master"),"perfect block sync master");
    ck(!nk::KillAlias("combat_combo_c07_left_hlbrd_lngsw_slave"),"combo slave");
    ck(!nk::KillAlias("relaxed_idle_finish_eat"),"non-combat finish");
    ck(!nk::KillAlias(nullptr),"null");
    // 1 saved strict gate predicates remain available when the optional backup is OFF
    namespace mg=kcd2vr::master_gate47;
    ck(!mg::allowed(mg::Reason::PairedOff),"PairedOff still denied (redirect kept) with no kill stamp");
    ck(!mg::allowed(mg::Reason::NotOpposite),"NotOpposite still denied (redirect kept) with no kill stamp");
    ck(mg::allowed(mg::Reason::Qualified),"qualified still allowed");
    // 2 clinch ahead of the death master: the kill is still observed (both orders), and it stays observed as long
    //   as it is in the FIFO (no 150 ms expiry while the clinch is first)
    const std::uintptr_t henry=0x1000,other=0x2000;
    const Entry clinch{0,11,true,true,"combat_clinch1_sz0_clinchtransition_step0_lngsw_master"};
    const Entry death{0,12,true,true,"combat_rg_sz1_dz2_masterstrikedeath_lngsw_master"};
    nk::State s{};
    scan(s,true,henry,{clinch,death},1000);ck(nk::standDown(s,true,henry,1000),"clinch first: kill observed");
    const auto serial=s.live.serial;
    scan(s,true,henry,{death,clinch},1100);ck(nk::standDown(s,true,henry,1100)&&s.live.serial==serial,"reversed order: same occurrence");
    for(std::uint64_t t=1200;t<=2400;t+=100)scan(s,true,henry,{clinch,death},t);
    ck(nk::standDown(s,true,henry,2400)&&s.live.serial==serial,"still present after 1.4 s: still the same live occurrence");
    scan(s,true,henry,{clinch},2500);ck(!nk::standDown(s,true,henry,2500),"kill left the FIFO: retired at once");
    // a death clip that is live but not a protected body owner (e.g. excluded facial/holding) never qualifies
    scan(s,true,henry,{Entry{0,13,true,false,"combat_rg_sz1_dz2_masterstrikedeath_lngsw_master"}},2600);
    ck(!nk::standDown(s,true,henry,2600),"unprotected entry is not a qualified kill");
    // new occurrence = new serial
    scan(s,true,henry,{death},2700);const auto s1=s.live.serial;
    scan(s,true,henry,{Entry{0,99,true,true,"combat_combodeath_c05_left_lngsw_master"}},2710);
    ck(s.live.serial!=s1&&nk::standDown(s,true,henry,2710),"different clip = new occurrence");
    // 3 OFF->ON: nothing observed while OFF, a pre-switch stamp is never consumed
    nk::State off{};scan(off,false,henry,{death},3000);ck(!off.live.serial,"OFF: inspector records nothing");
    ck(!nk::standDown(off,true,henry,3001),"OFF->ON without a new observation: no stand-down");
    scan(off,true,henry,{death},3002);ck(nk::standDown(off,true,henry,3002),"ON + new observation: stand-down");
    ck(!nk::standDown(off,false,henry,3003),"switch OFF again: never");
    //   stand-down only for the current Henry anim's occurrence
    ck(!nk::standDown(off,true,other,3003),"another skeleton anim (reloaded Henry / NPC): no stand-down");
    ck(!nk::standDown(off,true,0,3003),"unknown Henry anim: no stand-down");
    //   liveness guard: a stalled inspector does not keep authority
    ck(nk::standDown(off,true,henry,3002+nk::kLivenessMs),"inspector alive within liveness");
    ck(!nk::standDown(off,true,henry,3003+nk::kLivenessMs),"inspector stalled: no stand-down");
    ck(!nk::standDown(off,true,henry,3001),"clock before observation: no stand-down");
    //   runtime wrapper: real SetEnabled edges retire; unchanged menu application does not
    nk::SetEnabled(false);ck(nk::BeginScan()==0,"runtime OFF: no scan generation");nk::Observe(0,henry,true,0,12,5000);ck(!nk::StandDown(henry,5000),"runtime OFF: Observe ignored");
    nk::SetEnabled(true);ck(!nk::StandDown(henry,5001),"runtime OFF->ON: no stale occurrence");
    auto g=nk::BeginScan();ck(g!=0,"runtime ON: scan generation");
    nk::Observe(g,henry,true,0,12,5002);ck(nk::StandDown(henry,5002),"runtime ON + observation");
    ck(!nk::StandDown(other,5002),"runtime: other anim");
    const auto sameGeneration=nk::generation.load(),sameSerial=nk::liveSerial.load();nk::SetEnabled(true);
    ck(nk::generation.load()==sameGeneration&&nk::liveSerial.load()==sameSerial&&nk::StandDown(henry,5002),"D51 unchanged selection preserves current occurrence");
    g=nk::BeginScan();nk::Observe(g,henry,false,-1,-1,5003);ck(!nk::StandDown(henry,5003),"runtime: kill gone = retired");
    g=nk::BeginScan();nk::Observe(g,henry,true,0,12,5004);nk::SetEnabled(false);nk::SetEnabled(true);ck(!nk::StandDown(henry,5004),"runtime: toggle retires");
    g=nk::BeginScan();nk::Observe(g,henry,true,0,12,5005);nk::Retire();ck(!nk::StandDown(henry,5005),"runtime: explicit retire (inspection stopped)");
    // NF3 interleavings (Codex generation race): scan starts, the menu toggles, then the stale scan commits
    {auto s1=nk::BeginScan();nk::SetEnabled(false);nk::Observe(s1,henry,true,0,12,5100);
     ck(!nk::StandDown(henry,5100),"NF3: scan before OFF cannot publish after OFF");
     nk::SetEnabled(true);ck(!nk::StandDown(henry,5101),"NF3: ...nor after OFF->ON");
     nk::Observe(s1,henry,true,0,12,5102);ck(!nk::StandDown(henry,5102),"NF3: old generation stays stale after ON");
     auto s2=nk::BeginScan();nk::Observe(s2,henry,true,0,12,5103);ck(nk::StandDown(henry,5103),"NF3: fresh scan publishes");
     nk::Observe(s1,henry,false,-1,-1,5104);ck(nk::StandDown(henry,5104),"NF3: stale 'gone' cannot retire a newer generation's occurrence");
     auto s3=nk::BeginScan();nk::SetEnabled(false);nk::SetEnabled(true);nk::Observe(s3,henry,true,0,12,5105);
     ck(!nk::StandDown(henry,5105),"NF3: OFF->ON during scan drops the scan");
     ck(nk::staleScans.load()>=3,"NF3: stale scans counted");}
    // NF3: two threads hammering scan/commit vs toggles never leave a StandDown for a generation that was retired
    {std::atomic<bool> stop{false};std::atomic<int> bad{0};
     std::thread scanner([&]{std::uint64_t t=6000;while(!stop.load()){auto gg=nk::BeginScan();nk::Observe(gg,henry,true,0,12,++t);}});
     std::thread toggler([&]{for(int i=0;i<20000;++i){nk::SetEnabled(false);if(nk::StandDown(henry,~0ull>>1))bad.fetch_add(1);nk::SetEnabled(true);}});
     toggler.join();stop=true;scanner.join();
     nk::SetEnabled(false);ck(bad.load()==0&&!nk::StandDown(henry,7000),"NF3: no stand-down while OFF under contention");}
    std::printf("native_kill_anim_nf %d/%d\n",checks-fails,checks);return fails?1:0;
}
