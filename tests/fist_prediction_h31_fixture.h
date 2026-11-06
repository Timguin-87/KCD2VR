// Only exact retail function bodies are fixture leaves; production predictor
// input, interpolation, sweep and receiver adapters run unchanged.
static unsigned predictionsH31=0,sweepsH31=0,wristReadsH31=0;
static QuatT28 nativeWristH31{{0,0,0,1},{.1f,0,1.2f}},sweptFromH31{},sweptToH31{};
static QuatT28 nativeLeftWristH31{{0,0,0,1},{-.1f,0,1.2f}};
static void* sweepReceiverH31=nullptr;static void* sweepDetailsH31=nullptr;
static bool publishNewerH31=false;
static const QuatT28* __fastcall wristGetterH31(void*,int id){++wristReadsH31;return id==408?&nativeWristH31:id==443?&nativeLeftWristH31:nullptr;}
static QuatT28* __fastcall interpolateH31(QuatT28* out,const QuatT28*,const QuatT28*,const float*,float,float){return out;}
static void* __fastcall nativeSweepH31(void* collisions,int,int,const QuatT28* previous,const QuatT28* current,float){
    ++sweepsH31;sweptFromH31=*previous;sweptToH31=*current;
    if(publishNewerH31){CommittedFrame6695 newer{};FistContactFrameH11 later{};
        assert(g_committed_frame6695.read(newer)&&g_fist_contact_h11.read(later));++newer.stamp.sample;++later.sample;
        later.previous[1]=later.palm[1];later.previous[1].x+=.2;later.palm[1].x+=.2;
        g_committed_frame6695.publish(newer);g_fist_contact_h11.publish(later);}
    auto* point=static_cast<float*>(sweepDetailsH31);for(int i=0;i<3;++i)point[i]=current->t[i];
    const auto prior=hostReturnAddressValue;hostReturnAddressValue=reinterpret_cast<void*>(WhBase()+0xC52534);
    n::FistReceiverDetourH12(sweepReceiverH31,sweepDetailsH31);hostReturnAddressValue=prior;
    return static_cast<unsigned char*>(collisions)+0x98;
}
static void* __fastcall nativePredictionH31(void* wrapper,float dt){
    ++predictionsH31;auto* s=g_prediction_scope120;if(!s)return nullptr;
    DetourModKit::hook::MidContext mid{};
    DetourModKit::hook::gpr(mid,DetourModKit::hook::Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(s->action);
    DetourModKit::hook::gpr(mid,DetourModKit::hook::Gpr::R12)=reinterpret_cast<std::uintptr_t>(wrapper);PredictionMid120(mid);
    if(!s->redirected)return nullptr;
    QuatT28 interpolated{};const float axis[3]{1,0,0};
    PredictionInterpolate120(WhBase()+0xA7C96D,&interpolated,&s->input.previous,&s->input.target,axis,.01f,1.f);
    const auto prior=hostReturnAddressValue;hostReturnAddressValue=reinterpret_cast<void*>(WhBase()+0xA7C9C1);
    void* result=PhysNativeSweepProbe110(s->collisions,s->hand,s->bone,&s->input.previous,&interpolated,dt);
    hostReturnAddressValue=prior;return result;
}
