#pragma once
#include "ui_surface_geometry.h"
#include <array>
#include <cstdint>
#include <cmath>

namespace kcd2vr::ui_contextual_hud162r1 {
using Vec=ui_surface_geometry::Vec;
using Quat=ui_surface_geometry::Quat;
struct Palm {Vec position{}; Quat orientation{}; bool valid=false;};
struct Input {bool enabled=false,vrik=false,gameplay=false,headValid=false;Vec head{};Quat headOrientation{};std::array<Palm,2> palm{};std::uint64_t now=0;};
struct Output {bool active=false,left=false,right=false;};
inline float Dot(Vec a,Vec b){return a.x*b.x+a.y*b.y+a.z*b.z;}
inline bool Unit(Vec& v){const float n=std::sqrt(Dot(v,v));if(!std::isfinite(n)||n<.001f)return false;v={v.x/n,v.y/n,v.z/n};return true;}
inline bool ValidQuat(Quat q){const float n=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;return std::isfinite(n)&&std::fabs(n-1.f)<.02f;}
// Controller GRIP pose, OpenXR local space. Oculus Touch's inward palm normal
// is +X on the left hand, -X on the right. Aim pose is the pointing ray, not palm.
inline bool Looking(const Input& in,unsigned hand,bool retained){
    const auto&p=in.palm[hand];if(!p.valid||!ValidQuat(p.orientation)||!ValidQuat(in.headOrientation))return false;
    Vec toPalm{p.position.x-in.head.x,p.position.y-in.head.y,p.position.z-in.head.z};
    const float d=std::sqrt(Dot(toPalm,toPalm));if(!std::isfinite(d)||d<.15f||d>1.05f||!Unit(toPalm))return false;
    const Vec forward=ui_surface_geometry::Rotate(in.headOrientation,{0,0,-1});
    const Vec normal=ui_surface_geometry::Rotate(p.orientation,{hand==0?1.f:-1.f,0,0});
    const Vec toHead{-toPalm.x,-toPalm.y,-toPalm.z};
    // HMD orientation approximates eye gaze: 40/48 degrees with palm-facing 65/72 degrees.
    // D85R5R1: the signed inward palm normal must face the viewer. An
    // absolute dot also accepts the back of the hand (including a weapon grip).
    return Dot(forward,toPalm)>(retained?.669131f:.766044f)&&Dot(normal,toHead)>(retained?.309017f:.422618f);
}
class Gaze {
    struct State {bool shown=false,pending=false;std::uint64_t since=0;};std::array<State,2> s_{};
public:
    Output Update(const Input& in){
        Output o{};o.active=in.enabled&&in.gameplay; // R59.16C10R16: no VRIK requirement
        if(!o.active||!in.headValid){s_={};return o;}
        for(unsigned h=0;h<2;++h){auto&s=s_[h];
            if(!in.palm[h].valid){s={};continue;} // tracking loss never leaves a frozen visible palm HUD
            const bool want=Looking(in,h,s.shown);
            if(want==s.shown){s.pending=false;continue;}
            if(!s.pending||in.now<s.since){s.pending=true;s.since=in.now;}
            if(in.now-s.since>=(want?100u:160u)){s.shown=want;s.pending=false;}
        }
        o.left=s_[0].shown;o.right=s_[1].shown;return o;
    }
};
enum Group {Left=1,Right=2,Other=4};
// D84: physical left palm = compass; physical right palm = health/status.
inline constexpr unsigned CompassGroupD84=Left,VitalsGroupD84=Right;
struct Clip {const char* name;unsigned group;};
inline constexpr unsigned HideCrosshair163=8,HideCompass163=16,HideStatus163=32,HideLower163=64;
// C22b (Miki): the immersive HUD hides the native button hints (e.g. the quick-talk prompt) while Henry is in a fight.
inline constexpr unsigned HideFight22=128;
inline constexpr std::array<Clip,22> Clips{{
    {"Compass",CompassGroupD84|HideCompass163},{"Stats",VitalsGroupD84|HideLower163},{"Buffs",VitalsGroupD84|HideStatus163},{"QAMWeapon",VitalsGroupD84|HideLower163},{"QAMFood",VitalsGroupD84|HideLower163},
    {"Crime",VitalsGroupD84|HideLower163},{"Wanted",VitalsGroupD84|HideLower163},{"Trespassing",VitalsGroupD84|HideLower163},{"GameLog",Other},
    {"FancyEvent",Other},{"SkillCheck",Other},{"CommonEvent",Other},
    {"ShootingContest",Other},{"CombatCursorCross",Other|HideCrosshair163},{"CursorCross",Other|HideCrosshair163},{"CursorOutline",Other|HideCrosshair163},
    {"Hints",HideFight22},{"TutorialMessage",0},{"Subtitles",0},
    {"Cursor",Other|HideCrosshair163},{"CursorCrossPBSlot",Other|HideCrosshair163},
    {"CursorCrossOpponentLock",Other|HideCrosshair163}
}};
inline unsigned Hidden(Output o){return o.active?unsigned(Other)|(o.left?0u:unsigned(Left))|(o.right?0u:unsigned(Right)):0u;}
// Scoped draw mask. Native visibility/alpha/animation remain untouched. Read ALL
// scales before writes, zero only for Render(), restore exact live scales after.
// Backend owns exact lifetime and validated Flash functions. No persistent baseline.
template<class Backend> class DrawMask {
    Backend&io_;std::array<float,Clips.size()> xs_{},ys_{};std::array<bool,Clips.size()> use_{},written_{};
public:
    explicit DrawMask(Backend&io):io_(io){}
    bool Begin(unsigned hidden,float hints=1.f,float controls=1.f,float subtitles=1.f){
        const std::array<float,3> sizes{hints,controls,subtitles};
        for(float size:sizes)if(!std::isfinite(size)||size<.25f||size>1.5f)return false;
        for(unsigned i=0;i<Clips.size();++i)if((hidden&Clips[i].group)||(i>=16&&i<19&&sizes[i-16]!=1.f)){
            if(!io_.Resolve(i,Clips[i].name))continue; // optional native clip absence
            if(!io_.Get(i,xs_[i],ys_[i])||!std::isfinite(xs_[i])||!std::isfinite(ys_[i]))return false;
            use_[i]=true;
        }
        for(unsigned i=0;i<Clips.size();++i)if(use_[i]){
            written_[i]=true;float x=1,y=1;
            const float scale=(hidden&Clips[i].group)?0.f:sizes[i-16];
            const float tx=xs_[i]*scale,ty=ys_[i]*scale;
            if(!io_.Set(i,tx,ty)||!io_.Get(i,x,y)||std::fabs(x-tx)>.02f||std::fabs(y-ty)>.02f){Restore();return false;}
        }
        return true;
    }
    bool Restore(){bool ok=true;for(unsigned i=0;i<Clips.size();++i)if(written_[i]){
        float x=0,y=0;const bool restored=io_.Set(i,xs_[i],ys_[i])&&io_.Get(i,x,y)&&std::fabs(x-xs_[i])<.02f&&std::fabs(y-ys_[i])<.02f;
        ok=restored&&ok;if(restored)written_[i]=false;
    }return ok;}
    ~DrawMask(){Restore();}
};
}
