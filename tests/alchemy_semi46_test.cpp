#include "vrik/alchemy_semi46.h"
#include "vrik/workstation_sample46.h"
#include "core/book_gesture46.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
static unsigned checks=0;static void check(bool ok){++checks;if(!ok)std::fprintf(stderr,"failed check %u\n",checks);assert(ok);}
static workstation_sample46::Frame frame(std::uint64_t tick,std::uint64_t seq){
    workstation_sample46::Frame f{};f.sequence=seq;f.tick=tick;f.timeNs=tick*1000000ull;f.poseEpoch=7;f.sourceEpoch=9;f.allowed=f.headValid=true;
    f.head[1]=1.7f;f.hand[0].valid=f.hand[1].valid=true;
    f.hand[0].position[0]=-.10f;f.hand[0].position[1]=1.3f;f.hand[0].position[2]=-.40f;
    f.hand[1].position[0]=-.20f;f.hand[1].position[1]=1.3f;f.hand[1].position[2]=-.40f;return f;
}
static alchemy_semi46::WorkWitness makeWork(unsigned kind=alchemy_semi46::Bellows,int phase=2){
    alchemy_semi46::WorkWitness w{};w.instance=10;w.lifetime=2;w.parent=20;w.child=30;w.token=7;w.kind=kind;w.phase=phase;w.tick=1000;
    w.working=(kind==alchemy_semi46::Bellows?phase==2:phase==3);w.ready=(kind==alchemy_semi46::Bellows?phase==1:phase==2);return w;
}
int main(){
    using namespace alchemy_semi46;
    {Producer p;auto w=makeWork();auto f=frame(1000,1);auto c=p.step(f,true,10,2,w);check(c.enabled&&c.totalSeconds==0);
     for(unsigned n=1;n<=20;++n){f=frame(1000+n*10,1+n);f.hand[0].position[1]-=.001f*n;w.tick=f.tick;c=p.step(f,true,10,2,w);}
     check(c.totalSeconds>.09&&c.totalSeconds<.13&&c.hand==0); // slow one-mm samples select and accumulate real work
     const auto before=c.totalSeconds;f.tick+=10;f.timeNs+=10000000;f.sequence++;w.tick=f.tick;c=p.step(f,true,10,2,w);check(c.totalSeconds==before);
     f.head[0]+=.3f;f.head[1]-=.2f;f.tick+=10;f.timeNs+=10000000;f.sequence++;w.tick=f.tick;c=p.step(f,true,10,2,w);check(c.totalSeconds==before); // head-only lean is zero work
     c=p.step(f,true,10,2,w);check(c.totalSeconds==before); // duplicate preserves cumulative ledger without new credit
     f.hand[0].valid=false;f.tick+=10;f.timeNs+=10000000;f.sequence++;w.tick=f.tick;c=p.step(f,true,10,2,w);check(!c.enabled);
    }
    {Producer p;auto w=makeWork();auto f=frame(1000,1);p.step(f,true,10,2,w);f=frame(1010,2);f.hand[0].position[1]-=.02f;w.tick=1015;
     auto c=p.step(f,true,10,2,w);check(c.enabled&&c.totalSeconds>.1); // native witness can be newer than input sample
     f=frame(1020,3);f.poseEpoch++;w.tick=1020;c=p.step(f,true,10,2,w);check(c.totalSeconds==0); // recenter loses no bogus delta
     f=frame(1101,4);w.tick=1101;c=p.step(f,true,10,2,w);check(c.totalSeconds==0); // stall starts a new reference
     f=frame(1111,5);f.hand[0].position[1]-=.25f;w.tick=1111;c=p.step(f,true,10,2,w);check(c.totalSeconds==0); // pose jump rejected
    }
    {Producer p;auto w=makeWork(Bellows,1);auto f=frame(1000,1);p.step(f,true,10,2,w);
     f=frame(1020,2);f.hand[0].position[1]-=.03f;w.tick=1020;auto c=p.step(f,true,10,2,w);check(c.use&&c.useUntil==1100&&c.totalSeconds==0);
     f=frame(1040,3);f.hand[0].position[1]-=.04f;w.tick=1040;c=p.step(f,true,10,2,w);check(c.use&&c.useUntil==1100);
     f=frame(1110,4);w.tick=1110;c=p.step(f,true,10,2,w);check(!c.use); // no repeatedly renewed A
     w.phase=2;w.working=true;w.ready=false;w.child=31;f=frame(1130,5);w.tick=1130;c=p.step(f,true,10,2,w);check(!c.use&&c.totalSeconds==0);
     w.phase=1;w.working=false;w.ready=true;f=frame(1150,6);w.tick=1150;p.step(f,true,10,2,w);
     f=frame(1170,7);f.hand[1].position[1]-=.03f;w.tick=1170;c=p.step(f,true,10,2,w);check(c.use); // next native ready phase and either hand admit one pump
    }
    {Producer p;auto w=makeWork(Bellows,1);auto f=frame(1000,1);auto c=p.step(f,true,10,2,w,true);check(!c.use);
     f=frame(1020,2);f.hand[0].position[1]-=.04f;w.tick=f.tick;c=p.step(f,true,10,2,w,true);check(!c.use);
     c=p.step(f,true,10,2,w,false);check(!c.use&&c.hand==-1); // still READY: release rebases admission, including same XR sample
     f=frame(1040,3);f.hand[0].position[1]-=.071f;w.tick=f.tick;c=p.step(f,true,10,2,w);check(c.use); // first gesture after release starts work
     c=p.step(f,true,10,2,w,true);check(!c.use);p.step(f,true,10,2,w,false);
     f=frame(1060,4);f.hand[0].position[1]-=.105f;w.tick=f.tick;c=p.step(f,true,10,2,w);check(!c.use); // emitted gesture remains consumed after A toggle
    }
    {Producer p;auto w=makeWork(Mortar,3);auto f=frame(1000,1);p.step(f,true,10,2,w);f=frame(1020,2);f.hand[1].position[0]+=.04f;w.tick=1020;
     auto c=p.step(f,true,10,2,w);check(c.totalSeconds==0); // holding mortar left does not grind
     f=frame(1040,3);f.hand[0].position[0]+=.04f;w.tick=1040;c=p.step(f,true,10,2,w);check(c.totalSeconds>.1);
     c=p.step(f,true,10,2,w,false,true);check(!c.enabled&&!c.use); // native B remains normal cancellation
    }
    {Producer p;auto w=makeWork(Mortar,2);w.working=true;w.ready=false;auto f=frame(1000,1);p.step(f,true,10,2,w);
     f=frame(1010,2);f.hand[0].position[0]+=.0032f;w.tick=f.tick;auto c=p.step(f,true,10,2,w);
     f=frame(1020,3);f.hand[0].position[0]+=.0048f;w.tick=f.tick;c=p.step(f,true,10,2,w);check(std::fabs(c.totalSeconds-.03)<1e-5&&c.hand==0);
     const double credited=c.totalSeconds;const float reference=p.reference[0][0],last=p.last[0][0];
     w.phase=3;c=p.step(f,true,10,2,w);check(c.totalSeconds==credited&&c.hand==0&&p.owner.phase==3&&p.reference[0][0]==reference&&p.last[0][0]==last); // same sample observes native queue completion
     f=frame(1030,4);f.hand[0].position[0]+=.0064f;w.tick=f.tick;c=p.step(f,true,10,2,w);check(std::fabs(c.totalSeconds-.04)<1e-5&&c.hand==0); // first moving live sample conserves all credit
     c=p.step(f,true,10,2,w);check(std::fabs(c.totalSeconds-.04)<1e-5); // transition credit never replays
    }
    for(unsigned fault=0;fault<8;++fault){Producer p;auto w=makeWork(Mortar,2);w.working=true;w.ready=false;auto f=frame(1000,1);p.step(f,true,10,2,w);
     f=frame(1010,2);f.hand[0].position[0]+=.0032f;w.tick=f.tick;p.step(f,true,10,2,w);
     f=frame(1020,3);f.hand[0].position[0]+=.0048f;w.tick=f.tick;w.phase=3;
     if(fault==0)++w.child;if(fault==1)++w.token;if(fault==2)++w.parent;if(fault==3)++w.instance;if(fault==4)++w.lifetime;
     if(fault==5)++f.poseEpoch;if(fault==6)++f.sourceEpoch;
     if(fault==7){w.phase=2;w.working=false;w.ready=true;} // same numeric mortar phase has different READY meaning outside Queue
     const auto c=p.step(f,true,10,2,w);check(c.totalSeconds==0&&c.hand==-1); // every actual boundary still resets
    }
    {producer.reset();publishedSequence=publishedTick=0;auto w=makeWork();work.publish(w);auto f=frame(1000,1);
     publish(f,true,10,2);f=frame(1020,2);f.hand[0].position[1]-=.02f;w.tick=1020;work.publish(w);publish(f,true,10,2);
     Control a{};check(control.read(a)&&a.totalSeconds>.1);publish(f,true,10,2);Control b{};check(control.read(b)&&a.sequence==b.sequence&&a.totalSeconds==b.totalSeconds);
     publish(f,true,10,2,true);check(control.read(b)&&b.manualUse&&b.totalSeconds==a.totalSeconds); // same XR sample: manual takeover
     publish(f,true,10,2,false);check(control.read(b)&&!b.manualUse&&b.totalSeconds==a.totalSeconds); // release does not rewind ledger
     ++w.child;++w.token;work.publish(w);publish(f,true,10,2);check(control.read(b)&&b.child==w.child&&b.totalSeconds==0); // native work changed on same XR sample
     publish(f,false,10,2);check(control.read(b)&&!b.enabled); // disable same sample is immediate
    }
    {producer.reset();publishedSequence=publishedTick=0;hooksReady=true;minigame_hands45::mode=minigame_hands45::SemiPassenger;
     auto w=makeWork(Bellows,1);work.publish(w);auto f=frame(1000,1);publish(f,true,10,2);
     f=frame(1020,2);f.hand[0].position[1]-=.03f;w.tick=f.tick;work.publish(w);publish(f,true,10,2);check(pulse(1020));
     minigame_hands45::mode=minigame_hands45::Native;check(!pulse(1020)); // final atomic mode gate, no worker update
     minigame_hands45::mode=minigame_hands45::SemiPassenger;publish(f,true,10,2,true);check(!pulse(1020)); // A takeover revokes same-frame synth pulse
     hooksReady=false;
    }
    {Producer p;auto w=makeWork(Resource,1);w.working=true;w.ready=false;auto f=frame(1000,1);p.step(f,true,10,2,w);
     f=frame(1020,2);f.hand[0].position[0]+=.03f;w.tick=f.tick;auto c=p.step(f,true,10,2,w);check(c.totalSeconds==0); // translation cannot pour
     f=frame(1040,3);f.hand[0].rotation[1]=std::sin(.2f);f.hand[0].rotation[3]=std::cos(.2f);w.tick=f.tick;c=p.step(f,true,10,2,w);check(c.totalSeconds==0); // yaw cannot pour
     f=frame(1060,4);f.hand[1].rotation[2]=std::sin(.04f);f.hand[1].rotation[3]=std::cos(.04f);w.tick=f.tick;c=p.step(f,true,10,2,w);check(c.totalSeconds>.1&&c.hand==1);
     const auto total=c.totalSeconds;f.tick+=20;f.timeNs+=20000000;f.sequence++;w.tick=f.tick;c=p.step(f,true,10,2,w);check(c.totalSeconds==total); // still tilted = stopped working
    }
    {Producer p;auto w=makeWork(Bellows,0);auto f=frame(1000,1);auto c=p.step(f,true,10,2,w,true);check(!c.manualUse);
     w.phase=2;w.working=true;w.ready=false;f=frame(1020,2);w.tick=f.tick;c=p.step(f,true,10,2,w,true);check(!c.manualUse&&c.totalSeconds==0);
     f=frame(1040,3);w.tick=f.tick;f.hand[0].position[1]-=.02f;c=p.step(f,true,10,2,w,true);check(!c.manualUse&&c.totalSeconds>.1); // still-held admission does not retire motion work
     const auto total=c.totalSeconds;p.step(f,true,10,2,w,false);c=p.step(f,true,10,2,w,true);check(c.manualUse&&c.totalSeconds==total); // new working press is takeover
    }
    {book_gesture46::Arbiter a;auto f=frame(1000,1);auto r=a.step(book_pages45::Alchemy,5,1000,book_pages45::ThumbrestFlick,false,false,f,0);
     check(!r.requested);f=frame(1050,2);f.hand[0].position[0]=-.03f;r=a.step(book_pages45::Alchemy,5,1050,book_pages45::ThumbrestFlick,false,false,f,0);check(!r.requested);
     f=frame(1100,3);f.hand[0].position[0]=.04f;r=a.step(book_pages45::Alchemy,5,1100,book_pages45::ThumbrestFlick,false,false,f,0);
     check(r.accepted&&r.source==book_pages45::Flick&&r.dir==-1&&r.page.left&&!r.page.right);
     f=frame(1150,4);r=a.step(book_pages45::Alchemy,5,1150,book_pages45::ThumbrestFlick,false,false,f,0);check(!r.requested); // return does not flip
     f=frame(1200,5);f.allowed=false;r=a.step(book_pages45::Alchemy,5,1200,book_pages45::ThumbrestFlick,false,false,f,0);check(!r.page.left);
    }
    {book_gesture46::Arbiter a;auto f=frame(1000,1);a.step(book_pages45::Reading,5,1000,0,false,false,f,0);
     f=frame(1050,2);f.head[0]+=.20f;auto r=a.step(book_pages45::Reading,5,1050,0,false,false,f,0);check(!r.requested); // head-only lean not page
     f=frame(1100,3);f.hand[0].position[0]=.2f;f.poseEpoch++;r=a.step(book_pages45::Reading,5,1100,0,false,false,f,0);check(!r.requested);
     f=frame(1150,4);f.hand[0].position[0]=-.2f;f.poseEpoch+=2;r=a.step(book_pages45::Reading,5,1150,0,false,false,f,0);check(!r.requested);
     r=a.step(book_pages45::Reading,5,1250,0,false,false,f,0);check(!r.requested); // stale repeated sample
    }
    {book_gesture46::Arbiter a;auto f=frame(1000,1);book_surface46::Surface s{};s.instance=10;s.action=20;s.lifetime=2;s.entity=19;s.valid=true;
     s.poseEpoch=f.poseEpoch;s.sourceEpoch=f.sourceEpoch;s.right[0]=s.up[1]=s.normal[2]=1;s.halfWidth=.22f;s.halfHeight=.18f;s.handValid[1]=true;
     const auto step=[&](std::uint64_t tick,std::uint64_t seq,float x){f=frame(tick,seq);s.tick=tick;s.sequence=seq;s.hand[1][0]=x;s.hand[1][1]=0;s.hand[1][2]=.02f;book_surface46::current.publish(s);
         return a.step(book_pages45::Alchemy,5,tick,0,false,false,f,0,10,20,2);};
     check(!step(1000,1,.08f).requested);check(!step(1050,2,.01f).requested);auto r=step(1100,3,-.06f);
     check(r.accepted&&r.dir==1&&r.page.right);check(a.flick.surface); // either hand, native plane: right->left = NEXT
     s.action=21;r=step(1150,4,.12f);check(!r.requested&&!a.flick.surface); // wrong native owner drops old touch history
    }
    {book_gesture46::Arbiter a;auto f=frame(1000,1);hooksReady=true;auto w=makeWork(Book,0);w.working=w.ready=false;work.publish(w);
     bookEnable.publish({10,20,2,10,true});a.step(book_pages45::Alchemy,10,1000,0,false,false,f,0,10,20,2);
     f=frame(1050,2);f.hand[0].position[0]=-.02f;a.step(book_pages45::Alchemy,10,1050,0,false,false,f,0,10,20,2);
     f=frame(1100,3);f.hand[0].position[0]=.05f;auto r=a.step(book_pages45::Alchemy,10,1100,0,false,false,f,0,10,20,2);check(r.accepted&&a.flick.hand[0].consumed);
     bookEnable.publish({10,20,2,11,false});f=frame(1150,4);f.hand[0].position[0]=.05f;w.tick=1150;work.publish(w);
     r=a.step(book_pages45::None,0,1150,0,false,false,f,0);check(!r.page.left&&a.flick.hand[0].consumed);
     bookEnable.publish({10,20,2,12,true});f=frame(1200,5);f.hand[0].position[0]=-.10f;w.tick=1200;work.publish(w);
     r=a.step(book_pages45::Alchemy,12,1200,0,false,false,f,0,10,20,2);check(!r.requested&&a.flick.hand[0].consumed); // physical return cannot undo the page after native re-enable
     hooksReady=false;
    }
    std::printf("PASS C46 semi-passenger motion and book task frames: %u checks\n",checks);
}
