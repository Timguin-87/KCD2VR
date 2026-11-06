// Exact coherent callsite and physical kernel; the native sweep leaf only
// reports explicit synthetic clearance. The test proves policy/budget ownership,
// not native world geometry or a rigidbody push.
#define main historical_pose_adapter55
#include "pose_adapter_test.cpp"
#undef main
int main(){
 tests=failures=0;
 for(int mode:{4,6,7})for(bool fingers:{false,true}){
  Init();auto t=Tracking();Check(Run(t,3,true),"baseline ordinary pose establishes actual rig");
  g_weapon_control_mode=mode;mockPhysical=true;ph::preciseContacts44=fingers;
  auto& p=mockProfiles[0];p.key=777;p.generation=1;p.side=0;p.primary=true;p.classId=4;
  p.geometry=p.nativeSeed=true;p.tip={0,.8,0};p.axis={0,1,0};p.nativeWeapon.p={.25f,.1f,1.3f};
  mockSweepCalls55=0;mockSweepBudget55=false;mockSharedBudget124=true;handQueries124=0;
  ++t.sequence;t.tick+=11;
  Check(Run(t,4,true)&&physicalPublications==1,"ordinary handling publishes without a combat experiment");
  std::printf("POLICY55 mode=%d fingers=%d contact=%d queries=%u budget=%d\n",mode,fingers,mockPhysicalInput175.contact,mockSweepCalls55,mockSweepBudget55);
  Check(mockPhysicalInput175.contact&&mockSweepCalls55>0&&mockSweepBudget55,
        "every held-weapon mode owns current bounded sweep budget with optional hand contacts ON or OFF");
  Check((handQueries124>0)==fingers,"hand and finger contacts remain independently optional");
  Check(mockPhysicalOutput.active[0]&&mockPhysicalOutput.contactKnown[0],"native query clearance reaches actual held physical kernel");
  const auto queries=mockSweepCalls55;const auto publications=physicalPublications;
  g_gameplay_allowed=false;g_ctx_menu=true;++t.sequence;t.tick+=11;
  (void)Run(t,4,true);
  Check(mockSweepCalls55==queries&&physicalPublications==publications,"menu/native context cannot borrow weapon contact permission");
  g_gameplay_allowed=true;g_ctx_menu=false;t.right_valid=t.left_valid=false;++t.sequence;t.tick+=11;
  (void)Run(t,4,true);
  Check(mockSweepCalls55==queries&&physicalPublications==publications,"real tracking loss cannot grant a weapon sweep or current placement");
  mockSharedBudget124=false;
 }
 Init();ph::preciseContacts44=false;mockPhysical=true;g_weapon_control_mode=4;auto t=Tracking();
 mockProfiles={};mockSweepCalls55=0;(void)Run(t,4,true);
 Check(mockSweepCalls55==0&&!mockPhysicalOutput.active[0],"missing actual held profile cannot fabricate a physical weapon");
 ph::preciseContacts44=true;
 std::printf("weapon_contact_policy55=%d/%d; actual callsite/kernel; native/headset=NOT_EXECUTED\n",tests-failures,tests);
 return failures?1:0;
}
