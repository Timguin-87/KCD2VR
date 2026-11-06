// Sound delivery shares the existing original-first CScriptSystem callback.
// The native action producer has already finished its once-only semantic work.
static void AlchemyFeedbackServiceD80(const native_script_t2::Context& ctx,
    const kcd2vr::subsystems::alchemy::feedback_d80::Event& e) noexcept {
    namespace act=kcd2vr::activity_input163;
    const auto& w=act::witness175;const auto b=g_base.load(std::memory_order_acquire);
    if(!ctx.Current()||!e.serial||!e.tick||ctx.now<e.tick||ctx.now-e.tick>250||
       g_alchemy_instant_stop_d72.load()||!b||!retail_build112::Ready(b)||
       !w.ready.load()||w.transitionDepth175.load()||w.lifetime175.load()!=e.lifetime||
       w.active[unsigned(act::Kind175::Alchemy)].load()!=e.instance||
       e.controller!=reinterpret_cast<std::uintptr_t>(g_henry_ctrl.load())||!IsHenryController(reinterpret_cast<void*>(e.controller)))return;
    auto* instance=reinterpret_cast<void*>(e.instance);auto* player=reinterpret_cast<void*>(e.player);
    if(!Vft13(instance,0x3F60B08)||Rd<unsigned char>(instance,0x2C,0)!=1||
       Rd<void*>(instance,0x18,nullptr)!=player||!Vft13(player,0x3AA4B98))return;
    auto* entity=Rd<void*>(player,0x38,nullptr);const auto id=Rd<unsigned>(player,0x30,0);
    if(!id||double(float(id))!=double(id)||!Vft13(entity,0x4072C78)||Rd<unsigned>(entity,0xC,0)!=id||
       Rd<unsigned>(reinterpret_cast<void*>(e.controller),kCtrlEntityId,0)!=id||Entity13(id)!=entity||
       Rd<void*>(player,0x38,nullptr)!=entity||w.lifetime175.load()!=e.lifetime||
       w.active[unsigned(act::Kind175::Alchemy)].load()!=e.instance||!ctx.Current())return;
    char input[192]{},output[128]{};
    const int n=std::snprintf(input,sizeof(input),"A80|%llu|%llu|%llu|%u|%u",
        (unsigned long long)ctx.generation,(unsigned long long)e.lifetime,
        (unsigned long long)e.serial,id,unsigned(e.cue));
    const bool called=n>0&&std::size_t(n)<sizeof(input)&&ctx.CallPacket("kcd2vr_alchemy_sound_d80",input,output,sizeof(output));
    // One finite F5 receipt only; DISPATCHED means the audio API returned, not
    // that a headset heard it. A lost reply never replays this sound or recipe.
    namespace cap=kcd2vr::diagnostics::capture168;
    if(cap::active(cap::Domain::Interaction,ctx.now)){
        char line[320]{};std::snprintf(line,sizeof(line),
            "D80 ALCHEMY_SOUND serial=%llu actor=%u cue=%u packet=%d reply=%.96s nativeRecipeReplay=0 authoredBodyReplay=0 heard=UNOBSERVED",
            (unsigned long long)e.serial,id,unsigned(e.cue),called,called?output:"UNAVAILABLE");
        diagnostic_trace::Write(line);
    }
}
