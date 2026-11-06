#pragma once
// C45: page input for the two native books (ordinary Reading, alchemy recipe book / OpenBook).
// Native reading_base binds read_prev=LB (direction -1) and read_next=RB (direction +1); read_back (B) is never a page.
// This file produces 80 ms LB/RB pulses only; it never calls a book action, never turns a page itself and never owns
// the camera. Pure, host-testable: no Windows, no XR types.
#include "reading_pages42.h"
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::book_pages45 {
enum Mode : int {ThumbrestFlick=0,ThumbrestOnly=1,Native=2}; // INI VrReadingPages, CONTROLS page button
inline std::atomic<int> mode{ThumbrestFlick};
inline int clampMode(int m) noexcept {return m<0||m>2?0:m;}
enum Book : int {None=0,Reading=1,Alchemy=2};
enum Source : int {NoSource=0,Thumbrest=1,Flick=2,Stick=3};
inline const char* bookName(int b) noexcept {return b==Reading?"READING":b==Alchemy?"ALCHEMY":"NONE";}
inline const char* sourceName(int s) noexcept {return s==Thumbrest?"THUMBREST":s==Flick?"FLICK":s==Stick?"STICK":"NONE";}
inline const char* modeName(int m) noexcept {return m==ThumbrestOnly?"THUMBREST_ONLY":m==Native?"NATIVE":"THUMBREST_FLICK";}

// Flick over the book. The book surface is NOT observed (no proven book entity/attachment pose); the zone is a
// head-relative proxy: hand 10..75 cm in front of the eyes, 5..75 cm below them, within 45 cm laterally. Arms when the
// hand is inside the zone, fires once per stroke on >= 12 cm lateral travel inside 400 ms (lateral dominant), then stays
// consumed until the hand leaves the zone (rearm after departure), so the return move never flips back.
// Head frame: forward (fx,fz) horizontal look, right = (-fz, fx), up = +y (OpenXR, metres).
struct Hand {bool valid=false;float x=0,y=0,z=0;};
struct Head {bool valid=false;float x=0,y=0,z=0,fx=0,fz=-1;};
struct FlickRecognizer {
    static constexpr unsigned kRing=24;
    struct Sample {std::uint64_t t=0;float lat=0,up=0,fwd=0;};
    struct HandState {Sample ring[kRing]{};unsigned count=0,head=0;bool inZone=false,consumed=false;};
    HandState hand[2]{};std::uint64_t last=0;
    void reset() noexcept {*this=FlickRecognizer{};}
    static bool zone(float lat,float up,float fwd,bool wasIn) noexcept {
        const float h=wasIn?.05f:0.f; // hysteresis: leaving needs 5 cm beyond the bound
        return fwd>=.10f-h&&fwd<=.75f+h&&up<=-.05f+h&&up>=-.75f-h&&std::fabs(lat)<=.45f+h;
    }
    // Returns -1 (left->right = previous), +1 (right->left = next) or 0. One result per stroke per hand.
    int step(unsigned side,const Head& head,const Hand& h,std::uint64_t now) noexcept {
        if(side>1)return 0;auto& s=hand[side];
        if(!head.valid||!h.valid||!now||now<last||!std::isfinite(h.x+h.y+h.z+head.x+head.y+head.z+head.fx+head.fz)){s=HandState{};if(now>=last)last=now;return 0;}
        last=now;
        const float rx=h.x-head.x,ry=h.y-head.y,rz=h.z-head.z;
        float fx=head.fx,fz=head.fz;const float fl=std::sqrt(fx*fx+fz*fz);if(fl<1e-4f){s=HandState{};return 0;}fx/=fl;fz/=fl;
        const float fwd=rx*fx+rz*fz,lat=rx*(-fz)+rz*fx,up=ry;
        const bool in=zone(lat,up,fwd,s.inZone);
        if(!in){s=HandState{};return 0;} // departure rearms and drops the stroke history
        s.inZone=true;
        s.ring[s.head]={now,lat,up,fwd};s.head=(s.head+1)%kRing;if(s.count<kRing)++s.count;
        if(s.consumed)return 0;
        // oldest sample inside the 400 ms window
        int dir=0;
        for(unsigned i=0;i<s.count;++i){const auto& o=s.ring[(s.head+kRing-1-i)%kRing];if(now-o.t>400)break;
            const float dl=lat-o.lat,du=std::fabs(up-o.up),df=std::fabs(fwd-o.fwd);
            if(std::fabs(dl)>=.12f&&du<std::fabs(dl)*.8f&&df<std::fabs(dl)*.8f){dir=dl>0?-1:1;break;}}
        if(dir){s.consumed=true;return dir;}
        return 0;
    }
};

struct Request {int source=NoSource,dir=0;};
struct Result {reading_pages42::Output page{};int source=NoSource,dir=0;bool requested=false,accepted=false,busy=false;};
// One arbiter for every source: one 80 ms pulse at a time, 250 ms busy window after a pulse (no double page from two
// hands or thumbrest + flick in the same stroke). Book identity (kind + native lifetime) change resets everything.
struct Arbiter {
    std::uint64_t lifetime=0,last=0,until=0,busyUntil=0;int book=None,direction=0;
    bool prevLeft=false,prevRight=false;
    FlickRecognizer flick{};reading_pages42::State stick{};
    void reset() noexcept {*this=Arbiter{};}
    // thumbrestLeft/Right: the mod's D-pad (right thumbrest touch + left stick left/right) already digital.
    Result step(int currentBook,std::uint64_t owner,std::uint64_t now,int currentMode,bool thumbrestLeft,bool thumbrestRight,
                const Head& head,const Hand& right,const Hand& left,float stickX) noexcept {
        Result r{};
        if(currentBook==None||!owner||!now){reset();book=currentBook;return r;}
        if(book!=currentBook||lifetime!=owner||now<last){reset();book=currentBook;lifetime=owner;prevLeft=thumbrestLeft;prevRight=thumbrestRight;} // held on entry is not a page
        last=now;
        Request req{};
        if(currentMode==Native){ // C42 behaviour (ordinary Reading only), grips stay the native shoulders
            const auto o=stick.step(currentBook==Reading,owner,now,stickX);
            const bool edge=(o.left&&direction!=-1)||(o.right&&direction!=1);direction=o.left?-1:o.right?1:0;
            if(o.left||o.right){r.page=o;r.source=Stick;r.dir=o.left?-1:1;r.requested=r.accepted=edge;}
            prevLeft=thumbrestLeft;prevRight=thumbrestRight;return r;
        }
        if(thumbrestLeft&&!prevLeft)req={Thumbrest,-1};
        else if(thumbrestRight&&!prevRight)req={Thumbrest,1};
        prevLeft=thumbrestLeft;prevRight=thumbrestRight;
        if(currentMode==ThumbrestFlick){
            const int fr=flick.step(0,head,right,now),fl=flick.step(1,head,left,now);
            if(!req.source){if(fr)req={Flick,fr};else if(fl)req={Flick,fl};}
        }else flick.reset();
        if(req.source){
            r.requested=true;r.source=req.source;r.dir=req.dir;r.busy=now<busyUntil;
            if(!r.busy){direction=req.dir;until=now+80;busyUntil=now+250;r.accepted=true;}
        }
        r.page={now<until&&direction<0,now<until&&direction>0};
        return r;
    }
};
// Pad application. Thumbrest modes: the physical grips are NOT the page buttons any more (LB/RB carry only the pulse)
// and the consumed D-pad left/right edges do not reach the game. Native mode: untouched apart from the C42 stick pulse
// (reading_pages42::apply), i.e. grips = native LB/RB shoulders.
template<class Pad>
inline void apply(int currentBook,int currentMode,const Result& r,Pad& pad) noexcept {
    if(currentBook==None)return;
    if(currentMode==Native){if(currentBook==Reading)reading_pages42::apply(true,r.page,pad.lb,pad.rb,pad.lx,pad.rx);return;}
    pad.lb=r.page.left;pad.rb=r.page.right;pad.dpad_left=false;pad.dpad_right=false;
}
}
