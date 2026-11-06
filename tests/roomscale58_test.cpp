// V66.178R58 roomscale follow / dodge logic (pure header, no native state).
#include "../src/vrik/roomscale58.h"
#include <cstdio>
#include <cmath>
static int fails=0;
static void ck(bool ok,const char* what){std::printf("%s %s\n",ok?"PASS":"FAIL",what);if(!ok)++fails;}
int main(){
    using namespace kcd2vr::roomscale58;Config c{};State s{};
    ck(c.dodgeDisplacement==.10&&c.slowDodgeDisplacement==.10,"production default requires more than10cm lateral/backward HMD translation");
    // New human threshold: real default production configuration, no option override.
    const auto motion=[&](double x,double y){State q{};for(int i=0;i<=5;++i)observeHmd(q,10000+i*20,x*i/5.,y*i/5.);return drive(q,10100,x,y,false,true,true,Config{});};
    for(double x:{-.10,-.099,0.,.099,.10})ck(!motion(x,0).dodge,"at/below10cm lateral translation never requests dodge");
    ck(!motion(0,-.10).dodge,"exactly10cm backward translation is below strict qualification");
    ck(motion(-.101,0).dodge&&motion(.101,0).dodge&&motion(0,-.101).dodge,"more than10cm left/right/backward requests dodge");
    ck(!motion(0,.4).dodge,"large forward HMD translation never requests dodge");
    ck(!motion(0,0).dodge,"unchanged HMD position cannot request dodge irrespective of rotation outside this translation-only interface");
    State anchorOnly{};for(int i=0;i<=5;++i){anchorOnly.anchorX=-i*.05;observeHmd(anchorOnly,11000+i*20,0,0);}
    ck(!drive(anchorOnly,11100,0,0,true,true,true,Config{}).dodge,"owned native follow anchor changes cannot become tracked HMD displacement");
    // Historical10cm fixtures below retain their explicit configurable geometry;
    // their purpose is cooldown/follow/expiry/rearm, not the production default.
    c.dodgeDisplacement=c.slowDodgeDisplacement=.10;
    ck(c.dodgeCooldownMs==2000,"H17 preserves the requested two-second dodge cooldown");
    // Leaning inside the dead zone asks nothing.
    auto d=drive(s,1000,.08,.05,true,false,true,c);ck(!d.following&&d.lx==0&&d.ly==0,"lean inside the dead zone injects nothing");
    // 40 cm forward: a forward stick, walking speed cap.
    d=drive(s,1016,0,.40,true,false,true,c);ck(d.following&&d.ly>0.6f&&d.ly<=0.65f&&std::abs(d.lx)<1e-3f,"40 cm forward asks a forward walk");
    // The entity catches up 10 cm: the anchor absorbs it, the remainder shrinks.
    consume(s,1020,0,.40,0,.10,c);ck(std::abs(s.anchorY+.10)<1e-9&&std::abs(s.anchorX)<1e-9,"entity catch-up is subtracted from the view offset");
    consume(s,1020,0,.40,.05,0,c);ck(std::abs(s.anchorY+.10)<1e-9&&std::abs(s.anchorX)<1e-9,"sideways entity motion (stick, push) is not consumed");
    consume(s,1020,0,.40,0,-.10,c);ck(std::abs(s.anchorY+.10)<1e-9,"moving away from the player is not consumed");
    consume(s,1020,0,.40,0,5.0,c);ck(std::abs(s.anchorY+.10)<1e-9,"a teleport is ignored");
    for(int i=0;i<10;++i)consume(s,1020,0,.40,0,.10,c);ck(std::abs(s.anchorY+.40)<1e-9,"catch-up never passes the player (anchor capped at the offset)");
    d=drive(s,1032,0,.40,true,false,true,c);ck(!d.following,"arrived: nothing injected");
    // R59: the blend-out right after the stick drops is still consumed; long after it is not.
    s.anchorY=-.30;consume(s,1100,0,.40,0,.05,c);ck(std::abs(s.anchorY+.35)<1e-9,"blend-out inside the 300 ms grace is consumed");
    consume(s,1016+c.catchupGraceMs+1,0,.40,0,.05,c);ck(std::abs(s.anchorY+.35)<1e-9,"entity motion after the grace is not ours");
    s.anchorY=-.40;
    // Walking back to the origin: R becomes negative, Henry walks back.
    d=drive(s,1048,0,0,true,false,true,c);ck(d.following&&d.ly<-0.5f,"returning to the origin walks Henry back");
    // Dodge: only in combat with a locked opponent, sideways, >= 12 cm in 250 ms.
    State t{};for(int i=0;i<6;++i)observeHmd(t,2000+i*20,i*.03,0);
    d=drive(t,2100,.15,0,true,false,true,c);ck(!d.dodge,"no dodge outside a locked combat");
    d=drive(t,2100,.15,0,true,true,false,c);ck(!d.dodge,"no dodge when the option is off");
    for(int i=0;i<6;++i)observeHmd(t,2000+i*20,i*.03,0);
    t.anchorY=-.27;
    d=drive(t,2100,.15,0,true,true,true,c);ck(d.dodge&&d.dodgeLx>.99f&&std::abs(d.dodgeLy)<1e-3f,"a 15 cm sideways move in 100 ms is a dodge to the right");
    ck(t.anchorX==0&&t.anchorY==-.27,"requesting a dodge cannot reset the camera anchor");
    d=drive(t,2150,.15,0,true,true,true,c);ck(d.dodge,"the dodge is held for its window");
    d=drive(t,2130+c.dodgeHoldMs,.15,0,true,true,true,c);ck(!d.dodge,"the dodge window ends");
    State f{};for(int i=0;i<6;++i)observeHmd(f,3000+i*20,0,i*.03);
    d=drive(f,3100,0,.15,true,true,true,c);ck(!d.dodge,"a forward move is a step, never a dodge");
    State b{};for(int i=0;i<6;++i)observeHmd(b,4000+i*20,0,-i*.03);
    d=drive(b,4100,0,-.15,true,true,true,c);ck(d.dodge&&d.dodgeLy<-.99f,"a backward move is a backward dodge");
    d=drive(b,4100+c.dodgeHoldMs+10,0,-.15,true,true,true,c);ck(!d.dodge,"no second dodge inside the cooldown");
    State loss{};for(int i=0;i<6;++i)observeHmd(loss,5000+i*20,i*.03,0);
    ck(drive(loss,5100,.15,0,true,true,true,c).dodge,"fixture starts one dodge");
    ck(!drive(loss,5110,.15,0,true,false,true,c).dodge,"opponent loss revokes active dodge immediately");
    const auto ax=loss.anchorX,ay=loss.anchorY;
    drive(loss,5111,.15,0,false,false,false,c);consume(loss,5111,.15,0,.1,0,c);
    ck(loss.anchorX==ax&&loss.anchorY==ay,"unowned locomotion cannot adjust the camera anchor");
    State duplicate{};observeHmd(duplicate,6000,0,0);observeHmd(duplicate,6020,.15,0);
    for(int i=0;i<1000;++i)observeHmd(duplicate,6020,.15,0);
    ck(duplicate.histCount==2,"repeated worker reads do not consume the HMD history");
    ck(!drive(duplicate,6100,.15,0,true,true,true,c).dodge,"stale HMD motion cannot request dodge");
    {   // R59.16C5: HMD dodge without FOLLOW (dodge-only mode). No walking is ever injected.
        State u{};observeHmd(u,7000,0,0);observeHmd(u,7050,.04,0);observeHmd(u,7100,.09,0);observeHmd(u,7150,.15,0);
        auto e=drive(u,7160,.15,0,false,true,true,c);ck(e.dodge&&e.dodgeLx>.9f&&!e.following&&e.lx==0&&e.ly==0,"dodge-only: a quick side step dodges without follow");
        State v{};observeHmd(v,8000,0,0);observeHmd(v,8100,0,.40);auto g=drive(v,8110,0,.40,false,false,true,c);ck(!g.following&&!g.dodge,"dodge-only: standing 40 cm away never walks Henry");
        State w{};observeHmd(w,9000,0,0);observeHmd(w,9050,.15,0);ck(!drive(w,9060,.15,0,false,true,false,c).dodge,"both off: nothing");
    }
    ck(bodySelection(false,true,0)==0,"BODY enable preserves physical walking OFF");
    ck(bodySelection(false,true,3)==3,"BODY enable preserves dodge-only without adding walking");
    ck(bodySelection(true,true,0)==0,"subsequent explicit roomscale OFF remains off");
    ck(bodySelection(false,false,3)==3,"dodge alone does not enable walking");
    { // U16: a real step need not return to the original room position.
        State step{};
        for(int i=0;i<6;++i)observeHmd(step,10000+i*20,i*.024,0);
        ck(drive(step,10100,.12,0,false,true,true,c).dodge,"U16 first right displacement requests native dodge");
        // Settling at .12m must not trigger an opposite return gesture or latch forever.
        for(int i=1;i<=35;++i){const auto t=10100+i*20;observeHmd(step,t,.12,0);
            ck(!drive(step,t,.12,0,false,true,true,c).dodge||t<10220,"U16 hold expires and stationary offset never retriggers");}
        ck(!step.returning54,"U16 quiet new position rearms without returning within4cm");
        for(int i=1;i<=6;++i){const auto t=10800+i*20;observeHmd(step,t,.12-i*.024,0);d=drive(step,t,.12-i*.024,0,false,true,true,c);}
        ck(!d.dodge&&step.dodges==1,"H17 settled return motion inside two seconds cannot request opposite dodge");
        for(int i=1;i<=64;++i){const auto t=10920+i*20;observeHmd(step,t,-.024,0);drive(step,t,-.024,0,false,true,true,c);}
        for(int i=1;i<=6;++i){const auto t=12200+i*20;observeHmd(step,t,-.024-i*.024,0);d=drive(step,t,-.024-i*.024,0,false,true,true,c);}
        ck(d.dodge&&d.dodgeLx<-.99f,"H17 next left step works at a new origin after two-second cooldown");
        State left{};for(int i=0;i<6;++i)observeHmd(left,12000+i*20,-i*.024,0);
        ck(drive(left,12100,-.12,0,false,true,true,c).dodgeLx<-.99f,"U16 symmetric left threshold");
        State gap{};for(int i=0;i<6;++i)observeHmd(gap,13000+i*20,i*.024,0);
        ck(drive(gap,13100,.12,0,false,true,true,c).dodge,"U16 gap fixture begins once");
        observeHmd(gap,14000,.12,0);
        ck(!gap.returning54&&!drive(gap,14000,.12,0,false,true,true,c).dodge,"U16 tracking gap retires gesture without replay");
    }
    std::printf("%d failures\n",fails);return fails?1:0;
}
