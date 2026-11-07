#include "../src/comfort_vignette51.h"
#include "../src/subsystems/experimental/deepseek_options_h29.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>

using namespace kcd2vr::comfort_vignette51;
static unsigned checks = 0;
static void ck(bool ok) { ++checks; assert(ok); }

static Frame walking(int level = 2) {
    Frame f{};
    f.level=level;f.gameplay=true;f.headValid=true;f.inputFresh=true;
    f.smoothTurn=true;f.leftActive=true;f.rightActive=true;
    f.leftY=1.0f;f.nowNs=1000000000ull;f.epoch=1;
    return f;
}
static float advance(Envelope& e,Frame& f,unsigned frames,unsigned hz=100) {
    float result=0.0f;
    for(unsigned i=0;i<frames;++i) {
        f.nowNs+=1000000000ull/hz;
        result=e.Step(f);
        ck(std::isfinite(result)&&result>=0.0f&&result<=0.95f);
    }
    return result;
}

int main() {
    {
        using namespace kcd2vr::deepseek_h29;
        ck(!angularVignette.load());
        Envelope e;auto f=walking(3);e.Step(f);
        const float strength=advance(e,f,100);
        ck(EncodeVignette(strength,false)==strength);
        angularVignette.store(true);
        const float pair=EncodeVignette(strength,angularVignette.load());
        DisableAll(); // A UI toggle after capture must not change either eye.
        ck(pair==-strength&&std::fabs(pair)==strength);
        ck(!angularVignette.load()&&EncodeVignette(strength,angularVignette.load())==strength);
        f.gameplay=false;ck(EncodeVignette(e.Step(f),true)==0.f);
        f.gameplay=true;f.level=0;ck(EncodeVignette(e.Step(f),true)==0.f);
        ck(EncodeVignette(std::numeric_limits<float>::quiet_NaN(),true)==0.f);
        ck(EncodeVignette(std::numeric_limits<float>::infinity(),true)==0.f);
        ck(EncodeVignette(-.7f,true)==0.f);
    }
    // The feature is off by default and cannot change presentation because of
    // controller motion alone. All levels remain bounded, even diagonal input.
    {
        Envelope e;auto f=walking(0);
        ck(e.Step(f)==0.0f);ck(advance(e,f,30)==0.0f);
        f.level=-7;ck(advance(e,f,2)==0.0f);
        f.level=3;ck(e.Step(f)==0.0f);
        ck(advance(e,f,100)>.94f);
        f.leftX=10.0f;f.leftY=-10.0f;f.level=500;
        ck(advance(e,f,100)<=.95f);
        f.level=1;ck(advance(e,f,1)<=.45f);
    }
    // Real-time attack and release are stable at different display cadences.
    {
        float values[4]{};const unsigned rates[]{60,72,90,120};
        for(unsigned i=0;i<4;++i){Envelope e;auto f=walking();ck(e.Step(f)==0.0f);
            values[i]=advance(e,f,rates[i]/2,rates[i]);
            ck(values[i]>.685f&&values[i]<.70f);
            f.leftY=0.0f;const float stopped=advance(e,f,rates[i],rates[i]);
            ck(stopped>0.03f&&stopped<0.05f);}
        for(unsigned i=1;i<4;++i)ck(std::fabs(values[i]-values[0])<.001f);
    }
    // Thumbstick noise and snap-turn holds are not continuing artificial
    // locomotion. Smooth turns work from either direction without left input.
    {
        Envelope e;auto f=walking();f.leftY=.1f;f.leftX=.1f;f.rightX=.1f;
        ck(advance(e,f,30)==0.0f);
        f.rightX=1.0f;f.smoothTurn=false;ck(advance(e,f,30)==0.0f);
        f.smoothTurn=true;ck(advance(e,f,30)>.60f);
        Envelope other;f.rightX=-1.0f;ck(other.Step(f)==0.0f);ck(advance(other,f,30)>.60f);
    }
    // Moving a menu selector cannot accumulate a hidden mask and then flash
    // it on closing the menu. Disabling the option clears on the next frame.
    {
        Envelope e;auto f=walking(3);ck(e.Step(f)==0.0f);ck(advance(e,f,100)>.94f);
        f.gameplay=false;ck(advance(e,f,1)==0.0f);ck(advance(e,f,100)==0.0f);
        f.gameplay=true;ck(advance(e,f,1)==0.0f);
        const float restarted=advance(e,f,1);ck(restarted>0.0f&&restarted<.1f);
        ck(advance(e,f,100)>.94f);f.level=0;ck(advance(e,f,1)==0.0f);
        f.level=3;ck(advance(e,f,1)==0.0f);
    }
    // Tracking/focus/owner epochs are hard boundaries, not a release fade from
    // a stale held stick. First reacquisition sample never replays the mask.
    for(unsigned failure=0;failure<9;++failure){
        Envelope e;auto f=walking(3);e.Step(f);advance(e,f,100);
        const Frame good=f;
        if(failure==0)f.headValid=false;
        if(failure==1)f.inputFresh=false;
        if(failure==2)f.epoch=0;
        if(failure==3)f.epoch=2;
        if(failure==4)f.nowNs+=120000001ull;
        if(failure==5)f.nowNs-=1;
        if(failure==6)f.leftActive=f.rightActive=false;
        if(failure==7)f.leftX=std::numeric_limits<float>::quiet_NaN();
        if(failure==8)f.rightX=std::numeric_limits<float>::infinity();
        ck(e.Step(f)==0.0f);
        f=good;f.nowNs+=200000000ull;f.epoch=3;
        ck(e.Step(f)==0.0f);ck(advance(e,f,1)<.1f);
    }
    // Invalid axes from a non-tracked controller carry no authority and cannot
    // poison another controller. Separate manager instances share no clock.
    {
        Envelope a,b;auto f=walking(2);a.Step(f);ck(advance(a,f,50)>.68f);
        ck(b.Step(f)==0.0f);f.leftActive=false;
        f.leftX=f.leftY=std::numeric_limits<float>::quiet_NaN();f.rightX=1.0f;
        ck(advance(b,f,50)>.68f);
        a.Reset();ck(a.Step(f)==0.0f);
    }
    std::printf("comfort_vignette51_test: %u checks OK\n",checks);
    return 0;
}
