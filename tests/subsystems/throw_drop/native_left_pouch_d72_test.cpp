// Actual T22 service. Native create/cost/Shoot/Hide leaves are explicit mocks.
// This tests normal/refusal flow, not Windows structured exception semantics.
#define main PreviousAmmoServiceMainD72
#include "native_ammo_service_t22_test.cpp"
#undef main
int main(){using namespace fixture;
 image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5600000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(image);
 struct Pin{std::uintptr_t rva;std::vector<unsigned char>bytes;};const Pin pins[]{
#include "pins_t22.inc"
 };for(const auto& p:pins)std::memcpy(image+p.rva,p.bytes.data(),p.bytes.size());
 Put(image+0x4072C78,0x338,WhBase()+0x7F1E3C);Put(image+0x3A2F888,0x148,WhBase()+0x1A81E20);Put(image+0x3A2F888,0x1F0,WhBase()+0x51EF4C);
 Reset(3,0);in.chestKnivesT26=true;wrt1::SetMode(0);std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
 // The separate delivery mailbox is an older withdrawn frame. Only the exact
 // complete source supplies this callback's wrists/input; live authority remains required.
 Publish(0,0);auto offered=in;offered.allowed=false;offered.sample-=1;g_supply_input_t2.publish(offered);
 AmmoServiceScriptT22(context,offered);Check(g_ammo_lua_ready22&&g_ammo_launcher_k37.ready);
 modelReady=false;Draw(1);Check(previewCalls==1&&attachCalls==0&&g_ammo_current22[1]>=0&&g_ammo_current22[0]<0);
 const int slot=g_ammo_current22[1];Check(hideCallsD72==1&&showCallsD72==0&&(Get<unsigned>(&entities[1],8)&0x10u));
 modelReady=true;Hold(1,.2f);Check(g_ammo_service22[slot].stage==svc22::Stage::Held&&showCallsD72==1&&!(Get<unsigned>(&entities[1],8)&0x10u)&&payCalls==0);
 Check(worldDisableCallsT26==2&&(Get<unsigned>(&worldProxyT26[1],0x28)&0x40u));
 Hold(1,.35f);in.near18[1][0]=false;now+=20;in.wrist18[1][4]=.5f;Tick(0,0);now+=20;Tick(0,0);
 Check(payCalls==1&&shootCalls==1&&counts[0]==2&&trackCalls==1&&g_ammo_current22[1]<0&&slots[1]==0);
 now+=20;Tick(0,0);Check(payCalls==1&&shootCalls==1); // no repeated debit/flight
 Reset(3,0);in.chestKnivesT26=true;std::strcpy(filenames[1],"Objects/manmade/weapons/daggers/dagger_common.cgf");
 modelReady=false;Draw(1);Check(g_ammo_current22[1]>=0);modelReady=true;showNoopD72=true;Hold(1,.2f);now+=20;Tick(0,0);
 Check(payCalls==0&&shootCalls==0&&counts[0]==3&&g_ammo_current22[1]<0); // invisible preview never becomes a paid held knife
 Reset(3,0);in.chestKnivesT26=true;Draw(0);Check(previewCalls==0&&g_ammo_current22[0]<0);
 std::printf("D72 actual LEFT pouch service %u checks PASS; native leaves mocked; no game/SEH claim\n",checks);
}
