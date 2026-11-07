// Generated from current flashui_probe.cpp SHA256 c9192ee33ba8c8ca2c1d1577d1bbf96919fd8b90b9e373232705b63fd316d06a
bool ApplyReadablePromptScale(bool enabled,float hints,float controls) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    HudHolder holder{};void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return !enabled;}
    const auto previous=g_canvas_live_hud.exchange(hud,std::memory_order_acq_rel);
    if(previous!=hud)g_canvas_lifetime.fetch_add(1,std::memory_order_acq_rel);
    struct PromptBackendD74 : NativeCanvasBackend {
        bool Resolve(unsigned i,const char* name,std::uintptr_t& id) noexcept {
            if(i>=2||!NativeCanvasBackend::Resolve(i,name,id))return false;
            ui_hud_visibility175::Key child{};
            if(!ReadHudKey175(handles[i].clip,hud,HudGeneration(),child))return false;
            g_prompt_children_d74.Observe(i,child);id=child.object;return true;
        }
        std::uint64_t Revision(unsigned i) const noexcept {return g_prompt_children_d74.Revision(i);}
        bool Set(unsigned i,ui_native_canvas::Geometry value) noexcept {
            if(i>=2||!Alive())return false;
            alignas(16) auto bytes=ui_prompt_scale120r3::Notifications::PackScale(value);
            return CallFlashDisplayFnSEH(handles[i].set,handles[i].clip,bytes.data())&&Alive();
        }
    }io;io.hud=hud;io.generation=g_canvas_lifetime.load(std::memory_order_acquire);
    const bool ok=g_prompt162r1.Run(io,io.generation,enabled,std::clamp(hints,.25f,1.5f),std::clamp(controls,.25f,1.5f));
    // Same retained HUD and settings/lifecycle event as the established prompt
    // owner. Only these semantic children's local scale is written; no timer,
    // visibility, whole-parent transform or new native call/hook is introduced.
    struct ActionCursorBackendD69 : NativeCanvasBackend {
        bool Resolve(unsigned i,const char* name,std::uintptr_t& id) noexcept {
            if(i>=ui_prompt_scale120r3::ActionCursorD69::Names.size()||!NativeCanvasBackend::Resolve(i,name,id))return false;
            ui_hud_visibility175::Key child{};
            if(!ReadHudKey175(handles[i].clip,hud,HudGeneration(),child))return false;
            g_action_cursor_children_d69.Observe(i,child);id=child.object;return true;
        }
        std::uint64_t Revision(unsigned i) const noexcept {return g_action_cursor_children_d69.Revision(i);}
        bool Set(unsigned i,ui_native_canvas::Geometry value) noexcept {
            if(i>=ui_prompt_scale120r3::ActionCursorD69::Names.size()||!Alive())return false;
            alignas(16) auto bytes=ui_prompt_scale120r3::Notifications::PackScale(value);
            return CallFlashDisplayFnSEH(handles[i].set,handles[i].clip,bytes.data())&&Alive();
        }
    }cursor;
    cursor.hud=hud;cursor.generation=io.generation;
    const bool cursorOk=g_action_cursor_d69.Run(cursor,cursor.generation,enabled,std::clamp(controls,.25f,1.5f));
    ReleaseHudHolder(holder);return ok&&cursorOk;
}
