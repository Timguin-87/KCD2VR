#pragma once
#include "capture168.h"
#include <cstdio>
namespace kcd2vr::diagnostics::coverage_d42 {
// Compile-time coverage inventory, not readiness or a claim of native success.
// Only emitted on an explicit capture start; no scanning or new native reads.
template<class Sink> inline void begin(capture168::Domain d,std::uint64_t session,Sink sink) noexcept {
 const char* scope=nullptr;
 switch(d){
 case capture168::Domain::Interaction:scope="use=H29R4_F5_CYCLE pickup=NATIVE_GRASP_EXACT_TARGET doors=C54R3_F5_DOOR props=EXISTING_NATIVE_GRAB stations=D42_F5_STAGE soupWash=D42_F5_PASSENGER petting=NATIVE_CLIP_ONLY carry=C7G_F5_CARRY_OBSERVER map=M31_H24_H25_T5 throwing=T1_T8_T5 heldPhysics=REQUEST_ACK_REQUIRED missing=GESTURE_PROP_EFFECT_JOIN,LIVING_GRAB_DISARM,CARRY_HAND_TO_BACK,RELEASED_IRON_BODY,SEPARATE_TOOL_BODY";break;
 case capture168::Domain::Combat:scope="nativeHit=C54R3_HIT_CHAIN punch=C54R3_PUNCH_PATH technique=ACCEPTED_CONTACT_MATCHER_DIAGNOSTIC_ONLY freeTarget=D42_F3 currentTarget=EXISTING_NATIVE_SELECTION impale=FEATURE_AND_LIFECYCLE_NOT_CONNECTED npcReaction=NATIVE_OUTCOME_ONLY missing=TECHNIQUE_DISPATCH,EXACT_BODY_REGION,NPC_CONTACT_ADAPTATION,LIVING_DISARM,T4_SHARP_FEATURE_AND_IMPALE_TRANSACTION";break;
 case capture168::Domain::Physics:scope="corpse=SELECTED_GAMEPLAY_NOT_RECORDER candidate=EXACT_NATIVE_DEAD_IDENTITY hand=COMMITTED_RAW_OWNER motion=REQUEST_AND_OBSERVED_BODY_DELTA livingActuation=PAUSED sharedKeys=F3_F5_F7 additionalConstraint=UNPROVEN";break;
 case capture168::Domain::Bug:scope="frames=F2_FRAME ownership=F2_CAUSAL physicalPose=D42_F2_T4_STAGE timing=EXISTING_FRAME_AND_HOOK_COST missing=GPU_TIMING_CAUSALITY_AND_PRESENTED_PIXEL_PROOF noNewRendererProbe=1";break;
 case capture168::Domain::Vrik:scope="solve=F7_SUMMARY pose=VRIK_F7 physicalPose=D42_F7_T4_STAGE missing=FINAL_RENDERED_VISIBILITY_AND_MATERIAL_PROOF nativeSelectionDoesNotProveDisplayedPose=1";break;
 default:return;
 }
 char line[1100]{};std::snprintf(line,sizeof(line),"D42_COVERAGE domain=%s session=%llu inventory=SOURCE_CAPABILITY_NOT_LIVE_READINESS %s completion=REQUIRES_MATCHED_NATIVE_ACK absence=NOT_PROOF_OF_FAILURE inspect=CAPTURE_HEALTH_AND_END_COUNTERS",
  capture168::name(d),(unsigned long long)session,scope);sink(line);
}
}
