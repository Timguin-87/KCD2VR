#define main retained_bounds_main_f7_h29r4
#include "render_bounds63_adapter_test.cpp"
#undef main
#include <string>
namespace vd4=kcd2vr::diagnostics::vrik_h29;
namespace cap4=kcd2vr::diagnostics::capture168;
static std::vector<vd4::Record> records4(){std::vector<vd4::Record> out;vd4::Record r{};while(vd4::pop(r))out.push_back(r);return out;}
int main(){
 reset63();cap4::gate.select(cap4::Domain::Vrik,false,now63);const auto off=call63();const rb63::Box offReturned=*reinterpret_cast<const rb63::Box*>(off);
 const auto offReads=memoryReads63;const auto offNative=memory63;
 check63(off!=native63()&&records4().empty(),"F7 OFF actual renderer retains union with no record");
 reset63();cap4::gate.select(cap4::Domain::Vrik,true,now63);const auto on=call63();auto out=records4();
 check63(on!=native63()&&out.size()==1,"F7 ON successful real exact-caller query recorded");const auto r=out.at(0);
 check63(r.kind==vd4::Kind::Render&&r.reason==-1&&r.renderData&&r.sourceTick==1000&&r.sample==100&&r.epoch==2&&r.publication==8,
  "render use joins exact originally published pose/envelope, no fresh relabel");
 check63(r.nativeMax.y==.25f&&r.allowedMax.y>.95&&r.allowedMax.y<.97,"copied actual native and returned union geometry, no visibility claim");
 check63(std::memcmp(&offReturned,on,sizeof(offReturned))==0&&memory63==offNative&&memoryReads63==offReads&&offReads==3&&originalCalls63==1,
  "F7 ON/OFF returns byte-identical bounds, immutable native bytes, same3reads/originalonce");
 reset63();now63=1081;cap4::gate.select(cap4::Domain::Vrik,true,now63);call63();out=records4();
 check63(out.size()==1&&out[0].reason==int(BoundsReject63::EnvelopeFreshness)&&out[0].sourceTick==1000&&out[0].tick==1081,
  "81ms refused render envelope preserves original age and exact failed gate");
 reset63();cap4::gate.select(cap4::Domain::Vrik,true,now63);return63++;call63();
 check63(records4().empty()&&memoryReads63==0&&originalCalls63==1,"foreign render caller cannot produce Henry evidence or extra reads");
 reset63();cap4::gate.select(cap4::Domain::Vrik,true,now63);g_committed_pose_valid6689=false;call63();out=records4();
 check63(out.size()==1&&out[0].reason==int(BoundsReject63::Context)&&!out[0].renderData,"early refusal records context but does not fabricate unread geometry");
 reset63();cap4::gate.select(cap4::Domain::Vrik,true,now63);call63();std::string lines;vd4::drain(now63,[&](const char* p){lines+=p;lines+='\n';});
 check63(lines.find("VRIK_F7 RENDER ")!=lines.npos&&lines.find("meshVisibility=UNPROVEN")!=lines.npos&&lines.find("frustumDecision=UNOBSERVED")!=lines.npos,
  "render getter acceptance explicitly stops short of mesh/frustum visibility proof");
 cap4::gate.select(cap4::Domain::Vrik,false,now63);
 std::printf("VRIK_RENDER_H29R4 %u/%u actual-getter checks; native/headset=NOT_EXECUTED\n",checks63-failures63,checks63);return failures63?1:0;
}
