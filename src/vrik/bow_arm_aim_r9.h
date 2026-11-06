#pragma once
// R9 lane A (Claude, 25 Oct): two bow rules.
//  1 aim: the bow in the VR hand pointed ~80 deg to the LEFT of where the arm points (R8d2 log 16:51:35-37: the bow's
//    arrow axis in body space (-0.98,0.20,-0.02) with the limbs upright). the held bow copied the native hand bone
//    1:1 (authored socket, identity), and the VR hand bone does not sit like Henry's aiming hand. the draw coupling then
//    could only turn it back 60 deg (1.05 rad cap) -> still rotated left while aiming. now the arrow axis is the line
//    from the chest (mid of the shoulders, where the string is anchored) to the bow hand, limbs kept upright, the
//    hand turns with the bow. position of the grip is kept.
//  2 arrow hand: while a bow is in hand the RIGHT hand is the arrow hand. reaching over the right shoulder for an
//    arrow was also the HS2 polearm zone (+14 up/-12 back) -> "reload the bow and the heavy weapon comes up"
//    (16:51:38 HS2 WEAPON_SWITCH stow bow, draw polearm hand=0). no right hand draw / weapon switch while a bow is held.
#include "pose_math.h"
#include <atomic>
#include <cmath>

namespace kcd2vr::bow_arm_aim_r9 {
namespace m=pose_core;
inline std::atomic<bool> enabled{true}; // INI VrBowArmAim (default on)

// weapon: bow pose (model space). drawLocal = bow-local nock - grip (points to the archer, made square to the limbs),
// limbLocal = bow-local long axis (tip - grip). origin = mid of the shoulders, hand = bow hand (model space).
// returns the new bow rotation, grip point stays where it is (caller recomputes p).
inline m::Quat aim(m::Quat weapon,m::Vec3 drawLocal,m::Vec3 limbLocal,m::Vec3 origin,m::Vec3 hand) noexcept {
    const auto limb=m::unit(limbLocal);if(!limb)return weapon;
    const auto draw=m::unit(drawLocal-*limb*m::dot(drawLocal,*limb));const auto arm=m::unit(hand-origin); // draw is across the limbs
    if(!draw||!arm||!m::finite(origin)||!m::finite(hand)||m::length(hand-origin)<.20)return weapon;
    const m::Vec3 forward=*arm; // the arrow leaves from the chest through the bow hand
    const m::Vec3 nowForward=m::rotate(weapon,*draw*-1.0);
    m::Quat q=m::multiply(m::from_to(nowForward,forward),weapon);
    // limbs upright: twist about the arrow axis so the long axis is as vertical as it can be. arm pointing straight
    // up/down = keep the hand's own roll.
    m::Vec3 up{0,0,1};
    if(std::fabs(m::dot(up,forward))>.95)up=m::rotate(weapon,*limb);
    const m::Vec3 wantUp=up-forward*m::dot(up,forward);
    m::Vec3 haveUp=m::rotate(q,*limb);haveUp=haveUp-forward*m::dot(haveUp,forward);
    if(const auto a=m::unit(wantUp),h=m::unit(haveUp);a&&h){
        // exactly opposite: half turn about the arrow axis (from_to would pick some other axis)
        if(m::dot(*a,*h)<-.999999)q=m::multiply(m::Quat{forward.x,forward.y,forward.z,0},q);
        else q=m::multiply(m::from_to(*h,*a),q);}
    const auto u=m::unit(q);if(!u)return weapon;
    return *u;
}
// rule 2. drawHand = hand that would draw (0 right, 1 left).
// R11: only the quiver itself is reserved. the staged quiver box (staged_reload596c2.inl: right 6..55 cm, -35..+25 cm
// height, 6..55 cm behind the head) holds the right-shoulder holster (polearm/heavy); belt sword / right hip stay
// drawable with the bow out, so the HS2 bow->sword switch keeps working.
inline bool arrow_hand_blocks_draw(int drawHand,bool bowHeld,bool rightShoulderZone=true) noexcept {return bowHeld&&drawHand==0&&rightShoulderZone;}
}
