#include "gesture_recovery_h30_fixture.h"
#include "subsystems/experimental/live_settings_d39.h"
#include "vrik/master_gate47.h"
#include <map>
#include <string>
namespace trial=kcd2vr::claude_d70;
namespace live=kcd2vr::live_settings_d39;
static std::map<std::string,bool> saved;
static bool read(const char* key,bool fallback){const auto i=saved.find(key);return i==saved.end()?fallback:i->second;}
static bool write(const char* key,bool value){saved[key]=value;return true;}
static unsigned mask(){return unsigned(trial::StabFix())|(unsigned(trial::UnarmedFix())<<1)|(unsigned(trial::MasterstrikeFix())<<2);}
static bool actualForward(unsigned selected){
    RigH30 r(false,{0,1,0});moveH30(r,14);auto& s=r.state.weapon[0];auto& c=s.committedGestureH26;
    ckH30(c.gesture&&c.kind==1,"real production fixture committed a thrust");
    trial::SetD77((selected&1)!=0,(selected&2)!=0,(selected&4)!=0);
    r.in.tick+=10;r.in.timeNs+=10000000;++r.in.sample;c.tick=r.in.tick-154;
    r.in.hands[0].p=s.intentHand+ph::Vec3{0,-.01,0};r.in.intentHands[0].p=s.relativeIntentHand+ph::Vec3{0,-.01,0};
    const ph::Pose model{{},s.intentTip+ph::Vec3{0,.02,0}-r.profiles[0].tip};
    const ph::Pose tracked{{},s.relativeIntentTip+ph::Vec3{0,.02,0}-r.profiles[0].tip};
    ph::AdvanceCommittedGestureH8(r.in,r.profiles[0],s,0,model,tracked,true);
    return c.gesture!=0;
}
int main(){
    ckH30(mask()==0,"all three new controls preserve default OFF");
    for(bool legacy:{false,true}){
        saved.clear();saved["VrStabUnarmedMasterstrikeD70"]=legacy;trial::LoadD77(read);
        ckH30(mask()==(legacy?7u:0u),"legacy bundle load preserves all three effects");
        saved["VrStabFollowThroughD77"]=!legacy;trial::LoadD77(read);
        ckH30(mask()==(legacy?6u:1u),"explicit stab value overrides only stab; missing siblings inherit legacy");
        saved["VrUnarmedClinchD77"]=!legacy;trial::LoadD77(read);
        ckH30(mask()==(legacy?4u:3u),"explicit unarmed override preserves masterstrike fallback");
    }
    for(unsigned selection=0;selection<8;++selection){
        saved.clear();saved["VrStabUnarmedMasterstrikeD70"]=true;saved["UnrelatedSetting"]=true;
        trial::SetD77((selection&1)!=0,(selection&2)!=0,(selection&4)!=0);
        const auto revision=trial::combatRevision.load();trial::SetD77((selection&1)!=0,(selection&2)!=0,(selection&4)!=0);
        ckH30(trial::combatRevision.load()==revision,"unchanged menu apply preserves revision");
        ckH30(trial::SaveD77(write),"three-key save succeeds");
        ckH30(saved.size()==5&&saved["UnrelatedSetting"]&&saved["VrStabUnarmedMasterstrikeD70"],"save preserves legacy and unrelated keys");
        trial::SetCombatFix(false);trial::LoadD77(read);ckH30(mask()==selection,"all eight split settings roundtrip, explicit OFF beats legacy ON");
        ckH30(actualForward(selection)==((selection&1)!=0),"actual committed thrust consumes only stab control");
    }
    saved.clear();trial::SetCombatFix(false);trial::LoadD77(read);ckH30(mask()==7u,"R124: missing settings (also no legacy key) = all three ON (Miki's INI), stale OFF mask replaced");
    unsigned attempts=0;ckH30(!trial::SaveD77([&](const char*,bool){return ++attempts!=2;}),"partial write reports failure");
    ckH30(attempts==3,"all three writes attempted even after failure");
    const auto& rows=live::rows(live::kCombatPage2D70);
    ckH30(rows[0].id==live::Id::StabFixD77&&rows[1].id==live::Id::UnarmedFixD77&&rows[2].id==live::Id::MasterstrikeFixD77,"Combat page2 exposes distinct three controls");
    trial::SetCombatFix(false);trial::SetHudWalkoff(false);
    std::printf("D77 split controls %u/%u checks PASS (production thrust consumer, legacy fallback, explicit overrides, 8 roundtrips, page placement)\n",checksH30-failuresH30,checksH30);
    return failuresH30?1:0;
}
