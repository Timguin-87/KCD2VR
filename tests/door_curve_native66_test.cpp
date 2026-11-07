// Execute the actual native binder. Doubles replace only previously qualified
// native read/sampler leaves; no game is loaded and this is not HMD acceptance.
#include "../src/core/native_door19c4.h"
#include "../src/core/door_path66.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include <limits>
namespace door4=kcd2vr::door19c4;namespace d=kcd2vr::door_path66;
struct QuatT28 {float q[4]{0,0,0,1};float t[3]{};};
struct V3{float x=0,y=0,z=0;};struct Q4{float x=0,y=0,z=0,w=1;};
struct ModelWorld6692 {Q4 q{};V3 t{};float scale=1;};
namespace ph {struct Pose {d::Point p{};d::Rotation q{};};}
static constexpr std::uintptr_t kCEntityVftRva=0x4072C78,kCEntityGetCharacterRva=0x6411AC,kCDefaultSkeletonVftRva=0x406B930;
static std::map<std::uintptr_t,std::vector<unsigned char>> memory;
static constexpr std::uintptr_t entity=0x20000,character=0x30000,owner=0x40000,joints=0x50004,entries=0x60004,base=0x180000000;
static unsigned fault=0,samples=0,resolves=0,helperReads=0,modelReads=0;static float nativePhase=0;static ModelWorld6692 nativeWorld{};
static std::uint64_t now66=1000;static std::uint64_t GetTickCount64(){return now66;}
static std::uintptr_t WhBase(){return base;}static bool SupportedWhBuild(std::uintptr_t){return fault!=1;}
template<class T>static void put(std::uintptr_t p,const T& v){for(auto& [start,bytes]:memory)if(p>=start&&p+sizeof(T)<=start+bytes.size()){
 std::memcpy(bytes.data()+p-start,&v,sizeof(v));return;}assert(false);}
template<class T>static bool Read(const void* p,T& v){const auto address=reinterpret_cast<std::uintptr_t>(p);
 for(auto& [start,bytes]:memory)if(address>=start&&address+sizeof(T)<=start+bytes.size()){
  std::memcpy(&v,bytes.data()+address-start,sizeof(v));return true;}return false;}
static void* EntityById6694(std::uint32_t id){return id==42&&fault!=2?reinterpret_cast<void*>(entity):nullptr;}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t v=0;return fault!=3&&Read(p,v)&&v==base+rva;}
static void* CharacterForEntity6689(void*){return fault==4?nullptr:reinterpret_cast<void*>(character);}
static bool ValidateCharInstance(void* c,void*& anim){anim=reinterpret_cast<void*>(character+0x110);return c==reinterpret_cast<void*>(character)&&fault!=5;}
static bool ReadParentDirect(void*,int id,int& p){std::int16_t value=-2;
 if(!Read(reinterpret_cast<void*>(joints+id*0x30+0x16),value))return false;p=value;return true;}
static bool ReadModelWorld6692(void*,ModelWorld6692& w){++modelReads;w=nativeWorld;return fault!=6;}
static bool ReadCompanion(void*,int id,QuatT28& p){p={};p.t[2]=id?1.f:0.f;
 if(fault==7)p.q[3]=2;if(fault==8)p.t[2]=std::numeric_limits<float>::quiet_NaN();return fault!=9;}
static bool PhysHelper34(void*,const char*,ph::Pose& p,bool local){++helperReads;assert(local);
 const double a=nativePhase*1.5707963267948966;p={{0,0,1},{0,0,std::sin(a*.5),std::cos(a*.5)}};
 if(fault==10)p.p.x=.1;if(fault==11)p.q={0,0,0,1};return fault!=12;}
namespace animation_filter {
struct CurveReferenceInfo{int id=-1;std::uint32_t flags=0;std::uint64_t alias=0;};
static bool ResolveCurveReference(void*,const char*,CurveReferenceInfo& i){++resolves;i={17,0x800,0x88776655};return fault!=13;}
static int SampleCurveReference(void*,int id,std::uint32_t crc,float phase,float* p){
 ++samples;assert(id==17);for(int i=0;i<7;++i)p[i]=0;p[3]=1;
 if(fault==14)return 3; // CREATED is insufficient; no synchronous loading
 if(crc==0xBBB56880u||crc==0x56B1685Eu){const double a=phase*1.5707963267948966;p[2]=float(std::sin(a*.5));p[3]=float(std::cos(a*.5));
  // Real resident sampler leaves missing position at initialized zero. Binder
  // must restore native constant local translation, not consume this zero.
  if(fault==18)p[4]=.01f;
 }else assert(crc==0x50256111u||crc==0x9D39FE0Au);
 if(fault==15)p[3]=2;
 if(fault==16&&samples==20)put<std::uint32_t>(entries-4,2);
 if(fault==17&&samples==20)nativeWorld.t.x=.1f;
 return 0;
}
}
#include "../src/vrik/door_curve_native66.inl"
static unsigned checks=0;static void check(bool b){++checks;assert(b);}
static door4::Request request(){door4::Request r{};r.serial=21;r.id=42;r.entity=entity;r.player=999;r.side=0;r.epoch=1;
 r.trackingEpoch46=2;r.sourceGeneration=3;r.inputSettingsEpoch46=4;r.inputExperimentEpoch65=5;r.inputPress65=6;r.surface66={1,0,1};return r;}
static d::Curve seed(const door4::Request& r){d::Curve c{};auto& s=c.source;s.operation=r.serial;s.entity=r.entity;s.target=r.id;
 s.player=r.player;s.side=r.side;s.poseEpoch=r.epoch;s.trackingEpoch=r.trackingEpoch46;s.generation=r.sourceGeneration;
 s.settingsEpoch=r.inputSettingsEpoch46;s.experimentEpoch=r.inputExperimentEpoch65;s.press=r.inputPress65;return c;}
static void reset(bool left=true){memory.clear();memory[entity].resize(0x100);memory[character].resize(0xB00);memory[owner].resize(0x240);
 memory[joints-4].resize(4+6*0x30);memory[entries-4].resize(4+0x78);memory[base+kCEntityVftRva].resize(0x330);
 put(entity,base+kCEntityVftRva);put(base+kCEntityVftRva+0x328,base+kCEntityGetCharacterRva);put(owner,base+kCDefaultSkeletonVftRva);
 put(character+0xA70,owner);put(character+0x110+0x48,entries);put(entries-4,unsigned(1));put(entries+0x10,int(17));put(owner+8,joints);
 put(joints-4,unsigned(6));const std::uint32_t lc[]{0x50256111,0xBBB56880,0xD78F8711,0x559EC994,0xCC97982E,0x87D03C54};
 const std::uint32_t rc[]{0x9D39FE0A,0x56B1685E,0xB6097CC2,0xB0E35325,0x29EA029F,0x87D03C54};const std::int16_t par[]{-1,0,0,1,1,1};
 for(unsigned i=0;i<6;++i){put(joints+i*0x30+8,left?lc[i]:rc[i]);put(joints+i*0x30+0x16,par[i]);}
 fault=samples=resolves=helperReads=modelReads=0;nativePhase=0;nativeWorld={};now66=1000;door_curve_detail66::cache={};}
int main(){d::Pose frame{};auto r=request();reset();auto c=seed(r);check(DoorCurveBind66(r,"door_l_f_o_player",0,c,frame));
 check(samples==68&&resolves==1&&helperReads==1);check(d::valid(c));
 check(std::abs(c.samples[0].handle.p.x-1)<1e-6&&std::abs(c.samples[0].handle.p.z-1)<1e-6);
 check(std::abs(c.samples[32].handle.p.y-1)<1e-6&&std::abs(c.samples[32].handle.p.z-1)<1e-6);
 check(c.source.clip==0x88776655&&c.source.topology&&c.source.curve);
 // Later frames reuse exact curve/anchor despite native pose lag after a time seek.
 nativePhase=.4f;auto prior=c;auto again=seed(r);check(DoorCurveBind66(r,"door_l_f_o_player",.4f,again,frame));
 check(samples==68&&resolves==1&&helperReads==1&&again.source.curve==prior.source.curve);
 check(std::abs(again.samples[32].handle.p.y-1)<1e-6);
 // Same operation's accepted regrip updates source press only, never learns another axis/anchor.
 again=seed(r);again.source.press=700;nativeWorld.t={100,-20,4};check(DoorCurveBind66(r,"door_l_f_o_player",.4f,again,frame));
 check(again.source.press==700&&frame.p.x==100&&samples==68&&again.source.curve==prior.source.curve);
 auto wrong=seed(r);check(!DoorCurveBind66(r,"door_l_f_c_player",.4f,wrong,frame));check(samples==68);
 for(unsigned f=1;f<=18;++f){reset();fault=f;nativePhase=.2f;auto x=seed(r);check(!DoorCurveBind66(r,"door_l_f_o_player",.2f,x,frame));
  check(!door_curve_detail66::cache.ready);const auto n=samples;auto y=seed(r);check(!DoorCurveBind66(r,"door_l_f_o_player",.2f,y,frame));
  check(samples==n);}
 for(unsigned f=0;f<10;++f){reset();auto x=seed(r);
  switch(f){case 0:put(joints-4,unsigned(7));break;case 1:put(joints+5*0x30+8,std::uint32_t(123));break;
  case 2:put(joints+4*0x30+0x16,std::int16_t(0));break;case 3:put(entries-4,unsigned(2));break;
  case 4:put(entries+0x10,int(18));break;case 5:nativeWorld.scale=2;break;case 6:x.source.player=0;break;
  case 7:x.source.operation++;break;case 8:x.source.poseEpoch++;break;case 9:put(base+kCEntityVftRva+0x328,std::uintptr_t(123));break;}
  check(!DoorCurveBind66(r,"door_l_f_o_player",0,x,frame));}
 for(const char* name:{"door_l_f_o","door_l_f_o_player_extra","door_x_f_o_player","door_l_x_o_player","door_l_f_x_player","door_r_f_o_player"}){
  reset();auto x=seed(r);check(!DoorCurveBind66(r,name,0,x,frame));}
 reset(false);c=seed(r);check(DoorCurveBind66(r,"stealth_door_r_b_c_player",0,c,frame));
 check(d::valid(c)&&samples==68);
 for(bool left:{false,true})for(bool stealth:{false,true})for(char direction:{'b','f'})for(char action:{'c','o'}){
  reset(left);char name[32]{};std::snprintf(name,sizeof(name),"%sdoor_%c_%c_%c_player",stealth?"stealth_":"",left?'l':'r',direction,action);
  auto x=seed(r);check(DoorCurveBind66(r,name,0,x,frame));check(d::valid(x)&&samples==68);
 }
 reset();auto invalidSurface=r;invalidSurface.surface66.x=std::numeric_limits<float>::quiet_NaN();c=seed(invalidSurface);
 check(!DoorCurveBind66(invalidSurface,"door_l_f_o_player",0,c,frame)&&samples==0);
 // Actual current pivot at first bind is used, not a statically guessed entity frame.
 reset();nativePhase=.5f;r.surface66={.70710677f,.70710677f,1};c=seed(r);
 check(DoorCurveBind66(r,"door_l_f_o_player",.5f,c,frame));check(std::abs(c.samples[0].handle.p.x-1)<1e-5);
 // A replaced live native clip cannot reuse a formerly valid cached curve.
 put(entries+0x10,int(99));c=seed(r);check(!DoorCurveBind66(r,"door_l_f_o_player",.5f,c,frame));
 // Only initial finite native-pose/clock lag can be pending; same-frame polls
 // neither resample nor reanchor, and a later matching native pose binds once.
 reset();r=request();fault=11;nativePhase=.2f;c=seed(r);
 check(!DoorCurveBind66(r,"door_l_f_o_player",.2f,c,frame)&&DoorCurvePending66());const auto first=samples;
 now66+=8;c=seed(r);check(!DoorCurveBind66(r,"door_l_f_o_player",.2f,c,frame)&&DoorCurvePending66()&&samples==first);
 now66+=8;fault=0;c=seed(r);check(DoorCurveBind66(r,"door_l_f_o_player",.2f,c,frame)&&!DoorCurvePending66());
 check(samples==first+68&&door_curve_detail66::cache.attempts==2);
 reset();fault=11;nativePhase=.2f;
 for(unsigned i=0;i<8;++i){c=seed(r);check(!DoorCurveBind66(r,"door_l_f_o_player",.2f,c,frame));
  check(DoorCurvePending66()==(i<7));now66+=16;}
 c=seed(r);check(!DoorCurveBind66(r,"door_l_f_o_player",.2f,c,frame)&&!DoorCurvePending66()&&samples==16);
 reset();fault=11;nativePhase=.2f;c=seed(r);check(!DoorCurveBind66(r,"door_l_f_o_player",.2f,c,frame)&&DoorCurvePending66());
 now66+=251;fault=0;c=seed(r);check(!DoorCurveBind66(r,"door_l_f_o_player",.2f,c,frame)&&!DoorCurvePending66());
 std::printf("door_curve_native66: %u checks PASS (actual binder; native read/sampler leaves doubled)\n",checks);
}
