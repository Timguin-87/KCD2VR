#include "subsystems/alchemy/instant_native_d72.h"
#include <cassert>
#include <cstdio>
#include <utility>
#include <initializer_list>
namespace kcd2vr::native_script_t2 {static bool current=true;bool Context::Current()const noexcept{return current;}}
namespace a=kcd2vr::subsystems::alchemy::instant_d72;
namespace f=kcd2vr::subsystems::alchemy::feedback_d80;
static unsigned calls=0;static f::Event seen{};
static void consume(const kcd2vr::native_script_t2::Context&,const f::Event& e)noexcept{++calls;seen=e;}
int main(){
 a::Receipt r{};r.instance=1;r.lifetime=2;r.serial=3;r.tick=100;r.entered=r.effect=r.completed=r.semanticExecutedD80=true;r.side=0;r.source=7;r.destination=7;r.action=a::Action::TakeIngredient;
 for(int source=7;source<=9;++source){r.source=r.destination=source;assert(a::SoundCueD80(r)==f::Cue::TakeHerb);r.action=a::Action::ReturnIngredient;assert(a::SoundCueD80(r)==f::Cue::ReturnHerb);r.action=a::Action::TakeIngredient;}
 // Return operates on the held source even if current selection moved elsewhere.
 r.action=a::Action::ReturnIngredient;r.destination=6;assert(a::SoundCueD80(r)==f::Cue::ReturnHerb);
 r.action=a::Action::PlaceIngredient;
 for(auto p:{std::pair{0,f::Cue::HerbToBowl},{2,f::Cue::HerbToMortar},{6,f::Cue::HerbToPot}}){r.destination=p.first;assert(a::SoundCueD80(r)==p.second);}
 const auto valid=r;
 for(int fault=0;fault<10;++fault){r=valid;switch(fault){case 0:r.entered=false;break;case 1:r.effect=false;break;case 2:r.completed=false;break;case 3:r.semanticExecutedD80=false;break;case 4:r.serial=0;break;case 5:r.side=1;break;case 6:r.source=13;break;case 7:r.destination=3;break;case 8:r.action=a::Action::PourMortarD75;break;case 9:r.action=a::Action::PlaceBowl;break;}assert(a::SoundCueD80(r)==f::Cue::None);}
 for(int source=10;source<=12;++source){r=valid;r.source=r.destination=source;r.action=a::Action::TakeIngredient;assert(a::SoundCueD80(r)==f::Cue::TakeSpecial);
  r.action=a::Action::ReturnIngredient;assert(a::SoundCueD80(r)==f::Cue::ReturnSpecial);r.action=a::Action::PlaceIngredient;
  for(auto p:{std::pair{0,f::Cue::SpecialToBowl},{2,f::Cue::SpecialToMortar},{6,f::Cue::SpecialToPot}}){r.destination=p.first;assert(a::SoundCueD80(r)==p.second);}}
 for(int source:{0,1,5}){r=valid;r.source=r.destination=source;r.side=source==5?1:0;r.action=a::Action::TakeObject;
  assert(a::SoundCueD80(r)==(source==0?f::Cue::TakeBowl:source==1?f::Cue::TakeHourglass:f::Cue::TakeFlask));
  r.action=a::Action::ReturnObject;assert(a::SoundCueD80(r)==(source==0?f::Cue::ReturnBowl:source==1?f::Cue::ReturnHourglass:f::Cue::ReturnFlask));
  r.completed=false;assert(a::SoundCueD80(r)==f::Cue::None);}
 r=valid;assert(!a::PublishSoundD80(r,4,5));f::consumer=&consume;assert(!a::PublishSoundD80(r,4,5));f::transportReady=true;
 for(unsigned i=0;i<4;++i){r.serial=i+1;assert(a::PublishSoundD80(r,4,5));}
 r.serial=5;assert(!a::PublishSoundD80(r,4,5));assert(f::Wanted());
 kcd2vr::native_script_t2::Context ctx{};kcd2vr::native_script_t2::current=false;f::Service(ctx);assert(calls==0&&f::Wanted());
 kcd2vr::native_script_t2::current=true;
 for(unsigned i=1;i<=4;++i){f::Service(ctx);assert(calls==i&&seen.serial==i&&seen.player==4&&seen.controller==5);}
 assert(!f::Wanted());f::Service(ctx);assert(calls==4);
 // No retry after an entered/fallible consumer: it has already left the queue.
 f::consumer=+[](const kcd2vr::native_script_t2::Context&,const f::Event&)noexcept{++calls;};
 assert(a::PublishSoundD80(r,4,5));f::Service(ctx);f::Service(ctx);assert(calls==5&&!f::Wanted());
 std::puts("D81 actual herb, vessel and special-ingredient sound selection/queue PASS; native audio is not executed");
}
