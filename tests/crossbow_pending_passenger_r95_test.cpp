// Executes the actual production passenger consumer and quiver geometry.
// Native memory/attachment-completion remain explicit fixture leaves; the
// separate bolt adapter suite executes the native pending identity/timeout.
#define KCD2VR_SCOPE_NO_MAIN
#include "crossbow_scope_d82_test.cpp"
#undef KCD2VR_SCOPE_NO_MAIN
namespace geometryR95 {
namespace xcXR34=x;
struct V {float x=0,y=0,z=0;};
struct Q {float x=0,y=0,z=0,w=1;};
struct Snapshot {bool allowed=true,head_valid=true,right_valid=true,left_valid=true;
 bool head_held=false,right_held=false,left_held=false;V head{},right{},left{};Q hq{};};
#include "quiver_geometry_r95_actual.inc"
}
static unsigned checkedR95=0;
#define C95(c) do{++checkedR95;if(!(c)){std::cerr<<"FAIL_PENDING_R95 line="<<__LINE__<<" " #c "\n";return 1;}}while(0)
int main(){
 for(int kind:{10,14,15}){
  Fixture a;a.source.classId=kind;a.source.physicalD81=true;a.source.intentKnownD78=true;
  a.source.quiverKnownD72=true;a.spatial.toolBound=false;RetailScopeD82 scope(a,kind);
  pendingBoltLeafReadyR95=true;pendingBoltLeafSideR95=-1;start82(a,true);
  auto admission=admission82(a);auto pending=x::PassengerFromAdmission(admission);
  const auto check=[&](const x::Passenger& p){return ReloadPassengerCurrentXR34(p,a.source.character,a.source.body,a.now);};
  auto binding=pending.binding;binding.handBoltPendingR95=true;binding.handBoltBound=false;
  binding.boltId=77;binding.boltKey=pending.frame.ammoKey;binding.boltEntity=a.p(a.item);binding.boltAttachment=0;
  a.spatial=binding;a.put(a.p(a.parent)+0x88,77u);admission.binding=binding;x::admissionMailbox.publish(admission);pending.binding=binding;
  C95(check(pending)); // Fresh wrists survive only the positively observed native pending entity.
  auto changed=pending;changed.binding.boltId=78;C95(!check(changed));
  changed=pending;changed.binding.boltKey++;C95(!check(changed));
  changed=pending;changed.binding.boltEntity++;C95(!check(changed));
  changed=pending;changed.binding.boltAttachment=1;C95(!check(changed));
  changed=pending;changed.binding.handBoltBound=true;C95(!check(changed));
  pendingBoltLeafReadyR95=false;C95(!check(pending));pendingBoltLeafReadyR95=true; // native validator refusal/expiry
  pendingBoltLeafSideR95=1;C95(!check(pending));pendingBoltLeafSideR95=-1;
  a.put(a.p(a.parent)+0x88,78u);C95(!check(pending));a.put(a.p(a.parent)+0x88,77u);
  a.spatial.boltEntity++;C95(!check(pending));a.spatial.boltEntity--;
  // Native binding completes: old pending packet cannot acquire pose authority.
  a.spatial.handBoltPendingR95=false;a.spatial.handBoltBound=true;a.spatial.boltAttachment=a.p(a.skeleton);
  C95(!check(pending));
  auto bound=pending;bound.binding=a.spatial;admission.binding=a.spatial;x::admissionMailbox.publish(admission);
  C95(check(bound));
  a.put(a.p(a.parent)+0x88,0u);C95(!check(bound));C95(!check(pending));
  auto absent=pending;absent.binding.handBoltPendingR95=false;absent.binding.boltId=0;absent.binding.boltKey=0;
  absent.binding.boltEntity=absent.binding.boltAttachment=0;admission.binding=absent.binding;x::admissionMailbox.publish(admission);
  C95(check(absent));absent.binding.boltKey=1;C95(!check(absent));
  XR34BeforeStopNative(a.parent.data());
 }
 using namespace geometryR95;
 const auto zone=[](Snapshot t){x::Source s{};ChestFlagsD66C(s,t);return s;};
 Snapshot t{};t.right={.30f,-.10f,.40f};
 C95(zone(t).quiverKnownD72&&zone(t).handInQuiverD72[0]); // accepted bow reach outside former sphere
 t.right={.20f,-.10f,0};C95(!zone(t).handInQuiverD72[0]); // former front-of-shoulder sphere is not the bow region
 for(int axis=0;axis<3;++axis)for(int upper=0;upper<2;++upper){t={};t.right={.30f,-.10f,.30f};
  if(axis==0)t.right.x=upper?.55f:.06f;if(axis==1)t.right.y=upper?.25f:-.35f;if(axis==2)t.right.z=upper?.55f:.06f;
  C95(!zone(t).handInQuiverD72[0]);}
 t={};t.right={.30f,-.10f,.30f};t.hq={0,1,0,0};t.right={-.30f,-.10f,-.30f};C95(zone(t).handInQuiverD72[0]);
 t={};t.right={.30f,-.10f,.30f};t.hq={.25881904f,0,0,.96592583f};C95(zone(t).handInQuiverD72[0]);
 for(int fault=0;fault<7;++fault){t={};t.right={.30f,-.10f,.30f};
  if(fault==0)t.allowed=false;if(fault==1)t.right_held=true;if(fault==2)t.head_valid=false;
  if(fault==3)t.left_valid=false;if(fault==4)t.hq.w=0;if(fault==5)t.right.x=std::numeric_limits<float>::quiet_NaN();
  if(fault==6)t.hq={.70710678f,0,0,.70710678f};
  C95(!zone(t).quiverKnownD72&&!zone(t).handInQuiverD72[0]);}
 std::cout<<"PASS "<<checkedR95<<" actual passenger/quiver checks; native completion/timeout is an explicit fixture leaf\n";
}
