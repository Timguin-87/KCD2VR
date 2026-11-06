#include "../src/dialogue_state59c10r2.h"
#include "../src/activity_eye59c19.h"
#include "../src/dialogue_smooth38.h"
#include "../src/core/snapshot_mailbox.h"
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
namespace ds=kcd2vr::dialogue_state59c10r2;
namespace ae=kcd2vr::activity_eye59c19;
namespace core=kcd2vr::core;
struct Matrix34f {float m[3][4]{};};
static std::uint64_t now19=100;
std::uint64_t GetTickCount64() noexcept {return now19;}
namespace dsm38=kcd2vr::dialogue_smooth38;
static std::uint64_t DialogueSmoothNowNs38() noexcept {return now19*1000000ull;}
static ds::State state19;
static ds::NativeHead head19;
static ae::State activity19;
static bool haveHead19=true;
static bool haveViewHead28=false;
static bool bowNeutral35=true;
static bool rawHeadDelivery19=false; // exercises the callback's own defensive validation
namespace activity_lens59c10r1 {
ds::State Dialogue(std::uint64_t) noexcept {return state19;}
ae::State EyeState(std::uint64_t) noexcept {return activity19;}
}
namespace frik_pose_probe {
bool BowCameraNeutral35(std::uint64_t) noexcept {return bowNeutral35;}
bool NativeDialogueViewHead28(ds::NativeHead& out,const ds::State& state,std::uint64_t now) noexcept {
    if(!haveViewHead28||!ds::fresh(state,now)||!state.active||state.camera!=ds::Camera::FirstPerson||
       !ds::fresh(head19,now)||head19.entity!=state.entity)return false;
    out=head19;return true;
}
bool NativeDialogueHead59c10r2(ds::NativeHead& out,std::uint64_t now) noexcept {
    if(!haveHead19||(!rawHeadDelivery19&&!ds::fresh(head19,now)))return false;
    out=head19;return true;
}
}
// The production function has no HMD sampling/composition dependency. Its
// output remains a native BASE for the one existing camera composition.
#include "generated_dialogue_camera19c2.inc"
static Matrix34f native19(float x,float y,float z) {
    Matrix34f m{};for(unsigned i=0;i<3;++i)m.m[i][i]=1;
    m.m[0][3]=x;m.m[1][3]=y;m.m[2][3]=z;return m;
}
int main(){
    unsigned checks=0;
    const auto check=[&](bool value){++checks;if(!value)std::fprintf(stderr,"FAIL callback check %u\n",checks);assert(value);};
    g_dialogue_smooth_on38=false; // the inherited checks below pin the exact C37C2 per-sample base (menu OFF)
    state19.tick=100;state19.entity=0x100;state19.known=true;state19.camera=ds::Camera::FirstPerson;
    head19={100,1,0x100,0x200,{10,20,1.7f},{},1,{10,20,0},true};
    activity19.tick=100;activity19.known=true;
    auto native=native19(10,20.05f,1.756f);const auto untouched=native;
    // Common cull -> publish-reuse path: neutral must train without a second
    // render=true invocation. It must not move or rotate the native matrix.
    check(!DialogueEyeBase59c10r2(native,false));
    check(std::memcmp(&native,&untouched,sizeof native)==0);
    ds::EyeOffset learned{};check(g_dialogue_eye59c10r2.read(learned)&&learned.ready);
    check(std::fabs(learned.local.y-.05f)<1e-5f&&std::fabs(learned.local.z-.056f)<1e-5f);
    now19=110;state19.tick=110;state19.active=true;state19.instance=0x300;
    head19.tick=110;head19.world={30,40,1.6f};head19.modelOrigin={30,40,0};
    g_dialogue_policy59c10r2.publish({110,true});
    native=native19(32,44,2);check(DialogueEyeBase59c10r2(native,false));
    check(std::fabs(native.m[0][3]-30)<1e-5f&&std::fabs(native.m[1][3]-40.05f)<1e-5f&&std::fabs(native.m[2][3]-1.756f)<1e-5f);
    check(native.m[0][0]==1&&native.m[1][1]==1&&native.m[2][2]==1);
    DialogueReceipt59c10r2 receipt{};check(g_dialogue_receipt59c10r2.read(receipt));
    check(receipt.fromCull&&receipt.reason==0&&receipt.head&&receipt.learned&&receipt.allowed);
    // A normal publication still produces its own correctly labelled attempt.
    native=native19(32,44,2);check(DialogueEyeBase59c10r2(native,true));
    check(g_dialogue_receipt59c10r2.read(receipt)&&!receipt.fromCull&&receipt.reason==0);
    // Real body translation/rotation is current, never a frozen world eye.
    const float half=std::sqrt(.5f);head19.body={0,0,half,half};head19.world={50,60,1.4f};head19.modelOrigin={50,60,0};
    native=native19(2,4,5);check(DialogueEyeBase59c10r2(native,false));
    check(std::fabs(native.m[0][3]-49.95f)<1e-4f&&std::fabs(native.m[1][3]-60)<1e-5f);
    check(std::fabs(native.m[0][1]+1)<1e-5f);
    head19.world.z=.9f;native=native19(2,4,.7f);check(DialogueEyeBase59c10r2(native,false));
    check(std::fabs(native.m[2][3]-1.756f)<1e-5f); // authored native neck/head motion is NOT HMD travel
    head19.modelOrigin.z=.2f;native=native19(2,4,.7f);check(DialogueEyeBase59c10r2(native,false));
    check(std::fabs(native.m[2][3]-1.956f)<1e-5f); // genuine current body movement retained
    head19.modelOrigin.z=0;
    // Loss, old policy and an unknown owner may produce a refusal receipt but
    // never permission or a replayed camera. No head grace is introduced.
    auto refusal=[&](unsigned reason){native=native19(7,8,9);const auto before=native;
        check(!DialogueEyeBase59c10r2(native,false));check(std::memcmp(&before,&native,sizeof native)==0);
        check(g_dialogue_receipt59c10r2.read(receipt)&&receipt.fromCull&&receipt.reason==reason);};
    haveHead19=false;refusal(5);haveHead19=true;
    state19.known=false;refusal(1);state19.known=true;
    state19.camera=ds::Camera::Unknown;refusal(3);state19.camera=ds::Camera::FirstPerson;
    g_dialogue_policy59c10r2.publish({1,true});refusal(4);
    g_dialogue_policy59c10r2.publish({110,true});now19=171;state19.tick=171;refusal(5);
    // Authored activity/unknown lifecycle cannot train a new dialogue offset,
    // even though culling is now permitted to publish an ordinary neutral eye.
    now19=200;head19.tick=200;head19.body={};state19.tick=200;state19.active=false;
    activity19.active=true;g_dialogue_eye59c10r2.publish(ds::EyeOffset{});
    native=native19(50,60,1.456f);check(!DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&!learned.ready);
    activity19.active=false;activity19.known=false;check(!DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&!learned.ready);
    activity19.known=true;bowNeutral35=false;check(!DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&!learned.ready); // drawn bow cannot train dialogue eye
    bowNeutral35=true;
    // The exact default-eye option remains effective without neutral learning.
    state19.active=true;state19.instance=0x300;activity19.known=true;g_dialogue_policy59c10r2.publish({200,true});
    native=native19(7,8,9);check(DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_receipt59c10r2.read(receipt)&&receipt.defaultEye&&receipt.reason==0);
    // A character replacement can enter dialogue before neutral gameplay can
    // train a new offset. The former ready cache must not veto the selected
    // default fallback, and must never be applied to the replacement body.
    ds::EyeOffset previous{};previous.entity=head19.entity;previous.character=head19.character;
    previous.basis[0]={1,0,0};previous.basis[1]={0,1,0};previous.basis[2]={0,0,1};
    previous.local={0,.05f,.056f};previous.ready=true;g_dialogue_eye59c10r2.publish(previous);
    head19.character=0x201;native=native19(7,8,9);
    check(DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&!learned.ready);
    check(g_dialogue_receipt59c10r2.read(receipt)&&receipt.defaultEye&&receipt.reason==0);
    check(std::fabs(native.m[1][3]-head19.world.y)<1e-5f);
    // Missing/old body or an unknown/mismatched dialogue owner cannot retire
    // calibration. Only a fresh, currently qualified same-Henry body may do it.
    g_dialogue_eye59c10r2.publish(previous);haveHead19=false;
    native=native19(7,8,9);check(!DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&learned.ready&&learned.character==previous.character);
    haveHead19=true;state19.known=false;native=native19(7,8,9);
    check(!DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&learned.ready);
    state19.known=true;state19.entity=0x101;native=native19(7,8,9);
    check(!DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&learned.ready);
    state19.entity=head19.entity;rawHeadDelivery19=true;
    const auto validHead=head19;
    for(unsigned bad=0;bad<4;++bad){head19=validHead;
        if(bad==0)head19.tick=0;
        if(bad==1)head19.tick=now19-61;
        if(bad==2)head19.tick=now19+1;
        if(bad==3)head19.epoch=0;
        native=native19(7,8,9);check(!DialogueEyeBase59c10r2(native,false));
        check(g_dialogue_eye59c10r2.read(learned)&&learned.ready&&learned.character==previous.character);
    }
    head19=validHead;rawHeadDelivery19=false;g_dialogue_default_eye59c13=false;
    native=native19(7,8,9);check(!DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&!learned.ready);
    // Recenter/current epoch changes preserve calibration for the SAME body.
    previous.character=head19.character;g_dialogue_eye59c10r2.publish(previous);
    head19.epoch=2;native=native19(7,8,9);check(DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_eye59c10r2.read(learned)&&learned.ready&&learned.character==head19.character);
    check(std::fabs(native.m[1][3]-(head19.world.y+.05f))<1e-5f);
    // C21's actual failure: native FIRST_PERSON and policy allowed, but no
    // skinning head. The separate native-view producer closes that dependency.
    haveHead19=false;haveViewHead28=true;state19.camera=ds::Camera::FirstPerson;
    native=native19(7,8,9);check(DialogueEyeBase59c10r2(native,false));
    check(g_dialogue_receipt59c10r2.read(receipt)&&receipt.headFromView28&&receipt.headCharacter28==head19.character);
    check(std::fabs(native.m[1][3]-(head19.world.y+.05f))<1e-5f);
    const auto currentHead=head19;head19.tick=now19-61;
    native=native19(7,8,9);check(!DialogueEyeBase59c10r2(native,false));
    head19=currentHead;state19.active=false;
    native=native19(7,8,9);check(!DialogueEyeBase59c10r2(native,false));
    state19.active=true;state19.entity++;
    native=native19(7,8,9);check(!DialogueEyeBase59c10r2(native,false));
    // C38 (menu ON): same callback, anchor low-pass + bounded late-sample hold.
    state19=ds::State{};state19.entity=0x100;state19.known=true;state19.active=true;state19.instance=0x700;state19.camera=ds::Camera::FirstPerson;
    head19=currentHead;head19.body={};head19.entity=0x100;head19.modelOrigin={30,40,0};head19.world={30,40,1.6f};
    now19=5000;state19.tick=head19.tick=now19;g_dialogue_policy59c10r2.publish({now19,true});
    previous.entity=head19.entity;previous.character=head19.character;previous.modelLocal={0,.05f,1.756f};previous.modelLocalReady=true;g_dialogue_eye59c10r2.publish(previous);
    g_dialogue_smooth_on38=true;
    native=native19(1,2,3);check(DialogueEyeBase59c10r2(native,true));
    check(std::fabs(native.m[0][3]-30)<1e-4f&&std::fabs(native.m[1][3]-40.05f)<1e-4f); // first sample of the dialogue: exact
    // Body anchor steps 2 cm: the next 11 ms frame moves only part of it (no staircase), later converges.
    now19+=11;state19.tick=head19.tick=now19;g_dialogue_policy59c10r2.publish({now19,true});head19.modelOrigin.x=30.02f;
    native=native19(1,2,3);check(DialogueEyeBase59c10r2(native,true));check(native.m[0][3]>30.001f&&native.m[0][3]<30.01f);
    DialogueReceipt59c10r2 r38{};check(g_dialogue_receipt59c10r2.read(r38)&&r38.reason==0&&r38.smooth38&&!r38.held38&&r38.lagM38>.01f);
    for(unsigned f=0;f<40;++f){now19+=11;state19.tick=head19.tick=now19;g_dialogue_policy59c10r2.publish({now19,true});native=native19(1,2,3);check(DialogueEyeBase59c10r2(native,f%2==0));}
    check(std::fabs(native.m[0][3]-30.02f)<5e-4f);const float settled=native.m[0][3];
    // C40: no250ms world-eye replay. Expired head sample returns a native-camera no-op.
    now19+=61;state19.tick=now19;g_dialogue_policy59c10r2.publish({now19,true});
    native=native19(1,2,3);auto before38=native;check(!DialogueEyeBase59c10r2(native,true));check(std::memcmp(&before38,&native,sizeof native)==0);
    // A real fresh head sample restores the filtered base, without restamping the old one.
    now19+=11;state19.tick=head19.tick=now19;g_dialogue_policy59c10r2.publish({now19,true});native=native19(1,2,3);check(DialogueEyeBase59c10r2(native,true));
    const auto lastReal=now19;head19.tick=0;
    now19=lastReal+60;state19.tick=now19;g_dialogue_policy59c10r2.publish({now19,true});native=native19(1,2,3);check(DialogueEyeBase59c10r2(native,true));
    now19=lastReal+61;state19.tick=now19;g_dialogue_policy59c10r2.publish({now19,true});native=native19(1,2,3);before38=native;check(!DialogueEyeBase59c10r2(native,true));check(std::memcmp(&before38,&native,sizeof native)==0);
    now19=lastReal+1;head19.tick=lastReal;state19.tick=now19;state19.active=false;native=native19(1,2,3);before38=native;check(!DialogueEyeBase59c10r2(native,true));check(std::memcmp(&before38,&native,sizeof native)==0);
    state19.active=true;state19.camera=ds::Camera::Unknown;head19.tick=0;native=native19(1,2,3);before38=native;check(!DialogueEyeBase59c10r2(native,true));check(std::memcmp(&before38,&native,sizeof native)==0);
    state19.camera=ds::Camera::FirstPerson;g_dialogue_policy59c10r2.publish({now19,false});native=native19(1,2,3);check(!DialogueEyeBase59c10r2(native,true));
    // Menu OFF restores the raw per-sample base at once.
    g_dialogue_smooth_on38=false;g_dialogue_policy59c10r2.publish({now19,true});head19.tick=now19;head19.modelOrigin.x=30.2f;
    native=native19(1,2,3);check(DialogueEyeBase59c10r2(native,true));check(std::fabs(native.m[0][3]-30.2f)<1e-4f);
    std::printf("dialogue_camera19c2: %u actual callback checks PASS\n",checks);
}
