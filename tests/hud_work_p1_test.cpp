#include "ui_prompt_scale120r3.h"
#include "ui_hud_work59c7.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <limits>
using namespace kcd2vr;
namespace {
using Notifications=ui_prompt_scale120r3::Notifications;
struct Backend {
    std::array<ui_native_canvas::Geometry,6> geometry{};
    std::array<unsigned,6> resolveCalls{};
    std::array<std::uint64_t,6> revision{{2,2,2,2,2,2}};
    unsigned present=63,gets=0,sets=0;
    int badSet=-1;
    Backend(){for(auto&g:geometry){g.xs=g.ys=100;}}
    bool Resolve(unsigned i,const char*,std::uintptr_t& id){++resolveCalls[i];id=i+1;return present&(1u<<i);}
    bool Get(unsigned i,ui_native_canvas::Geometry& out){++gets;out=geometry[i];return true;}
    bool Set(unsigned i,ui_native_canvas::Geometry value){++sets;if(int(i)==badSet)return false;geometry[i]=value;return true;}
    std::uint64_t Revision(unsigned i){return revision[i];}
    unsigned resolves()const{unsigned n=0;for(auto i:resolveCalls)n+=i;return n;}
};
struct Harness {
    ui_hud_work59c7::SizedChildren<6> children;
    ui_hud_work59c7::ChildWork<6> work;
    std::array<ui_hud_visibility175::Key,6> keys{};
    Notifications tx;Backend io;unsigned transactions=0;
    Harness(){for(unsigned i=0;i<6;++i){keys[i]={i+1,i+10,i+20,40,50};children.Observe(i,keys[i]);}}
    unsigned Tick(std::uint64_t now,bool enabled=true,std::uint64_t generation=1){
        std::uint64_t base=1469598103934665603ull;
        ui_hud_work59c7::Mix(base,generation);ui_hud_work59c7::Mix(base,enabled);
        auto mask=work.Due(base,now,children);
        if(mask){++transactions;(void)tx.Run(io,generation,enabled,mask);const auto&r=tx.LastReport();work.Complete(mask,r.resolved,r.verified);}
        return mask;
    }
};
void Churn(int kind){
    Harness h;
    for(unsigned frame=0;frame<600;++frame){
        if(frame&&kind){if(kind==1)h.io.geometry[0].xs=h.io.geometry[0].ys=100;
            assert(h.children.NativeScaleWrite(h.keys[0]));}
        const auto mask=h.Tick(1000+frame*16);
        assert(mask==(frame?(kind?1u:0u):63u));
    }
    assert(h.io.resolves()==(kind?605u:6u));
    assert(h.io.sets==(kind==1?605u:6u));
    assert(h.children.Revision(0)==2);
    for(unsigned i=1;i<6;++i)assert(h.io.resolveCalls[i]==1);
    assert(h.work.Ok());
    std::printf("P1 scenario=%d frames=600 transactions=%u resolves=%u gets=%u sets=%u\n",kind,h.transactions,h.io.resolves(),h.io.gets,h.io.sets);
}
}
int main(){
    Churn(0);Churn(1);Churn(2);
    Harness missing;missing.io.present&=~2u;
    for(unsigned n=0;n<600;++n){if(n){missing.io.geometry[0].xs=100;missing.children.NativeScaleWrite(missing.keys[0]);}missing.Tick(1000+n*16);}
    assert(missing.io.resolveCalls[1]==3); // other animation cannot renew failed child's budget
    assert(missing.work.Ok()); // optional absence remains historically nonfatal
    missing.io.present|=2u;missing.children.Observe(1,{100,101,102,40,50});++missing.io.revision[1];
    assert(missing.Tick(12000)==2);assert(std::fabs(missing.io.geometry[1].xs-30)<.02f);assert(missing.work.Ok());
    Harness fail;fail.io.badSet=1;assert(fail.Tick(1000)==63);assert(!fail.work.Ok());
    assert(fail.Tick(1100)==0);assert(fail.Tick(1200)==2);assert(!fail.work.Ok());
    fail.io.badSet=-1;assert(fail.Tick(1400)==2);assert(fail.work.Ok());assert(fail.Tick(1600)==0);
    Harness failedChurn;failedChurn.io.badSet=1;
    for(unsigned n=0;n<600;++n){if(n){failedChurn.io.geometry[0].xs=100;failedChurn.children.NativeScaleWrite(failedChurn.keys[0]);}failedChurn.Tick(1000+n*16);}
    assert(failedChurn.io.resolveCalls[1]==3&&!failedChurn.work.Ok());
    assert(std::fabs(failedChurn.io.geometry[0].xs-30)<.02f);
    // A newer native event arriving after scheduling stays pending after completion.
    Harness race;std::uint64_t base=1469598103934665603ull;ui_hud_work59c7::Mix(base,1);ui_hud_work59c7::Mix(base,true);
    auto mask=race.work.Due(base,1000,race.children);assert(mask==63);
    race.children.NativeScaleWrite(race.keys[2]);assert(race.tx.Run(race.io,1,true,mask));
    race.work.Complete(mask,race.tx.LastReport().resolved,race.tx.LastReport().verified);
    assert(race.Tick(1001)==4);assert(race.Tick(1002)==0);
    // Production AfterAcquire replans every child if HUD acquisition advances
    // the lifetime after the worker scheduled one dirty child.
    Harness replacedHud;assert(replacedHud.Tick(1000)==63);
    replacedHud.children.NativeScaleWrite(replacedHud.keys[0]);
    const auto oldWork=replacedHud.work.Due(base,1001,replacedHud.children);assert(oldWork==1);
    std::uint64_t newBase=1469598103934665603ull;ui_hud_work59c7::Mix(newBase,2);ui_hud_work59c7::Mix(newBase,true);
    const auto newWork=replacedHud.work.AfterAcquire(oldWork,base,newBase,1001,replacedHud.children);assert(newWork==63);
    for(auto&g:replacedHud.io.geometry){g.xs=80;g.ys=120;}
    assert(replacedHud.tx.Run(replacedHud.io,2,true,newWork));
    replacedHud.work.Complete(newWork,replacedHud.tx.LastReport().resolved,replacedHud.tx.LastReport().verified);
    for(unsigned i=0;i<6;++i){assert(replacedHud.io.geometry[i].xs==80*Notifications::Scales[i]);assert(replacedHud.io.geometry[i].ys==120*Notifications::Scales[i]);}
    assert(replacedHud.Tick(1002,true,2)==0);
    assert(replacedHud.Tick(1003,false,2)==63);for(auto&g:replacedHud.io.geometry){assert(g.xs==80&&g.ys==120);}
    assert(!replacedHud.tx.Active());
    Harness life;assert(life.Tick(1000)==63);assert(life.Tick(1001)==0);
    // Request changes restore/apply all owned children immediately.
    assert(life.Tick(1002,false)==63);for(auto&g:life.io.geometry)assert(g.xs==100&&g.ys==100);
    assert(!life.tx.Active());assert(life.Tick(1003,true)==63);
    life.children.Remove(life.keys[3].wrapper);life.io.present&=~8u;
    assert(life.Tick(1004)==8);assert(life.work.Ok());
    auto newKey=life.keys[3];newKey.handle=888;newKey.object=999;life.children.Observe(3,newKey);
    ++life.io.revision[3];life.io.present|=8u;life.io.geometry[3].xs=200;life.io.geometry[3].ys=240;
    assert(life.Tick(1005)==8);assert(life.io.geometry[3].xs==100&&life.io.geometry[3].ys==120);
    for(auto&g:life.io.geometry){g.xs=80;g.ys=120;}
    assert(life.Tick(1006,true,2)==63);for(unsigned i=0;i<6;++i){assert(life.io.geometry[i].xs==80*Notifications::Scales[i]);assert(life.io.geometry[i].ys==120*Notifications::Scales[i]);}
    assert(life.children.WorkRevision(6)==0);
    std::puts("PASS current production per-child notification work: unchanged-child0queries; no delay; bounded independent failure; native event race; native lifetime; restoration");
}
