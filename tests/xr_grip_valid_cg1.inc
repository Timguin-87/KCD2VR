bool XrInput::GetFloat(XrAction action, XrPath subaction_path, float& value, bool* active_out)
{
    if(active_out)*active_out=false;
    XrActionStateGetInfo info{XR_TYPE_ACTION_STATE_GET_INFO};
    info.action = action; info.subactionPath = subaction_path;
    XrActionStateFloat state{XR_TYPE_ACTION_STATE_FLOAT};
    if (XR_FAILED(xrGetActionStateFloat(session_, &info, &state))) return false;
    value = state.isActive ? state.currentState : 0.0f;
    if(active_out)*active_out=state.isActive==XR_TRUE;
    return true;
}


void XrInput::ReadGripsForTest(){
    left_.grip_valid=right_.grip_valid=false;
    GetFloat(grip_action_, left_path_, left_.grip,&left_.grip_valid);
    GetFloat(grip_action_, right_path_, right_.grip,&right_.grip_valid);
}
