#include "subsystems/dialogues/ratio_strips_d79.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
namespace d=kcd2vr::dialogue_ratio_d79;
unsigned checks=0;void check(bool x,const char* s){++checks;if(!x){std::fprintf(stderr,"FAIL %s\n",s);std::exit(1);}}
struct Call{bool visible;std::uint64_t expected;};
struct IO{std::vector<Call> calls;bool good=true;std::uint64_t generation=1;
 d::Result operator()(bool v,std::uint64_t e){calls.push_back({v,e});return {good,generation};}};
int main(){
 using namespace kcd2vr;
 dialogue_state59c10r2::State s{};s.tick=100;s.known=s.active=true;s.entity=11;s.instance=12;s.camera=dialogue_state59c10r2::Camera::FirstPerson;
 ui_menu_router::Decision ui{};
 auto wanted=[&](int mode=2,bool theater=false,bool matrix=false,bool reference=false,bool activity=false,std::uint64_t now=100){return d::Wanted(s,mode,theater,matrix,reference,activity,ui,now);};
 check(wanted(),"fresh exact first-person conversation");check(wanted(1),"legacy mode only with actual FirstPerson");check(!wanted(0),"native dialogue untouched");
 check(!wanted(2,true),"Theater cutscene untouched");check(!wanted(2,false,true),"matrix untouched");check(!wanted(2,false,false,true),"native reference untouched");check(!wanted(2,false,false,false,true),"station untouched");
 check(!wanted(2,false,false,false,false,181),"stale conversation cannot hide");check(!wanted(2,false,false,false,false,99),"future witness cannot hide");
 s.active=false;check(!wanted(),"speech without active conversation untouched");s.active=true;s.known=false;check(!wanted(),"unknown conversation untouched");s.known=true;s.instance=0;check(!wanted(),"no instance untouched");s.instance=12;
 s.camera=dialogue_state59c10r2::Camera::Dialogue;check(!wanted(),"authored dialogue camera untouched");s.camera=dialogue_state59c10r2::Camera::Unknown;check(!wanted(),"unknown camera untouched");s.camera=dialogue_state59c10r2::Camera::FirstPerson;
 ui.owner=ui_menu_router::Owner::Pause;check(!wanted(),"pause untouched");ui.owner=ui_menu_router::Owner::None;ui.uncertain=1;check(!wanted(),"uncertain UI untouched");ui.uncertain=0;
 {d::Lease lease;IO io;for(unsigned n=1;n<100;++n)lease.Update(false,0,1,n,io);check(io.calls.empty(),"inactive no native work");}
 {d::Lease lease;IO io;lease.Update(true,12,1,100,io);check(io.calls.size()==1&&!io.calls.back().visible&&io.calls.back().expected==0,"immediate entry hide");
 lease.Update(true,12,1,179,io);check(io.calls.size()==1,"no entry polling");lease.Update(true,12,1,180,io);check(io.calls.size()==2,"80ms reassert even success");
 lease.Update(true,12,1,299,io);check(io.calls.size()==2,"200ms not yet due");lease.Update(true,12,1,300,io);check(io.calls.size()==3,"final200ms reassert");lease.Update(true,12,1,99999,io);check(io.calls.size()==3,"no keepalive");
 lease.Update(false,0,1,100000,io);check(io.calls.size()==4&&io.calls.back().visible&&io.calls.back().expected==1,"exit immediate same-generation restore");lease.Update(false,0,1,100080,io);check(io.calls.size()==4,"restore successful no repeated show");}
 {d::Lease lease;IO io;lease.Update(true,12,1,100,io);io.generation=2;lease.Update(true,12,2,101,io);check(io.calls.size()==2&&!io.calls.back().visible,"replacement HUD immediate hide");
 io.generation=3;lease.Update(false,0,3,102,io);check(io.calls.size()==2,"exit never shows untouched replacement");}
 {d::Lease lease;IO io;lease.Update(true,12,1,100,io);lease.Update(true,13,1,101,io);check(io.calls.size()==2,"new conversation same HUD reasserts");lease.Update(false,0,1,102,io);check(io.calls.back().visible,"first cutscene worker tick releases");}
 {d::Lease lease;IO io;io.good=false;for(auto n:{100,180,300,400,100000})lease.Update(true,12,1,n,io);check(io.calls.size()==3,"failures bounded");lease.Update(false,0,1,100001,io);check(io.calls.size()==3,"failed hide never forces visible");}
 {d::Lease lease;IO io;lease.Update(true,12,1,100,io);io.good=false;for(auto n:{101,181,301,401,501,701})lease.Update(false,0,1,n,io);check(io.calls.size()==5,"exit failures back off but retain restoration debt");io.good=true;lease.Update(false,0,1,901,io);check(io.calls.size()==6&&io.calls.back().visible,"four failures cannot abandon cutscene restoration");lease.Update(false,0,1,1101,io);check(io.calls.size()==6,"successful late restoration retires debt");}
 {d::Lease lease;IO io;lease.Update(true,12,1,100,io);io.good=false;lease.Update(false,0,1,101,io);io.generation=2;io.good=true;lease.Update(false,0,2,102,io);check(io.calls.size()==2,"later HUD never restores old ownership");}
 {d::Lease lease;IO io;io.generation=2;lease.Update(true,12,1,100,io);lease.Update(true,12,2,101,io);check(io.calls.size()==1,"acquire replacement generation adopted without extra entry reset");lease.Update(false,0,2,102,io);check(io.calls.back().expected==2,"restores actual acquired generation");}
 {d::Lease lease;IO io;lease.Update(true,12,1,100,io);lease.Update(true,12,1,99,io);check(io.calls.size()==1,"clock rollback no new writes");}
 std::printf("D79 dialogue ratio PASS %u checks\n",checks);
}
