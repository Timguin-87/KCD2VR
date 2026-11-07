#include "subsystems/doors/hand_only_h40.h"
#include <cassert>
#include <cstdio>
#include <string>
int main(){
    namespace d=kcd2vr::hand_only_door_h40;
    d::Frame f{};f.player=19;f.body=22;f.request=33;f.tick=1000;
    f.point[0][0]=.1f;f.point[1][0]=.2f;
    const float grip[2]{.8f,.9f};const std::uint64_t claim[2]{41,42};
    const std::uint32_t offered[2]{101,202};char packet[512]{};
    assert(d::Packet(f,1,"feed",grip,packet,sizeof(packet),claim,offered));
    assert(std::string(packet).ends_with("|41|42|101|202"));
    std::printf("OFFERED %s\n",packet);
    assert(d::Packet(f,2,"feed",grip,packet,sizeof(packet),claim));
    assert(std::string(packet).ends_with("|41|42|0|0"));
    std::printf("UNSPECIFIED %s\n",packet);
    const float released[2]{-1,-1};
    assert(d::Packet(f,3,"clear",released,packet,sizeof(packet)));
    std::printf("CLEAR %s\n",packet);
    char shortPacket[10]{};assert(!d::Packet(f,3,"clear",released,shortPacket,sizeof(shortPacket)));
    d::claims[0].store(41);d::entered[0].store(40);d::claims[1].store(42);d::entered[1].store(42);
    assert(d::Reserved()==3&&d::Entered()==2);
    d::entered[0].store(41);assert(d::Entered()==3);
    d::claims[0].store(0);assert(d::Entered()==2);
    std::puts("PASS D48 actual packet serializer, bounded output and same-token native entry admission");
}
