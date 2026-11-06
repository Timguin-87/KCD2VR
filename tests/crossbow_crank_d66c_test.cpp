// D66C heavy (crank) crossbow, class 15, against the ACTUAL XR34 Client and the
// actual native adapter (crossbow_native_xr34.inl) with the same explicit mocked
// native leaves as crossbow_native_xr34_test.cpp. No WHGame.dll, no headset claim.
// Builds on linux with:
//   clang++ -std=c++20 -w -fms-extensions -Itests/syntax_shims -Itests/openxr_sdk -I/tmp/w/shim3 -Isrc -Itests
//     -I/tmp/w/shim2 -include tests/host_test_runtime.h -include /tmp/w/li_shim.h
//     '-D__try=try' '-D__finally=catch(...){throw;}' tests/crossbow_crank_d66c_test.cpp
// (no -Dnear=: native_grasp13.h has a field called nearSince).
#define KCD2VR_XR34_NATIVE_NO_MAIN 1
#include "crossbow_native_xr34_test.cpp"

namespace {
constexpr double kStringY=.312299996614,kStringZ=.059749998152; // "Crossbow_heavy_string" CHR800 default (RangedDefaultsXR34)
struct Heavy {
    Fixture a;
    Heavy(){
        a.source.classId=15;
        a.source.chestKnown=true;a.source.handInChest[0]=false; // producers evaluate the tracked chest box per sample
        a.spatial.stringRest={0,kStringY,kStringZ};a.spatial.rail={0,.138,.0596};
        a.spatial.grip=a.spatial.stringRest;             // crank engages the string
        a.spatial.axis={0,1,0};                           // string -> stirrup (forward), unit
        a.spatial.travel=x::kCrankTravelDefaultD66C;      // .08
        a.spatial.crankQualified=true;a.spatial.crankProgress=0;
        a.spatial.toolBound=true;a.spatial.toolVisible=false;a.spatial.toolPoseReady=true;
    }
    // working hand = 0 (primary 1). chest(true) = the tracked working hand is in the chest box this sample.
    void chest(bool in){a.source.handInChest[0]=in;}
    void frame(double grip,x::Vec hand){a.input(grip,hand);nativeFrame(a);}
    float normalized()const{return a.get<float>(a.entry()+0x44);}
};
x::Vec at(double forward,double lateral=0,double up=0){return {lateral,kStringY+forward,kStringZ+up};}
// The real heavy player cocking clip (Tables.pak Libs/Tables/animation/anim_fragment_events.xml,
// Load, tags l_crossbow+player+heavy+lever): SpawnLever .1961905, LoadReachedCocking .7314583,
// "DespawLever" (sic) .8178125 -> crc32("despawlever") = 0x441B89A7.
void heavyClipEvents(Fixture& a){const auto ep=a.p(a.eventStorage)+16;a.put(ep-4,3u);
    a.event(0,.1961905f,0xAFE0B3FF);a.event(1,.7314583f,0x8ABB129A);a.event(2,.8178125f,x::kDespawnLeverTypoCrcD66C);}
// setup() with the real heavy event layout instead of the fixture's .2/.8/.95 defaults.
void setupHeavy(Fixture& a){g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;x::revokedNativeLease=0;
    g_henry_ctrl.store(a.controller.data());
    x::registrationReader=&FixtureRegistrationXR34;x::identityReader=&FixtureIdentityXR34;x::RefreshRegistration();x::SetSpatialReader(&spatial);x::PublishSource(a.source);heavyClipEvents(a);
    ++a.starts;a.put(a.p(a.child)+0x5C,2.5f);XR34PostStartNative(a.parent.data(),true);
    assert(a.get<float>(a.p(a.child)+0x5C)==0&&g_crossbow_xr34.pending());a.activate();
    a.put(a.p(a.parent)+0x9A,(unsigned char)1);XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
    assert(g_crossbow_xr34.status().phase==x::Phase::Setup);
    a.source.completedLease=g_crossbow_xr34.status().owner.serial;x::PublishSource(a.source);}
}

int main(){
    using namespace kcd2vr::ranged_options_d66c;
    assert(crankTravelSetting(NAN)==.08f&&crankTravelSetting(INFINITY)==.08f&&crankTravelSetting(-INFINITY)==.08f);
    assert(crankTravelSetting(.01f)==.04f&&crankTravelSetting(.3f)==.20f&&crankTravelSetting(.12f)==.12f);
    // The producer calls this actual helper after float-setting -> double transport.
    assert(x::crankTravelValidD66C(double(crankTravelSetting(.04f))));
    assert(x::crankTravelValidD66C(double(crankTravelSetting(.20f))));
    assert(!x::crankTravelValidD66C(.0399)&&!x::crankTravelValidD66C(.2001));
    assert(!x::crankTravelValidD66C(NAN)&&!x::crankTravelValidD66C(INFINITY));
    // Both slider endpoints also admit a real heavy grasp/stroke through the Client.
    for(float setting:{.04f,.20f}){Heavy h;auto& a=h.a;
        a.spatial.travel=double(crankTravelSetting(setting));
        a.spatial.crankQualified=x::crankTravelValidD66C(a.spatial.travel);setup(a);
        h.chest(true);h.frame(0,{0,-.3,-.3});h.chest(false);
        for(int i=0;i<40&&a.spawns==0;++i)h.frame(0,at(0));
        assert(a.spawns==1&&g_crossbow_xr34.pending());h.frame(0,at(0));h.frame(.8,at(0));
        h.frame(.8,at(a.spatial.travel+1e-8));assert(g_crossbow_xr34.status().phase==x::Phase::Tail);}
    // rotateInverse: world up expressed in a weapon frame rotated 90 deg about X is local +Y.
    {const float q[4]{float(std::sqrt(.5)),0,0,float(std::sqrt(.5))};const auto u=x::rotateInverse(q,{0,0,1});
        assert(std::abs(u.x)<1e-6&&std::abs(u.y-1)<1e-6&&std::abs(u.z)<1e-6);
        const float id[4]{0,0,0,1};const auto v=x::rotateInverse(id,{0,0,1});assert(std::abs(v.z-1)<1e-9);}
    // chest box (tracking space, Y up): 10..40 cm below the head, within 25 cm of its vertical axis.
    {const x::Vec head{1,1.6,2};
        assert(x::handInChestD66C(head,{1,1.35,2}));      // 25 cm below
        assert(!x::handInChestD66C(head,{1,1.55,2}));     // 5 cm below: too high
        assert(x::handInChestD66C(head,{1,1.15,2}));      // 45 cm below: belt pouch (R108)
        assert(!x::handInChestD66C(head,{1,1.05,2}));     // 55 cm below: too low
        assert(x::handInChestD66C(head,{1.3,1.35,2}));    // 30 cm sideways: controller beyond the palm (R108)
        assert(!x::handInChestD66C(head,{1.35,1.35,2}));  // 35 cm sideways: too far
        assert(x::handInChestD66C(head,{1.2,1.35,2.1}));  // 22 cm off axis
        assert(!x::handInChestD66C({NAN,1,1},{1,1,1}));}

    // 1. Full heavy cocking pass: no spawn until chest visit + return, then grasp at the
    //    string, 8 cm forward level push, cock, native completion. Loads never touched.
    {Heavy h;auto& a=h.a;setup(a);
        // hand near the string from the start, never at the chest: clock holds BEFORE spawn (.2)
        for(int i=0;i<120;++i)h.frame(0,at(0));
        assert(a.spawns==0&&a.cocks==0&&a.finals==0);
        assert(h.normalized()<.2f&&h.normalized()>.19f);
        assert(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().phase==x::Phase::Setup&&g_crossbow_xr34.status().reason==x::Reason::WaitingChest);
        // chest visit (tracked hand in the box), far from the string
        h.chest(true);for(int i=0;i<3;++i)h.frame(0,{0,-.3,-.3});
        assert(a.spawns==0&&g_crossbow_xr34.status().reason==x::Reason::WaitingChest);
        // back to the string (out of the chest box, within 30 cm of the string rest): spawn is granted
        h.chest(false);for(int i=0;i<40&&a.spawns==0;++i)h.frame(0,at(-.05));
        assert(a.spawns==1&&a.cocks==0&&a.spatial.toolVisible);
        h.frame(0,at(-.05));assert(g_crossbow_xr34.status().phase==x::Phase::Span);
        // grasp needs an open hand first, then grip>.65 within 12 cm of the string rest
        h.frame(.8,at(.3));assert(g_crossbow_xr34.status().progress==0);      // 30 cm away: no grasp
        h.frame(0,at(.3));h.frame(.8,at(0));                                   // open, then grasp AT the string
        assert(g_crossbow_xr34.status().reason==x::Reason::Working);
        h.frame(.8,at(.06,.06));assert(g_crossbow_xr34.status().progress==0);  // 6 cm lateral: not level, no credit
        h.frame(.8,at(.06,0,.05));assert(g_crossbow_xr34.status().progress==0);// 5 cm up: not level, no credit
        h.frame(.8,at(.04));assert(g_crossbow_xr34.status().progress>.49&&g_crossbow_xr34.status().progress<.51);
        // Config changes cannot finish a stationary stroke: retain its admitted grasp travel.
        a.spatial.travel=.04;h.frame(.8,at(.04));assert(g_crossbow_xr34.status().progress>.49&&g_crossbow_xr34.status().progress<.51);
        h.frame(.8,at(-.02));assert(g_crossbow_xr34.status().progress>.49);     // backwards keeps the best
        assert(a.cocks==0&&h.normalized()<.8f);
        h.frame(.8,at(.08,.03,.03));                                            // 8 cm forward, 3/3 cm off: still level
        assert(g_crossbow_xr34.status().phase==x::Phase::Tail);
        for(int i=0;i<300&&a.finals==0;++i)h.frame(.8,at(.08));
        assert(a.cocks==1&&a.finals==1&&a.loads==0&&a.spawns==1&&a.originalEvents==3);
        assert(a.spatial.nativeCocked);
        a.input(.8,at(.08));tick(a);assert(g_crossbow_xr34.status().reason==x::Reason::NativeAck); // after the finalizer: tick only, no second native frame
        XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
        // the loading child of the SAME heavy weapon: bolt in the hand, place at the nock, native load
        a.put(a.p(a.child)+0x54,18u);a.put(a.p(a.child)+0x28,1);a.put(a.p(a.scope)+0x148,std::uintptr_t(0));
        a.put(a.p(a.controller)+0x50,a.p(a.queued)+8);a.put(a.p(a.child)+0x10,0.f);a.put(a.entry()+0x44,0.f);
        a.remaining=5;a.spatial.toolBound=a.spatial.toolVisible=a.spatial.toolPoseReady=false;a.spatial.nativeCocked=true;
        a.put(a.p(a.parent)+0x9A,(unsigned char)0);
        setup(a,true,false);
        nativeUntil(a,.010001f);assert(a.spatial.handBoltBound&&a.loads==0);
        a.input(0);nativeFrame(a);a.input(.8);nativeFrame(a);a.input(0);nativeFrame(a);assert(a.loads==0&&a.finals==1);
        a.input(.8);nativeFrame(a);a.spatial.handBoltPlaced=true;a.input(0);nativeFrame(a);
        assert(g_crossbow_xr34.status().phase==x::Phase::Tail);
        for(int i=0;i<300&&a.finals<2;++i){a.input(0);nativeFrame(a);}
        assert(a.loads==1&&a.finals==2&&a.cocks==1);}

    // 2. No chest visit at all: the crank never spawns, the native cock never runs, no fallback line.
    {Heavy h;auto& a=h.a;setup(a);const unsigned lines=g_xr34_visible_lines_d65c;
        for(int i=0;i<300;++i)h.frame(i%2?.9:0,at(i%3*.03));
        assert(a.spawns==0&&a.cocks==0&&a.finals==0&&g_crossbow_xr34.pending());
        assert(g_crossbow_xr34.status().reason==x::Reason::WaitingChest&&g_xr34_visible_lines_d65c==lines);}

    // 3. Chest visited but the hand stays away from the string: still no spawn.
    {Heavy h;auto& a=h.a;setup(a);h.chest(true);for(int i=0;i<3;++i)h.frame(0,{0,-.3,-.3});
        h.chest(false);for(int i=0;i<60;++i)h.frame(0,{0,-.3,-.3});assert(a.spawns==0&&g_crossbow_xr34.status().reason==x::Reason::WaitingChest);
        for(int i=0;i<40&&a.spawns==0;++i)h.frame(0,at(.1));assert(a.spawns==1);}

    // 4. No tracked head/hands this sample: a named, visible fallback (native finishes), not a silent hang.
    {Heavy h;auto& a=h.a;a.source.chestKnown=false;setup(a);const unsigned lines=g_xr34_visible_lines_d65c;
        h.frame(0,at(0));assert(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
        assert(g_crossbow_xr34.status().reason==x::Reason::ChestUnknown&&g_xr34_visible_lines_d65c==lines+1);}

    // 5. Producer contract missing (crankQualified false) is GEOMETRY_UNAVAILABLE, as before.
    {Heavy h;auto& a=h.a;a.spatial.crankQualified=false;setup(a);h.frame(0,at(0));
        assert(!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::GeometryUnavailable);}

    // 6. Travel outside .04..0.20 in Span is refused by name; the class 14 window (.03..0.5) is not used.
    {Heavy h;auto& a=h.a;a.spatial.travel=.30;setup(a);
        h.chest(true);h.frame(0,{0,-.3,-.3});h.chest(false);for(int i=0;i<40&&a.spawns==0;++i)h.frame(0,at(0));
        assert(a.spawns==1); // Setup -> Span happens in the tick that first runs the Span contract check
        h.frame(0,at(0));assert(!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::GeometryUnavailable);}

    // 7. The medium goat's foot (class 14) is untouched: no chest gate, spawn at once, 8 cm lateral window.
    {Fixture a;setup(a);nativeUntil(a,.200001f);assert(a.spawns==1);
        for(int i=0;i<40;++i){a.input(0);nativeFrame(a);}
        a.input(.8);nativeFrame(a);a.input(.8,{.07,-.1315,0});nativeFrame(a);
        assert(g_crossbow_xr34.status().phase==x::Phase::Tail);}

    // Auto-start mailbox/executor intentionally omitted: native LT starts the reload.

    // 10. Real heavy clip layout from the game data: spawn .196, cock .7315, despawn .8178 spelled
    //     "DespawLever". The clip qualifies; the native no-op marker must retain tool placement.
    {Heavy h;auto& a=h.a;setupHeavy(a);
        const auto& c=g_crossbow_xr34.clip();assert(c.cocking&&!c.loading&&std::abs(c.spawn-.1961905f)<1e-6&&std::abs(c.cock-.7314583f)<1e-6&&std::abs(c.despawn-.8178125f)<1e-6);
        for(int i=0;i<60;++i)h.frame(0,at(0));assert(a.spawns==0&&h.normalized()<.1961905f&&h.normalized()>.19f);
        h.chest(true);h.frame(0,{0,-.3,-.3});h.chest(false);for(int i=0;i<40&&a.spawns==0;++i)h.frame(0,at(0));
        assert(a.spawns==1);h.frame(0,at(0));h.frame(.8,at(0));h.frame(.8,at(.08));assert(g_crossbow_xr34.status().phase==x::Phase::Tail);
        assert(g_crossbow_xr34.status().placeTool);
        for(int i=0;i<400&&a.finals==0;++i){h.frame(.8,at(.08));if(a.originalEvents==3)break;}
        assert(a.cocks==1&&a.originalEvents==3&&g_crossbow_xr34.status().placeTool);
        for(int i=0;i<400&&a.finals==0;++i)h.frame(.8,at(.08));assert(a.finals==1&&a.loads==0);
        XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending());}
    // Correctly spelled native event still withdraws before the native hide.
    {Heavy h;setup(h.a);g_crossbow_xr34.eventBefore(x::kDespawnLeverTypoCrcD66C);
        assert(g_crossbow_xr34.status().placeTool);g_crossbow_xr34.eventBefore(0x3D6870EBu);
        assert(!g_crossbow_xr34.status().placeTool);}

    std::cout<<"D66 corrected heavy crossbow: native LT start / exact-owner chest gate -> spawn -> grasp -> 8 cm level push -> cock -> bolt -> place, real heavy clip layout PASS (host fixture, no headset claim)\n";
}
