// Actual production owner SHA256 c9192ee33ba8c8ca2c1d1577d1bbf96919fd8b90b9e373232705b63fd316d06a
void UpdateReadablePromptsD74(bool enabled,bool station,float hints,float controls,std::uint64_t now) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    const auto scales=ui_station_prompts_d74::Choose(station,hints,controls);
    const auto keyFor=[&](){
        std::uint64_t key=1469598103934665603ull;
        for(auto value:{HudGeneration(),g_canvas_lifetime.load(),g_prompt_children_d74.Epoch(),g_action_cursor_children_d69.Epoch()})
            ui_hud_work59c7::Mix(key,value);
        ui_hud_work59c7::Mix(key,enabled);ui_hud_work59c7::Mix(key,station);
        for(float value:{scales.hints,scales.controls}){std::uint32_t bits=0;std::memcpy(&bits,&value,4);ui_hud_work59c7::Mix(key,bits);}
        return key;
    };
    static ui_hud_work59c7::Retry retry;
    const auto key=keyFor();if(!retry.Due(key,now))return;
    const bool ok=ApplyReadablePromptScale(enabled,scales.hints,scales.controls);
    const auto& hint=g_prompt162r1.LastReport();const auto& cursor=g_action_cursor_d69.LastReport();
    const bool complete=ok&&(!enabled||(hint.resolved==3&&hint.verified==3&&cursor.resolved==3&&cursor.verified==3));
    // First acquisition may bind newly observed children; next iteration seeds
    // that new event key. It does not let an old key hide a failed transaction.
    if(keyFor()==key)retry.Complete(complete);
    static ui_hud_work59c7::CaptureBudget capture;
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Hud)].load();
    if(capture.Take(HudCaptureActive175(),session,true)){char message[240]{};
        std::snprintf(message,sizeof(message),"D74 HUD_PROMPT station=%d enabled=%d hints=%.2f controls=%.2f resolved=%X/%X verified=%X/%X complete=%d position=NATIVE",
            station,enabled,double(scales.hints),double(scales.controls),hint.resolved,cursor.resolved,hint.verified,cursor.verified,complete);
        diagnostic_trace::Write(message);
    }
}
