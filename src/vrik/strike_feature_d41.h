#pragma once
// Motion and contacted weapon feature are independent evidence. A forward
// mace strike is not a piercing tip; a pommel swing is not a cutting edge.
// Cosmetic finishers never change the native damage/defence/death result.
#include "body_combo59c7g.h"
#include <cmath>

namespace kcd2vr::strike_feature_d41 {
using Feature=bodycombo59c7g::Feature;
// weapon_class.xml: dagger0, sword1, sabre2, axe3, longsword4, mace5,
// flail6, halberd7, hunting sword16. An Unknown/Any proxy feature is not
// evidence of an edge or point, even on an otherwise sharp weapon class.
inline bool sword(int weaponClass) noexcept {
    return weaponClass==0||weaponClass==1||weaponClass==2||weaponClass==4||weaponClass==16;
}
inline bool canCut(int feature,int weaponClass) noexcept {
    const bool blade=feature==int(Feature::Blade),tip=feature==int(Feature::Tip);
    if(sword(weaponClass))return blade||tip; // a transverse cut may contact near the sword point
    // These need a certified cutting feature. A generic axe/pole head or
    // proxy endpoint does not distinguish a cutting edge from a blunt face.
    return (weaponClass==3||weaponClass==7)&&blade;
}
// Tip contact only (the D41 rule). Used where no contact geometry travels with the evidence.
inline bool canPierce(int feature,int weaponClass) noexcept {
    return feature==int(Feature::Tip)&&(sword(weaponClass)||weaponClass==7);
}
// R97 (Miki: killing-blow impale "we had it before working partially"): the sparse sweep can stop on a BLADE sample
// without ever querying the tip (Codex R96R1 reproduction: tipQueries=0 while an independent tip probe also hits). In the
// 4 Nov 18:06:59 kill (serial 11, longsword) the contact was BLADE, normal (0.10,-0.97,-0.22), the blade ~60 deg off
// the surface normal: the point was already under his skin. But a blade touch alone is not proof: a thrust that slides
// past his flank also touches with the blade while the point is outside him (Codex's only_blade_contact case). So a
// BLADE contact pierces only on a sword-class thrust whose TIP lies at least kBladeTipDepth96 under the touched surface
// (signed distance of the tip behind the contact plane, measured on the contact normal at the same sweep step).
// A flat graze gives ~0 (or negative), a stab at 30 deg to the skin with the contact 7 cm behind the tip gives ~3.5 cm.
// Unknown depth (no plane normal) = tip only. The stuck point is still checked on the blade line by
// impale47::pickStuckPoint (60 cm from tip / 35 cm off the line) before anything is pinned.
inline constexpr double kBladeTipDepth96=.02;
inline bool canPierce(int feature,int weaponClass,double tipDepth) noexcept {
    if(canPierce(feature,weaponClass))return true;
    return feature==int(Feature::Blade)&&sword(weaponClass)&&std::isfinite(tipDepth)&&tipDepth>=kBladeTipDepth96&&tipDepth<=.60;
}
// signed depth of the tip behind the contact plane (normal points out of the struck surface, toward the weapon);
// NaN when it cannot be measured. Same frame for all three (world metres).
inline double tipDepth96(const double tip[3],const double contact[3],const double normal[3]) noexcept {
    double n2=0,d=0;for(int i=0;i<3;++i){if(!std::isfinite(tip[i])||!std::isfinite(contact[i])||!std::isfinite(normal[i]))return std::nan("");n2+=normal[i]*normal[i];}
    if(!(n2>.25&&n2<4))return std::nan("");const double n=std::sqrt(n2);
    for(int i=0;i<3;++i)d+=(contact[i]-tip[i])*normal[i]/n;
    return d;
}
template<class Receipt> inline bool severWound(const Receipt& r) noexcept {
    return r.serial&&r.attempt&&r.target&&r.targetIdentity&&r.collider&&r.rawPart>=0&&
        r.key&&r.generation&&r.epoch&&r.request&&r.continuation&&r.seen&&r.accepted&&r.applied&&r.fields&&
        r.bodyContact&&r.kind==0&&canCut(r.feature,r.weaponClass)&&
        r.blocked==0&&r.perfect==0&&r.parry==0&&std::isfinite(r.damage)&&r.damage>0&&
        std::isfinite(r.point[0])&&std::isfinite(r.point[1])&&std::isfinite(r.point[2]);
}
}
