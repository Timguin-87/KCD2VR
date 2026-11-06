// V66.178R10 host checks: F4 rung cycle, R3 special hold window, F11 hammer stroke detector.
#include "vrik/combat_rules178r10.h"
#include "vrik/user_modes175.h"
#include <cstdio>
#include <cmath>
#include <string_view>
static unsigned checks=0,failures=0;
static void ck(bool ok,const char* m){++checks;if(!ok){++failures;std::printf("FAIL %s\n",m);}}
int main(){
    using namespace kcd2vr;using namespace kcd2vr::combat_rules178r10;using user175::Mode;
    // F4: OFF -> 1 -> 2 -> 3 -> OFF, and rung numbering.
    ck(user175::combat(false,false,false)==Mode::Combat,"F4 from off selects rung 1");
    ck(user175::combat(true,false,false)==Mode::CombatPhysical,"F4 from rung 1 selects rung 2");
    ck(user175::combat(true,true,false)==Mode::CombatFull,"F4 from rung 2 selects rung 3");
    ck(user175::combat(true,true,true)==Mode::Calibrated,"F4 from rung 3 turns the experiment off (3-rung form)");
    ck(user175::combat(true,true,true,3)==Mode::CombatCombo&&user175::combat(true,true,true,4)==Mode::CombatImpale&&user175::combat(true,true,true,5)==Mode::CombatGeometry&&user175::combat(true,true,true,6)==Mode::Calibrated,"V66.178R15/R27: rung 3 -> 4 -> 5 -> 6 -> off");
    ck(user175::configuration(Mode::CombatImpale).rung178r15==5&&user175::configuration(Mode::CombatCombo).rung178r15==4&&user175::configuration(Mode::CombatFull).rung178r15==3,"rung numbers carried by the configuration");
    ck(user175::f4Rung(false,false,false)==0&&user175::f4Rung(true,false,false)==1&&user175::f4Rung(true,true,false)==2&&user175::f4Rung(true,true,true)==3,"rung numbers");
    const auto full=user175::configuration(Mode::CombatFull);
    ck(full.combat==3&&full.interaction==2&&full.pose==4&&full.weapon==7&&full.experiment&&full.physical177&&full.full178r10,"rung 3 keeps the rung 2 gameplay tuple and adds the VR-rules flag");
    ck(std::string_view(user175::name(Mode::CombatFull))=="F4_RUNG3_VR_RULES","rung 3 name");

    // R3 special: one LT hold window at a time, long enough for combat_locked's 0.25 s holdTriggerDelay.
    specialUntil.store(0);
    ck(kHoldMs>=350&&requestSpecial(1000)&&specialHeld(1000)&&specialHeld(1000+kHoldMs-1)&&!specialHeld(1000+kHoldMs),"hold window covers the native hold delay and ends");
    ck(!requestSpecial(1100)&&requestSpecial(1000+kHoldMs),"no stacking inside the window; a new request after it");

    // V66.178R16 acknowledged transport: a release before the down was dispatched waits; abort orders A before RT-up.
    {namespace ax=attack_xi178r8;ax::reset();float rt=0,lt=0;bool a=false;using ax::translate;using ax::apply;using ax::ackADown;using ax::ownedMask;using ax::rtHeld;using ax::rtReleasePending;using ax::abortPending;using ax::ownedDispatch;
     ck(translate(0,1,5000)&&translate(1,2,5003),"prepare then release accepted");
     apply(5010,rt,lt,a);ck(rt==1.f&&(ownedMask.load()&1u)&&!a,"RT stays down (owned) until the emitter acknowledges the down");
     ax::ackRtDown(ax::prepareSerial.load());rt=0;apply(5041,rt,lt,a);ck(rt==0.f&&!rtHeld.load()&&!rtReleasePending.load(),"after the acknowledged down the pending release goes through on the next pad");
     ax::reset();ck(translate(0,1,6000),"prepare");apply(6001,rt,lt,a);ax::ackRtDown(ax::prepareSerial.load());
     ck(translate(2,1,6050),"abort");rt=0;a=false;apply(6051,rt,lt,a);ck(rt==1.f&&a&&(ownedMask.load()&5u)==5u,"abort: A pressed while RT is still down (abort before release)");
     ackADown();rt=0;a=false;apply(6060,rt,lt,a);ck(rt==0.f&&a&&!abortPending.load(),"after the acknowledged A the RT release follows; A stays for its window");
     rt=0;a=false;apply(6300,rt,lt,a);ck(rt==0.f&&!a&&ownedMask.load()==0,"everything released after the abort window");
     ax::reset();ck(translate(4,1,7000)&&ax::specialHeld(7100)&&!translate(4,1,7100),"attack_special rides the LT hold and does not stack");
     rt=0;lt=0;a=false;apply(7100,rt,lt,a);ck(lt==1.f&&(ownedMask.load()&2u),"the special hold is published as an owned LT");
     ck(!ownedDispatch,"provenance is off outside the emitter's dispatch scope");ax::reset();}
    {namespace ax=attack_xi178r8;ax::reset();
     const auto stab=ax::unpack(ax::pack(1,0,false,false,1,9,2));
     ck(stab.stroke==2&&stab.serial==9,"gesture travels with its pad packet");
     ax::GestureLatch51 l;
     ck(l.update(true,true,stab.stroke,stab.serial)==2,"delayed stab down keeps stab class");
     ck(l.update(true,true,1,10)==2&&l.serial==9,"new cut cannot overwrite a held stab");
     ck(l.update(false,false,1,10)==2,"stab release retains its down identity");
     ck(l.update(true,true,1,10)==1,"next independent down can be a cut");
     ck(ax::translate(0,1,8000),"first prepare");ax::ackRtDown(ax::prepareSerial.load());
     ck(ax::translate(1,2,8010),"release requested");
     ck(!ax::translate(0,1,8011),"cannot merge next prepare across undelivered release");
     ck(!ax::translate(0,1,8300),"R53: still waiting inside the 400 ms expiry");
     ax::ackRtUp51(ax::prepareSerial.load()-1);ck(!ax::rtAwaitUp51.load(),"R53: any owned RT-up dispatch clears the transport (one RT symbol; the gesture serial is not the emitter serial)");
     ck(ax::translate(0,1,8020),"fresh prepare after observed up");
     ck(ax::translate(1,2,8030),"release requested again");ck(!ax::translate(0,1,8040),"waiting for the up");
     ck(ax::translate(0,1,8450),"R53: an up that was never dispatched expires after 400 ms instead of blocking every later prepare");ax::reset();}
    // Hammer: a 0.5 m down-swing at ~2.5 m/s then stop = one stroke; slow lowering = none; refractory.
    HammerDetector h{};unsigned strokes=0;std::uint64_t t=0;float y=1.2f;
    auto feed=[&](float ny,std::uint64_t dt){t+=dt;y=ny;if(h.update(y,t))++strokes;};
    feed(1.2f,20);
    for(int i=0;i<10;++i)feed(y-0.05f,20);        // 2.5 m/s down
    feed(y-0.005f,20);feed(y,20);                  // landing: speed collapses
    ck(strokes==1,"one stroke per fast down-swing that stops");
    for(int i=0;i<10;++i)feed(y+0.05f,20);         // raise the hammer: no stroke
    ck(strokes==1,"raising the hammer is not a stroke");
    for(int i=0;i<20;++i)feed(y-0.01f,20);         // 0.5 m/s lowering: no stroke
    feed(y,20);
    ck(strokes==1,"slow lowering below the speed threshold is not a stroke");
    for(int i=0;i<6;++i)feed(y-0.06f,20);feed(y,20);  // second strike
    ck(strokes==2,"a second strike after the refractory window counts");
    const auto before=strokes;const auto tb=t;
    for(int i=0;i<3;++i)feed(y-0.06f,20);feed(y,20);  // immediate re-strike within 320 ms of the last one
    ck(t-tb<kStrokeRefractoryMs&&strokes==before,"a re-strike inside the refractory window is ignored");
    h.reset();strokes=0;t=0;feed(1.0f,20);feed(0.5f,500);feed(0.5f,20);
    ck(strokes==0,"a tracking gap (>120 ms) cannot fake a stroke");
    std::printf("combat_rules178r10=%u/%u\n",checks-failures,checks);
    return failures?1:0;
}
