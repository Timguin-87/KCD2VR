#pragma once
#include "native_door_script19c4r4.h"
#include <cstdint>
#include <cstring>
namespace kcd2vr::door_curve_signal66 {
struct Query {door_script19c4r4::CleanupIdentity lease{};std::uint32_t source=0,micro=0;char clip[81]{};};
inline bool number(const char*& p,char separator,std::uint32_t limit,std::uint32_t& value) noexcept {
 value=0;unsigned count=0;if(!p||*p<'0'||*p>'9')return false;
 while(*p>='0'&&*p<='9'){const auto d=unsigned(*p-'0');if(++count>8||value>limit/10||(value==limit/10&&d>limit%10))return false;value=value*10+unsigned(*p++-'0');}
 if(*p!=separator)return false;if(separator)++p;return true;
}
// Alias is copied, bounded and ASCII. Nothing from Lua is a native pointer.
inline bool parse(const char* name,Query& q) noexcept {
 q={};constexpr char prefix[]="kcd2vr_door_curve_";
 if(!name||std::strncmp(name,prefix,sizeof(prefix)-1))return false;
 const char* p=name+sizeof(prefix)-1;
 if(!number(p,'_',8388606,q.source)||q.source<2||(q.source&1)||!number(p,'_',8388607,q.lease.request)||!q.lease.request)return false;
 const char* begin=p;const char* last=nullptr;unsigned count=0;
 for(;*p&&count<89;++p,++count){const unsigned char c=*p;if(c=='_')last=p;else if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'))return false;}
 if(*p||!last||last==begin||last-begin>80)return false;
 const auto n=std::size_t(last-begin);std::memcpy(q.clip,begin,n);q.clip[n]=0;p=last+1;
 return number(p,0,999500,q.micro);
}
using Handler=std::uint32_t(*)(const Query&,std::uint64_t) noexcept; //0 refused,1 ready,2 bounded initial pose wait
}
