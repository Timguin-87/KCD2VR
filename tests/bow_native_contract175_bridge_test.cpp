// Executes helpers and the actual charge callback from the complete production
// translation unit. A native callback spy is used; no retail function is called.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
static unsigned checks=0,failures=0;static float forwarded=-1;
static void ck(bool ok,const char* name){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
static std::uint32_t ChargeSpy(void*,float dt){forwarded=dt;return 17;}
int main(){
    {PhysNativeOwner6698 n{};n.player=reinterpret_cast<void*>(0x12);n.request=3;n.epoch=4;n.key=7;
     n.bowHeld=n.physicalBow175=true;n.bowGeneration175=8;n.bowBeginSample175=10;
     PhysFrame6698 f{};f.request=3;f.input.epoch=3;f.input.bodyEpoch=4;f.input.sample=12;
     f.input.tick=1000;f.input.allowed=f.input.bowActions=f.input.tracked[0]=f.input.tracked[1]=true;
     f.input.bowDrawPolicy175=ph::BowDrawPolicy175::PhysicalString;f.input.bowStringTrigger160=true;
     f.profiles[1].key=7;f.profiles[1].generation=8;f.profiles[1].side=1;f.profiles[1].classId=9;
     f.profiles[1].primary=f.profiles[1].support=true;f.profiles[1].axis={0,-1,0};
     ph::Command c{};c.kind=ph::CommandKind::BowFire;c.side=1;c.key=7;c.generation=8;c.epoch=3;c.bodyEpoch=4;
     c.sample=12;c.tick=1000;c.fraction=.65;
     ck(!PhysBowReleaseReady175(n,c,f,1000,true,true),"actual native release predicate rejects missing automatic-string charge ACK");
     n.bowChargeObserved114=true;
     ck(PhysBowReleaseReady175(n,c,f,1000,true,true),"acknowledged intentional physical release passes native request preflight");
     f.input.bowGripDraw47c2=true;f.input.trigger[0]=0;f.input.grip[0]=0;
     ck(PhysBowReleaseReady175(n,c,f,1000,true,true),"Grip release uses the same native charge-ACK/identity preflight with Trigger idle");
     n.bowChargeObserved114=false;ck(!PhysBowReleaseReady175(n,c,f,1000,true,true),"Grip mode cannot bypass missing native charge ACK");
     n.bowChargeObserved114=true;f.input.bowGripDraw47c2=false;
     c.fraction=0;ck(!PhysBowReleaseReady175(n,c,f,1000,true,true),"native dispatch cannot fire stationary physical Trigger");c.fraction=.65;
     f.input.nativeBowTrigger126=true;ck(!PhysBowReleaseReady175(n,c,f,1000,true,true),"physical draw owner cannot switch to timed Trigger before release");f.input.nativeBowTrigger126=false;
     f.profiles[1].support=false;ck(!PhysBowReleaseReady175(n,c,f,1000,true,true),"lost nock geometry rejects native physical release");f.profiles[1].support=true;
     ck(!PhysBowReleaseReady175(n,c,f,1000,false,true),"lost live string hand rejects native release");
     c.generation=9;ck(!PhysBowReleaseReady175(n,c,f,1000,true,true),"replacement bow generation cannot inherit native ACK");c.generation=8;
     c.sample=10;ck(!PhysBowReleaseReady175(n,c,f,1000,true,true),"older release cannot fire a new begin on same bow");c.sample=12;
     ck(!PhysBowReleaseReady175(n,c,f,1101,true,true),"release lifetime remains original bounded 100 ms");
     f.input.nativeBowTrigger126=true;n.physicalBow175=false;c.fraction=0;
     ck(PhysBowReleaseReady175(n,c,f,1000,true,true),"explicit native draw retains native Trigger request semantics");}
    {const auto oldBase=hostModuleHandleW;const auto pinned=g_wh_pinned104.exchange(0);
     std::array<unsigned char,0xB00> character{};std::array<unsigned char,16> skeleton{},entity{};
     hostModuleHandleW=character.data();const auto base=WhBase();
     const auto put=[](unsigned char* p,const auto& value){std::memcpy(p,&value,sizeof(value));};
     const std::uintptr_t ownerType=base+kCDefaultSkeletonVftRva,poseType=base+0x1234;
     void* owner=skeleton.data();put(skeleton.data(),ownerType);
     put(character.data()+kPoseInCharInstanceOffset,poseType);
     put(character.data()+kPoseInCharInstanceOffset+kPoseDefaultSkeletonOffset,owner);
     const float world[8]{0,0,0,1,10,20,30,1};std::memcpy(character.data()+0xA08,world,sizeof(world));
     PhysEquipment6698 e{};e.request=4;e.epoch=5;
     PhysEquipmentItem6698 item{};item.held=true;item.entity=entity.data();
     item.profile.classId=9;item.profile.side=1;item.profile.key=7;item.profile.generation=8;
     PhysFrame6698 f{};f.request=4;f.input.bodyEpoch=5;f.input.tick=1000;f.input.sample=9;
     f.input.allowed=f.input.tracked[0]=f.input.tracked[1]=true;f.profiles[1]=item.profile;
     ModelWorld6692 body{};body.scale=1;ph::Pose target{{},{10,20,30}};
     g_bow_identity166.publish({character.data(),entity.data(),1000});g_bow_lifetimes59c10r2={};
     PhysPublishBowPlacement59c10r2(e,item,f,body,target,1000);
     bowgeom59::Placement receipt{};g_bow_placement59c10r2.read(receipt);
     ck(bowgeom59::fresh(receipt,1000)&&receipt.identity.owner==owner&&receipt.identity.poseType==poseType,
        "actual attachment receipt reads exact current pose owner and matching placed world transform");
     const auto serial=receipt.lifetime;
     target.p.x+=.031;PhysPublishBowPlacement59c10r2(e,item,f,body,target,1000);g_bow_placement59c10r2.read(receipt);
     ck(!bowgeom59::fresh(receipt,1000),"current placement mismatch cannot renew local geometry permission");
     target.p.x-=.031;PhysPublishBowPlacement59c10r2(e,item,f,body,target,1000);g_bow_placement59c10r2.read(receipt);
     ck(bowgeom59::fresh(receipt,1000)&&receipt.lifetime!=serial,"recovered placement starts a new local rig lifetime");
     f.input.tracked[0]=false;PhysPublishBowPlacement59c10r2(e,item,f,body,target,1000);g_bow_placement59c10r2.read(receipt);
     ck(!bowgeom59::fresh(receipt,1000),"lost string hand retires placement receipt even with matching native transform");
     f.input.tracked[0]=true;PhysPublishBowPlacement59c10r2(e,item,f,body,target,1081);g_bow_placement59c10r2.read(receipt);
     ck(!bowgeom59::fresh(receipt,1081),"attachment callback cannot freshen an old source frame");
     g_bow_identity166.publish({});g_bow_placement59c10r2.publish({});g_bow_lifetimes59c10r2={};
     g_wh_pinned104=pinned;hostModuleHandleW=oldBase;}
    {const auto oldBase=hostModuleHandleW;const auto pinned=g_wh_pinned104.exchange(0);
     std::array<unsigned char,0xB8> action{};hostModuleHandleW=action.data();
     const std::uintptr_t vft=WhBase()+0x47083C0;std::memcpy(action.data(),&vft,sizeof(vft));
     const auto oldOriginal=g_phys_charge_original6698;g_phys_charge_original6698=&ChargeSpy;
     g_phys_native6698={};g_phys_native6698.bowHeld=g_phys_native6698.physicalBow175=true;
     g_phys_native6698.bowChargeObserved114=true;g_head_valid=false;g_phys_abort6698=false;
     g_phys_charge_player175=reinterpret_cast<void*>(0x12);g_phys_charge_action175=action.data();
     ck(PhysChargeDetour6698(action.data(),.016f)==17&&forwarded==0&&g_phys_abort6698&&!g_phys_native6698.bowChargeObserved114,
        "actual owned charge callback uses zero dt and cancels on invalid tracking, preserving native callback");
     g_phys_charge_action175=nullptr;g_phys_charge_player175=nullptr;g_phys_abort6698=false;
     ck(PhysChargeDetour6698(action.data(),.016f)==17&&forwarded==.016f&&!g_phys_abort6698,
        "native unowned charge retains original dt");
     g_phys_native6698={};g_phys_charge_original6698=oldOriginal;g_wh_pinned104=pinned;hostModuleHandleW=oldBase;}
    {g_phys_native_complete175={};PhysFrame6698 f{};f.input.sample=21;f.input.tick=9980;f.input.timeNs=9980000000;
     g_phys_frame6698.publish(f);g_phys_ready_sample6698=21;
     ck(PhysLatestComplete175().input.sample==21,"complete native charge frame is accepted after publication marker");
     f.input.sample=22;f.input.tick=9990;f.input.timeNs=9990000000;g_phys_frame6698.publish(f);
     const auto& pending=PhysLatestComplete175();
     ck(pending.input.sample==21&&pending.input.tick==9980&&pending.input.timeNs==9980000000,
        "in-progress frame never replaces prior complete charge frame or rejuvenates source timestamp");
     g_phys_ready_sample6698=22;ck(PhysLatestComplete175().input.sample==22,"completed new frame becomes charge owner after marker");}
    {ph::Profile p{};p.classId=9;p.side=1;p.primary=p.geometry=true;p.key=9;p.generation=3;p.tip={0,0,.6};
     pm::Vec3 axis{};ck(ph::structural_bow_axis175(p,{0,-.15,.08},axis)&&axis.y<-.99&&std::abs(axis.z)<1e-9,
        "named resting string and real bow long axis qualify first draw before a full-shot calibration");
     ck(!ph::structural_bow_axis175(p,{0,0,.3},axis),"degenerate resting string cannot invent a draw axis");
     PhysEquipment6698 e{};e.request=4;e.epoch=5;
     BowGeometry166 b{};b.key=9;b.generation=3;b.request=4;b.epoch=5;b.tick=1000;
     b.character=reinterpret_cast<void*>(0x1234);g_bow_char128=b.character;b.nock={0,-.15,.08};b.axis={0,-1,0};b.restStructural175=true;
     b.entity=reinterpret_cast<void*>(0x2345);b.pose=reinterpret_cast<void*>(0x3456);b.owner=reinterpret_cast<void*>(0x4567);
     b.poseType=0x5678;b.scale=1;b.lifetime=1;
     bowgeom59::Placement placed{static_cast<const bowgeom59::Identity&>(b),1,1000,1000,1};g_bow_placement59c10r2.publish(placed);
     e.valid=true;e.tick=1000;e.count=1;e.heldIndex[1]=0;e.handKnown[1]=true;e.handKey[1]=9;
     e.items[0].held=true;e.items[0].entity=b.entity;e.items[0].profile=p;
     ck(PhysApplyBowGeometry166(b,e,p,1000)&&p.support&&p.bowDrawLength166==0,
        "qualified structural geometry uses existing calibrated draw length with no full native shot requirement");
     e.tick=1151;ck(!PhysApplyBowGeometry166(b,e,p,1151),"named rest geometry requires fresh placement, not an extended tracking timeout");
     placed.tick=placed.sourceTick=1151;g_bow_placement59c10r2.publish(placed);
     ck(PhysApplyBowGeometry166(b,e,p,1151),"fresh placement retains local named-rest geometry beyond former 150 ms expiry");
     b.generation=4;ck(!PhysApplyBowGeometry166(b,e,p,1151),"another bow generation cannot donate resting string geometry");
     g_bow_placement59c10r2.publish({});}
    {const auto oldBase=hostModuleHandleW;const auto pinned=g_wh_pinned104.exchange(0);
     std::array<unsigned char,0xA00> player{};std::array<unsigned char,0x240> actor{},opponent{};
     std::array<unsigned char,0xD10> state{},enemyState{};std::array<unsigned char,0x290> targetActor{};
     std::array<unsigned char,0x30> entity{},henryEntity{};hostModuleHandleW=player.data();const auto base=WhBase();
     const auto put=[](unsigned char* p,const auto& value){std::memcpy(p,&value,sizeof(value));};
     const auto pointer=[&](unsigned char* p,void* value){put(p,value);};
     std::uintptr_t vt=base+0x3AA4B98;put(player.data(),vt);vt=base+0x3A80610;put(actor.data(),vt);
     vt=base+0x3E3FAC8;put(opponent.data(),vt);vt=base+0x4072C78;put(entity.data(),vt);put(henryEntity.data(),vt);
     pointer(player.data()+0x38,henryEntity.data());g_last_player_entity=henryEntity.data();
     std::uint32_t id=42;put(entity.data()+0xC,id);
     pointer(player.data()+0x278,actor.data());pointer(actor.data()+0x1F8,player.data());
     pointer(actor.data()+0x210,state.data());pointer(state.data()+0xCE0,actor.data());pointer(state.data()+0xCF8,opponent.data());
     pointer(opponent.data()+0x210,enemyState.data());pointer(enemyState.data()+0xCE0,opponent.data());
     pointer(opponent.data()+0x1F8,targetActor.data());pointer(targetActor.data()+0x278,opponent.data());pointer(targetActor.data()+0x38,entity.data());
     PhysEquipment6698 e{};e.valid=true;e.player=player.data();e.tick=hostTickCount64;e.request=1;e.epoch=2;
     g_weapon_request_epoch6694=1;g_pose_epoch6689=2;g_phys_equipment6698.publish(e);g_npc_query172.requested=g_npc_query172.ready=true;
     kcd2vr::retail_build112::hostRetailVerified112=true;
     npc172::Witness w{};w.mask=npc172::BodiesAndWorld;w.body=npc172::Body::Articulated;w.part=3;
     w.ownerRead=true;w.owner=reinterpret_cast<std::uintptr_t>(entity.data());w.entityId=id;w.collider=0x1234;
     ck(PhysContactOpponent175(w),"first-contact intent requires complete current Henry-opponent-to-ray-entity identity");
     pointer(targetActor.data()+0x278,actor.data());ck(!PhysContactOpponent175(w),"missing reciprocal NPC combat identity remains passive contact");
     pointer(targetActor.data()+0x278,opponent.data());pointer(state.data()+0xCF8,actor.data());
     ck(!PhysContactOpponent175(w),"Henry self/current-opponent change cannot inherit actor-contact admission");
     pointer(state.data()+0xCF8,opponent.data());w.entityId=43;ck(!PhysContactOpponent175(w),"articulated body from another entity cannot admit current-target stroke");
     kcd2vr::retail_build112::hostRetailVerified112=false;g_last_player_entity=nullptr;g_npc_query172={};g_wh_pinned104=pinned;hostModuleHandleW=oldBase;}
    {BowRig128 r{};r.count=2;r.wroteLast=true;
     std::array<QuatT28,kBowJoints128> cur{};for(auto& q:cur)q.q[3]=1;
     cur[1].t[1]=-.3f;r.lastWritten=cur;
     for(auto tick:{1000ull,1200ull,1400ull,2000ull})BowLearnNativeSample175(r,cur,tick,false,0);
     ck(r.wroteLast&&!r.haveRest&&r.learnRest==0&&r.idleSince==0,
        "repeated identical own bow output across idle and skipped callbacks is never relearned as native rest");
     BowLearnNativeSample175(r,cur,2500,true,1);
     ck(r.wroteLast&&!r.haveFull&&r.learnFull==0,
        "unchanged own bow output cannot become a native full-draw calibration sample");
     cur[1].t[1]=0;BowLearnNativeSample175(r,cur,3000,false,0);BowLearnNativeSample175(r,cur,3300,false,0);
     ck(!r.wroteLast&&r.haveRest&&r.learnRest==1&&r.restModel[1].t[1]==0,
        "different native rest clears output provenance and learns after the original idle dwell");
     r={};r.count=2;BowLearnNativeSample175(r,cur,4000,false,0);BowLearnNativeSample175(r,cur,4300,false,0);
     ck(r.haveRest&&r.learnRest==1&&!r.wroteLast,
        "new validated rig lifetime can learn its native rest without inheriting old visual output");}
    {BowRig128 r{};r.count=4;r.haveRest=true;r.namedString166=1;r.namedArrow166=2;
     r.parent[0]=-1;r.parent[1]=0;r.parent[2]=1;r.parent[3]=0;BowComposeOrder128(r);
     std::array<QuatT28,kBowJoints128> cur{},local{},model{};std::array<bool,kBowJoints128> mask{};
     for(auto& q:r.restModel)q.q[3]=1;r.restModel[2].t[0]=.02f;cur=r.restModel;
     cur[1].t[1]=-.20f;cur[2].t[1]=-.20f;cur[3].t[1]=-.04f;
     ck(BowDrawPartial48(r,cur,.10,local,model,mask),"first partial draw has whole-chain visual ownership before a full shot");
     ck(std::fabs(model[1].t[1]+.10f)<1e-6&&std::fabs(model[3].t[1]+.02f)<1e-6,
        "first pull interpolates centre and other string branches together");
     const auto arrow=RelativeFromAbsolute(model[1],model[2]);
     ck(std::fabs(arrow.t[0]-.02f)<1e-6&&std::fabs(arrow.t[1])<1e-6,"first-shot arrow retains its native string-child offset");
     ck(BowDrawPartial48(r,cur,.4,local,model,mask)&&std::fabs(model[1].t[1]+.20f)<1e-6,"partial preview never extrapolates an unobserved full pose");
     BowLearnNativeSample175(r,cur,1000,true,.4f);BowLearnNativeSample175(r,cur,1010,true,.4f);
     ck(!r.partialReady48,"one/two native samples cannot establish a partial axis");
     BowLearnNativeSample175(r,cur,1020,true,.4f);ck(r.partialReady48&&r.partialAxis48.y<-.99f&&!r.haveFull,
        "three consistent partial samples qualify real draw direction without claiming a full draw");
     cur[1].t[0]=.3f;BowLearnNativeSample175(r,cur,1030,true,.4f);ck(r.partialAxis48.y<-.99f,"qualified partial axis remains immutable until the full native pair is learned");
     r.haveRest=false;ck(!BowDrawPartial48(r,cur,.1,local,model,mask),"unknown rest cannot authorize first-draw writes");}
    {BowRig128 r{};r.count=4;r.namedString166=1;r.namedArrow166=2;
     r.parent[0]=-1;r.parent[1]=0;r.parent[2]=1;r.parent[3]=0;BowComposeOrder128(r);
     std::array<QuatT28,kBowJoints128> local{},model{};std::array<bool,kBowJoints128> mask{};
     for(auto& p:local)p.q[3]=1;
     local[1].t[1]=-.2f;local[2].t[0]=.02f;local[2].t[1]=.06f;local[3].t[2]=.6f;
     for(unsigned k=0;k<r.count;++k){int j=r.order[k],p=r.parent[j];model[j]=p<0?local[j]:AbsoluteFromRelative(model[p],local[j]);}
     const auto original=model;QuatT28 arrowOffset{};const double half=std::sqrt(.5);
     ph::Pose target{{0,0,half,half},{.2,-.4,.3}};
     ck(BowGripHierarchy36c1(r,true,target,arrowOffset,local,model,mask),"actual bow hierarchy writer stages Grip rotation and child arrow");
     ck(Len(Sub(FromT(model[1]),VP(target.p)))<1e-6&&pm::angle(PQ(QFromQuatT(model[1])),target.q)<1e-6,
        "actual string model pose matches the full rotated hand target");
     auto relative=RelativeFromAbsolute(model[1],model[2]);
     ck(Len(Sub(FromT(relative),FromT(arrowOffset)))<1e-6&&pm::angle(PQ(QFromQuatT(relative)),PQ(QFromQuatT(arrowOffset)))<1e-6,
        "actual arrow keeps its captured nock position and rotation");
     ck(!mask[0]&&mask[1]&&mask[2]&&!mask[3]&&std::memcmp(&model[3],&original[3],sizeof(QuatT28))==0,
        "Grip writes only string subtree; root and unrelated bow limb remain native");
     local[2].t[0]=.15f;QToQuatT(local[2],{0,0,float(half),float(half)});
     target.q={half,0,0,half};target.p={-.1,-.5,.2};
     ck(BowGripHierarchy36c1(r,false,target,arrowOffset,local,model,mask),"held Grip updates using same native arrow attachment");
     relative=RelativeFromAbsolute(model[1],model[2]);
     ck(Len(Sub(FromT(relative),FromT(arrowOffset)))<1e-6&&pm::angle(PQ(QFromQuatT(relative)),PQ(QFromQuatT(arrowOffset)))<1e-6,
        "native local-arrow animation cannot pull or rotate arrow out of latched fingers");
     r.parent[2]=0;BowComposeOrder128(r);
     ck(!BowGripHierarchy36c1(r,true,target,arrowOffset,local,model,mask),"unrelated arrow joint is never treated as string child");}
    {BowRig128 r{};r.count=5;r.namedString166=1;r.namedArrow166=2;
     r.parent[0]=-1;r.parent[1]=0;r.parent[2]=1;r.parent[3]=0;r.parent[4]=0;BowComposeOrder128(r);
     for(auto* a:{&r.restModel,&r.fullModel})for(auto& q:*a)q.q[3]=1;
     r.restModel[1].t[1]=-.15f;r.fullModel[1].t[1]=-.70f;
     r.restModel[2].t[1]=-.10f;r.fullModel[2].t[1]=-.65f;
     // Top/bottom string vertices are separate branches, not centre descendants.
     for(int j:{3,4}){r.restModel[j].t[1]=-.075f;r.fullModel[j].t[1]=-.35f;
         r.restModel[j].t[2]=r.fullModel[j].t[2]=j==3?.3f:-.3f;}
     r.haveRest=r.haveFull=true;BowAnalyse128(r);ck(r.analysed,"multi-branch native string rig admits measured geometry");
     const auto rest=r.restModel,full=r.fullModel;
     auto cur=r.fullModel;cur[2].t[0]=.15f;
     std::array<QuatT28,kBowJoints128> local{},model{};std::array<bool,kBowJoints128> mask{};
     ck(BowDrawHierarchy44c2(r,cur,.5f,local,model,mask),"production draw stage composes one fraction across separate string branches");
     ck(std::abs(model[1].t[1]+.425f)<1e-6&&std::abs(model[3].t[1]+.2125f)<1e-6&&
        std::abs(model[4].t[1]+.2125f)<1e-6&&model[2].t[0]==0&&mask[1]&&mask[2]&&mask[3]&&mask[4]&&!mask[0],
        "centre, ends and arrow stay in the authored draw plane; native arrow offset drift cannot deform the string");
     BowLearnNativeSample175(r,cur,1000,false,0);BowLearnNativeSample175(r,cur,1500,false,0);
     BowLearnNativeSample175(r,cur,2000,true,1);
     ck(std::memcmp(rest.data(),r.restModel.data(),r.count*sizeof(QuatT28))==0&&
        std::memcmp(full.data(),r.fullModel.data(),r.count*sizeof(QuatT28))==0,
        "analysed rest/full geometry remains one immutable pair for the same bow lifetime");
     ck(!BowDrawHierarchy44c2(r,cur,NAN,local,model,mask),"invalid draw fraction cannot stage native writes");}
    std::printf("bow_native_contract175_bridge=%u/%u native_execution=NONE\n",checks-failures,checks);return failures?1:0;
}
