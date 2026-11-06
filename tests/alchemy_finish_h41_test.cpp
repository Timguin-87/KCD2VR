// Actual production detour body and reads; original native recipe leaf modeled.
#include "../src/subsystems/alchemy/finish_observer_cb1.cpp"
#include <cassert>
namespace af=kcd2vr::alchemy_finish_cb1;
namespace kcd2vr::diagnostic_trace {void Write(const char*)noexcept{}}
namespace kcd2vr::retail_build112 {bool Ready(std::uintptr_t)noexcept{return false;}bool Verify(std::uintptr_t)noexcept{return false;}}
static int calls=0,checks=0,readerCalls=0;static bool verdict=true,changed=false,throws=false,currentKnown=true,stopCapture=false;
static af::Context current{0x1000,0x2000,0x3000,0x4000,11};
static bool reader(af::Context&out)noexcept{++readerCalls;out=current;return currentKnown;}
static bool __fastcall original(void*m,void*s,const unsigned char*d,unsigned char*c,int*n){++calls;if(throws)throw 17;if(changed)++current.lifetime;if(n)*n=3;
 if(stopCapture)kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,GetTickCount64());return verdict;}
#define CK(x) do{++checks;assert(x);}while(0)
int main(){af::g_original=original;af::SetCurrentReader(reader);unsigned char data[48]{};int recipe=19;std::memcpy(data,&recipe,4);int count=0;af::Result r{};
 CK(!af::Enabled());CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==1&&af::Count()==0);af::SetEnabled(true);
 CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==2&&af::Count()==1&&af::Last(r));CK(r.valid&&r.success&&r.dataRead&&r.recipeId==19&&r.outCount==3&&r.context.lifetime==11);
 verdict=false;CK(!af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==3&&af::Count()==2&&af::Last(r)&&!r.success);
 // A recipe verdict never depends on created items/delivery. Null diagnostic
 // data merely makes dataRead unknown; it never changes the native return.
 verdict=true;CK(af::Detour(nullptr,(void*)current.soul,nullptr,nullptr,nullptr));CK(af::Last(r)&&r.success&&!r.dataRead&&calls==4);
 CK(af::Detour(nullptr,(void*)0xBAD,data,nullptr,&count));CK(calls==5&&af::Count()==3);
 currentKnown=false;CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==6&&af::Count()==3);currentKnown=true;
 changed=true;CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==7&&af::Count()==3);changed=false;
 throws=true;bool caught=false;try{af::Detour(nullptr,(void*)current.soul,data,nullptr,&count);}catch(int e){caught=e==17;}CK(caught&&calls==8&&af::Count()==3);throws=false;
 af::SetEnabled(false);CK(!af::Last(r));CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==9&&af::Count()==3);
 // D42: model the existing successful hook-install leaf only. The real
 // production detour and finite capture/session rechecks execute unchanged.
 namespace cap=kcd2vr::diagnostics::capture168;af::g_installed=true;
 cap::gate.select(cap::Domain::Interaction,true,GetTickCount64());
 const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load();af::SetCaptureSessionD42(session);
 CK(!af::Enabled());const auto popups=kcd2vr::visible_failure::nextSerial.load();
 CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==10&&af::Count()==4&&af::Last(r));
 CK(kcd2vr::visible_failure::nextSerial.load()==popups); // F5 alone is silent in the game
 stopCapture=true;CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));stopCapture=false;
 CK(calls==11&&af::Count()==4&&!af::Last(r)); // stop during original cannot publish an old session
 const auto readsBefore=readerCalls;CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));
 CK(calls==12&&readerCalls==readsBefore&&af::Count()==4); // expired F5 does no context read
 cap::gate.select(cap::Domain::Interaction,true,GetTickCount64());
 CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==13&&af::Count()==4); // old session cannot borrow new lease
 af::SetCaptureSessionD42(cap::gate.session[unsigned(cap::Domain::Interaction)].load());
 CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==14&&af::Count()==5&&af::Last(r));
 af::SetCaptureSessionD42(0);CK(!af::Last(r));af::SetEnabled(true);
 cap::gate.select(cap::Domain::Interaction,false,GetTickCount64());
 CK(af::Detour(nullptr,(void*)current.soul,data,nullptr,&count));CK(calls==15&&af::Count()==6&&af::Last(r));
 CK(kcd2vr::visible_failure::nextSerial.load()>popups); // explicit INI feedback retained
 af::SetEnabled(false);af::g_installed=false;
 std::printf("H41 actual alchemy detour checks=%d PASS; original calls=%d\n",checks,calls);
}
