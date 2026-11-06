#pragma once
#include "held_map_projection_t2.h"
#include <array>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string_view>
namespace kcd2vr::held_map_feed_t2 {
enum class Kind : std::uint8_t {Quest=1,Waypoint=2,Poi=3};
struct Marker {float x=0,y=0,z=0;int nativeType=0;std::uint64_t id=0;Kind kind=Kind::Poi;int state=0,tracking=-1,objective=-1;};
struct Snapshot {
    std::array<Marker,96> markers{};unsigned count=0;
    std::uint64_t tick=0,activation=0,world=0,vmGeneration=0;
    Region region=Region::Unknown;
    float x=0,y=0,z=0,yaw=0,waypointX=0,waypointY=0;
    bool valid=false,playerKnown=false,compassKnown=false,waypointKnown=false;
    // Exact native compass entries include tracked objectives/givers/POIs;
    // this does not claim every undiscovered/off-compass native map mark.
    bool trackedCompassOnly=true;
};
struct Span {std::uintptr_t begin=0,end=0;};
struct NativeSnapshot {std::array<Marker,96> markers{};unsigned count=0;std::uintptr_t apse=0;bool compassKnown=false;};
inline bool Coordinate(float f) noexcept {return std::isfinite(f)&&f>-20000&&f<20000;}
inline bool Count(Span s,unsigned stride,unsigned cap,unsigned& n) noexcept {
    if(!stride||s.end<s.begin||(s.end-s.begin)%stride||(s.end-s.begin)/stride>cap)return false;
    n=unsigned((s.end-s.begin)/stride);return !n||s.begin>=0x10000;
}
inline bool PoiType(int type) noexcept {
    return (type>=4&&type<=6)||type==9||(type>=69&&type<=71)||type==78||type==79||
        (type>=81&&type<=85)||(type>=87&&type<=94);
}
template<class Read> bool Capture(Read& read,std::uintptr_t base,NativeSnapshot& out) noexcept {
    out={};std::uintptr_t framework=0,ui=0;
    if(!read(base+0x549D388,&framework,8)||!framework||!read(framework+0xE8,&ui,8)||!ui)return false;
    Span drivers{};unsigned n=0;
    if(!read(ui+0x40,&drivers,sizeof(drivers))||!Count(drivers,16,300,n))return false;
    std::uintptr_t compass=0;
    for(unsigned i=0;i<n;++i){std::uintptr_t object=0,vt=0;
        if(!read(drivers.begin+i*16,&object,8))return false;
        if(!object)continue; // native registry can contain legitimate null entries.
        if(!read(object,&vt,8))return false;
        if(vt==base+0x3C118B8){if(compass)return false;compass=object;}
        if(vt==base+0x3ED9140){if(out.apse)return false;out.apse=object;}
    }
    Span marks{};unsigned count=0;
    if(compass&&(!read(compass+0x60,&marks,sizeof(marks))||!Count(marks,16,4096,count)))return false;
    std::array<Marker,32> poi{};unsigned poiCount=0;
    for(unsigned i=0;i<count;++i){
        std::uintptr_t object=0;if(!read(marks.begin+i*16,&object,8))return false;if(!object)continue;
        std::array<unsigned char,0x78> b{};if(!read(object,b.data(),b.size()))return false;
        std::uintptr_t vt=0,dataVt=0;std::memcpy(&vt,b.data(),8);std::memcpy(&dataVt,b.data()+0x10,8);
        if(vt!=base+0x3A6B4C8||dataVt!=base+0x3A6B4F0)continue;
        Marker m{};std::memcpy(&m.nativeType,b.data()+0x4C,4);std::memcpy(&m.x,b.data()+0x50,4);
        std::memcpy(&m.y,b.data()+0x54,4);std::memcpy(&m.z,b.data()+0x58,4);m.id=object;
        if(!Coordinate(m.x)||!Coordinate(m.y)||!Coordinate(m.z))continue;
        // Exact AddCompassMarker C4D2E8: +68 tracking colour, +6C objective,
        // +74 discovered state. These are cached native semantics, never inferred
        // from the static catalog or from transient pointer identity.
        std::memcpy(&m.tracking,b.data()+0x68,4);std::memcpy(&m.objective,b.data()+0x6C,4);
        std::memcpy(&m.state,b.data()+0x74,4);
        if((m.nativeType>=1&&m.nativeType<=3)||m.nativeType==86){m.kind=Kind::Quest;if(out.count<out.markers.size())out.markers[out.count++]=m;}
        else if(PoiType(m.nativeType)&&poiCount<poi.size())poi[poiCount++]=m;
    }
    for(unsigned i=0;i<poiCount&&out.count<out.markers.size();++i)out.markers[out.count++]=poi[i];
    Span after{},driverAfter{};std::uintptr_t current=0,currentUi=0,vt=0;
    const bool registryKnown=read(base+0x549D388,&current,8)&&current==framework&&
        read(framework+0xE8,&currentUi,8)&&currentUi==ui&&
        read(ui+0x40,&driverAfter,sizeof(driverAfter))&&driverAfter.begin==drivers.begin&&driverAfter.end==drivers.end;
    if(!registryKnown)return false;
    out.compassKnown=compass&&read(compass,&vt,8)&&vt==base+0x3C118B8&&
        read(compass+0x60,&after,sizeof(after))&&after.begin==marks.begin&&after.end==marks.end;
    if(compass&&!out.compassKnown)return false;
    if(out.apse&&(!read(out.apse,&vt,8)||vt!=base+0x3ED9140))return false;
    return true; // current APSE/player path does not depend on compass presence.
}
inline bool Fresh(const Snapshot& s,std::uint64_t now,std::uint64_t activation) noexcept {
    return s.valid&&s.playerKnown&&s.region!=Region::Unknown&&s.activation==activation&&s.tick&&now>=s.tick&&now-s.tick<=1500;
}
// Own Lua callback metadata. Strict bounded fields, finite numbers, no stale
// previous-level fallback or native pointers sent to the renderer.
inline bool PlayerPacket(std::string_view packet,Snapshot& out) noexcept {
    std::array<std::string_view,8> fields{};unsigned n=0;std::size_t start=0;
    if(packet.size()>1024)return false;
    for(std::size_t i=0;i<=packet.size();++i)if(i==packet.size()||packet[i]=='|'){
        if(n>=fields.size())return false;fields[n++]=packet.substr(start,i-start);start=i+1;
    }
    if(n!=8||fields[0]!="T2"||fields[1].empty()||fields[1].size()>64)return false;
    std::uint64_t world=1469598103934665603ull;
    for(unsigned char c:fields[1]){if(c<32||c>126||c=='|')return false;world=(world^c)*1099511628211ull;}
    Region region=Region::Unknown;
    if(fields[2]=="trosecko")region=Region::Trosky;else if(fields[2]=="kutnohorsko")region=Region::Kuttenberg;else if(fields[2]=="klaster")region=Region::Monastery;
    if(region==Region::Unknown)return false;
    float values[4]{};
    for(unsigned i=0;i<4;++i){const auto s=fields[i+3];const auto parsed=std::from_chars(s.data(),s.data()+s.size(),values[i]);
        if(s.empty()||parsed.ec!=std::errc{}||parsed.ptr!=s.data()+s.size()||!std::isfinite(values[i]))return false;}
    // Eighth field is an explicit world generation, not silently ignored data.
    std::uint64_t generation=0;const auto gen=fields[7];const auto parsed=std::from_chars(gen.data(),gen.data()+gen.size(),generation);
    if(gen.empty()||parsed.ec!=std::errc{}||parsed.ptr!=gen.data()+gen.size()||!generation||
       !Coordinate(values[0])||!Coordinate(values[1])||!Coordinate(values[2])||std::abs(values[3])>100000)return false;
    out.world=world^(generation*0x9E3779B97F4A7C15ull);out.region=region;out.x=values[0];out.y=values[1];out.z=values[2];out.yaw=values[3];out.playerKnown=true;return true;
}
}
