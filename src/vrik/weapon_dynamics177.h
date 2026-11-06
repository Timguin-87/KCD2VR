#pragma once
#include "pose_math.h"
#include <atomic>

// F4 rung 2: mod-owned bounded presentation mechanics. SI design estimates,
// NOT retail rigid-body parameters or native damage/stamina units. No ABI calls.
namespace kcd2vr::dynamics177 {
namespace m=pose_core;
using m::Vec3; using m::Quat;
inline Vec3 cap(Vec3 v,double limit) noexcept {
    const auto n=m::length(v);return n>limit&&n>0?v*(limit/n):v;
}
struct Body {
    double mass=1, length=1, com=.2, transverse=.1, axial=.001, gripInertia=.14;
    bool estimated=true;
};
inline Body body(int cls,double inventoryWeight,double length) noexcept {
    // Classes verified in Tables.pak weapon_class.xml. These masses are tuning
    // estimates, not conversions of inventory units to kilograms.
    double mass=1.1, reference=3, fraction=.20;
    switch(cls){
    case 2:mass=1.2;reference=3.2;fraction=.28;break; // sabre
    case 3:mass=1.8;reference=6.5;fraction=.65;break; // axe
    case 4:mass=1.4;reference=3.4;fraction=.20;break; // longsword
    case 5:mass=1.5;reference=7;fraction=.65;break; // mace
    case 7:mass=3;reference=9.6;fraction=.60;break; // halberd; class 16 is a hunting sword
    default:break;
    }
    Body b{};
    b.length=std::isfinite(length)?std::clamp(length,.15,2.5):1;
    const double ratio=std::isfinite(inventoryWeight)?std::clamp(inventoryWeight/reference,.4,2.5):1;
    b.mass=mass*std::sqrt(ratio);b.com=fraction*b.length;
    b.transverse=std::max(.003,b.mass*b.length*b.length/12.0);
    b.axial=std::max(.0004,b.mass*.02*.02*.5);
    b.gripInertia=b.transverse+b.mass*b.com*b.com;
    return b;
}
// H2: bring the older bounded drive onto the same class/geometry estimates as
// the physical drive. These are equivalent tuning loads, never inventory kg.
// Preserve the 3.4-unit / 0.9 m longsword reference exactly. Rotation depends on
// grip inertia (length squared + balance), translation only on estimated mass.
struct HandlingLoadsH2 { double linear=1,angular=1; };
inline HandlingLoadsH2 handlingLoadsH2(int cls,double inventoryWeight,double gripToTipMetres) noexcept {
    const double fallback=std::isfinite(inventoryWeight)&&inventoryWeight>0?inventoryWeight:1.;
    if(!((cls>=1&&cls<=5)||cls==7||cls==16)||
       !std::isfinite(gripToTipMetres)||gripToTipMetres<=0)return {fallback,fallback};
    const auto current=body(cls,inventoryWeight,gripToTipMetres);
    const auto reference=body(4,3.4,.9);
    return {3.4*std::clamp(current.mass/reference.mass,.35,3.),
            3.4*std::clamp(current.gripInertia/reference.gripInertia,.35,3.)};
}
inline Vec3 inertia(Vec3 v,Vec3 axis,double perpendicular,double parallel) noexcept {
    const auto along=axis*m::dot(v,axis);return (v-along)*perpendicular+along*parallel;
}
struct Authority {double force=180,torque=35;};
inline Authority authority(double strength,double agility,double stamina,bool live,bool support) noexcept {
    // Inputs use the qualified R59.12C1 stamina/base-level reader. Missing/nonfinite
    // attributes use the neutral design profile, never stale inferred stats.
    live=live&&std::isfinite(strength)&&std::isfinite(agility)&&std::isfinite(stamina);
    const double str=live?std::clamp(strength/10.,.35,3.):1;
    const double agi=live?std::clamp(agility/10.,.5,2.):1;
    const double fatigue=live?.55+.45*std::clamp(stamina,0.,1.):1;
    return {180*(.6+.4*str)*fatigue*(support?1.7:1),
            35*(.6+.25*str+.15*agi)*fatigue*(support?1.8:1)};
}
inline Authority scaledAuthority44c1(Authority a,double scale) noexcept {
    const double k=std::isfinite(scale)?std::clamp(scale,.2,2.):1.;
    return {a.force*k,a.torque*k};
}
inline Authority fatiguedAuthority59c10r2(Authority a,double userScale,double exhaustion) noexcept {
    a=scaledAuthority44c1(a,userScale);
    const double divisor=std::isfinite(exhaustion)?std::clamp(exhaustion,1.,5.):1.;
    // Clamp the user's setting before fatigue. Clamping their product to .2
    // erased part of the requested fatigue (.6 / 5 became .2 instead of .12).
    return {a.force/divisor,a.torque/divisor};
}
struct Compliance59c10r2 {double metres=0, radians=0;};
inline Compliance59c10r2 compliance59c10r2(double exhaustion,bool support) noexcept {
    const double tired=std::isfinite(exhaustion)?std::clamp((exhaustion-1.)/4.,0.,1.):0.;
    // Finite presentation/comfort limits, not native physical parameters. A
    // fixed 5-degree motor previously erased most of the limited-torque solve.
    // The primary grip stays close; support narrows the allowed deviation and
    // separately increases the arm's authority. No tracking grace is added.
    const double degrees=support?6.+7.*tired:8.+9.*tired;
    return {support?.030+.025*tired:.055+.035*tired,degrees*3.14159265358979323846/180.};
}
// Class handling originated with R59.16C10R14. The current request keeps weapon heft but removes
// the exaggerated supported polearm lag and springy rebound. One-hand penalties remain distinct.
// 1. Class handling: how much of the arm's authority a ONE-handed grip gets on this class, and how far the
//    weapon may trail the hand (the compliance envelope - without opening it the authority limit is invisible,
//    the weapon was never more than 5.5 cm / 8 deg from the controller). Two hands improve polearm authority
//    without erasing heft (support already multiplies 1.7/1.8 in authority()).
struct Handling59c14 {double authority=1,envelope=1;};
inline Handling59c14 handling59c14(int cls,bool support,double poleOneHand) noexcept {
    const double pole=std::isfinite(poleOneHand)?std::clamp(poleOneHand,.15,1.):.40;
    switch(cls){
    // D86: a second hand gets the existing two-hand authority and tighter
    // envelope. H21 cancelled that leverage (.8/1.7) then widened lag x1.6,
    // on top of real length-squared inertia and the user's RPG/fatigue scale.
    // Retain one-hand heft and the body estimate; remove the duplicated penalty.
    case 7: return support?Handling59c14{1,1}:Handling59c14{pole,std::clamp(1./pole,1.,4.)};
    case 3: case 5: return support?Handling59c14{1,1}:Handling59c14{.85,1.25}; // head-heavy axe/mace
    default: return {1,1};
    }
}
// 2. Fatigue ramp: the weapon gets heavier as stamina FALLS from full, not only below 15 % (C7B keeps the x5
//    floor). ramp 0 = C7B only; ramp 1 = x4 at empty. Returns an exhaustion divisor >= 1 like exhaustionScale59c7b.
inline double fatigueRamp59c14(double stamina,bool live,double ramp) noexcept {
    if(!live||!std::isfinite(stamina)||!std::isfinite(ramp))return 1.0;
    const double r=std::clamp(ramp,0.,1.),s=std::clamp(stamina,0.,1.);
    return 1.0+3.0*r*(1.0-s);
}
inline Authority classAuthority59c14(Authority a,Handling59c14 h) noexcept {return {a.force*h.authority,a.torque*h.authority};}
inline Compliance59c10r2 classCompliance59c14(Compliance59c10r2 c,Handling59c14 h) noexcept {
    const double e=std::clamp(h.envelope,1.,4.);return {std::min(c.metres*e,.30),std::min(c.radians*e,.70)};
}
// R59.16C10R17 RPG STATS (Miki: "the weapon drag should be also scaled by Henry's RPG stats (strength, speed)"). Relative to the
// WEAPON's own requirements (melee def +0x118 StrReq / +0x11C AgiReq) with the game's own +-10 window (rpg_param
// MaxStatToAttackStatDiff): strength = how hard the arm can drive the weapon, agility = how fast it follows. k = VrWeaponStatEffect
// (0 = R16 handling). Stat == requirement is the R14/R16 feel. Fatigue stays ONLY in the exhaustion divisor (C7B + ramp).
//   xS = clamp((Str - StrReq)/10, -1, 1) -> S = 1 + k(xS >= 0 ? .30 xS : .667 xS)   (.33 .. 1.30 at k = 1; C26c: 3x heavier)
//   xA = clamp((Agi - AgiReq)/10, -1, 1) -> A = 1 + k(xA >= 0 ? .25 xA : .50 xA)    (.50 .. 1.25)
//   force x S, torque x S sqrt(A), follow gains / angular-speed cap x A, envelope x clamp(1/sqrt(S A), .8, 1.6)
struct Stats59c17 {double S=1,A=1;bool active=false;};
inline Stats59c17 stats59c17(double strength,double agility,int strReq,int agiReq,double k,bool live) noexcept {
    if(!live||!std::isfinite(k)||k<=0||!std::isfinite(strength)||!std::isfinite(agility))return {};
    k=std::min(k,1.5);
    const double xS=strReq>=0?std::clamp((strength-strReq)/10.,-1.,1.):0.,xA=agiReq>=0?std::clamp((agility-agiReq)/10.,-1.,1.):0.;
    Stats59c17 s{};s.active=true;
    // C26c (Miki 16 Oct): a weapon 10 strength above Henry feels 3x heavier one-handed (S 1/3); two hands (support x1.7
    // authority) bring it to ~1.8x. Was .60 (2.5x / 1.5x).
    s.S=std::clamp(1+k*(xS>=0?.30*xS:(2./3.)*xS),.10,1.45);
    s.A=std::clamp(1+k*(xA>=0?.25*xA:.50*xA),.25,1.40);
    return s;
}
// C26c (Miki 16 Oct): the WEAPON SKILL of this class (skill.xml / weapon_class.xml: sword+sabre+longsword+hunting
// sword = weapon_sword 16, axe+mace = heavy_weapons 17, flail+halberd = weapon_large 23, dagger+torch = weapon_dagger 22,
// unarmed 24) makes the swing easier and faster: x(1 + 0.02 (level - 15)), 0.70 at level 0 .. 1.30 at level 30, on both
// S (drive) and A (follow speed). Feel only - the game's own skill rules (attack speed, damage, block) stay native.
inline int skillSlot26(int cls) noexcept {
    switch(cls){case 1:case 2:case 4:case 16:return 0;case 3:case 5:return 1;case 6:case 7:return 2;case 0:case 11:return 3;case 12:return 4;default:return -1;}
}
inline double skillFactor26(int level) noexcept {
    if(level<0)return 1.;return std::clamp(1.+.02*(double(std::min(level,30))-15.),.70,1.30);
}
inline Stats59c17 rpg26(Stats59c17 s,int skillLevel) noexcept {
    if(!s.active)return s;const double k=skillFactor26(skillLevel);
    s.S=std::clamp(s.S*k,.10,1.6);s.A=std::clamp(s.A*k,.25,1.4);return s;
}
// C26c "SIMULATE HENRY" (WEAPON FEEL page): feel-only presets so the progression can be tried with a skilled save.
// Never written to the game; damage/stamina/skills/perks stay the real ones.
inline std::atomic<int> statSim26{0}; // menu/INI VrSimulateHenry, read by the pose producer
struct Sim26 {double strength=10,agility=10;int skill=-1;};
inline bool sim26(int mode,Sim26& s) noexcept {
    switch(mode){case 1:s={6,6,2};return true;case 2:s={14,14,12};return true;case 3:s={25,25,25};return true;default:return false;}
}
inline Authority statAuthority59c17(Authority a,Stats59c17 s) noexcept {
    if(!s.active)return a;return {a.force*s.S,a.torque*s.S*std::sqrt(s.A)};
}
inline Compliance59c10r2 statCompliance59c17(Compliance59c10r2 c,Stats59c17 s) noexcept {
    if(!s.active)return c;const double e=std::clamp(1/std::sqrt(s.S*s.A),.8,1.6);return {std::min(c.metres*e,.30),std::min(c.radians*e,.70)};
}
struct Effort {double linear=0,angular=0;};
// INVESTIGATE_D67R1[WEAPON_WEIGHT]: this model already rotates about the saved
// primary grip and estimates inertia about that grip. Static gravity support
// is deliberately compensated below; class-estimated COM is not native mass
// evidence. Evaluate a bounded COM/support torque separately from controller
// translation, support-hand authority and the final palette/attachment join.
// Do not cure visible grip wobble with independent weapon-position filtering.
inline Effort advance(const Body& b,Authority a,Vec3 localAxis,Vec3 gravity,
        Vec3 target,Quat targetQ,Vec3 targetVelocity,Vec3 targetAngularVelocity,
        double dt,Vec3& pivot,Quat& q,Vec3& velocity,Vec3& omega,double agility59c17=1.) noexcept {
    if(!(dt>0&&dt<=.0041))return {};
    const double A=std::isfinite(agility59c17)?std::clamp(agility59c17,.25,1.4):1.; // R59.16C10R17 follow speed (1 = R16)
    // C19C1: static support is separate from steering authority. A gravity
    // floor here erased fatigue on long weapons even though support is already
    // reserved below. Keep the measured/stat-scaled movement budgets unchanged.
    const auto axis=m::unit(m::rotate(q,localAxis)).value_or(Vec3{0,1,0});
    // Velocity feed-forward removes the constant-speed lag of a stationary-
    // target PD spring. Targets are interpolated across the producer interval.
    // The linear PD law is critically damped only before authority saturates.
    // Bound its position-correction speed by the distance needed to brake with
    // the available force. Otherwise a tired long weapon oscillates for seconds
    // after a reversal even though the stationary-from-rest guard test passes.
    const auto linearError=target-pivot;
    // R59.16C10R14: the arm carries the weapon's own weight FIRST (static support reserved, exactly as the torque
    // path below reserves the gravity torque); a.force is only what is left to MOVE it. Before R14 gravity came out
    // of the same capped budget, so a heavy, tired, one-handed pole with the wider R14 envelope sagged and hunted
    // for seconds instead of settling into a guard (C7E contract).
    const double brakeForce=a.force;
    const auto linearCorrection=cap(linearError*(30*A),std::sqrt(2*brakeForce/b.mass*m::length(linearError)));
    const auto force=(targetVelocity+linearCorrection-velocity)*(120*A*b.mass);
    velocity=cap(velocity+cap(force,a.force)*(1/b.mass)*dt,10*A); // weight support (-m g) + gravity (m g) cancel exactly
    pivot=pivot+velocity*dt;
    const auto error=m::rotation_vector(m::multiply(targetQ,m::conjugate(q)));
    const auto gravityTorque=m::cross(axis*b.com,gravity*b.mass);
    const double angle=m::length(error);
    const auto errorAxis=angle>1e-12?error*(1/angle):axis;
    const double errorInertia=m::dot(errorAxis,inertia(errorAxis,axis,b.gripInertia,b.axial));
    const double brakeTorque=a.torque;
    const auto angularCorrection=cap(error*(30*A),std::sqrt(2*brakeTorque/errorInertia*angle));
    const auto torque=inertia((targetAngularVelocity+angularCorrection-omega)*(120*A),axis,b.gripInertia,b.axial);
    // Static grip support is reserved separately from dynamic steering. Without
    // that reservation, a tired horizontal pole spends almost its whole torque
    // budget holding gravity and cannot brake a reversal before the pose bound.
    const auto motorTorque=cap(torque,a.torque)-gravityTorque;
    omega=cap(omega+inertia(motorTorque+gravityTorque,axis,1/b.gripInertia,1/b.axial)*dt,std::clamp(28*A,20.,32.));
    q=m::unit(m::multiply(m::from_rotation_vector(omega*dt),q)).value_or(targetQ);
    return {m::length(force)/a.force,m::length(torque)/a.torque};
}
// D69 OFF-default rigid-grip trial: the accepted angular motor only. The
// current controller owns primary translation; a later real sweep still owns
// collision yielding. No second low-pass or replacement mass/torque model.
inline double advanceAngularD69(const Body& b,Authority a,Vec3 localAxis,Vec3 gravity,
        Quat targetQ,Vec3 targetAngularVelocity,double dt,Quat& q,Vec3& omega,
        double agility59c17=1.) noexcept {
    if(!(dt>0&&dt<=.0041))return 0.;
    const double A=std::isfinite(agility59c17)?std::clamp(agility59c17,.25,1.4):1.;
    const auto axis=m::unit(m::rotate(q,localAxis)).value_or(Vec3{0,1,0});
    const auto error=m::rotation_vector(m::multiply(targetQ,m::conjugate(q)));
    const auto gravityTorque=m::cross(axis*b.com,gravity*b.mass);
    const double angle=m::length(error);
    const auto errorAxis=angle>1e-12?error*(1/angle):axis;
    const double errorInertia=m::dot(errorAxis,inertia(errorAxis,axis,b.gripInertia,b.axial));
    const auto angularCorrection=cap(error*(30*A),std::sqrt(2*a.torque/errorInertia*angle));
    const auto torque=inertia((targetAngularVelocity+angularCorrection-omega)*(120*A),axis,b.gripInertia,b.axial);
    const auto motorTorque=cap(torque,a.torque)-gravityTorque;
    omega=cap(omega+inertia(motorTorque+gravityTorque,axis,1/b.gripInertia,1/b.axial)*dt,std::clamp(28*A,20.,32.));
    q=m::unit(m::multiply(m::from_rotation_vector(omega*dt),q)).value_or(targetQ);
    return m::length(torque)/a.torque;
}
struct Impact {double speed=0,effectiveMass=0,energy=0,impulse=0;Vec3 linear{},angular{};};
inline Impact impact(const Body& b,Vec3 axis,Vec3 gripToContact,Vec3 normal,
        Vec3 contactVelocity) noexcept {
    Impact result{};const auto a=m::unit(axis),n=m::unit(normal);
    if(!a||!n||!m::finite(gripToContact)||!m::finite(contactVelocity))return result;
    result.speed=std::max(0.,-m::dot(contactVelocity,*n));
    const Vec3 r=gripToContact-*a*b.com,rx=m::cross(r,*n);
    const double inverseMass=1/b.mass+m::dot(rx,inertia(rx,*a,1/b.transverse,1/b.axial));
    result.effectiveMass=1/inverseMass;
    result.energy=.5*result.effectiveMass*result.speed*result.speed;
    // Unknown material: low-restitution generic contact, never penetration.
    // Full material/body ownership must be proven before choosing flesh/plate.
    result.impulse=1.08*result.effectiveMass*std::min(result.speed,8.);
    // Bounded perceptual recoil, not an unbounded simulated free rigid body.
    result.linear=cap(*n*(.18*result.impulse/b.mass),1.2);
    result.angular=cap(inertia(m::cross(gripToContact,*n)*(.18*result.impulse),*a,
                              1/b.gripInertia,1/b.axial),3.);
    return result;
}
}
