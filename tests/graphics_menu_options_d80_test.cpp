#include "graphics_menu_options.h"
#include "graphics_owner31.h"
#include <cassert>
#include <string>
#include <vector>
int main(){
    namespace g=kcd2vr::graphics_menu_options;
    constexpr int choices[]{0,1,2,4};int tech=0;
    for(int expected:{1,2,4,0}){tech=g::Next(tech,1);assert(tech==expected);}
    for(int expected:{4,2,1,0}){tech=g::Next(tech,-1);assert(tech==expected);}
    assert(!g::Supported(3)&&!g::Supported(-1)&&!g::Supported(5));
    assert(g::Next(-1,1)==0&&g::Next(-1,-1)==4);
    assert(g::DefaultQuality(4)==2&&g::QualityCount(4)==7);
    assert(g::DefaultQuality(2)==1&&g::QualityCount(2)==6);
    assert(g::DefaultQuality(1)==0&&g::QualityCount(1)==5);
    assert(std::string(g::QualityKey(4))=="r_SuperResolution_INTEL_XeSS_QualityMode");
    assert(g::QualityKey(0)==nullptr);
    for(int t:choices)for(int q=0;q<g::QualityCount(t);++q){
        std::vector<std::pair<std::string,int>> values;
        assert(kcd2vr::graphics_owner31::upscalerSettingsR27({t,q,0},values));
        assert(values.back().first=="r_SuperResolution_Mode"&&values.back().second==t);
        if(t){bool found=false;for(const auto& v:values)if(v.first==g::QualityKey(t)){assert(v.second==q);found=true;}assert(found);}
    }
}
