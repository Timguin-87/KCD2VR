#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include "hand_claim_fixture_t24.h"
using Byte=unsigned char;
static std::array<Byte,0x200> player{}, model{}, visual{}, component{}, attachment{}, object{}, entity{}, weapon{};
static constexpr std::uintptr_t base=0x10000000;
static std::uintptr_t WhBase(){return base;}
static const char* nativeName="entity_right";
static std::map<void*,std::uintptr_t> vfts;
static std::map<unsigned,void*> entities;
static void* faultRead=nullptr;
static void* handLookup=nullptr;static void* nockLookup=nullptr;static void* currentVisual=nullptr;
static bool mutateModel=false,mutateWeapon=false,mutateObject=false,mutateEntity=false;
static unsigned objectReads=0,handCalls=0,nockCalls=0,checks=0;
template<class T> static void put(std::array<Byte,0x200>& b,std::size_t at,T v){std::memcpy(b.data()+at,&v,sizeof(v));}
template<class T> static bool Read(const void* p,T& v){
 if(p==faultRead)return false;
 if(reinterpret_cast<std::uintptr_t>(p)==base+0x40AC298){if(sizeof(T)!=sizeof(nativeName))return false;std::memcpy(&v,&nativeName,sizeof(T));return true;}
 if(mutateObject&&p==attachment.data()+0x20&&++objectReads==2)put(attachment,0x20,static_cast<void*>(entity.data()));
 const auto a=reinterpret_cast<std::uintptr_t>(p);
 for(auto* b:{&player,&model,&visual,&component,&attachment,&object,&entity,&weapon}){
  const auto x=reinterpret_cast<std::uintptr_t>(b->data());if(a>=x&&a+sizeof(T)<=x+b->size()){std::memcpy(&v,p,sizeof(T));return true;}}
 return false;
}
static bool PhysVft6698(void* p,std::uintptr_t rva){return p&&vfts.count(p)&&vfts[p]==rva;}
static void* EntityById6694(unsigned id){return entities.count(id)?entities[id]:nullptr;}
static bool ManualPlayer596(void* p){return p==player.data();}
static void* ManualVisual59c9(void* c){return c==component.data()?currentVisual:nullptr;}
struct ManualBow596 {void* player=nullptr;void* entity=nullptr;unsigned hand=0;int side=2;std::uint64_t epoch=1,request=1;};
struct ManualItems596 {void* equipment=nullptr;void* weapon=nullptr;void* ammo=nullptr;void* component=nullptr;std::uint64_t weaponKey=0,ammoKey=0;bool loaded=false;unsigned nock=0;void* visual=nullptr;std::uint64_t nockedKey=0;};
static void* ManualFindHandAttachment19c4r4(void* m,const char* n){++handCalls;if(m!=model.data()||n!=nativeName)return nullptr;if(mutateModel)put(player,0x1B0,static_cast<void*>(weapon.data()));return handLookup;}
static void* ManualFindNockAttachment19c4r4(void* v){++nockCalls;if(v!=visual.data())return nullptr;if(mutateWeapon)put(visual,0x58,static_cast<void*>(model.data()));if(mutateEntity)entities[77]=weapon.data();return nockLookup;}
static bool attachmentPins42=true;static unsigned shows42=0;
static bool BowAttachPins59c15(){return attachmentPins42;}
static bool AttachmentHidden59c15(void* a,bool& h){unsigned f=0;if(!Read(static_cast<unsigned char*>(a)+8,f))return false;h=(f&0x10000)!=0;return true;}
static bool AttachmentHide59c15(void* a,bool hide){if(a!=attachment.data())return false;++shows42;put(attachment,8,unsigned(hide?0x10000:0));return true;}
static bool entityVisibleRead59=true,entityHideOk59=true;static unsigned entityShows59=0;
static bool EntityHiddenFlag59c14(void* e,bool& hidden){unsigned f=0;if(!entityVisibleRead59||e!=entity.data()||!Read(static_cast<unsigned char*>(e)+8,f))return false;hidden=(f&0x10)!=0;return true;}
static bool EntityHide59c14(void* e,bool hide){if(!entityHideOk59||e!=entity.data())return false;++entityShows59;put(entity,8,unsigned(hide?0x10:0));return true;}
#include "manual_reload_attachment19c4r4.inc"
static void ck(bool b,const char* msg){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",msg);std::exit(1);}}
static ManualBow596 bow;static ManualItems596 items;
static void reset(){
 hand_claim_fixture_t24::Reset(player.data(),reinterpret_cast<std::uintptr_t>(model.data()));
 player.fill(0);model.fill(0);visual.fill(0);component.fill(0);attachment.fill(0);object.fill(0);entity.fill(0);weapon.fill(0);
 entities={{77,entity.data()}};vfts={{entity.data(),0x4072C78},{attachment.data(),0x3A657C0},{object.data(),0x3A68F68}};
 attachmentPins42=true;shows42=0;faultRead=nullptr;mutateModel=mutateWeapon=mutateObject=mutateEntity=false;objectReads=handCalls=nockCalls=0;
 entityVisibleRead59=entityHideOk59=true;entityShows59=0;
 handLookup=nockLookup=attachment.data();currentVisual=visual.data();nativeName="entity_right";
 put(player,0x1B0,static_cast<void*>(model.data()));put(model,0x20,static_cast<void*>(player.data()));put(model,0x2C,unsigned(77));
 put(attachment,0x20,static_cast<void*>(object.data()));put(object,8,unsigned(77));
 put(visual,0x58,static_cast<void*>(weapon.data()));put(visual,0xDC,unsigned(77));put(component,0xE0,std::uint64_t(99));
 bow={player.data(),entity.data(),77,0};items={};items.component=component.data();items.visual=visual.data();items.weapon=weapon.data();items.loaded=true;items.nock=77;items.nockedKey=items.ammoKey=99;
}
int main(){
 reset();ck(ManualHandAttached19c4r4(bow),"actual matching native right attachment acknowledges hand");ck(ManualNockAttached19c4r4(items),"actual matching native arrow attachment acknowledges nock");
 reset();handLookup=nullptr;ck(!ManualHandAttached19c4r4(bow),"created hand entity with queued attachment is pending");
 reset();nockLookup=nullptr;ck(!ManualNockAttached19c4r4(items),"logical loaded plus created nock entity is not attachment");
 reset();put(attachment,0x20,static_cast<void*>(nullptr));ck(!ManualNockAttached19c4r4(items),"empty native attachment object remains pending");
 reset();vfts[object.data()]=0;ck(!ManualHandAttached19c4r4(bow),"nonentity attachment cannot acknowledge hand");
 reset();vfts[attachment.data()]=0;ck(!ManualNockAttached19c4r4(items),"foreign attachment interface fails closed");
 reset();vfts[entity.data()]=0;ck(!ManualNockAttached19c4r4(items),"foreign entity fails closed");
 reset();entities[77]=weapon.data();ck(!ManualHandAttached19c4r4(bow),"reused hand entity ID cannot acknowledge stale identity");
 reset();put(object,8,unsigned(88));ck(!ManualNockAttached19c4r4(items),"attachment to different entity denied");
 reset();put(model,0x20,static_cast<void*>(weapon.data()));ck(!ManualHandAttached19c4r4(bow)&&handCalls==0,"foreign actor-model backlink denies before native lookup");
 reset();bow.side=1;ck(!ManualHandAttached19c4r4(bow)&&handCalls==0,"left ammo hand is not bow contract");
 reset();put(model,0x2C,unsigned(88));ck(!ManualHandAttached19c4r4(bow),"current native hand replaced");
 reset();items.nockedKey=98;ck(!ManualNockAttached19c4r4(items),"logical bound ammo differs from current selected ammo");
 reset();items.loaded=false;ck(!ManualNockAttached19c4r4(items),"empty native logical component cannot fake ready");
 reset();put(component,0xE0,std::uint64_t(98));ck(!ManualNockAttached19c4r4(items),"logical key changed after item read");
 reset();currentVisual=weapon.data();ck(!ManualNockAttached19c4r4(items),"visual replacement invalidates nock");
 reset();put(visual,0x58,static_cast<void*>(model.data()));ck(!ManualNockAttached19c4r4(items),"visual belongs to other weapon");
 reset();put(visual,0xDC,unsigned(88));ck(!ManualNockAttached19c4r4(items),"visual current nock replaced");
 reset();mutateModel=true;ck(!ManualHandAttached19c4r4(bow),"native lookup replacing actor-model invalidates acknowledgment");
 reset();mutateWeapon=true;ck(!ManualNockAttached19c4r4(items),"native lookup replacing weapon invalidates acknowledgment");
 reset();mutateObject=true;ck(!ManualNockAttached19c4r4(items),"attachment change during revalidation denied");
 reset();mutateEntity=true;ck(!ManualNockAttached19c4r4(items),"lookup recycled entity denied");
 reset();faultRead=object.data()+8;ck(!ManualNockAttached19c4r4(items),"unreadable live entity attachment fails closed");
 reset();put(attachment,8,unsigned(0x10000));ck(ManualArrowVisible42(bow)&&shows42==1,"owned hidden native arrow is shown once");
 reset();ck(ManualArrowVisible42(bow)&&shows42==0,"already visible arrow incurs no native show call");
 {bool restored=false;put(attachment,8,unsigned(0x10000));ck(ManualArrowVisible42(bow,&restored)&&restored&&shows42==1,"same actual manual arrow rehidden after first ACK is restored");
  restored=true;ck(ManualArrowVisible42(bow,&restored)&&!restored&&shows42==1,"unchanged visible manual arrow does not churn native hide state");}
 reset();attachmentPins42=false;ck(!ManualArrowVisible42(bow)&&shows42==0,"missing visibility pins never writes");
 reset();entities[77]=weapon.data();ck(!ManualArrowVisible42(bow)&&shows42==0,"replaced arrow entity cannot be shown");
 reset();put(entity,8,unsigned(0x10));{bool restored=false;ck(ManualArrowVisible42(bow,&restored)&&restored&&entityShows59==1&&shows42==0,"C59 visible attachment with independently hidden actual arrow entity is revealed once");}
 reset();put(entity,8,unsigned(0x10));put(attachment,8,unsigned(0x10000));ck(ManualArrowVisible42(bow)&&entityShows59==1&&shows42==1,"C59 both visibility layers must be restored for the same attached native arrow");
 reset();entityVisibleRead59=false;ck(!ManualArrowVisible42(bow)&&entityShows59==0,"C59 unknown entity visibility grants no reveal call");
 reset();put(entity,8,unsigned(0x10));entityHideOk59=false;ck(!ManualArrowVisible42(bow)&&entityShows59==0,"C59 failed native reveal is not reported visible");
 reset();put(entity,8,unsigned(0x10));entities[77]=weapon.data();ck(!ManualArrowVisible42(bow)&&entityShows59==0&&shows42==0,"C59 recycled native hand entity cannot be revealed");

 reset();hand_claim_fixture_t24::Publish(0);ck(!ManualHandAttached19c4r4(bow)&&handCalls==0,"T24 same-hand claim refuses actual reader before attachment lookup");
 reset();hand_claim_fixture_t24::Publish(1);ck(ManualHandAttached19c4r4(bow),"T24 opposite hand claim preserves native right attachment");
 std::printf("%u actual attachment-reader checks passed; native lookups doubled\n",checks);
}
