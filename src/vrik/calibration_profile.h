#pragma once
#include "finger_capture.h"
#include "tracking_origin125.h"
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>
namespace kcd2vr::calibration_profile {
namespace pc=pose_core;namespace fc=finger_capture;
struct Arms {
 double span=0,leftReach=0,rightReach=0,headToHandPlane=0;
 // Relative to the head, in the calibration's horizontal model frame. These
 // are anthropometry, not saved room coordinates or controller grip offsets.
 pc::Vec3 leftOffset{},rightOffset{};bool measuredOffsets=false;
};
inline bool valid(const Arms&a) noexcept {
 return std::isfinite(a.span)&&std::isfinite(a.leftReach)&&std::isfinite(a.rightReach)&&std::isfinite(a.headToHandPlane)&&a.span>.65&&a.span<2.20&&a.leftReach>.30&&a.leftReach<1.30&&a.rightReach>.30&&a.rightReach<1.30&&std::abs(a.leftReach-a.rightReach)<.28&&a.headToHandPlane>-.1&&a.headToHandPlane<.65&&
  (!a.measuredOffsets||(pc::finite(a.leftOffset)&&pc::finite(a.rightOffset)&&
   std::abs(pc::length(a.leftOffset)-a.leftReach)<.001&&std::abs(pc::length(a.rightOffset)-a.rightReach)<.001&&
   std::abs(pc::length(a.rightOffset-a.leftOffset)-a.span)<.001&&
   std::abs((a.leftOffset.z+a.rightOffset.z)*.5+a.headToHandPlane)<.001));
}
inline bool tpose(pc::Vec3 h,pc::Vec3 l,pc::Vec3 r) noexcept {
 Arms a{pc::length(r-l),pc::length(l-h),pc::length(r-h),h.y-(l.y+r.y)*.5};
 return pc::finite(h)&&pc::finite(l)&&pc::finite(r)&&valid(a)&&std::abs(l.y-r.y)<.20&&pc::length((l+r)*.5-h)<.70;
}
struct Window {
 std::uint64_t start=0,last=0;unsigned count=0;pc::Vec3 h{},l{},r{},firstH{},firstL{},firstR{};bool moved=false;
 void add(pc::Vec3 head,pc::Vec3 left,pc::Vec3 right,std::uint64_t tick) noexcept {
  if(!tpose(head,left,right)) {moved=true;return;}
  if(!count){start=tick;firstH=head;firstL=left;firstR=right;}
  if(tick<last||(count&&tick-last>200)||pc::length(head-firstH)>.08||pc::length(left-firstL)>.12||pc::length(right-firstR)>.12)moved=true;
  h=h+head;l=l+left;r=r+right;last=tick;++count;
 }
 bool measure(Arms&a,pc::Quat trackingYaw={}) const noexcept {
  if(moved||count<20||last<start||last-start<1000)return false;
  const auto mh=h*(1./count),ml=l*(1./count),mr=r*(1./count);
  a={pc::length(mr-ml),pc::length(ml-mh),pc::length(mr-mh),mh.y-(ml.y+mr.y)*.5};
  a.leftOffset=tracking125::model_vector(ml-mh,trackingYaw);
  a.rightOffset=tracking125::model_vector(mr-mh,trackingYaw);a.measuredOffsets=true;
  return valid(a);
 }
};
inline std::uint64_t hash(const std::string&s) noexcept {std::uint64_t h=1469598103934665603ull;for(unsigned char c:s){h^=c;h*=1099511628211ull;}return h;}
inline std::string seal(std::string s) {while(!s.empty()&&s.back()==' ')s.pop_back();std::ostringstream out;out.imbue(std::locale::classic());out<<std::hex<<hash(s)<<' '<<s;return out.str();}
inline bool unseal(const std::string&s,std::string&body) {const auto pos=s.find(' ');if(pos==std::string::npos)return false;std::istringstream in(s.substr(0,pos));in.imbue(std::locale::classic());std::uint64_t sum=0;if(!(in>>std::hex>>sum))return false;in>>std::ws;if(!in.eof())return false;body=s.substr(pos+1);return hash(body)==sum;}
inline std::string encode(const Arms&a) {std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(17)<<(a.measuredOffsets?"ARMS2 ":"ARMS1 ")<<a.span<<' '<<a.leftReach<<' '<<a.rightReach<<' '<<a.headToHandPlane;if(a.measuredOffsets)for(const auto v:{a.leftOffset,a.rightOffset})s<<' '<<v.x<<' '<<v.y<<' '<<v.z;return seal(s.str());}
inline bool decode(const std::string&s,Arms&a) {std::string body,tag;if(!unseal(s,body))return false;std::istringstream in(body);in.imbue(std::locale::classic());Arms p{};if(!(in>>tag>>p.span>>p.leftReach>>p.rightReach>>p.headToHandPlane)||(tag!="ARMS1"&&tag!="ARMS2"))return false;if(tag=="ARMS2"){if(!(in>>p.leftOffset.x>>p.leftOffset.y>>p.leftOffset.z>>p.rightOffset.x>>p.rightOffset.y>>p.rightOffset.z))return false;p.measuredOffsets=true;}if(!valid(p))return false;in>>std::ws;if(!in.eof())return false;a=p;return true;}
inline void write_hand(std::ostream&s,const fc::Hand&h) {s<<h.count<<' ';for(double b:h.bend)s<<b<<' ';for(int i=0;i<h.count;++i){const auto&j=h.joints[i];s<<j.id<<' '<<j.parent<<' '<<j.digit<<' '<<j.crc<<' '<<j.q.x<<' '<<j.q.y<<' '<<j.q.z<<' '<<j.q.w<<' '<<j.t.x<<' '<<j.t.y<<' '<<j.t.z<<' ';}}
inline bool read_hand(std::istream&s,fc::Hand&h) {if(!(s>>h.count)||h.count<10||h.count>fc::MaxJoints)return false;for(double&b:h.bend)if(!(s>>b))return false;for(int i=0;i<h.count;++i){auto&j=h.joints[i];if(!(s>>j.id>>j.parent>>j.digit>>j.crc>>j.q.x>>j.q.y>>j.q.z>>j.q.w>>j.t.x>>j.t.y>>j.t.z))return false;}return fc::valid(h);}
inline std::string encode(const fc::Pair&p) {std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(9)<<"FINGERS1 "<<p.rig<<' ';for(int i=0;i<2;++i){write_hand(s,p.open[i]);write_hand(s,p.closed[i]);}return seal(s.str());}
inline bool decode(const std::string&s,fc::Pair&p) {std::string body,tag;if(!unseal(s,body))return false;std::istringstream in(body);in.imbue(std::locale::classic());fc::Pair candidate{};if(!(in>>tag>>candidate.rig)||tag!="FINGERS1")return false;for(int i=0;i<2;++i)if(!read_hand(in,candidate.open[i])||!read_hand(in,candidate.closed[i]))return false;in>>std::ws;if(!in.eof()||!fc::valid(candidate))return false;p=candidate;return true;}
// Use one single-key payload per slot. The active pointer changes only AFTER
// successful write/flush/readback. An interrupted replacement preserves the old slot.
template<class Read,class Write,class Flush> bool save_bank(const std::string&payload,Read read,Write write,Flush flush) {
 const std::string active=read("Active");const char* slot=active=="A"?"B":"A";
 if(!write(slot,payload)||!flush()||read(slot)!=payload)return false;
 return write("Active",slot)&&flush()&&read("Active")==slot;
}
template<class T,class Read> bool load_bank(T&out,Read read) {
 const std::string active=read("Active");
 if(active!="A"&&active!="B")return false;
 if(decode(read(active.c_str()),out))return true;
 return decode(read(active=="A"?"B":"A"),out);
}
}
