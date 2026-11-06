#pragma once
namespace kcd2vr::pause_chord58c2 {
struct Decision { bool fire=false,consume=false; };
// One owner before XI/fallback/UI routing. Reset of a gameplay router cannot
// re-arm a held chord. Inactive controllers require a fresh neutral Menu.
struct State {
    bool quarantined=true,consumed=false;
    Decision update(bool active,bool allowed,bool menu,bool thumbrest) noexcept {
        if(!active){quarantined=true;return {false,menu||consumed};}
        if(!menu){quarantined=false;consumed=false;return {};}
        if(!allowed){quarantined=true;return {false,consumed};}
        if(quarantined||consumed)return {false,consumed};
        if(thumbrest){consumed=true;return {true,true};}
        return {};
    }
};
}
