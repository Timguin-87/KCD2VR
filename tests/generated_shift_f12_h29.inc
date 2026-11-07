// Generated verbatim from current mod.cpp by extract_user_controls175.py
if(developerMode&&!matrix_active && f12_down && !f12_down_prev){
            if(GetAsyncKeyState(VK_SHIFT)&0x8000){
                if(vr_menu_)vr_menu_->ToggleDeepSeekLabH29();
            }else{
                const int test=(kcd2vr::routerH3::mode.load()+1)%3;
                kcd2vr::routerH3::mode.store(test,std::memory_order_release);
                char line[256]{};std::snprintf(line,sizeof(line),
                    "H3 F12 test=%d state=%s liveRouter=UNQUALIFIED nativeRpgDecisions=PRESERVED forcedOutcomes=NONE F4=UNCHANGED deepseekLab=SHIFT_F12",
                    test,test==0?"OFF":test==1?"ROUTER_OBSERVATION":"NATIVE_WINDOWS_ONLY");
                diagnostic_trace::Write(line);KCD2VR_DIAG_LOG(Bug,warning,"{}",line);LabLadderCue(test);
            }
        }
