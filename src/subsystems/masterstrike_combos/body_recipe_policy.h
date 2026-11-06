#pragma once
// Physical recipe recognition only. A match is not native technique admission.
// Called once per completed contact, never once per render/physics frame.
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace kcd2vr::bodycombo59c7g {
enum class Region : unsigned { Unknown, Any, Head, Face, Neck, Torso, Chest, Abdomen, Arm, Leg, Knee, Ribs };
enum class Feature : unsigned { Unknown, Any, Blade, Tip, Pommel, Shield, Fist, ShieldEdge, Handle, WeaponHead };
enum class Motion : unsigned { Any, Cut, Thrust, Strike, BluntStrike, CutAfterDeflection, Bash, Trip };
enum class Outcome : unsigned { Unknown, Rejected, Hit, Block, PerfectBlock };
inline const char* name(Region r) noexcept {
 switch(r){case Region::Any:return "ANY";case Region::Head:return "HEAD";case Region::Face:return "FACE";
 case Region::Neck:return "NECK";case Region::Torso:return "TORSO";case Region::Chest:return "CHEST";
 case Region::Abdomen:return "ABDOMEN";case Region::Arm:return "ARM";case Region::Leg:return "LEG";
 case Region::Knee:return "KNEE";case Region::Ribs:return "RIBS";default:return "UNKNOWN";}
}
// Material ids are NOT bone/part ids. This table is human-race body_material2subpart.xml.
// The caller must separately establish that its value belongs to that domain.
inline Region humanMaterial(int m) noexcept {
 if(m==3)return Region::Face;
 if(m>=1&&m<=5)return Region::Head;
 if(m==38)return Region::Neck;
 if(m==6||m==7)return Region::Chest;
 if(m==8||m==9)return Region::Abdomen;
 if(m==10||m==11)return Region::Torso;
 if(m>=12&&m<=25)return Region::Arm;
 if(m>=26&&m<=37)return Region::Leg;
 return Region::Unknown;
}
// NR1 (Claude 24 Oct 2026): the NPC hit reaction follows the blade. the native picks the victim
// reaction from the attacker's zone prop (D4921C reads attacker state +0x150 and turns it into the
// victim's dZ reaction tag via combat_zone + the relative direction), not from the body part. our
// published zone is the swing/prediction one, so a knee stab played a torso/side reaction.
// combat_zone.xml gives each native zone the human body spot it lands on (approx_orig_mat_id /
// approx_bone_name): 0 head/HEAD, 1 upper_left/UPPERARM_L (mat 24), 2 upper_right/UPPERARM_R (mat 17),
// 5 lower (angle 225-315, the low attack). zones 3/4 have radius 0 and combat_action_hit.xml has NO
// reaction rows for them (block_zone_id only 0,1,2,5: 49/59/58/107 rows) - NR1R1 sent legs there, so a
// leg hit found no zone row. R2: torso and legs -> 5 (the native low/body reactions, dZ5).
// 9397CC/D4921C: the zone only weights the reaction rows (B25D44 zone x row table), damage untouched.
inline std::atomic<bool> npcReactsToBlade{true}; // VrNpcReactsToBlade, A/B switch
inline int reactionZoneForMaterial(int m,bool thrust) noexcept {
 (void)thrust;
 if((m>=1&&m<=5)||m==38)return 0;
 if(m>=19&&m<=25)return 1;
 if(m>=12&&m<=18)return 2;
 if((m>=6&&m<=11)||(m>=26&&m<=37))return 5;
 return -1;
}
// I10 (NR2 stamina part only): 18 of 31 native reactions in Miki's R14 fights came from hits that only cost stamina
// (armour); NR1 skipped them (health > 0 gate) so their reaction ignored the blade. D4921C's hit strength is
// health + stamina damage (hitInfo +0x30 + +0x2C), so with VrNpcReactsStaminaHits (default on) that sum is the 'it landed'
// test. a blocked hit never borrows. no sector / predicted side here: torso keeps the NR1 spot mapping.
inline std::atomic<bool> staminaHits{true}; // VrNpcReactsStaminaHits
inline bool landedForReactionNR2(float health,float stamina,bool blockedA,bool blockedB,bool allowStamina) noexcept {
 if(blockedA||blockedB||!std::isfinite(health)||!std::isfinite(stamina))return false;
 return health>0.f||(allowStamina&&stamina>0.f);
}
inline std::atomic<std::uint64_t> nr2Stamina{0}; // landed stamina-only hits given the blade spot
// NR1 R3: the borrowed attacker value must leave the victim guard comparison (D49488, guard == value) as it was.
inline bool guardComparisonKept(int guard,int before,int value) noexcept {return guard!=-99&&((guard==value)==(guard==before));}
inline bool regionMatches(Region wanted,Region got) noexcept {
 if(wanted==Region::Any)return true;
 if(got==Region::Unknown)return false;
 return wanted==got||(wanted==Region::Head&&(got==Region::Face||got==Region::Neck))||
  (wanted==Region::Torso&&(got==Region::Chest||got==Region::Abdomen))||
  (wanted==Region::Leg&&got==Region::Knee);
}
struct Step {
 Region region=Region::Any;Feature feature=Feature::Any;Motion motion=Motion::Any;
 int origin=-1;bool twoHands=false,allowBlock=false;
};
struct Recipe {
 const char* name="";const char* technique="";const char* equipment="";
 std::array<Step,6> steps{};unsigned count=0;
 // VR recognition timeout, not a replacement for the native combo window.
 std::uint64_t gapMs=1500;
};
struct Contact {
 std::uint64_t attempt=0,tick=0,epoch=0,request=0,key=0,generation=0;
 std::uint32_t target=0;std::uintptr_t targetIdentity=0;
 Region region=Region::Unknown;Feature feature=Feature::Unknown;Motion motion=Motion::Any;
 Outcome outcome=Outcome::Unknown;int origin=-1,weaponClass=-1;bool shield=false,twoHands=false,bodyContact=false,continuation=false;
};
inline bool equipmentMatches(const char* group,const Contact& c) noexcept {
 if(!std::strcmp(group,"Longsword"))return c.weaponClass==4&&!c.shield;
 if(!std::strcmp(group,"Halberd"))return c.weaponClass==7&&!c.shield;
 const bool sword=c.weaponClass==1||c.weaponClass==2||c.weaponClass==16;
 const bool blunt=c.weaponClass==3||c.weaponClass==5;
 if(!std::strcmp(group,"Shortsword"))return sword&&!c.shield;
 if(!std::strcmp(group,"ShortswordShield"))return sword&&c.shield;
 if(!std::strcmp(group,"Blunt"))return blunt&&!c.shield;
 if(!std::strcmp(group,"BluntShield"))return blunt&&c.shield;
 return false;
}
inline bool qualifies(const Step& s,const Contact& c) noexcept {
 const bool hit=c.outcome==Outcome::Hit&&c.bodyContact;
 const bool block=s.allowBlock&&c.outcome==Outcome::Block;
 // A successful enemy perfect block interrupts our sequence. A blocked blade
 // cannot establish a body location; only an ANY-region prefix may accept it.
 if(!hit&&!(block&&s.region==Region::Any))return false;
 return regionMatches(s.region,c.region)&&(s.feature==Feature::Any||s.feature==c.feature)&&
  (s.motion==Motion::Any||s.motion==c.motion)&&(s.origin<0||s.origin==c.origin)&&(!s.twoHands||c.twoHands);
}
struct Progress {unsigned next=0;std::uint64_t last=0;};
struct Result {std::uint64_t completed=0;unsigned advanced=0;bool duplicate=false,reset=false;};
template<std::size_t N> struct Matcher {
 static_assert(N<=64);std::array<Progress,N> progress{};Contact owner{};
 // Gestures are monotonically increasing pose sample IDs within an ownership
 // epoch. A target switch must not let the same swing advance another recipe.
 std::uint64_t lastAttempt=0,lastEpoch=0,lastRequest=0;bool have=false;
 void clear() noexcept {progress={};owner={};have=false;}
 Result observe(const std::array<Recipe,N>& recipes,const Contact& c) noexcept {
  Result result{};
  if(lastEpoch!=c.epoch||lastRequest!=c.request){clear();lastAttempt=0;lastEpoch=c.epoch;lastRequest=c.request;}
  if(c.attempt&&c.attempt<=lastAttempt){result.duplicate=true;return result;}
  if(c.attempt)lastAttempt=c.attempt;
  if(!c.attempt||!c.tick||!c.target||!c.targetIdentity||!c.key||!c.generation||!c.continuation){clear();result.reset=true;return result;}
  const bool same=have&&owner.epoch==c.epoch&&owner.request==c.request&&owner.key==c.key&&
   owner.generation==c.generation&&owner.target==c.target&&owner.targetIdentity==c.targetIdentity&&
   owner.weaponClass==c.weaponClass&&owner.shield==c.shield;
  if(!same){clear();result.reset=true;}
  else if(c.tick<owner.tick){clear();result.reset=true;return result;}
  owner=c;have=true;
  if(c.outcome==Outcome::PerfectBlock||c.outcome==Outcome::Rejected||c.outcome==Outcome::Unknown){progress={};result.reset=true;return result;}
  for(std::size_t i=0;i<N;++i){const auto& r=recipes[i];auto& p=progress[i];
   if(!r.count||r.count>r.steps.size()||!equipmentMatches(r.equipment,c)){p={};continue;}
   if(p.next>=r.count||!p.last||c.tick-p.last>r.gapMs)p={};
   if(qualifies(r.steps[p.next],c)){
    p.last=c.tick;++p.next;++result.advanced;
    if(p.next==r.count){result.completed|=std::uint64_t(1)<<i;p={};}
   }else{
    p={};if(qualifies(r.steps[0],c)){p.next=1;p.last=c.tick;++result.advanced;
     if(r.count==1){result.completed|=std::uint64_t(1)<<i;p={};}}
   }
  }
  return result;
 }
};
} // namespace
