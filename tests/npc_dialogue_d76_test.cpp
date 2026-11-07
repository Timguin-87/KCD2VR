#include "subsystems/dialogues/dialogue_participants_d76.h"
#include <cstdio>
#include <cstring>
#include <map>
using namespace kcd2vr;
static unsigned checks=0,failed=0;
static void ck(bool b,const char* text){++checks;if(!b){++failed;std::printf("FAIL %s\n",text);}}
int main(){
 using namespace lively_npc_d73;using namespace dialogue_state59c10r2;
 ck(!Enabled()&&!ListeningGestures(),"both optional features OFF by default");
 SetEnabled(true);auto g=mode.load();SetListeningGestures(true);
 ck(Enabled()&&ListeningGestures()&&mode.load()!=g,"body gesture flag has its own generation");
 SetListeningGestures(false);ck(Enabled()&&!ListeningGestures(),"body OFF preserves NPC life");
 State s{};s.known=true;s.entity=17;s.player=18;s.instance=200;s.tick=100;s.camera=Camera::Dialogue;
 s.membership.known=s.membership.active=s.membership.participant=s.membership.listed=true;
 s.membership.sceneD52=300;s.membership.managerD52=100;s.membership.playerSoulD52=0x123;
 ck(DialogueScene(s,180),"native current dialogue can present NPC while Henry body/input disabled");
 ck(!s.active&&!s.inputOwner.enabled,"fixture deliberately does not grant native input/player body");
 auto bad=s;bad.membership.participant=false;ck(!DialogueScene(bad,100),"not Henry participant refused");
 bad=s;bad.membership.listed=false;ck(!DialogueScene(bad,100),"inactive native scene refused");
 bad=s;bad.camera=Camera::Unknown;ck(!DialogueScene(bad,100),"external authored camera excluded");
 ck(!DialogueScene(s,181)&&!DialogueScene(s,99),"stale and future scene refused");
 Frame f{100,mode.load(),17,300,false,true,true,1,2,3,0,1,0};f.dialogueInstance=200;f.dialogueManager=100;f.playerSoul=0x123;
 constexpr std::uintptr_t base=0x10000000;std::map<std::uintptr_t,unsigned char> mem;
 auto put=[&](auto a,auto v){auto* b=reinterpret_cast<unsigned char*>(&v);for(unsigned i=0;i<sizeof(v);++i)mem[a+i]=b[i];};
 auto read=[&](std::uintptr_t a,auto& v){auto* b=reinterpret_cast<unsigned char*>(&v);for(unsigned i=0;i<sizeof(v);++i){auto at=mem.find(a+i);if(at==mem.end())return false;b[i]=at->second;}return true;};
 // Keep fixture addresses separate, as the real objects are.
 f.dialogueManager=0x1000;f.dialogueInstance=0x2000;f.scene=0x3000;
 put(f.dialogueManager,base+0x3E64C90);put(f.dialogueManager+0x90,f.dialogueInstance);
 put(f.dialogueInstance,base+0x3E63A40);put(f.dialogueInstance+0x18,base+0x3E63B60);
 put(f.dialogueInstance+0x158,f.scene);put(f.dialogueInstance+0x278,std::uint32_t(1));put(f.scene+0xBC,std::uint32_t(0));
 put(f.scene,std::uintptr_t(0x4000));put(f.scene+8,std::uintptr_t(0x4060));
 for(unsigned i=0;i<3;++i){put(0x4000+0x20*i,std::uint64_t(i?0xFEDCBA9876543210ull+i:f.playerSoul));put(0x4018+0x20*i,static_cast<unsigned char>(1));}
 auto m=Members(base,f,read);ck(m.known&&m.count==2&&m.ids[0]==0xFEDCBA9876543211ull,"exact native members retain all 64 bits, omit Henry");
 put(0x4038,static_cast<unsigned char>(0));m=Members(base,f,read);ck(m.known&&m.count==1&&m.ids[0]==0xFEDCBA9876543212ull,"nonparticipating role omitted");
 put(0x4018,static_cast<unsigned char>(0));ck(!Members(base,f,read).known,"Henry no longer participant retires snapshot");put(0x4018,static_cast<unsigned char>(1));
 put(f.scene+0xBC,std::uint32_t(8));ck(!Members(base,f,read).known,"native stop request wins");put(f.scene+0xBC,std::uint32_t(0));
 put(f.dialogueInstance+0x278,std::uint32_t(0x17));ck(!Members(base,f,read).known,"native stopped state wins");put(f.dialogueInstance+0x278,std::uint32_t(1));
 put(f.scene+8,std::uintptr_t(0x4220));ck(!Members(base,f,read).known,"bounded participant count");put(f.scene+8,std::uintptr_t(0x4060));
 unsigned reads=0;auto changed=[&](std::uintptr_t a,auto& v){const bool ok=read(a,v);if(a==f.scene&&++reads==2)put(f.dialogueManager+0x90,std::uintptr_t(0));return ok;};
 // Mutation at the first participant read must be caught by final identity join.
 auto mutate=[&](std::uintptr_t a,auto& v){const bool ok=read(a,v);if(a==0x4000)put(f.dialogueManager+0x90,std::uintptr_t(0));return ok;};
 ck(!Members(base,f,mutate).known,"scene replacement during snapshot cannot borrow list");
 std::printf("D76 NPC scene/member admission %u/%u; native memory mocked\n",checks-failed,checks);return failed?1:0;
}
