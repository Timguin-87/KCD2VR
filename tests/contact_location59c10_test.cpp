#include "../src/vrik/contact_location59c10.h"
#include <array>
#include <cassert>
#include <cstring>
int main(){
    std::array<unsigned char,0xE0> details{};details.fill(0xAB);
    for(int material:{0,1,3,9,28,38,32767}){
        auto before=details;
        assert(kcd2vr::contact_location59c10::writeOriginalMaterial(details.data(),material));
        std::int32_t copied=-1;std::memcpy(&copied,details.data()+0x68,4);assert(copied==material);
        for(unsigned i=0;i<details.size();++i)if(i<0x68||i>=0x6C)assert(details[i]==before[i]);
    }
    for(int invalid:{-1,-2,32768,2147483647}){
        const auto before=details;
        assert(!kcd2vr::contact_location59c10::writeOriginalMaterial(details.data(),invalid));
        assert(details==before);
    }
    assert(!kcd2vr::contact_location59c10::writeOriginalMaterial(nullptr,3));
}
