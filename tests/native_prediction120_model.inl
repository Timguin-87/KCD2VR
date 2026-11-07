// Explicit native replacements. These exercise the production adapters and do
// NOT emulate retail damage, native threads, instruction relocation or Havok.
static unsigned predictionCalls120=0,interpolationCalls120=0;
static QuatT28 setupPrevious120{},sweepPrevious120{},sweepCurrent120{};
static double driverAngle120=0;
static QuatT28* __fastcall interpolateModel120(QuatT28* out,const QuatT28*,const QuatT28*,const float*,float,float){
    ++interpolationCalls120;*out={{0,0,0,1},{99,99,99}};return out;
}
static void* __fastcall predictionModel120(void* wrapper,float){
    ++predictionCalls120;
    using namespace DetourModKit::hook;
    void* action=nullptr;std::memcpy(&action,wrapper,sizeof(action));
    MidContext ctx{};gpr(ctx,Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(action);
    gpr(ctx,Gpr::R12)=reinterpret_cast<std::uintptr_t>(wrapper);PredictionMid120(ctx);
    if(gpr(ctx,Gpr::Rdi)==reinterpret_cast<std::uintptr_t>(action))return reinterpret_cast<void*>(0x1234);
    auto* input=reinterpret_cast<PredictionInputs120*>(gpr(ctx,Gpr::Rdi));
    const auto direction=pm::unit(pm::Vec3{input->direction[0],input->direction[1],input->direction[2]});
    const auto targetAxis=pm::unit(pm::rotate(PhysPose6698(input->target).q,{0,0,-1}));
    driverAngle120=std::acos(std::clamp(pm::dot(*direction,*targetAxis),-1.0,1.0));
    // Recovered native loop admission/subdivision, not native hit eligibility.
    if(driverAngle120<=.0001||std::ceil(driverAngle120/2.094395160675049)!=1)return nullptr;
    setupPrevious120=input->previous; // same input consumed synchronously by A7CCCC
    QuatT28 output{};float axis[]{1,0,0};
    PredictionInterpolate120(WhBase()+0xA7C96D,&output,&input->previous,&input->target,axis,.01f,1.f);
    sweepPrevious120=input->previous;sweepCurrent120=output;
    auto* scope=g_prediction_scope120;
    ck(PredictionSweep120(WhBase()+0xA7C9C1,scope->collisions,scope->hand,scope->bone),"only matched native prediction sweep is attributed");
    return reinterpret_cast<void*>(0x5678);
}
