// Executes the extracted production publisher. Native identity and mailboxes are explicit leaves.
#include <cassert>
#include <cstdint>
#include <iostream>
namespace xcXR34 {
struct Source { unsigned owner=1; std::uint64_t tick=100; bool allowed=true; };
enum class Phase { None, Retiring };
struct Owner { Source source; };
struct Status { Owner owner; Phase phase=Phase::None; bool clockOwned=true; };
inline Source oldSource, published;
inline Status active;
inline bool mailbox=true;
inline unsigned writes=0;
void RefreshRegistration() {}
template<class T> void SetSpatialReader(T) {}
bool ReadStatus(Status& out) {out=active;return true;}
bool ReadSource(Source& out) {out=oldSource;return mailbox&&oldSource.allowed;}
bool fresh(const Source& s,std::uint64_t now) {return s.allowed&&now>=s.tick&&now-s.tick<=80;}
bool sameSource(const Source& a,const Source& b) {return a.owner==b.owner;}
void PublishSource(const Source& s) {published=s;++writes;}
}
static std::uint64_t now=110, before=2, after=2;
static unsigned reads=0;
static bool available=false,enabled=true,identity=true;
static void RangedReloadEquipmentXR34() {}
static void RangedSpatialXR34() {}
static std::uint64_t ReadCompletedPublicationT23() {return reads++?after:before;}
static bool RangedSourceXR34(xcXR34::Source& s) {s=xcXR34::Source{};s.allowed=available;return available;}
static std::uint64_t GetTickCount64() {return now;}
static bool CrossbowReloadEnabledD65C() {return enabled;}
static bool ReloadNativeOwnerXR34(const xcXR34::Owner&,bool) {return identity;}
#include "crossbow_publisher_actual.inc"
static void reset(){now=110;before=after=2;reads=0;available=false;enabled=identity=true;
 xcXR34::oldSource={};xcXR34::active={};xcXR34::mailbox=true;xcXR34::writes=0;}
int main(){
 reset();before=after=3;RangedPublishXR34();assert(xcXR34::writes==0&&xcXR34::oldSource.tick==100);
 reset();before=2;after=4;RangedPublishXR34();assert(xcXR34::writes==0);
 reset();before=after=3;now=181;RangedPublishXR34();assert(xcXR34::writes==1&&!xcXR34::published.allowed);
 reset();before=after=3;identity=false;RangedPublishXR34();assert(xcXR34::writes==1);
 reset();before=after=3;enabled=false;RangedPublishXR34();assert(xcXR34::writes==1);
 reset();before=after=3;xcXR34::oldSource.owner=2;RangedPublishXR34();assert(xcXR34::writes==1);
 reset();before=after=3;xcXR34::active.phase=xcXR34::Phase::Retiring;RangedPublishXR34();assert(xcXR34::writes==1);
 reset();before=after=3;xcXR34::active.clockOwned=false;RangedPublishXR34();assert(xcXR34::writes==1);
 reset();before=after=3;xcXR34::mailbox=false;RangedPublishXR34();assert(xcXR34::writes==1);
 reset();RangedPublishXR34();assert(xcXR34::writes==1&&!xcXR34::published.allowed);
 reset();available=true;RangedPublishXR34();assert(xcXR34::writes==1&&xcXR34::published.allowed);
 std::cout<<"Crossbow publication: 11 transient/loss/recovery cases passed; native context/mailbox leaves mocked\n";
}
