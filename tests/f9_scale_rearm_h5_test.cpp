#include "generated_f9_scale_fixture_h5.inc"
static bool CapturedItemSourceH5(int id,void* body,const void* entity) noexcept {
 NativeHeldSourceT4 s{};return g_native_held_valid_t4&&g_native_held_source_t4.read(s)&&s.valid&&
  id==s.input.nativeBodyId&&body==reinterpret_cast<void*>(s.input.nativeBody)&&entity==reinterpret_cast<void*>(s.input.owner.entityPointer);
}
static bool CapturedPlayerSourceH5(int id,void* body,const void* entity) noexcept {
 NativeHeldSourceT4 s{};return g_native_held_valid_t4&&g_native_held_source_t4.read(s)&&s.valid&&
  id==s.input.playerBodyId&&body==reinterpret_cast<void*>(s.input.playerBody)&&entity==reinterpret_cast<void*>(s.input.playerEntityPointer);
}
static unsigned checksH5=0,failuresH5=0;
static void checkH5(bool ok,const char* why){++checksH5;if(!ok){++failuresH5;std::fprintf(stderr,"FAIL %s\n",why);}}
static float translationH5=1;
static void WorldH5(double angle,float scale=1,bool planar=false){
 if(planar){const float co=float(std::cos(angle)),si=float(std::sin(angle));sourceWorldH5={{co*scale,-si*scale,0,translationH5,si*scale,co*scale,0,2,0,0,scale,3}};return;}
 const double sh=std::sin(angle*.5)/(planar?1.:std::sqrt(14.));
 const float x=planar?0.f:float(sh),y=planar?0.f:float(2*sh),z=float((planar?1:3)*sh),w=float(std::cos(angle*.5));
 sourceWorldH5={{(1-2*(y*y+z*z))*scale,2*(x*y-z*w)*scale,2*(x*z+y*w)*scale,translationH5,
  2*(x*y+z*w)*scale,(1-2*(x*x+z*z))*scale,2*(y*z-x*w)*scale,2,
  2*(x*z-y*w)*scale,2*(y*z+x*w)*scale,(1-2*(x*x+y*y))*scale,3}};
}
static heldT4::Input CaptureH5(const PhysEquipment6698& e,double angle,float scale=1,bool planar=false){
 WorldH5(angle,scale,planar);NativeHeldCaptureT4(e,fixture::now);NativeHeldSourceT4 s{};
 checkH5(g_native_held_valid_t4&&g_native_held_source_t4.read(s)&&s.valid,"actual current native capture accepts supplied finite affine source");
 checkH5(s.tick==fixture::now&&s.input.weaponWorld.p.x==translationH5,"production captured source tick and actual matrix position remain current before XR provider stamping");
 kcd2vr::grasp13::Pose raw{};const bool converted=kcd2vr::grasp13::fromMatrix(sourceWorldH5.m,raw);
 checkH5(converted&&s.input.weaponWorld.p.y==raw.p[1]&&s.input.weaponWorld.p.z==raw.p[2]&&
  s.input.weaponWorld.q.x==raw.q[0]&&s.input.weaponWorld.q.y==raw.q[1]&&s.input.weaponWorld.q.z==raw.q[2]&&s.input.weaponWorld.q.w==raw.q[3],
  "actual captured quaternion and position match current supplied matrix before XR provider stamping");
 auto in=s.input;in.allowed=in.tracked=true;in.sample=fixture::serial+1;in.tick=fixture::now;
 return in;
}
int main(){using namespace fixture;namespace w=kcd2vr::native_weapon_body_t4;
 checkH5(w::Install(),"actual backend Install with explicit native providers succeeds");
 for(float scale:{.5f,1.f,1.25f,2.f}){
  w::SetEnabled(true);auto e=setup(0);auto in=CaptureH5(e,0,scale);Active(in);
  const auto lease=in.colliderLease,incarnation=w::record.incarnation;const auto createCount=creates;
  const auto sourceTick0=in.tick;const auto firstScale=in.scale;
  for(double angle:{.681,.682,.689,.690,.696,.697,.770,.771}){
   now+=10;translationH5+=.001f;in=CaptureH5(e,angle,scale);
   checkH5(in.colliderLease==lease,"same authored scale 3D rotation preserves current collider lease");
   checkH5(in.scale==firstScale,"retained canonical geometry scale does not follow numerical column norm noise");
   checkH5(in.tick>sourceTick0&&std::abs(in.weaponWorld.q.z)>0.01,"fresh source clock and actual rotation are not frozen");
   body=w::policy::bodyTarget(in,w::policy::shape(in)); // explicit current solver/status leaf, not production pose write
   Fresh(in);Tick();w::Achieved achieved{};
   checkH5(w::Read(achieved)&&achieved.state==w::State::Active&&achieved.poseValid&&collision&&creates==createCount&&w::record.incarnation==incarnation,
    "actual producer -> actual backend Step keeps one live collider without reholster or recreate");
  }
  std::printf("LIFECYCLE trueScale=%.3g createDelta=%u destroyCount=%u originalIncarnation=%llu currentIncarnation=%llu\n",scale,creates-createCount,destroys,(unsigned long long)incarnation,(unsigned long long)w::record.incarnation);
  Cleanup();
 }
 w::SetEnabled(true); // Cleanup deliberately disabled the real backend API above.
 // A direct planar orthonormal float matrix is the negative control; it had
 // no source-scale churn in the original pure conversion probe.
 {auto e=setup(0);auto in=CaptureH5(e,0);const auto lease=in.colliderLease;
  for(double a:{.01,.1,.3,.8}){now+=10;in=CaptureH5(e,a,1,true);checkH5(in.colliderLease==lease,"planar constant authored scale retains original source identity");}}
 // Native object+0C/+10/+14 are EXACT identity fields even inside the former
 // <=1e-5 uniformity allowance: an authored one-ULP change must revoke.
 for(int axis=0;axis<3;++axis){auto e=setup(0);auto in=CaptureH5(e,0);const auto lease=in.colliderLease;
  put(object,0xC+4*axis,std::nextafter(1.f,2.f));now+=10;in=CaptureH5(e,0);
  checkH5(in.colliderLease!=lease,"any exact authored native scale axis change revokes source lease, even one ULP");}
 // Meaningful uniform source transforms still create a fresh geometry lease.
 for(bool authored:{false,true}){auto e=setup(0);auto in=CaptureH5(e,0);const auto lease=in.colliderLease;
  if(authored)for(int axis=0;axis<3;++axis)put(object,0xC+4*axis,1.01f);
  now+=10;in=CaptureH5(e,0,1.01f);checkH5(in.colliderLease!=lease,"genuine uniform matrix/object scale change revokes original collider lease");}
 // No tolerance walking: retained scale stays at1 through two float epsilons;
 // the third ULP exceeds THAT original anchor and must create a different lease.
 {auto e=setup(0);auto in=CaptureH5(e,0);const auto lease=in.colliderLease;float measured=1;
  for(int ulp=1;ulp<=4;++ulp){measured=std::nextafter(measured,2.f);now+=10;in=CaptureH5(e,0,measured);
   checkH5(ulp<=2?(in.colliderLease==lease&&in.scale==1):(in.colliderLease!=lease),"scale noise cannot walk canonical geometry beyond its original two-epsilon window");}}
 // The canonical geometry may be reused only from an actual recent prior
 // source. Original tick is never renewed to extend the80ms condition.
 for(unsigned gap:{80u,81u}){auto e=setup(0);auto in=CaptureH5(e,0);const auto lease=in.colliderLease;
  now+=gap;in=CaptureH5(e,.681);checkH5((in.colliderLease==lease)==(gap==80),"canonical retention uses original actual prior source80ms boundary");}
 {auto e=setup(0);auto in=CaptureH5(e,0);const auto lease=in.colliderLease;
  --now;in=CaptureH5(e,.681);checkH5(in.colliderLease!=lease,"backward source clock cannot admit canonical retention");++now;}
 // Other actual source identity changes remain hard owner changes.
 for(int change=0;change<7;++change){auto e=setup(0);auto in=CaptureH5(e,0);const auto lease=in.colliderLease;
  if(change==0){++e.items[0].profile.key;e.handKey[0]=e.items[0].profile.key;}
  if(change==1)++e.items[0].profile.generation;
  if(change==2)++e.request;
  if(change==3)++e.epoch;
  if(change==4)e.items[0].profile.weight+=.01;
  if(change==5)put(weaponBody,0x44,25u);
  if(change==6){put(model,0x2C,0u);put(model,0x2C+4*0x24,42u);fixed(WhBase()+0x40AC288+4*8,static_cast<const char*>("hand_right"));}
  now+=10;in=CaptureH5(e,.681);checkH5(in.colliderLease!=lease,"item/generation/request/bodyepoch/mass/nativebodyID/native slot changes revoke despite rounding window");}
 // Attachment/object/source-body loss never adopts a noisy saved pose. A later
 // valid same item must acquire a NEW lease after the real source was revoked.
 for(int loss=0;loss<4;++loss){auto e=setup(0);auto in=CaptureH5(e,0);const auto lease=in.colliderLease;
  if(loss==0)put(attachment,0x20,static_cast<void*>(nullptr));
  if(loss==1)put(weaponProxy,0x38,static_cast<void*>(nullptr));
  if(loss==2)e.handKey[1]=99;
  if(loss==3)put(link,0x75,std::uint8_t(1));
  now+=10;WorldH5(.681);NativeHeldCaptureT4(e,now);checkH5(!g_native_held_valid_t4,"hard current attachment/body/offhand/profile loss refuses source before canonicalization");
  if(loss==0)put(attachment,0x20,static_cast<void*>(object.data()));
  if(loss==1)put(weaponProxy,0x38,static_cast<void*>(weaponBody.data()));
  if(loss==2)e.handKey[1]=0;
  if(loss==3)put(link,0x75,std::uint8_t(0));
  now+=10;in=CaptureH5(e,.681);checkH5(in.colliderLease!=lease,"reacquisition after real source loss cannot reuse prior collider lease");}
 std::printf("F9_SCALE_REARM_H5 checks=%u failures=%u\n",checksH5,failuresH5);return failuresH5?1:0;
}
