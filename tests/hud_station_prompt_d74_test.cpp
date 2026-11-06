#define KCD2VR_D69_NO_MAIN
#include "hud_item_prompt_d69_test.cpp"
#include "ui_station_prompts_d74.h"
#include "diagnostics/capture168.h"
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {std::abort();}}
namespace kcd2vr::flashui_probe {
static bool HudCaptureActive175(){return false;}
#include "generated/station_prompt_d74.inl"
static int runD74(){
 nativeD69();
 UpdateReadablePromptsD74(true,false,.75f,.5f,1000);
 UpdateReadablePromptsD74(true,false,.75f,.5f,1001); // first binding changes generation key
 ckD69(nearD69(clipsD69[0].value.xs,75)&&nearD69(clipsD69[2].value.xs,50),"gameplay user scales");
 unsigned before=acquiresD69;
 for(unsigned i=0;i<1000;++i)UpdateReadablePromptsD74(true,false,.75f,.5f,1200+i);
 ckD69(acquiresD69==before,"1000 idle service calls make no native acquisitions");
 UpdateReadablePromptsD74(true,true,.75f,.5f,2300);
 for(unsigned i=0;i<4;++i){const float factor=i==0?.75f:.5f;ckD69(nearD69(clipsD69[i].value.xs,100*factor)&&nearD69(clipsD69[i].value.ys,150*factor)&&nearD69(clipsD69[i].value.x,float(i*10)),"D75 station honors saved HUD scales and preserves native aspect/position");}
 UpdateReadablePromptsD74(true,true,.40f,.30f,2350);
 ckD69(nearD69(clipsD69[0].value.xs,40)&&nearD69(clipsD69[2].value.xs,30),"D75 station sliders change current semantic children");
 UpdateReadablePromptsD74(true,true,.75f,.5f,2360);
 // Same wrapper, a native timeline writes one scale axis: observed work epoch
 // reopens service without treating our own output as a new native baseline.
 clipsD69[0].value.xs=120;
 ui_hud_visibility175::Key key{};ckD69(ReadHudKey175(&clipsD69[0],currentHudD69,HudGeneration(),key),"native same-wrapper key");
 ckD69(g_prompt_children_d74.NativeScaleWrite(key),"native prompt reset observed");
 UpdateReadablePromptsD74(true,true,.75f,.5f,2400);
 ckD69(nearD69(clipsD69[0].value.xs,90)&&nearD69(clipsD69[0].value.ys,112.5f),"same-child reset repaired without compounded y scale");
 UpdateReadablePromptsD74(true,false,.75f,.5f,2500);
 ckD69(nearD69(clipsD69[0].value.xs,90)&&nearD69(clipsD69[2].value.xs,50),"station exit restores current gameplay factors");
 UpdateReadablePromptsD74(false,false,.75f,.5f,2600);
 ckD69(nearD69(clipsD69[0].value.xs,120)&&nearD69(clipsD69[2].value.xs,100),"owner exit restores native scales");
 availableD69=false;++hudGenerationD69;before=acquiresD69;
 for(unsigned i=0;i<1000;++i)UpdateReadablePromptsD74(true,true,.75f,.5f,3000+i);
 ckD69(acquiresD69-before==3,"missing HUD has three bounded attempts then no polling");
 availableD69=true;++hudGenerationD69;currentHudD69=reinterpret_cast<void*>(0x3000);nativeD69(80,120);
 UpdateReadablePromptsD74(true,true,.75f,.5f,4500);
 ckD69(nearD69(clipsD69[0].value.xs,60)&&nearD69(clipsD69[2].value.ys,60),"new movie takes own baseline");
 ckD69(acquiresD69==releasesD69,"all HUD holders released");
 std::printf("D75 production station service %u checks PASS; native leaves mocked; headset NOT_RUN\n",checksD69);return 0;
}
}
int main(){return kcd2vr::flashui_probe::runD74();}
