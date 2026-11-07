// Current production transaction/registry; native Flash leaves are mocked.
#include "ui_prompt_scale120r3.h"
#include "ui_hud_work59c7.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <limits>
using namespace kcd2vr;
namespace {
unsigned checks=0;
void check(bool v,const char* label){++checks;if(!v){std::fprintf(stderr,"FAIL %s\n",label);std::abort();}}
bool near(float a,float b){return std::fabs(a-b)<.02f;}
struct Backend {
    std::array<ui_native_canvas::Geometry,6> value{};
    std::array<std::uintptr_t,6> ids{{1,2,3,4,5,6}};
    std::array<std::uint64_t,6> revisions{{1,1,1,1,1,1}};
    unsigned present=63,sets=0;bool moveOnGet=false;int failSets=0;
    bool Resolve(unsigned i,const char* name,std::uintptr_t& id){
        assert(i<6);assert(std::strcmp(name,ui_prompt_scale120r3::Notifications::Names[i])==0);
        id=ids[i];return (present&(1u<<i))!=0;
    }
    bool Get(unsigned i,ui_native_canvas::Geometry& g){if(moveOnGet){value[i].x+=3;value[i].y-=2;}g=value[i];return true;}
    bool Set(unsigned i,ui_native_canvas::Geometry g){
        ++sets;if(failSets>0){--failSets;return false;}
        // Same flags used by the real notification backend; native movement survives.
        const auto packed=ui_prompt_scale120r3::Notifications::PackScale(g);
        std::uint16_t flags=0;std::memcpy(&flags,packed.data()+0x34,2);assert(flags==0x18);
        std::memcpy(&value[i].xs,packed.data()+12,4);std::memcpy(&value[i].ys,packed.data()+16,4);return true;
    }
    std::uint64_t Revision(unsigned i)const{return revisions[i];}
};
struct PromptBackend {
    std::array<ui_native_canvas::Geometry,2> value{};
    bool Resolve(unsigned i,const char* name,std::uintptr_t& id){id=i+1;return std::strcmp(name,i?"TutorialMessage":"Hints")==0;}
    bool Get(unsigned i,ui_native_canvas::Geometry& g){g=value[i];return true;}
    bool Set(unsigned i,ui_native_canvas::Geometry g){value[i]=g;return true;}
};
}
int main(){
    using T=ui_prompt_scale120r3::Notifications;
    Backend b;T t;for(unsigned i=0;i<6;++i)b.value[i]={float(i*10),float(i*11),100,150,0};
    check(t.Run(b,1,true),"all six notifications apply");
    for(unsigned i=0;i<6;++i){check(near(b.value[i].xs,100*T::Scales[i])&&near(b.value[i].ys,150*T::Scales[i]),"half prior intended size");
        check(near(b.value[i].x,float(i*10))&&near(b.value[i].y,float(i*11)),"position is not owned");}
    check(t.LastReport().resolved==T::All&&t.LastReport().written==T::All&&t.LastReport().verified==T::All,"receipt covers every actual child");
    const auto initialSets=b.sets;for(int n=0;n<100;++n)check(t.Run(b,1,true),"repeat remains valid");
    check(b.sets==initialSets,"refresh never compounds scale");
    b.value[0].xs=100;check(t.Run(b,1,true),"same-child x reset repaired");
    check(near(b.value[0].xs,30)&&near(b.value[0].ys,45),"one-axis reset does not shrink other axis twice");
    b.value[0].ys=200;check(t.Run(b,1,true),"same-child y reset repaired");
    check(near(b.value[0].xs,30)&&near(b.value[0].ys,60),"new native y baseline preserved independently");
    b.value[1].xs=120;b.value[1].ys=180;check(t.Run(b,1,true),"full native reset repaired");
    check(near(b.value[1].xs,36)&&near(b.value[1].ys,54),"native resize uses new baseline");
    b.present&=~(1u<<2);check(t.Run(b,1,true),"temporary child absence tolerated");
    check(!(t.LastReport().resolved&(1u<<2)),"missing child not falsely reported as resolved");
    b.present=T::All;check(t.Run(b,1,true),"same child returns");
    check(near(b.value[2].xs,30)&&near(b.value[2].ys,45),"temporary absence retains baseline");
    ++b.revisions[2];b.value[2].xs=200;b.value[2].ys=220;check(t.Run(b,1,true),"reused object address with new binding revision");
    check(near(b.value[2].xs,60)&&near(b.value[2].ys,66),"new child has own baseline");
    b.moveOnGet=true;b.value[3].xs=110;check(t.Run(b,1,true),"animated position does not fail scale readback");
    check(near(b.value[3].xs,55)&&near(b.value[3].ys,75),"moving message scale repaired");
    check(t.Run(b,1,false),"disable restores unscaled baselines while native position moves");
    check(near(b.value[0].xs,100)&&near(b.value[0].ys,200)&&near(b.value[1].xs,120)&&near(b.value[2].xs,200)&&near(b.value[3].xs,110),"restore uses independently observed native axes");
    check(!t.Active(),"ownership released");
    b.moveOnGet=false;for(auto& g:b.value)g={0,0,80,120,0};
    check(t.Run(b,2,true),"new HUD generation applies");check(near(b.value[0].xs,24)&&near(b.value[5].ys,60),"new generation never restores old baseline");
    Backend failed;T ft;failed.failSets=2;check(!ft.Run(failed,1,true),"write and rollback failure reported");
    check(ft.Active()&&!(ft.LastReport().verified&1),"uncertain write keeps repair ownership without verified receipt");
    check(ft.Run(failed,1,true)&&near(failed.value[0].xs,30),"bounded retry recovers failed write");
    failed.value[0].xs=std::numeric_limits<float>::quiet_NaN();const auto before=failed.sets;
    check(!ft.Run(failed,1,true)&&failed.sets==before,"invalid geometry never written");
    ui_hud_work59c7::SizedChildren<6> children;
    ui_hud_visibility175::Key key{10,20,30,40,50};auto epoch=children.Epoch();
    children.Observe(5,key);check(children.Epoch()==epoch+1&&children.Revision(5)==2,"sixth notification has independent tracked identity");
    epoch=children.Epoch();const auto revision=children.Revision(5);
    check(children.NativeScaleWrite(key)&&children.Epoch()==epoch+1&&children.Revision(5)==revision,"native scale write dirties work only");
    auto wrong=key;++wrong.handle;epoch=children.Epoch();
    check(!children.NativeScaleWrite(wrong)&&children.Epoch()==epoch,"foreign handle cannot dirty known child");
    children.Observe(6,key);check(children.Revision(6)==0&&children.Epoch()==epoch,"out-of-range index rejected");
    children.Remove(key.wrapper);check(!children.NativeScaleWrite(key),"destroyed child cannot request work");
    ui_hud_work59c7::Retry retry;check(retry.Due(1,1000),"first event starts work");retry.Complete(true);
    check(!retry.Due(1,3000)&&retry.Due(2,3001),"new native event wakes completed work");
    ui_hud_work59c7::CaptureBudget capture;for(unsigned i=0;i<8;++i)check(capture.Take(true,1,true),"finite capture slot");
    check(!capture.Take(true,1,true)&&!capture.Take(false,2,true)&&capture.Take(true,2,true),"capture bounded and off quiet");
    const auto bytes=T::PackScale({17,19,31,37,0});std::uint16_t flags=0;std::memcpy(&flags,bytes.data()+0x34,2);
    check(flags==0x18,"native packet owns scale only, no fade/visibility/position");
    PromptBackend prompts;ui_prompt_scale120r3::Transaction hints;
    check(hints.Run(prompts,1,true,.75f,.8f)&&near(prompts.value[0].xs,75)&&near(prompts.value[1].ys,80),"existing hint factors unchanged");
    check(hints.Run(prompts,1,true,.75f,.8f)&&near(prompts.value[0].xs,75),"existing hint idempotence unchanged");
    check(hints.Run(prompts,1,false,.75f,.8f)&&near(prompts.value[0].xs,100)&&near(prompts.value[1].ys,100),"existing hint restoration unchanged");
    std::printf("hud_notifications_d59r1=%u/%u; native/headset=NOT_EXECUTED\n",checks,checks);
}
