#include "core/book_pages45.h"
#include <cassert>
using namespace kcd2vr::book_pages45;
struct Pad {bool lb=false,rb=false,dpad_left=false,dpad_right=false;float lx=0,rx=0;};
static Head head(){Head h;h.valid=true;h.x=0;h.y=1.7f;h.z=0;h.fx=0;h.fz=-1;return h;}
static Hand hand(float lat,float up,float fwd){Hand h;h.valid=true;h.x=lat;h.y=1.7f+up;h.z=-fwd;return h;}
int main(){
 // --- flick recognizer: left->right = previous (-1), right->left = next (+1), one per stroke, rearm after departure
 {FlickRecognizer f;const auto hd=head();
  assert(f.step(0,hd,hand(-.10f,-.40f,.40f),1000)==0); // arm inside the zone
  assert(f.step(0,hd,hand(-.04f,-.40f,.40f),1050)==0);
  assert(f.step(0,hd,hand(.03f,-.41f,.41f),1100)==-1); // 13 cm to the right within 100 ms: previous
  assert(f.step(0,hd,hand(.20f,-.41f,.41f),1150)==0); // consumed: no second pulse in the same stroke
  assert(f.step(0,hd,hand(-.10f,-.41f,.41f),1400)==0); // the return move does not flip back
  assert(f.step(0,hd,hand(-.10f,.20f,.41f),1500)==0); // hand lifted above the eyes = departure
  assert(f.step(0,hd,hand(.10f,-.40f,.40f),1600)==0); // re-entry arms again
  assert(f.step(0,hd,hand(-.03f,-.40f,.40f),1700)==1); // right->left: next
  // too slow: 12 cm over 600 ms is not a flick
  FlickRecognizer g;assert(g.step(1,hd,hand(0,-.4f,.4f),2000)==0);assert(g.step(1,hd,hand(.06f,-.4f,.4f),2300)==0);assert(g.step(1,hd,hand(.13f,-.4f,.4f),2600)==0);
  // vertical dominant is not a flick
  FlickRecognizer v;assert(v.step(1,hd,hand(0,-.6f,.4f),3000)==0);assert(v.step(1,hd,hand(.13f,-.3f,.4f),3100)==0);
  // outside the zone (behind / far) never fires
  FlickRecognizer o;assert(o.step(0,hd,hand(0,-.4f,1.2f),4000)==0);assert(o.step(0,hd,hand(.2f,-.4f,1.2f),4100)==0);
  Head bad;assert(o.step(0,bad,hand(0,-.4f,.4f),4200)==0);
 }
 // --- arbiter: thumbrest edges, busy window, book identity, modes
 {Arbiter a;const auto hd=head();Hand none{};
  auto r=a.step(Reading,7,1000,ThumbrestOnly,true,false,hd,none,none,0);assert(!r.requested&&!r.page.left); // held on entry
  r=a.step(Reading,7,1010,ThumbrestOnly,false,false,hd,none,none,0);assert(!r.requested);
  r=a.step(Reading,7,1020,ThumbrestOnly,true,false,hd,none,none,0);assert(r.requested&&r.accepted&&r.source==Thumbrest&&r.dir==-1&&r.page.left&&!r.page.right);
  r=a.step(Reading,7,1050,ThumbrestOnly,true,false,hd,none,none,0);assert(!r.requested&&r.page.left); // 80 ms pulse continues, no re-request
  r=a.step(Reading,7,1110,ThumbrestOnly,false,true,hd,none,none,0);assert(r.requested&&!r.accepted&&r.busy&&!r.page.left&&!r.page.right); // busy 250 ms
  r=a.step(Reading,7,1300,ThumbrestOnly,false,false,hd,none,none,0);r=a.step(Reading,7,1310,ThumbrestOnly,false,true,hd,none,none,0);
  assert(r.accepted&&r.dir==1&&r.page.right&&r.source==Thumbrest);
  r=a.step(Alchemy,7,1600,ThumbrestOnly,false,true,hd,none,none,0);assert(!r.requested); // book change: held edge dropped
  r=a.step(Alchemy,7,1610,ThumbrestOnly,false,false,hd,none,none,0);r=a.step(Alchemy,7,1620,ThumbrestOnly,false,true,hd,none,none,0);assert(r.accepted&&r.dir==1);
  r=a.step(None,0,1700,ThumbrestOnly,false,true,hd,none,none,0);assert(!r.requested&&!r.page.right);
  // stick alone never flips in the thumbrest modes
  r=a.step(Reading,8,2000,ThumbrestOnly,false,false,hd,none,none,0);r=a.step(Reading,8,2020,ThumbrestOnly,false,false,hd,none,none,1.f);assert(!r.requested&&!r.page.right);
  // flick in THUMBREST+FLICK mode, refused in THUMBREST ONLY
  Arbiter b;b.step(Reading,9,3000,ThumbrestFlick,false,false,hd,hand(-.10f,-.4f,.4f),none,0);
  r=b.step(Reading,9,3100,ThumbrestFlick,false,false,hd,hand(.04f,-.4f,.4f),none,0);assert(r.accepted&&r.source==Flick&&r.dir==-1&&r.page.left);
  Arbiter c;c.step(Reading,9,3000,ThumbrestOnly,false,false,hd,hand(-.10f,-.4f,.4f),none,0);
  r=c.step(Reading,9,3100,ThumbrestOnly,false,false,hd,hand(.04f,-.4f,.4f),none,0);assert(!r.requested);
  // NATIVE keeps the C42 stick pulse for Reading only
  Arbiter n;n.step(Reading,10,4000,Native,false,false,hd,none,none,0);r=n.step(Reading,10,4020,Native,false,false,hd,none,none,1.f);assert(r.accepted&&r.source==Stick&&r.page.right);
  r=n.step(Reading,10,4030,Native,false,false,hd,none,none,1.f);assert(!r.requested&&r.page.right); // edge-only request
  Arbiter m;m.step(Alchemy,11,5000,Native,false,false,hd,none,none,0);r=m.step(Alchemy,11,5020,Native,false,false,hd,none,none,1.f);assert(!r.requested&&!r.page.right);
 }
 // --- pad application
 {Pad p;p.lb=p.rb=true;p.dpad_left=true;p.lx=1;Result r{};
  apply(None,ThumbrestOnly,r,p);assert(p.lb&&p.rb&&p.dpad_left);
  apply(Reading,ThumbrestOnly,r,p);assert(!p.lb&&!p.rb&&!p.dpad_left&&p.lx==1); // grips are not page buttons; consumed D-pad dropped; axes untouched
  r.page={false,true};apply(Alchemy,ThumbrestFlick,r,p);assert(!p.lb&&p.rb);
  Pad q;q.lb=true;q.lx=1;Result s{};s.page={true,false};apply(Reading,Native,s,q);assert(q.lb&&!q.rb&&q.lx==0); // C42 apply
  Pad t;t.lb=t.rb=true;apply(Alchemy,Native,s,t);assert(t.lb&&t.rb); // native alchemy: untouched
 }
 return 0;
}
