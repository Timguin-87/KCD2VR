// Reuses the actual lexical input adapter fixture, now fed by the production
// Dog observer/cache with simulated native memory leaves. No object=true Dog.
#define main retained_misc_input_main
#include "misc_input_d59_test.cpp"
#undef main
using namespace kcd2vr;
static palm59c9::Pair pair(std::uint64_t now) {
 palm59c9::Pair p{};
 for(int side=0;side<2;++side){auto& r=p.ray[side];p.eligible[side]=true;
  r.hand=true;r.owner=side;r.tick=now;r.sourceEpoch=20;r.sourcePlayer=100;r.ox=.1f*side;}
 return p;
}
int main(){
 // All retained adapter/button/reservation/washing checks execute too.
 retained_misc_input_main();
 auto fixture=reset();assert(!fixture.candidate59c7f.object);
 assert(interaction57c1::Classify("Dog")==interaction57c1::Kind::Actor);
 petting_d59::Recognizer recognizer;
 for(int i=0;i<3;++i){testD59::now=1000+60*i;
  auto& body=frik_pose_probe::source;body.tick=testD59::now;body.palm[0][0]=.1f+.041f*i;
  fixture.xr.stamp.tick=fixture.candidate59c7f.tick=fixture.candidate59c7f.sourceTick=testD59::now;
  fixture.tick();assert(canPet());
  const auto& frame=petting_d59::published;
  assert(frame.hand[0].candidateGeneration&&frame.hand[0].candidateContext);
  assert(recognizer.update(frame)==(i==2?0:-1));
 }
 // The ACTUAL failed-reader contract flows through the ACTUAL input adapter.
 testD59::bodyOkay=false;testD59::now+=30;fixture.xr.stamp.tick=testD59::now;fixture.tick();
 assert(!petting_d59::published.playerIdentity);
 assert(recognizer.update(petting_d59::published)<0&&recognizer.latched);
 testD59::bodyOkay=true;
 for(int i=0;i<3;++i){testD59::now+=60;frik_pose_probe::source.tick=testD59::now;
  frik_pose_probe::source.palm[0][0]+=.041f;
  fixture.xr.stamp.tick=fixture.candidate59c7f.tick=fixture.candidate59c7f.sourceTick=testD59::now;
  fixture.tick();assert(recognizer.update(petting_d59::published)<0&&recognizer.latched);}

 using namespace interaction_alignment;
 test_dog_d59r1::reset();auto p=pair(2000);test_dog_d59r1::palms(p);
 const float point[]{.05f,.05f,0};DogHandCandidateD59R1 c{};
 // OFF does no new native reads and returns no candidate.
 test_dog_d59r1::observe(2000,point);assert(!testNativeReadsD59R1);
 assert(!ReadDogHandCandidateD59R1(0,2000,c));
 SetDogGestureObservationD59R1(true);test_dog_d59r1::observe(2000,point);
 assert(ReadDogHandCandidateD59R1(0,2000,c)&&c.entity==123&&c.owner==0);
 const auto first=c;
 assert(ReadDogHandCandidateD59R1(1,2000,c)&&c.owner==1); // independent LEFT
 // Unrelated actor enumeration cannot steal the Dog observation.
 const auto reads=testNativeReadsD59R1;test_dog_d59r1::observe(2000,point,456,"NPC");
 assert(testNativeReadsD59R1==reads&&ReadDogHandCandidateD59R1(0,2000,c)&&c.entity==123);
 // Current distance, not the stored 7 cm range, is authoritative.
 p.ray[0].ox=.8f;p.ray[0].tick=2010;test_dog_d59r1::palms(p);
 assert(!ReadDogHandCandidateD59R1(0,2010,c));assert(ReadDogHandCandidateD59R1(1,2010,c));
 p=pair(2010);p.ray[0].sourceEpoch=21;test_dog_d59r1::palms(p);
 assert(!ReadDogHandCandidateD59R1(0,2010,c));
 p=pair(2010);p.ray[0].sourcePlayer=101;test_dog_d59r1::palms(p);
 assert(!ReadDogHandCandidateD59R1(0,2010,c));
 p=pair(2010);p.locked=1;test_dog_d59r1::palms(p);
 assert(!ReadDogHandCandidateD59R1(0,2010,c)&&ReadDogHandCandidateD59R1(1,2010,c));
 p.lockedEntity=999;test_dog_d59r1::palms(p);assert(!ReadDogHandCandidateD59R1(1,2010,c));
 p=pair(2010);test_dog_d59r1::palms(p);
 // An actual native-player change during the observer cannot publish a witness.
 testReplacePlayerD59R1=true;test_dog_d59r1::observe(2010,point);
 assert(!ReadDogHandCandidateD59R1(0,2010,c));testReplacePlayerD59R1=false;testPlayerEntityD59R1=100;
 test_dog_d59r1::observe(2010,point);assert(ReadDogHandCandidateD59R1(0,2010,c));
 testReadsD59R1=false;test_dog_d59r1::observe(2010,point);
 assert(!ReadDogHandCandidateD59R1(0,2010,c));testReadsD59R1=true;
 test_dog_d59r1::observe(2010,point);assert(ReadDogHandCandidateD59R1(0,2010,c));
 // Context and trial epochs revoke old observations even after re-enable.
 ++g_door_acquisition_generation65;assert(!ReadDogHandCandidateD59R1(0,2010,c));
 test_dog_d59r1::observe(2010,point);assert(ReadDogHandCandidateD59R1(0,2010,c));
 assert(c.source.context!=first.source.context);
 SetDogGestureObservationD59R1(false);SetDogGestureObservationD59R1(true);
 assert(!ReadDogHandCandidateD59R1(0,2010,c));
 test_dog_d59r1::observe(2010,point);assert(ReadDogHandCandidateD59R1(0,2010,c));
 assert(c.source.generation!=first.source.generation);
 g_visual_fallback_enabled=true;assert(!ReadDogHandCandidateD59R1(0,2010,c));g_visual_fallback_enabled=false;
 g_enabled=false;assert(!ReadDogHandCandidateD59R1(0,2010,c));g_enabled=true;
 g_split_ready57c1=false;assert(!ReadDogHandCandidateD59R1(0,2010,c));g_split_ready57c1=true;
 // New native observations outside reach, stale palms and stale metadata revoke.
 const float far[]{1,1,0};test_dog_d59r1::observe(2010,far);assert(!ReadDogHandCandidateD59R1(0,2010,c));
 test_dog_d59r1::observe(2010,point,123,"Dog",1929);assert(!ReadDogHandCandidateD59R1(0,2010,c));
 test_dog_d59r1::observe(2091,point);assert(!ReadDogHandCandidateD59R1(0,2091,c));
 std::puts("D59R1 pet producer/input/recognizer and context/tracking negatives PASS");
}
