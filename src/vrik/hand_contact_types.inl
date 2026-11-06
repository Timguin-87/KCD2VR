#define KCD2VR_CONTACT_PROXY_D63 1
// Included after V3/Q4 and the portable finger_surface header. Private POD only.
// DS-F105: the plane now carries the qualified synchronous npc_contact172
// witness (entity/part/body) from the actual hit producer - the REAL contact
// target identity for the touch reaction (Codex's touch-identity requirement).
#include "finger_contact_h31.h"
struct PalmPlane6697 {V3 point{},normal{};int feature=-1;std::uint64_t witness169r1=0;npc172::Witness native172{};bool spatialProbeD81=false;float spatialPushD81=0;};
struct FingerContactGeometry103 {
    kcd2vr::pose_core::FingerCurve103 curve[5]{};float input[5]{};bool enabled[5]{};Q4 orientation{};
    kcd2vr::finger_contact_h31::Source springSourceH31{};
};
struct HandContact6695 {
    V3 worldAllowed{},offset[7]{};PalmPlane6697 planes[7]{};int planeCount=0,featureCount=5;
    V3 rawFeatureWorld55[7]{};bool rawFeatureReady55=false; // previous complete intent, independent of achieved/fitted contact
    std::uint64_t epoch=0,lastCompleteTickH18=0;void* character=nullptr;bool have=false,blocked=false;
    // Only repeated callbacks of this exact completed XR publication may reuse its endpoint.
    std::uint64_t completeSampleD80=0,completeTrackingD80=0;
    // D53: last accepted displayed wrist, separate from measured contact.
    // Reacquisition starts here after a budget/reset gap instead of sweeping
    // from the unrelated authored native hand. It grants no surface/clearance.
    V3 presentationWorldD53{};std::uint64_t presentationTickD53=0,presentationTrackingD53=0;
    std::uint64_t glideTickR8b=0; // R8b: last contact-pass tick the expiry glide moved on (once per pass, time based)
    double curl[5]{};bool fingerContact[5]{};
    V3 clearCorrection49{};bool completeClear49=false;
    // D45: measured contact correction before later IK, not an anatomical or
    // native-pose error. No prior plane is retained as release authority.
    V3 releaseRawWorldD45{},releaseCorrectionWorldD45{};
    std::uint64_t releaseTickD45=0,releaseNsD79=0;bool releaseActiveD45=false;
    bool releaseWorldD62=false; // positively world-only source of the measured release offset
    bool releaseEntitySurfaceD63=false; // current BASE or exact same-query rigid receipt; return candidates still query all features
    bool bodyLocalSeedD71=false;unsigned bodyPartRequeryMaskD71=0; // copied current-pass diagnostics, no native reads
    bool releaseBodySurfaceD71=false; // gated exact current front-face body result; next candidate still queries every feature
    bool balancedD79=false; // mode2 feature layout is not the precise-finger layout
    bool stationPermissiveD75=false;std::uint64_t stationSampleD75=0,stationTrackingD75=0;
    float stationWorldScaleD75=0; // already-read exact model->world scale, never inferred from calibration
    bool stationPresentationLimitedD81=false; // bounded visible wrist is not the measured sweep origin
    bool fresh169r1=false; // complete current query only, not cached/budget-held planes
    kcd2vr::finger_contact_h31::Source springMeasuredH31{};
    kcd2vr::finger_contact_h31::State springH31{};
    double hardCurlH31[5]{}; // original mandatory fit, separate from optional extra clearance
};

// Preserve every default member value while avoiding GCC 13's nested-array
// aggregate assignment/temporary ICE (also reproduced on the D52 type).
static void ResetHandContact6695(HandContact6695& c) noexcept {
    const HandContact6695 empty{};c=empty;
}

// D56: transient occupancy uncertainty retires every measured plane/fit, while
// preserving the bounded accepted-wrist restart source. A known item/new body
// still uses the complete reset. Presentation is never a clearance witness.
static void ResetHandGeometryD56(HandContact6695& c) noexcept {
    const auto world=c.presentationWorldD53;const auto tick=c.presentationTickD53,tracking=c.presentationTrackingD53,epoch=c.epoch;
    const auto character=c.character;ResetHandContact6695(c);
    c.presentationWorldD53=world;c.presentationTickD53=tick;c.presentationTrackingD53=tracking;
    c.epoch=epoch;c.character=character;
}

// Same-pass query work only. These copies never authorize a later frame or
// become a new contact owner. The caller completes required sweeps for both
// hands before spending any remaining shared credit on current-surface recovery.
struct HandContactPending6695 {
    HandContact6695 before{},queried{};Q4 worldRotation{};V3 worldTranslation{};float worldScale=1;
    V3 desired{},rawDesiredD45{},requested{},offsets[7]{},rawFeatures[7]{};float skin=0,fallbackFraction=1;
    const FingerContactGeometry103* fingers=nullptr;bool ready=false,unqualifiedHitD62=false;
};
