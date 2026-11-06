// R9 lane A host test: bow follows the bow arm, right hand = arrow hand while a bow is held, knife refresh text.
#include "vrik/bow_arm_aim_r9.h"
#include "subsystems/throw_drop/ammo_readiness_k32.h"
#include <cstdio>
#include <cstring>
#include <cmath>
namespace m=kcd2vr::pose_core;namespace ba=kcd2vr::bow_arm_aim_r9;namespace k32=kcd2vr::ammo_readiness_k32;
static int checks=0,failures=0;
static void ck(bool ok,const char* what){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",what);}
static double deg(m::Vec3 a,m::Vec3 b){auto ua=m::unit(a),ub=m::unit(b);if(!ua||!ub)return 999;return std::acos(std::fmax(-1.0,std::fmin(1.0,m::dot(*ua,*ub))))*57.29577951;}
int main(){
    // R8d2 log 16:51:35.984: bow world q (-0.0621,0.00566,0.9721,0.2259), Henry root yaw q (0,0,0.61128,0.79141)
    const m::Quat root{0,0,.6112848,.7914107},bowWorld{-.06209143,.005655109,.9721465,.2259293};
    const m::Quat bow=*m::unit(m::multiply(m::conjugate(root),bowWorld));
    const m::Vec3 nock{0,-.159,.044},tip{0,0,.70}; // bow local: string behind the grip, limbs along +z (V66.175 nockLocal)
    const m::Vec3 shoulder{0,.05,1.38},hand{-.20,.62,1.32}; // chest (mid shoulders) -> bow hand, arm forward
    const m::Vec3 arm=hand-shoulder;
    const double before=deg(m::rotate(bow,nock*-1.0),arm);
    std::printf("logged bow arrow axis vs arm: %.1f deg\n",before);
    ck(before>60,"logged R8d2 bow points far off the bow arm (the rotated-left bow)");
    const auto q=ba::aim(bow,nock,tip,shoulder,hand);
    const m::Vec3 fwd=m::rotate(q,m::Vec3{0,.159,0});
    ck(deg(fwd,arm)<.5,"arrow axis = chest -> bow hand");
    const m::Vec3 limb=m::rotate(q,tip);
    ck(m::unit(limb)->z>.99,"limbs upright for a level arm");
    ck(m::angle(ba::aim(q,nock,tip,shoulder,hand),q)<1e-6,"stable: a second pass changes nothing");
    // arm raised 30 deg: limbs as upright as the arrow allows (square to the arrow)
    const m::Vec3 up30{-.20,.05+.6*.866,1.38+.6*.5};const auto q30=ba::aim(bow,nock,tip,shoulder,up30);
    ck(deg(m::rotate(q30,m::Vec3{0,1,0}),up30-shoulder)<.5,"raised arm: arrow follows");
    const auto l30=*m::unit(m::rotate(q30,tip));ck(std::fabs(m::dot(l30,*m::unit(up30-shoulder)))<1e-6&&l30.z>.85,"raised arm: limbs square to the arrow, upright");
    // bow turned upside down by the hand: half turn about the arrow is taken about the arrow axis
    const m::Quat flipped=m::multiply(m::Quat{0,1,0,0},bow);const auto qf=ba::aim(flipped,nock,tip,shoulder,hand);
    ck(deg(m::rotate(qf,m::Vec3{0,1,0}),arm)<.5&&m::unit(m::rotate(qf,tip))->z>.99,"upside-down hand: bow upright again, arrow on the arm");
    // arm hanging straight down: arrow down the arm, roll kept from the hand
    const m::Vec3 low{-.18,.10,.80};const auto ql=ba::aim(bow,nock,tip,shoulder,low);
    ck(deg(m::rotate(ql,m::Vec3{0,1,0}),low-shoulder)<.5,"hanging arm: bow points down along the arm");
    ck(m::angle(ba::aim(bow,nock,tip,shoulder,shoulder+m::Vec3{0,.1,0}),bow)<1e-9,"hand at the shoulder (<20 cm): unchanged");
    ck(m::angle(ba::aim(bow,nock,tip,shoulder,m::Vec3{NAN,0,0}),bow)<1e-9,"non-finite hand: unchanged");
    ck(m::angle(ba::aim(bow,m::Vec3{0,0,.3},tip,shoulder,hand),bow)<1e-9,"nock on the limb axis (no draw direction): unchanged");
    // rule 2
    ck(ba::arrow_hand_blocks_draw(0,true),"bow held: right hand cannot draw (arrow hand)");
    ck(!ba::arrow_hand_blocks_draw(1,true),"bow held: left hand still draws/stows");
    ck(!ba::arrow_hand_blocks_draw(0,false),"no bow: right hand draws as before");
    ck(!ba::arrow_hand_blocks_draw(0,true,false),"R11 bow held: right hand still draws the belt sword / hip (HS2 switch)");
    ck(ba::arrow_hand_blocks_draw(0,true,true),"R11 bow held: right shoulder (polearm) = quiver, blocked");
    ck(!ba::arrow_hand_blocks_draw(1,true,true),"R11 bow held: left hand unaffected");
    // knife refresh text
    ck(!std::strcmp(k32::refreshText(k32::Refresh::Status,"WAIT"),"knives: game script waits for Henry (WAIT)"),"WAIT named");
    ck(k32::refreshText(k32::Refresh::Status,"READY")==nullptr,"READY = no line");
    ck(!std::strncmp(k32::refreshText(k32::Refresh::CallFailed,nullptr),"knives: game script did not answer",33),"no reply named");
    ck(!std::strcmp(k32::refreshText(k32::Refresh::Status,"BUSY"),"knives: a knife is still reserved (BUSY)"),"BUSY named");
    ck(!std::strcmp(k32::refreshText(k32::Refresh::Status,"BAD"),"knives: game script refused the check"),"BAD named");
    ck(!std::strcmp(k32::refreshText(k32::Refresh::NoInput,"READY"),"knives: Henry not ready for the knife check yet"),"no input named");
    std::printf("%d/%d checks passed\n",checks-failures,checks);return failures?1:0;
}
