#include "../src/vrik/calibration_profile.h"
#include <cstdio>
#include <map>
#include <limits>
using namespace kcd2vr::calibration_profile;
static int tests=0,failed=0;static void Check(bool b,const char*n){++tests;failed+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static fc::Hand hand(int side,double curl){fc::Hand h;h.count=15;for(int d=0;d<5;++d){h.bend[d]=curl;for(int k=0;k<3;++k){int i=d*3+k;h.joints[i]={100+side*100+i,k?99+side*100+i:side+1,d,unsigned(i+1+side*100),{0,0,std::sin(curl*.25),std::cos(curl*.25)},{.03,0,0}};}}return h;}
struct Bank{std::map<std::string,std::string> m;std::string failedKey;bool truncate=false;int flushes=0;
 std::string read(const char*k){return m[k];}
 bool save(const std::string&s){return save_bank(s,[&](const char*k){return read(k);},[&](const char*k,const std::string&v){if(failedKey==k)return false;m[k]=truncate?v.substr(0,v.size()/2):v;return true;},[&](){++flushes;return true;});}
 template<class T>bool load(T&v){return load_bank(v,[&](const char*k){return read(k);});}};
int main(){
 Arms a{1.45,.76,.77,.22},out;Check(valid(a),"plausible saved controller reach accepted");Check(decode(encode(a),out)&&std::abs(out.span-a.span)<1e-12,"arm profile roundtrip");
 auto broken=encode(a);broken.back()^=1;Check(!decode(broken,out),"modified arm payload checksum rejected");
 Check(!decode(seal("ARMS2 1.45 .76 .77 .22"),out),"unknown arm recipe rejected");Check(!decode(seal("ARMS1 nan .76 .77 .22"),out),"NaN arm reach rejected");Check(!decode(seal("ARMS1 1.45 .76 .77 .22 extra"),out),"trailing garbage rejected");
 Check(!decode(seal("ARMS1 100 .76 .77 .22"),out),"unreasonable reach rejected");
 Window w;for(unsigned n=0;n<101;++n)w.add({0,1.7,0},{-.72,1.48,0},{.72,1.48,0},1000+n*20);Check(w.measure(out)&&std::abs(out.span-1.44)<1e-9,"two-second stable T pose measures span");
 Check(out.measuredOffsets&&std::abs(out.rightOffset.x-.72)<1e-9&&std::abs(out.rightOffset.z+.22)<1e-9,"T-pose retains actual shoulder-relative geometry");
 Arms measured;Check(decode(encode(out),measured)&&measured.measuredOffsets&&pc::length(measured.rightOffset-out.rightOffset)<1e-9,"ARMS2 measured geometry survives persistence");
 measured.rightOffset.x+=.1;Check(!decode(encode(measured),out),"inconsistent measured geometry cannot change saved reach");
 Window tiny;for(unsigned n=0;n<5;++n)tiny.add({0,1.7,0},{-.72,1.48,0},{.72,1.48,0},1000+n*20);Check(!tiny.measure(out),"too few samples never save calibration");
 Window fast;for(unsigned n=0;n<30;++n)fast.add({0,1.7,0},{-.72,1.48,0},{.72,1.48,0},1000+n);Check(!fast.measure(out),"many samples in short time do not satisfy hold duration");
 auto moved=w;moved.add({.2,1.7,0},{-.72,1.48,0},{.72,1.48,0},3030);Check(!moved.measure(out),"head movement invalidates T pose window");
 auto lost=w;lost.add({0,1.7,0},{-.72,1.48,0},{.72,1.48,0},3500);Check(!lost.measure(out),"tracking sample gap invalidates arm save");
 auto arm=w;arm.add({0,1.7,0},{-.45,1.48,0},{.72,1.48,0},3030);Check(!arm.measure(out),"arm motion invalidates window");
 Check(!tpose({0,1.7,0},{-.1,1.48,0},{.1,1.48,0}),"hands near torso are not T pose");Check(!tpose({0,1.7,0},{-.7,1.48,0},{.7,.9,0}),"unequal hand heights rejected");
 auto invalid=w;invalid.add({0,std::numeric_limits<double>::quiet_NaN(),0},{-.7,1.48,0},{.7,1.48,0},3030);Check(!invalid.measure(out),"invalid position does not preserve apparent successful average");
 fc::Pair pair;pair.rig=123;pair.open={hand(0,0),hand(1,0)};pair.closed={hand(0,2),hand(1,1.7)};fc::Pair copy;
 auto text=encode(pair);Check(decode(text,copy)&&copy.rig==pair.rig&&fc::change(copy.closed[0],pair.closed[0])<1e-6,"complete hand pair roundtrip independent native rotations and offsets");
 Check(text.size()<30000&&text.back()!=' ',"payload fits INI buffer without trimmed trailing space");
 text[100]^=1;Check(!decode(text,copy),"hand payload corruption rejected");Check(!decode(seal("FINGERS1 123 99"),copy),"malformed count cannot overflow arrays");
 auto mismatch=pair;mismatch.closed[1].joints[0].crc++;Check(!decode(encode(mismatch),copy),"mismatched open/fist identity rejected on load");
 Bank b;Check(!b.load(out),"no active record is not a fabricated calibration");Check(b.save(encode(a))&&b.load(out)&&out.span==a.span,"first calibration commits verified inactive bank");
 Arms second=a;second.span=1.55;Check(b.save(encode(second))&&b.read("Active")=="B"&&b.load(out)&&out.span==second.span,"next save swaps to other bank");
 b.m["B"]="damaged";Check(b.load(out)&&out.span==a.span,"corrupt newest slot falls back to valid previous profile");
 Bank failure;failure.save(encode(a));failure.failedKey="B";Check(!failure.save(encode(second))&&failure.load(out)&&out.span==a.span,"payload write failure leaves old active bank");
 failure.failedKey="Active";Check(!failure.save(encode(second))&&failure.load(out)&&out.span==a.span,"pointer write failure preserves previous active profile");
 Bank truncated;truncated.save(encode(a));truncated.truncate=true;Check(!truncated.save(encode(second))&&truncated.load(out)&&out.span==a.span,"truncated payload fails readback before pointer update");
 Bank fingers;Check(fingers.save(encode(pair))&&fingers.load(copy)&&fc::valid(copy),"finger bank persists separately from anthropometry");
 Check(fingers.read("Active")=="A"&&b.read("Active")=="B","independent bank selectors remain independent");
 std::printf("calibration_profile=%d/%d\n",tests-failed,tests);return failed?1:0;
}
