// V66.100 persistent anthropometry and explicit native-finger capture.
// Included after the coherent anatomy + curve reference implementation.
namespace fc100=kcd2vr::finger_capture;
namespace cp100=kcd2vr::calibration_profile;
struct FingerPacket66100 {fc100::Pair pair{};std::uint64_t generation=0,session=0,tick=0;};
static kcd2vr::core::SnapshotMailbox<FingerPacket66100> g_finger_saved66100,g_finger_offer66100,g_finger_candidate66100;
static std::atomic<bool> g_finger_candidate_ready66100{false};
static std::atomic<std::uint64_t> g_finger_rig_request66100{0},g_finger_saved_generation66100{0};
static std::atomic<int> g_finger_capture_status66100{0},g_persistence_status66100{0};
// Stable structural identity is separate from tolerant rest-geometry matching.
// Old raw-float hash sections are searched by the worker, never by skinning.
struct FingerRigRequest101R2 {std::uint64_t rig=0;std::array<fc100::Hand,2> hand{};};
static kcd2vr::core::SnapshotMailbox<FingerRigRequest101R2> g_finger_rig_descriptor101R2;
static std::atomic<bool> g_finger_ini_diag105{false}; // V66.105 rung-1 request for an INI explanation
static std::atomic<int> g_finger_profile_status101R2{0}; // 0 pending; 1 loaded; -1 missing; -2 ambiguous

static char g_calibration_ini66100[MAX_PATH*2]{}; // Assigned once, before hooks/worker start.
struct CaptureRuntime66100 {
 void* owner=nullptr;void* character=nullptr;std::uint32_t count=0;std::uint64_t rig=0,session=0,epoch=0,applied=0;fc100::Recorder recorder{};
 std::uint32_t crc[2][32]{};bool offerQueued=false;
};
static CaptureRuntime66100 g_capture_runtime66100{}; // Native callback, under g_pose_mutex6689 ONLY.
static std::string ReadProfileKey66100(const char*section,const char*key) {
 std::array<char,32768> buffer{};
 const auto n=GetPrivateProfileStringA(section,key,"",buffer.data(),static_cast<DWORD>(buffer.size()),g_calibration_ini66100);
 return n>=buffer.size()-1?std::string{}:std::string(buffer.data(),n);
}
template<class T>static bool LoadProfile66100(const char*section,T&out) {
 return cp100::load_bank(out,[&](const char*k){return ReadProfileKey66100(section,k);});
}
template<class T>static bool SaveProfile66100(const char*section,const T&value) {
 if(!g_calibration_ini66100[0])return false;
 const auto text=cp100::encode(value);if(text.size()>=30000)return false;
 kcd2vr::core::config_file_guard_d62::Lease iniLeaseD62(g_calibration_ini66100);
 if(!iniLeaseD62.valid())return false; // keep in-memory bank; never create a partial settings file
 return cp100::save_bank(text,[&](const char*k){return ReadProfileKey66100(section,k);},
  [&](const char*k,const std::string&v){return WritePrivateProfileStringA(section,k,v.c_str(),g_calibration_ini66100)!=0;},
  [&](){
   // Microsoft documents ZERO on a flush, too. Readback, not that return value,
   // verifies the data. Never interpret a successful flush as a failed save.
   (void)WritePrivateProfileStringA(nullptr,nullptr,nullptr,g_calibration_ini66100);return true;}
);
}
static bool SaveHeadKeyD62(const char* key,const char* value) {
 if(!g_calibration_ini66100[0])return false;
 kcd2vr::core::config_file_guard_d62::Lease iniLeaseD62(g_calibration_ini66100);
 if(!iniLeaseD62.valid())return false;
 const bool ok=WritePrivateProfileStringA("VRIK.Head",key,value,g_calibration_ini66100)!=0;
 (void)WritePrivateProfileStringA(nullptr,nullptr,nullptr,g_calibration_ini66100);return ok;
}
static bool CompatibleFingerPair101R2(const fc100::Pair& pair,const FingerRigRequest101R2& rig) noexcept {
 if(!fc100::valid(pair))return false;
 for(int side=0;side<2;++side)if(!fc100::same_rig(rig.hand[side],pair.open[side])||!fc100::same_rig(rig.hand[side],pair.closed[side]))return false;
 return true;
}
static bool EquivalentFingerPair101R2(const fc100::Pair& a,const fc100::Pair& b) noexcept {
 for(int side=0;side<2;++side)for(int n=0;n<a.open[side].count;++n){
  if(pc::angle(a.open[side].joints[n].q,b.open[side].joints[n].q)>1e-5||pc::angle(a.closed[side].joints[n].q,b.closed[side].joints[n].q)>1e-5)return false;
 }return true;
}
// Returns 1 compatible, 0 absent/incompatible/corrupt, -1 ambiguous/truncated.
static int RecoverFingerPair101R2(const char* exact,const FingerRigRequest101R2& rig,fc100::Pair& result) {
 fc100::Pair candidate{};
 if(LoadProfile66100(exact,candidate)&&CompatibleFingerPair101R2(candidate,rig)){result=candidate;return 1;}
 std::array<char,65536> names{};
 const auto used=GetPrivateProfileSectionNamesA(names.data(),static_cast<DWORD>(names.size()),g_calibration_ini66100);
 if(used>=names.size()-2)return -1;
 bool found=false;unsigned examined=0;
 for(std::size_t pos=0;pos<used&&names[pos];){const char* section=names.data()+pos;
  const auto length=std::strlen(section);pos+=length+1;
  if(std::strncmp(section,"VRIK.Fingers.",13)!=0)continue;
  if(++examined>256)return -1;
  if(!LoadProfile66100(section,candidate)||!CompatibleFingerPair101R2(candidate,rig))continue;
  if(found&&!EquivalentFingerPair101R2(result,candidate))return -1;
  result=candidate;found=true;
 }
 return found?1:0;
}
static void ConfigurePersistence66100(const char*path) noexcept {
 if(!path||!*path||std::strlen(path)>=sizeof(g_calibration_ini66100))return;
 std::memcpy(g_calibration_ini66100,path,std::strlen(path)+1);
 {char t[32]{};GetPrivateProfileStringA("VRIK.Head","StandingEyeHeightM","",t,sizeof(t),g_calibration_ini66100);char* e=nullptr;const float v=std::strtof(t,&e);
  if(e!=t&&std::isfinite(v)&&v>.5f&&v<2.5f){g_standing_eye_floor_m107=v;char m[160]{};std::snprintf(m,sizeof(m),"V66.107-HEAD STANDING_REFERENCE_LOADED standingEye=%.3fm",v);diagnostic_trace::Write(m);}}
 {char t[96]{};GetPrivateProfileStringA("VRIK.Head","CalibratedHeadAnchorM","",t,sizeof(t),g_calibration_ini66100);float a[3]{};
  if(std::sscanf(t,"%f %f %f",&a[0],&a[1],&a[2])==3&&kcd2vr::calibration_restore43::anchorPlausible(a)){
   g_cal_anchor_x43=a[0];g_cal_anchor_y43=a[1];g_cal_anchor_z43=a[2];g_cal_anchor_saved43.store(true,std::memory_order_release);
   char m[200]{};std::snprintf(m,sizeof(m),"C43 CALIBRATION HEAD_ANCHOR_LOADED anchor=(%.3f,%.3f,%.3f) (Henry's head at the F8 T-pose; re-seats keep it)",a[0],a[1],a[2]);diagnostic_trace::Write(m);}}
 try {cp100::Arms profile{};if(LoadProfile66100("VRIK.ArmCalibration",profile)){
   (void)g_arm_profile66100.publish(profile);g_saved_arm_ready66100=true;g_cal_reanchor66100=true;
   diagnostic_trace::Write("V66.100-CAL ARM_PROFILE_LOADED source=ASI_SIBLING_INI origin=REANCHOR_LAZILY fingerCapture=NOT_REQUIRED");
 }}catch(...){diagnostic_trace::Write("V66.100-CAL INI_LOAD_FAILED noRepeatedCapture=1 nativeFallback=1");}
}
// ONLY the mod worker calls this; never serializes, accesses files, or invokes
// native gameplay getters from a skinning callback.
static void ServiceCalibrationIo66100() noexcept {
 static std::uint64_t armAttempt=0,rigAttempt=0,offerAttempt=0,generation=0;
 try {
  const auto armGeneration=g_arm_save_generation66100.load(std::memory_order_acquire);
  if(armGeneration&&armGeneration!=armAttempt){cp100::Arms profile{};if(g_arm_profile66100.read(profile)){
   armAttempt=armGeneration;const bool ok=SaveProfile66100("VRIK.ArmCalibration",profile);g_persistence_status66100=ok?2:-1;
   diagnostic_trace::Write(ok?"V66.100-CAL ARM_PROFILE_SAVED verifiedReadback=1 ephemeralOrigins=NOT_SAVED":"V66.100-CAL ARM_PROFILE_SAVE_FAILED inMemoryCalibrationRetained=1");
  }}
  if(g_standing_eye_save107.exchange(false,std::memory_order_acq_rel)&&g_calibration_ini66100[0]){
   char v[32]{};std::snprintf(v,sizeof(v),"%.4f",double(g_standing_eye_floor_m107.load()));
   const bool ok=SaveHeadKeyD62("StandingEyeHeightM",v);
   char m[160]{};std::snprintf(m,sizeof(m),"V66.107-HEAD STANDING_REFERENCE_SAVED standingEye=%sm ok=%d",v,ok?1:0);diagnostic_trace::Write(m);
  }
  if(g_cal_anchor_save43.exchange(false,std::memory_order_acq_rel)&&g_calibration_ini66100[0]&&g_cal_anchor_saved43.load(std::memory_order_acquire)){
   char v[96]{};std::snprintf(v,sizeof(v),"%.4f %.4f %.4f",double(g_cal_anchor_x43.load()),double(g_cal_anchor_y43.load()),double(g_cal_anchor_z43.load()));
   const bool ok=SaveHeadKeyD62("CalibratedHeadAnchorM",v);
   char m[200]{};std::snprintf(m,sizeof(m),"C43 CALIBRATION HEAD_ANCHOR_SAVED anchor=(%s) ok=%d",v,ok?1:0);diagnostic_trace::Write(m);
  }
  const auto rig=g_finger_rig_request66100.load(std::memory_order_acquire);
  if(rig&&rig!=rigAttempt&&g_calibration_ini66100[0]){rigAttempt=rig;char section[64]{};std::snprintf(section,sizeof(section),"VRIK.Fingers.%016llX",(unsigned long long)rig);
   FingerRigRequest101R2 descriptor{};
   if(!g_finger_rig_descriptor101R2.read(descriptor)||descriptor.rig!=rig){rigAttempt=0;
    static std::uint64_t lastLog=0;const auto now=std::uint64_t(GetTickCount64());
    if(now-lastLog>5000){lastLog=now;diagnostic_trace::Write("V66.105-FINGERS PROFILE_LOAD_DEFERRED reason=RIG_DESCRIPTOR_NOT_READABLE (was silent before V66.105)");}
    return;}
   fc100::Pair pair{};const int recovered=RecoverFingerPair101R2(section,descriptor,pair);
   if(recovered==1){const bool migrated=pair.rig!=rig;pair.rig=rig;FingerPacket66100 packet{pair,++generation,0};
    if(g_finger_saved66100.publish(packet)){g_finger_saved_generation66100.store(packet.generation,std::memory_order_release);g_finger_profile_status101R2=1;
     char m[640]{};std::snprintf(m,sizeof(m),"V66.101R2-FINGERS PROFILE_LOADED source=INI legacyHashRecovered=%d structuralAndGeometryGate=PASS savedRotations=UNCHANGED path=%s",migrated,g_calibration_ini66100);diagnostic_trace::Write(m);
    }else rigAttempt=0;
   }else{g_finger_profile_status101R2=recovered<0?-2:-1;
    char m[640]{};std::snprintf(m,sizeof(m),"V66.101R2-FINGERS PROFILE_UNAVAILABLE reason=%s automaticRecapture=DISABLED curveFallback=NOT_ACCEPTED_PROFILE path=%s",recovered<0?"AMBIGUOUS_OR_TRUNCATED":"MISSING_CORRUPT_OR_INCOMPATIBLE",g_calibration_ini66100);diagnostic_trace::Write(m);
   }
  }
  if(g_finger_ini_diag105.exchange(false,std::memory_order_acq_rel)&&g_calibration_ini66100[0]){
   // V66.105 F10 rung 1: explain exactly what the INI holds and why it is (not) used.
   std::array<char,65536> names{};const auto used=GetPrivateProfileSectionNamesA(names.data(),static_cast<DWORD>(names.size()),g_calibration_ini66100);
   FingerRigRequest101R2 descriptor{};const bool haveDescriptor=g_finger_rig_descriptor101R2.read(descriptor);
   unsigned sections=0;
   for(std::size_t pos=0;pos<used&&names[pos];){const char* section=names.data()+pos;pos+=std::strlen(section)+1;
    if(std::strncmp(section,"VRIK.Fingers.",13)!=0)continue;++sections;
    fc100::Pair candidate{};const bool decoded=LoadProfile66100(section,candidate);
    const char* reason=decoded?"COMPATIBLE":"DECODE_FAILED_OR_NO_ACTIVE_BANK";
    int badSide=-1,badJoint=-1;double badT=0;
    if(decoded&&haveDescriptor)for(int side=0;side<2&&badSide<0;++side){const auto& a=descriptor.hand[side];const auto& b=candidate.open[side];
     if(a.count!=b.count){reason="JOINT_COUNT_DIFFERS";badSide=side;break;}
     for(int i=0;i<a.count;++i){const auto& x=a.joints[i];const auto& y=b.joints[i];
      if(x.id!=y.id){reason="JOINT_ID_DIFFERS";badSide=side;badJoint=i;break;}
      if(x.parent!=y.parent){reason="PARENT_DIFFERS";badSide=side;badJoint=i;break;}
      if(x.crc!=y.crc){reason="JOINT_NAME_CRC_DIFFERS";badSide=side;badJoint=i;break;}
      if(x.digit!=y.digit){reason="DIGIT_DIFFERS";badSide=side;badJoint=i;break;}
      const double t=pc::length(x.t-y.t);if(t>.003){reason="BONE_OFFSET_DIFFERS_GT_3MM";badSide=side;badJoint=i;badT=t;break;}}}
    else if(decoded)reason="DECODED_BUT_RUNTIME_RIG_NOT_READY";
    char m[512]{};std::snprintf(m,sizeof(m),"V66.105-FINGER-DIAG INI section=%s decoded=%d storedRig=%016llX runtimeRig=%016llX verdict=%s side=%d joint=%d offsetDiff=%.4f",
     section,decoded?1:0,(unsigned long long)candidate.rig,(unsigned long long)(haveDescriptor?descriptor.rig:0),reason,badSide,badJoint,badT);
    diagnostic_trace::Write(m);
   }
   char m[400]{};std::snprintf(m,sizeof(m),"V66.105-FINGER-DIAG INI path=%s fingerSections=%u runtimeRigPublished=%d profileStatus=%d (1 loaded,0 pending,-1 missing/incompatible,-2 ambiguous)",
    g_calibration_ini66100,sections,haveDescriptor?1:0,g_finger_profile_status101R2.load());diagnostic_trace::Write(m);
  }
  FingerPacket66100 offer{};
  if(g_finger_offer66100.read(offer)&&offer.session&&offer.session!=offerAttempt&&offer.session==g_capture_session66100.load(std::memory_order_acquire)&&fc100::valid(offer.pair)){
   // The complete candidate is accepted only on an observed release or explicit
   // F10 preview request. Pressed buttons alone cannot produce this offer.
   offerAttempt=offer.session;offer.generation=++generation;
   if(!g_finger_saved66100.publish(offer)){offerAttempt=0;return;}
   g_finger_saved_generation66100.store(offer.generation,std::memory_order_release);
   char section[64]{};std::snprintf(section,sizeof(section),"VRIK.Fingers.%016llX",(unsigned long long)offer.pair.rig);
   const bool ok=SaveProfile66100(section,offer.pair);g_persistence_status66100=ok?2:-1;
   diagnostic_trace::Write(ok?"V66.100-FINGERS PAIR_SAVED verifiedReadback=1 nativeLocals=1 geometryGate=PASS meshAcceptance=USER_F10_2":"V66.100-FINGERS PAIR_SAVE_FAILED inMemoryPairRetained=1");
  }
 }catch(...){g_persistence_status66100=-1;diagnostic_trace::Write("V66.100-CAL PERSISTENCE_FAILED previousBankRetained=1");}
}
static bool ReadCaptureHand66100(const CoherentState6689&s,int side,fc100::Hand&out) noexcept {
 out={};out.count=s.fingerCount[side];if(!s.fingerMap[side]||out.count<10||out.count>32)return false;
 int n=0;
 for(int digit=0;digit<5;++digit){const auto&chain=s.fingers[side][digit];
  for(int k=0;k<chain.count;++k,++n){if(n>=out.count)return false;const int id=chain.id[k],p=s.parent[id];if(p<0)return false;
   const auto local=RelativeFromAbsolute(s.original[p],s.original[id]);if(!ValidPose6689(local)||Len(Sub(FromT(local),FromT(s.restLocal[id])))>.003f)return false;
   auto& j=out.joints[n];j.id=id;j.parent=p;j.digit=digit;j.crc=g_capture_runtime66100.crc[side][n];j.q=PQ(QNormalize(QFromQuatT(local)));j.t=PV(FromT(local));
   if(k>0&&k+1<chain.count){auto a=pc::unit(PV(Sub(FromT(s.original[id]),FromT(s.original[chain.id[k-1]]))));auto b=pc::unit(PV(Sub(FromT(s.original[chain.id[k+1]]),FromT(s.original[id]))));if(!a||!b)return false;out.bend[digit]+=std::acos(std::clamp(pc::dot(*a,*b),-1.,1.));}
  }
 }
 return n==out.count&&fc100::valid(out);
}
static void FingerCaptureAndLoad66100(CoherentState6689&s,const TrackingSnapshot&ts,bool capture) noexcept {
 auto& state=g_capture_runtime66100;
 if(state.owner!=s.owner||state.character!=s.character||state.count!=s.count){
  // Finger identity is independent of curve acquisition/animation residency.
  // Same verified skeleton record layout as the existing CRC reader; once/rig.
  CaptureRuntime66100 next{};void* joints=nullptr;
  if(!Read(static_cast<unsigned char*>(s.owner)+8,joints)||!joints)return;
  auto h=FingerHash6694("NATIVE_HAND_PROFILE_STRUCTURE_V2",sizeof("NATIVE_HAND_PROFILE_STRUCTURE_V2")-1);
  for(int side=0;side<2;++side){h=FingerHash6694(&s.fingerCount[side],sizeof(int),h);
   for(int n=0;n<s.fingerCount[side];++n){const int id=s.fingerIds[side][n];
    if(!Read(static_cast<unsigned char*>(joints)+std::size_t(id)*0x30+8,next.crc[side][n])||!next.crc[side][n])return;
    h=FingerHash6694(&next.crc[side][n],sizeof(std::uint32_t),h);h=FingerHash6694(&id,sizeof(id),h);
    h=FingerHash6694(&s.parent[id],sizeof(int),h);
   }}
  FingerRigRequest101R2 descriptor{};descriptor.rig=h;
  for(int side=0;side<2;++side){auto& hand=descriptor.hand[side];hand.count=s.fingerCount[side];int n=0;
   for(int digit=0;digit<5;++digit)for(int k=0;k<s.fingers[side][digit].count;++k,++n){
    const int id=s.fingers[side][digit].id[k];if(n>=hand.count)return;
    hand.joints[n]={id,s.parent[id],digit,next.crc[side][n],PQ(QFromQuatT(s.restLocal[id])),PV(FromT(s.restLocal[id]))};
   }
   if(n!=hand.count)return;
  }
  if(!g_finger_rig_descriptor101R2.publish(descriptor))return;
  next.owner=s.owner;next.character=s.character;next.count=s.count;next.rig=h;state=next;g_finger_profile_status101R2=0;g_finger_rig_request66100.store(h,std::memory_order_release);
 }
 if(!capture){
  const auto generation=g_finger_saved_generation66100.load(std::memory_order_acquire);
  if(generation&&(generation!=state.applied||!s.open[0].ready||!s.open[1].ready||!s.closed[0].ready||!s.closed[1].ready)){FingerPacket66100 packet{};
   if(g_finger_saved66100.read(packet)&&packet.pair.rig==state.rig&&fc100::valid(packet.pair)){
    bool ok=true;for(int side=0;side<2;++side){const auto&h=packet.pair.open[side];ok=ok&&h.count==s.fingerCount[side];for(int n=0;ok&&n<h.count;++n){const int id=s.fingerIds[side][n];ok=h.joints[n].id==id&&h.joints[n].parent==s.parent[id]&&h.joints[n].crc==state.crc[side][n]&&pc::length(h.joints[n].t-PV(FromT(s.restLocal[id])))<.003&&pc::length(packet.pair.closed[side].joints[n].t-PV(FromT(s.restLocal[id])))<.003;}}
    if(ok){for(int side=0;side<2;++side){for(int n=0;n<s.fingerCount[side];++n){const int id=s.fingerIds[side][n];s.open[side].local[n]=s.closed[side].local[n]=s.restLocal[id];QToQuatT(s.open[side].local[n],QP(packet.pair.open[side].joints[n].q));QToQuatT(s.closed[side].local[n],QP(packet.pair.closed[side].joints[n].q));}s.open[side].ready=s.closed[side].ready=true;}
     state.applied=packet.generation;diagnostic_trace::Write("V66.100-FINGERS PROFILE_APPLIED parentLocal=1 nativeSegmentOffsets=FIXED wristAndArmMapping=UNCHANGED");}
   }
  }return;
 }
 const auto session=g_capture_session66100.load(std::memory_order_acquire),epoch=g_pose_epoch6689.load(std::memory_order_acquire);
 if(state.session!=session||state.epoch!=epoch){state.session=session;state.epoch=epoch;state.recorder.reset();state.offerQueued=false;}
 if(state.recorder.sample==ts.sequence)return; // Once per tracking sample, never per eye.
 fc100::Frame frame{};frame.sample=ts.sequence;frame.tick=ts.tick;frame.rig=state.rig;frame.allowed=ts.allowed&&!diagnostics::lab::NativeReference();frame.tracked=ts.head_valid&&ts.left_valid&&ts.right_valid;
 const auto qvalid=[](Q4 q){const double n=double(q.x)*q.x+double(q.y)*q.y+double(q.z)*q.z+double(q.w)*q.w;return std::isfinite(n)&&n>.9&&n<1.1;};
 frame.tracked=frame.tracked&&qvalid(ts.hq)&&qvalid(ts.lq)&&qvalid(ts.rq)&&pc::finite(PV(ts.head))&&pc::finite(PV(ts.left))&&pc::finite(PV(ts.right));
 frame.trigger[0]=ts.right_trigger;frame.trigger[1]=ts.left_trigger;frame.grip[0]=ts.right_grip;frame.grip[1]=ts.left_grip;
 void*manager=g_henry_item_attach_manager.load(std::memory_order_acquire);std::uint64_t r=0,l=0;
 frame.empty=NativeHandKey6697(manager,0,r)&&NativeHandKey6697(manager,1,l)&&PhysEmpty6698(r)&&PhysEmpty6698(l);
 if(!ReadCaptureHand66100(s,0,frame.hand[0])||!ReadCaptureHand66100(s,1,frame.hand[1]))frame.tracked=false;
 fc100::Pair pair{};const bool done=state.recorder.update(frame,pair);
 g_finger_capture_status66100.store(static_cast<int>(state.recorder.stage),std::memory_order_release);
 // A finite numeric candidate lets F10 leave 1.5 immediately, even if the
 // next skinning callback is absent. No repeated-rung trap on tracking loss.
 const bool candidate=frame.allowed&&frame.empty&&frame.tracked&&state.recorder.candidate(pair);
 if(candidate){FingerPacket66100 packet{pair,0,session,ts.tick};
  if(g_finger_candidate66100.publish(packet))g_finger_candidate_ready66100.store(true,std::memory_order_release);
 }else g_finger_candidate_ready66100.store(false,std::memory_order_release);
 if((done||(candidate&&state.recorder.stage==fc100::Stage::Ready))&&!state.offerQueued){FingerPacket66100 offer{pair,0,session,ts.tick};if(g_finger_offer66100.publish(offer)){state.offerQueued=true;g_finger_capture_status66100=5;}}

}
struct CalibrationReceiptsH46 {
 char lines[4][400]{};unsigned count=0;
 void add(const char* text) noexcept {if(count<4){std::snprintf(lines[count],sizeof(lines[count]),"%s",text);++count;}}
 void flush() const noexcept {for(unsigned i=0;i<count;++i)diagnostic_trace::Write(lines[i]);}
};
static void ApplySessionCalibrationH46(const TrackingSnapshot&ts,const QuatT28&head,void* character,std::uint64_t loadH46,const kcd2vr::tracking125::Origin* referenceH46,CalibrationReceiptsH46& receipts) noexcept {
 const bool sharedH46=referenceH46!=nullptr;
 const auto originH46=sharedH46?VP(referenceH46->position):ts.head;
 const auto heading=sharedH46?referenceH46->yaw:kcd2vr::tracking125::yaw_only(PQ(ts.hq),{0,g_cal_hq_z.load(),0,g_cal_hq_w.load()});
 const Q4 frame{0,0,float(-heading.y),float(heading.w)};
 // Fresh per-session alignment, never persisted room coordinates or wrist offsets.
 // A same-reference owner exit keeps the established anatomical eye offset.
 // Explicit recenter already invalidated its epoch in RequestTrackingReanchor.
 if(!sharedH46)g_pose_epoch6689.fetch_add(1,std::memory_order_acq_rel);
 g_committed_pose_valid6689=false;
 g_cal_head_x=originH46.x;g_cal_head_y=originH46.y;g_cal_head_z=originH46.z;
 // V66.107 HEAD HEIGHT: a recenter/session start used to take the CURRENT headset height
 // as "Henry's standing head". Recentering while sitting, crouching or leaning therefore
 // shifted Henry's head, torso and hands for the rest of the session. With a standing
 // reference (eye height above the floor, from the F8 T-pose), the vertical reference is
 // corrected to where the headset would be if you stood straight now.
 {const float standing=g_standing_eye_floor_m107.load(std::memory_order_acquire);
  const bool live=g_eye_floor_valid107.load(std::memory_order_acquire);const float now107=g_eye_floor_m107.load(std::memory_order_acquire);
  char hm[320]{}; // R59.16C10: longer receipt
  if(sharedH46){
   std::snprintf(hm,sizeof(hm),"H46 CALIBRATION_REFERENCE source=CAMERA_ORIGIN trackedBaseXr=(%.3f,%.3f,%.3f) standingCorrection=ALREADY_APPLIED savedReachAndFingers=RETAINED",double(originH46.x),double(originH46.y),double(originH46.z));
  }else if(g_cal_center_reanchor125.load()){
   // R59.16C10: numbers on the receipt. After an Oculus/menu recenter the CURRENT headset pose is Henry's neutral head
   // (eye recenter: your eyes now = Henry's eyes); the standing reference is kept for the next session start only.
   // C43: with VrCalibrationAutoRestore (or the menu RESTORE button) the recenter keeps the calibrated eye height too:
   // the body base is the headset corrected by (current eye - standing eye), the camera origin gets the same shift
   // (camera_hook recenter), so recentering slouched or after a knockdown does not move the view/body for the session.
   const float shift43=StandingEyeShift43();
   if(shift43!=0.f)g_cal_head_y=ts.head.y-shift43;
   std::snprintf(hm,sizeof(hm),"V66.125 BODY_REANCHOR cameraRecenter=1 currentHmdOrigin=%d savedStandingReferencePreserved=1 trackedBaseXr=(%.3f,%.3f,%.3f) currentEye=%.3fm standingEye=%.3fm floorTracking=%d C43verticalCorrection=%+.3fm",
    shift43==0.f?1:0,double(ts.head.x),double(ts.head.y),double(ts.head.z),double(now107),double(standing),live?1:0,double(-shift43));
  }else if(kcd2vr::start_r111::henryEyes.load(std::memory_order_acquire)){
   // R111 (VrStartAtHenryEyesR111): the headset where it is now is Henry's eye point - seated or standing, the start is
   // Henry's eyes; only the head motion from here moves him. no standing-eye correction.
   std::snprintf(hm,sizeof(hm),"R111 HEAD REANCHOR start=HENRY_EYES currentEye=%.3fm standingEye=%.3fm floorTracking=%d verticalCorrection=+0.000m",double(now107),double(standing),live?1:0);
  }else if(standing>.5f&&live){const float shift=now107-standing;if(std::fabs(shift)<1.2f)g_cal_head_y=ts.head.y-shift;
   std::snprintf(hm,sizeof(hm),"V66.107-HEAD REANCHOR standingEye=%.3fm currentEye=%.3fm verticalCorrection=%+.3fm (reference = standing straight)",standing,now107,-(now107-standing));}
  else std::snprintf(hm,sizeof(hm),"V66.107-HEAD REANCHOR standingReference=%s floorTracking=%d -> current headset height used (do F8 once while standing)",standing>.5f?"OK":"MISSING",live?1:0);
  receipts.add(hm);}
 g_cal_frame_qx=frame.x;g_cal_frame_qy=frame.y;g_cal_frame_qz=frame.z;g_cal_frame_qw=frame.w;
 // Yaw-only neutral: starting while looking down must not redefine down as neutral.
 g_cal_hq_x=0;g_cal_hq_y=0;g_cal_hq_z=-frame.z;g_cal_hq_w=frame.w;
 {// C43: Henry's head anchor comes from the calibration, not from whatever stance/knockdown pose he is in right now.
  const float live43[3]{head.t[0],head.t[1],head.t[2]},cal43[3]{g_cal_anchor_x43.load(),g_cal_anchor_y43.load(),g_cal_anchor_z43.load()};
  const bool keep43=kcd2vr::calibration_restore43::keepCalibratedAnchor(g_cal_anchor_saved43.load(std::memory_order_acquire),cal43,live43);
  const float drift43=keep43?kcd2vr::calibration_restore43::anchorDriftM(cal43,live43):0.f;
  if(keep43){g_cal_native_head_x=cal43[0];g_cal_native_head_y=cal43[1];g_cal_native_head_z=cal43[2];}
  else{g_cal_native_head_x=head.t[0];g_cal_native_head_y=head.t[1];g_cal_native_head_z=head.t[2];}
  if(keep43&&drift43>=kcd2vr::calibration_restore43::kDriftReportM){g_cal_drift_count43.fetch_add(1,std::memory_order_relaxed);char dm[240]{};
   std::snprintf(dm,sizeof(dm),"C43 CALIBRATION drift detected owner=REANCHOR liveHeadZ=%.3f calibratedHeadZ=%.3f driftM=%.3f -> calibrated anchor kept (stance/knockdown pose not latched)",double(live43[2]),double(cal43[2]),double(drift43));receipts.add(dm);}
 }
 g_cal_pos_l_x=g_cal_pos_l_y=g_cal_pos_l_z=0;g_cal_pos_r_x=g_cal_pos_r_y=g_cal_pos_r_z=0;
 g_cal_rot_l_x=g_cal_rot_l_y=g_cal_rot_l_z=0;g_cal_rot_l_w=1;g_cal_rot_r_x=g_cal_rot_r_y=g_cal_rot_r_z=0;g_cal_rot_r_w=1;
 if(character){g_cal_character_h46=character;g_cal_load_h46=loadH46;}
 g_cal_native_anchor_pending=false;g_vrik_calibration_requested=false;g_cal_state.store(2,std::memory_order_release);g_cal_reanchor66100=false;g_cal_center_reanchor125=false;
 receipts.add("V66.100-CAL SESSION_REANCHORED savedReachReused=1 TposeRequired=0 wristOffsets=NONE liveHmdFollow=NONE");
 {g_cal_restore_count43.fetch_add(1,std::memory_order_relaxed);char rm[240]{};
  std::snprintf(rm,sizeof(rm),"C43 CALIBRATION restored by=REANCHOR eyeHeight=%.3f headOffset=(%.3f,%.3f,%.3f) trackedBaseY=%.3f autoRestore=%d",
   double(g_standing_eye_floor_m107.load()),double(g_cal_native_head_x.load()),double(g_cal_native_head_y.load()),double(g_cal_native_head_z.load()),double(g_cal_head_y.load()),g_cal_auto_restore43.load()?1:0);receipts.add(rm);}
 if(referenceH46){
  g_cal_room_origin_d76.store(kcd2vr::roomscale_d76::originRevision.load(std::memory_order_acquire),std::memory_order_release);
  kcd2vr::roomscale_d76::recenterPending.store(false,std::memory_order_release);
 }
 g_cal_restore_force43.store(false,std::memory_order_release);
}
static void RestoreSessionCalibration66100(const TrackingSnapshot&ts,const QuatT28&head,void* character=nullptr) noexcept {
 const auto loadH46=kcd2vr::inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire);
 if(character&&(g_cal_character_h46!=character||g_cal_load_h46!=loadH46)&&g_saved_arm_ready66100.load(std::memory_order_acquire))
     g_cal_reanchor66100.store(true,std::memory_order_release);
 if(!g_cal_reanchor66100.load(std::memory_order_acquire)||!g_saved_arm_ready66100.load(std::memory_order_acquire)||g_arm_capture_active66100.load()||!ts.allowed||!ts.head_valid||!ValidPose6689(head))return;
 const auto now=static_cast<std::uint64_t>(GetTickCount64());if(!ts.tick||now<ts.tick||now-ts.tick>100)return;
 const double qnorm=double(ts.hq.x)*ts.hq.x+double(ts.hq.y)*ts.hq.y+double(ts.hq.z)*ts.hq.z+double(ts.hq.w)*ts.hq.w;
 if(!std::isfinite(qnorm)||qnorm<.90||qnorm>1.10)return;
 if(!pc::finite(PV(ts.head)))return;
 CalibrationReceiptsH46 receipts{};
 const auto probeH46=g_tracking_origin_probe_h46.load(std::memory_order_acquire);
 if(!probeH46){ApplySessionCalibrationH46(ts,head,character,loadH46,nullptr,receipts);receipts.flush();return;}
 struct Request {const TrackingSnapshot&ts;const QuatT28&head;void* character;std::uint64_t load;CalibrationReceiptsH46& receipts;};
 Request request{ts,head,character,loadH46,receipts};
 // The view reference stays locked until its numeric body calibration is applied.
 // Native gameplay, file IO and camera recenter are not called by this visitor.
 (void)probeH46(+[](const kcd2vr::tracking125::Origin& origin,void* context) noexcept {
  const auto& q=origin.yaw;const double norm=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
  if(!pc::finite(origin.position)||!std::isfinite(norm)||std::fabs(norm-1.)>.01||std::fabs(q.x)>.0001||std::fabs(q.z)>.0001)return false;
  auto& r=*static_cast<Request*>(context);
  ApplySessionCalibrationH46(r.ts,r.head,r.character,r.load,&origin,r.receipts);return true;
 },&request);
 receipts.flush();
}

// H45: whole parent-local hands from the recorded native unarmed punch pose.
// Capture SHA5e2255728825dfcf..., tick935961531; never an idle/washing maximum.
// Only the optional closed endpoint changes. Saved open/closed banks, joint
// translations, wrist ownership and combat inputs are not mutated.
static bool LoadCapturedPunchH45(CoherentState6689& s) noexcept {
    // W1: the table lives in vrik/finger_rig_w1.h and is matched by finger name crc + chain position,
    // so the captured fist also applies on Henry rigs with other joint numbers (not only 408/443).
    namespace fr=kcd2vr::finger_rig_w1;const auto& reference=fr::kPunch;
    for(int side=0;side<2;++side)s.nativeCurveClosedH7[side].ready=false;
    const auto& capture=g_capture_runtime66100;
    if(capture.owner!=s.owner||capture.character!=s.character||capture.count!=s.count)return false;
    for(int side=0;side<2;++side){
        if(!s.fingerMap[side]||s.fingerCount[side]!=22)return false;
        std::array<fr::Chain,5> chains{};
        for(int f=0;f<5;++f){chains[f].count=s.fingers[side][f].count;for(int k=0;k<chains[f].count&&k<6;++k)chains[f].id[k]=s.fingers[side][f].id[k];}
        const int hand=side?s.rig.hand_l:s.rig.hand_r;
        if(!fr::PunchMatches(chains,hand,side!=0,[&](int id){return id>=0&&id<int(s.count)?s.parent[id]:-2;},
            [&](int n){return capture.crc[side][n];}))return false;
    }
    for(int side=0;side<2;++side){auto& endpoint=s.nativeCurveClosedH7[side];
        for(int n=0;n<22;++n){const int id=s.fingerIds[side][n];endpoint.local[n]=s.restLocal[id];
            std::copy_n(reference[side][n].q,4,endpoint.local[n].q);
        }
        endpoint.sequence=935961531;endpoint.issue="CAPTURED_NATIVE_PUNCH_H45";endpoint.ready=true;
    }
    return true;
}
