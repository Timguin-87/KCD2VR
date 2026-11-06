#include "../src/core/native_door_script19c4r4.h"
#include <cassert>
#include <vector>
#include <string>
using namespace kcd2vr::door_script19c4r4;
struct IO {
 bool permitted=true,haveHand=true,send=true,haveAck=false;Point point{};Ack answer{};std::vector<Command> sent;
 bool publish(Command c){if(!send)return false;sent.push_back(c);return true;}
 bool ack(Ack& a){a=answer;return haveAck;}
 bool allowed(const Request&){return permitted;}
 bool hand(const Request&,Point& p){p=point;return haveHand;}
 std::vector<float> achieved64;
 void receipt(const Request&,const char* stage,float value,bool held){
  if(std::string(stage)=="LUA_PROGRESS_READBACK"){assert(held);achieved64.push_back(value);}
 }
 void held(std::uint64_t now,float t=.1f){const auto& c=sent.back();answer={c.sequence,c.request,1,t,now};haveAck=true;}
};
static Request request(){return {1,1000,5,11,12,13,21,0,{}};}
int main(){
 {Owner o;IO io;auto r=request();o.tick(io,r,1000);assert(o.pending&&!o.active&&io.sent.back().phase==1);
  io.held(1001);o.tick(io,r,1001);assert(o.active&&io.sent.back().phase==2&&io.sent.back().fraction==.1f);
  io.point={.11f,0,0};io.held(1010);o.tick(io,r,1010);assert(std::fabs(io.sent.back().fraction-.3f)<.0001f);
  io.point={.055f,0,0};io.held(1020);o.tick(io,r,1020);assert(std::fabs(io.sent.back().fraction-.2f)<.0001f);
  io.permitted=false;o.tick(io,r,1030);assert(!o.active&&io.sent.back().phase==0);
  const auto n=io.sent.size();io.permitted=true;o.tick(io,r,1040);assert(io.sent.size()==n);}
 for(int bad=0;bad<7;++bad){Owner o;IO io;auto r=request();o.tick(io,r,1000);io.held(1001);o.tick(io,r,1001);
  io.held(1010);if(bad==0)io.permitted=false;if(bad==1)io.haveHand=false;if(bad==2)io.answer.tick=1011;
  if(bad==3)io.answer.request=2;if(bad==4)io.answer.source=999;if(bad==5)io.answer.state=0;if(bad==6)io.point={2,0,0};
  o.tick(io,r,1010);assert(!o.active&&io.sent.back().phase==0);}
 {Owner o;IO io;auto r=request();o.tick(io,r,1000);io.held(1001);o.tick(io,r,1001);o.tick(io,r,1082);assert(!o.active&&io.sent.back().phase==0);}
 {Owner o;IO io;auto r=request();o.tick(io,r,1000);o.tick(io,r,1251);assert(!o.pending&&io.sent.back().phase==0);}
 {Owner o;IO io;auto r=request();o.tick(io,r,1000);io.held(1001);o.tick(io,r,1001);r.serial=2;r.tick=1010;
  o.tick(io,r,1010);assert(!o.active&&o.pending&&io.sent[io.sent.size()-2].phase==0&&io.sent.back().request==2);}
 {Owner o;IO io;auto r=request();r.serial=8388608ull;o.tick(io,r,1000);assert(!o.pending&&!o.active);}
 {Owner o;IO io;auto r=request();o.sequence=8388604u;o.tick(io,r,1000);assert(!o.pending&&!o.active&&io.sent.empty());}
 {Owner o;IO io;auto r=request();r.id=16777216;o.tick(io,r,1000);assert(!o.pending&&!o.active);}

 // A controller command alone is not evidence that Lua applied it. A fresh
 // matching readback must arrive first; reverse motion reports its actual
 // fraction too, stationary heartbeats stay silent, and the budget is bounded.
 {Owner o;IO io;auto r=request();o.tick(io,r,1000);io.held(1001);o.tick(io,r,1001);
  io.point={.165f,0,0};io.held(1010);o.tick(io,r,1010);
  assert(io.sent.back().fraction>.39f&&io.achieved64.empty());
  io.held(1020,.4f);o.tick(io,r,1020);assert(io.achieved64.size()==1&&io.achieved64.back()==.4f);
  io.held(1030,.4f);o.tick(io,r,1030);assert(io.achieved64.size()==1);
  io.point={.055f,0,0};io.held(1040,.4f);o.tick(io,r,1040);assert(io.achieved64.size()==1);
  io.held(1050,.2f);o.tick(io,r,1050);assert(io.achieved64.size()==2&&io.achieved64.back()==.2f);
  for(unsigned j=0;j<24;++j){const auto now=1060+10*j;io.held(now,j%2?.2f:.6f);o.tick(io,r,now);}
  assert(io.achieved64.size()==16&&o.active);
  io.answer.request=999;o.tick(io,r,1400);assert(!o.active&&io.achieved64.size()==16);
 }

}
