#pragma once
#include "ui_contextual_hud162r1.h"
namespace kcd2vr::ui_palm171 {
inline bool Open(float grip,float trigger,bool button,bool modifier){
    return std::isfinite(grip)&&std::isfinite(trigger)&&grip>=0&&trigger>=0&&
        grip<=.20f&&trigger<=.15f&&!button&&!modifier;
}
// D85R5R1: stale/inactive input zeros cannot prove an open hand. Where a
// capacitive index binding is present, resting on the trigger is not OPEN.
// Controllers without that optional binding still use valid squeeze/trigger.
inline bool OpenCurrent(float grip,float trigger,bool button,bool gripValid,bool triggerValid,
                        bool triggerTouch,bool triggerTouchValid,bool held) noexcept {
    return gripValid&&triggerValid&&!held&&(!triggerTouchValid||!triggerTouch)&&
        Open(grip,trigger,button,false);
}
class Gaze {
    bool left_=false,right_=false;
public:
    ui_contextual_hud162r1::Output Update(const ui_contextual_hud162r1::Input& in,
                                        const std::array<bool,2>& open){
        using namespace ui_contextual_hud162r1;
        Output out{};out.active=in.enabled&&in.gameplay; // R59.16C10R16: no VRIK requirement - the palm glance needs only the controllers and the headset
        left_=out.active&&in.headValid&&open[0]&&Looking(in,0,left_);
        right_=out.active&&in.headValid&&open[1]&&Looking(in,1,right_);
        out.left=left_;out.right=right_;return out;
    }
};
}
