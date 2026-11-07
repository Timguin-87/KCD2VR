// Runs the complete existing action-owner TU. Only engine/Windows leaves are
// inherited from the retained fixture; the production diagnostic and clock
// decisions below are not copied into the test.
#define KCD2VR_CRAFTING_CLOCK_D81_NO_MAIN
#ifndef _MSC_VER
#define __fastcall
#endif
#include "crafting_clock_d81_test.cpp"

namespace kcd2vr::action_time_owner59c11 {namespace {
static unsigned receiptChecksD84=0;
static void receiptCheckD84(bool ok,const char* why){
    ++receiptChecksD84;if(!ok){std::fprintf(stderr,"D84 presentation receipt: %s\n",why);std::abort();}
}
static std::vector<std::string> presentationReceiptsD84(){
    std::vector<std::string> out;for(const auto& line:lines)
        if(line.find("D84 F5_ALCHEMY_PRESENTATION ")==0)out.push_back(line);
    return out;
}
static void presentationSetupD84(ap81::Kind kind,bool capture=true){
    alchemy81(kind);lines.clear();traceEnabled=true;
    namespace cap=kcd2vr::diagnostics::capture168;
    cap::gate.select(cap::Domain::Interaction,false,1000);
    if(capture)cap::gate.select(cap::Domain::Interaction,true,1000);
}
static int presentationCasesD84(){
    for(auto kind:{ap81::Kind::Transfer,ap81::Kind::Book,ap81::Kind::Resource}){
        presentationSetupD84(kind,false);UpdateDetour(controller.data(),.01f);
        receiptCheckD84(presentationReceiptsD84().empty(),"F5 off produces no diagnostic");
        receiptCheckD84(g_alchemy_presentation_pending_d81&&originalDt81==.01f,"F5 off preserves actual native clock decision");
        presentationSetupD84(kind);UpdateDetour(controller.data(),.01f);auto rows=presentationReceiptsD84();
        receiptCheckD84(rows.size()==1&&rows[0].find("result=APPLIED")!=std::string::npos&&
            rows[0].find("appliedRate=8.000")!=std::string::npos,"qualified presentation reports verified actual rate");
        receiptCheckD84(rows[0].find("eventOutcome=NOT_OBSERVED")!=std::string::npos,"clock write never claims recipe/audio outcome");
    }
    presentationSetupD84(ap81::Kind::Transfer);UpdateDetour(controller.data(),.033f);
    auto rows=presentationReceiptsD84();
    receiptCheckD84(rows.size()==1&&rows[0].find("appliedRate=2.424")!=std::string::npos,"80ms native-step bound is visible");
    presentationSetupD84(ap81::Kind::Resource);semi46::WorkWitness owner{};
    {OwnerTransaction19c4 tx;AlchemyUpdateScopeR27 scope(controller.data());
        receiptCheckD84(SemiAlchemyParent46(controller.data(),owner),"current resource owner is qualified");}
    g_semi_resource46.owner=owner;UpdateDetour(controller.data(),.01f);rows=presentationReceiptsD84();
    receiptCheckD84(rows.size()==1&&rows[0].find("result=ATTACHED_RESOURCE_WORK")!=std::string::npos&&
        rows[0].find("appliedRate=0.000")!=std::string::npos,"attached gesture work reports no presentation write");
    presentationSetupD84(ap81::Kind::Transfer);testWorkstationFrame46.tick=900;
    UpdateDetour(controller.data(),.01f);rows=presentationReceiptsD84();
    receiptCheckD84(rows.size()==1&&rows[0].find("result=STALE_OR_UNOWNED_TRACKING")!=std::string::npos,"stale input refusal is distinguishable");
    presentationSetupD84(ap81::Kind::Transfer);put(action.data(),kActSpeedBias,.6f);
    UpdateDetour(controller.data(),.01f);rows=presentationReceiptsD84();
    receiptCheckD84(rows.size()==1&&rows[0].find("result=NATIVE_CHILD_CHANGED")!=std::string::npos&&
        Rd<float>(action.data(),kActSpeedBias,-1)==.6f,"foreign rate is never called native1x or overwritten");
    namespace cap=kcd2vr::diagnostics::capture168;
    presentationSetupD84(ap81::Kind::Transfer);cap::gate.select(cap::Domain::Vrik,true,1000);
    UpdateDetour(controller.data(),.01f);
    receiptCheckD84(presentationReceiptsD84().size()==1,"F5 and F7 can coexist");
    cap::gate.select(cap::Domain::Interaction,false,1000);lines.clear();
    NoteAlchemyPresentationD84(owner,ap81::Kind::Resource,AlchemyPresentationResultD84::Applied,8,8,.01f,1,1000);
    receiptCheckD84(presentationReceiptsD84().empty(),"F7 alone does not keep F5 receipts alive");
    cap::gate.select(cap::Domain::Vrik,false,1000);
    std::printf("D84 actual alchemy presentation receipts: %u checks PASS; native leaves substituted; headset NOT_RUN\n",receiptChecksD84);
    return 0;
}
}}
int main(){return kcd2vr::action_time_owner59c11::presentationCasesD84();}
