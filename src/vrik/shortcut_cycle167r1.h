#pragma once
#include "f11_tests167.h"
// Mod-control-thread state only. Existing mode setters perform all native work.
namespace kcd2vr::shortcut_cycle167r1 {
using Modes=f11_tests167::Modes;
template<class Interaction,class Combat,class Weapon,class Pose>
void apply_modes(Modes mode,Interaction interaction,Combat combat,Weapon weapon,Pose pose){
    interaction(mode.interaction);combat(mode.combat);
    // Existing combat setters may auto-select a weapon mode; weapon setters
    // may auto-select pose mode 3. The explicit saved pose must be last.
    weapon(0);weapon(mode.weapon);pose(mode.pose);
}
enum class Key {None,F4,F8};
enum class Result {On,Off,Blocked,Failed};
struct Cycle {
    Key key=Key::None;int rung=0;f11_tests167::Lease lease{};
    template<class Read,class Apply,class Lifecycle>
    bool stop(bool restore,Read read,Apply apply,Lifecycle lifecycle){
        if(key==Key::None)return true;
        Modes baseline{};const bool owned=lease.end(read(),baseline);
        key=Key::None;rung=0;lifecycle(false);
        if(restore&&owned){apply(baseline);return f11_tests167::same(read(),baseline);}
        return true;
    }
    template<class Read,class Apply,class Lifecycle>
    bool reconcile(Read read,Apply apply,Lifecycle lifecycle){
        if(key!=Key::None&&!lease.current(read())){stop(false,read,apply,lifecycle);return true;}
        return false;
    }
    template<class Read,class Apply,class Lifecycle>
    Result press(Key pressed,bool calibrated,Read read,Apply apply,Lifecycle lifecycle){
        reconcile(read,apply,lifecycle);
        int next=pressed==Key::F4?(key==Key::F4?0:1):(key==Key::F8?(rung+1)%3:1);
        if(!next)return stop(true,read,apply,lifecycle)?Result::Off:Result::Failed;
        if(pressed==Key::F4&&!calibrated)return Result::Blocked;
        // Native lifecycle capture must remain usable without VRIK calibration.
        if(pressed==Key::F8&&next==1&&!calibrated)next=2;
        const auto current=read();lease.begin(current);lifecycle(false);
        const Modes wanted=pressed==Key::F4?Modes{3,0,current.pose==4?4:3,7}:
            next==1?Modes{0,0,4,6}:Modes{0,1,lease.base.pose,0};
        apply(wanted);
        if(!f11_tests167::same(read(),wanted)||(pressed==Key::F8&&next==2&&!lifecycle(true))){
            const auto baseline=lease.base;key=Key::None;rung=0;lease={};lifecycle(false);apply(baseline);
            return Result::Failed;
        }
        key=pressed;rung=next;lease.selected=wanted;return Result::On;
    }
};
}
