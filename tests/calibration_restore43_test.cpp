// C43 host checks: calibration restore rules (vertical reference, head anchor keep/drift, debounce) and the F4 OFF rule.
#include "vrik/calibration_restore43.h"
#include <cstdio>
#include <cmath>
static unsigned checks=0,failures=0;
static void ck(bool ok,const char* m){++checks;if(!ok){++failures;std::printf("FAIL %s\n",m);}}
int main(){
    using namespace kcd2vr;namespace cr=kcd2vr::calibration_restore43;using user175::Mode;
    // a) vertical reference: the 17 Oct 19:53:57 recenter (currentEye=1.031, standingEye=1.128) must correct by -0.097.
    ck(std::fabs(cr::verticalShift(true,true,1.031f,1.128f)-(-0.097f))<1e-4f,"recenter slouched: shift = current - standing");
    ck(cr::verticalShift(false,true,1.031f,1.128f)==0.f,"auto restore off: no correction");
    ck(cr::verticalShift(true,false,1.031f,1.128f)==0.f,"no floor tracking: no correction");
    ck(cr::verticalShift(true,true,1.128f,0.f)==0.f,"no standing reference: no correction");
    ck(cr::verticalShift(true,true,2.5f,1.1f)==0.f,"implausible 1.4 m shift rejected");
    ck(cr::verticalShift(true,true,NAN,1.1f)==0.f,"NaN rejected");
    // head anchor: the calibrated (F8) anchor is kept over a stance/knockdown live head; a different body is not.
    const float cal[3]{-0.044f,0.063f,1.5339f},stance[3]{-0.04f,-0.10f,1.37f},knocked[3]{0.1f,0.2f,0.95f},other[3]{0.f,0.f,0.6f};
    ck(cr::keepCalibratedAnchor(true,cal,stance),"combat stance head (-16 cm) does not replace the calibrated anchor");
    ck(cr::keepCalibratedAnchor(true,cal,knocked),"knockdown head (-58 cm) does not replace the calibrated anchor");
    ck(!cr::keepCalibratedAnchor(true,cal,other),"a head 0.93 m away is another body/scale: live anchor, reported");
    ck(!cr::keepCalibratedAnchor(false,cal,stance),"no stored anchor: live anchor");
    const float bad[3]{0.f,0.f,0.3f};ck(!cr::keepCalibratedAnchor(true,bad,stance)&&!cr::anchorPlausible(bad)&&cr::anchorPlausible(cal),"stored anchor must be plausible standing anatomy");
    ck(std::fabs(cr::anchorDriftM(cal,stance)-std::sqrt(0.004f*0.004f+0.163f*0.163f+0.1639f*0.1639f))<1e-3f,"drift metric");
    ck(cr::anchorDriftM(cal,stance)>=cr::kDriftReportM&&cr::anchorDriftM(cal,cal)<cr::kDriftReportM,"3 cm report threshold");
    // debounce: one restore per 500 ms burst (dialogue exit + activity exit edges in the same frame pair).
    cr::Debounce d{};ck(cr::admit(d,1000)&&!cr::admit(d,1200)&&!cr::admit(d,1499)&&cr::admit(d,1500)&&cr::admit(d,2600),"debounce 500 ms");
    // b) F4 OFF rule: the C4R2 cycle exits to the remembered selection, never to ALL_VRIK_OFF.
    ck(user175::hybridCurrent19c4r2(true,false,true)==Mode::Weapons,"C44: the exit press selects normal VR_WEAPONS, never ALL_VRIK_OFF (the C41 log 19:49:13 / 19:54:13 landed in ALL_VRIK_OFF)");
    ck(cr::f4Next(false,false,false,Mode::Calibrated)==Mode::CombatSynchronized,"F4 press 1 unchanged");
    ck(cr::f4Next(true,true,false,Mode::Calibrated)==Mode::CombatContactLab,"F4 press 2 unchanged");
    ck(cr::f4Next(true,false,true,Mode::Calibrated)==Mode::Calibrated,"F4 exit -> CALIBRATED_HANDLING");
    ck(cr::f4Next(true,false,true,Mode::Weapons)==Mode::Weapons,"F4 exit -> the VR_WEAPONS selection active before F4");
    ck(cr::f4Next(true,false,true,Mode::Hands)==Mode::Hands,"F4 exit -> fists selection restored");
    ck(cr::f4Next(true,false,true,Mode::Off)==Mode::Calibrated,"VRIK was off before F4: exit keeps the calibrated body on");
    ck(cr::f4Exit(Mode::CombatContactLab)==Mode::Calibrated&&cr::f4Exit(Mode::CombatSynchronized)==Mode::Calibrated,"a remembered combat mode never re-enters combat on exit");
    ck(!cr::combatMode(Mode::Calibrated)&&cr::combatMode(Mode::CombatSynchronized)&&cr::combatMode(Mode::CombatContactLab),"combat classification");
    ck(cr::selectionFromTuple(3,6,false)==Mode::Calibrated&&cr::selectionFromTuple(5,0,false)==Mode::Hands&&cr::selectionFromTuple(4,7,false)==Mode::Weapons&&cr::selectionFromTuple(5,7,false)==Mode::WeaponsHands40&&cr::selectionFromTuple(0,0,false)==Mode::Off&&cr::selectionFromTuple(4,7,true)==Mode::Off,"runtime tuple -> public selection");
    // c) menu VRIK: a stored calibration needs no T-pose.
    ck(!cr::needTpose(true)&&cr::needTpose(false),"T-pose only without a stored calibration");
    std::printf("calibration_restore43_test checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
