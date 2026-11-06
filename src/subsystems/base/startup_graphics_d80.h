#pragma once
#include "graphics_owner31.h"
#include "stereo_upscaler_d81r2.h"
#include <iterator>

namespace kcd2vr::startup_graphics_d80 {
// Native help and Engine.pak/Config/CVarGroups/sys_spec_Full.cfg define
// these values. This is a next-launch config writer, never a renderer hook.
struct Choice { int value; const wchar_t* label; };
inline constexpr Choice technologies[]{
    {-1,L"Keep current game setting"}, {0,L"Native (no upscaling)"},
    {2,L"NVIDIA DLSS"}, {1,L"AMD FSR"}, {4,L"Intel XeSS"}
};
inline constexpr Choice presets[]{
    {-1,L"Keep current / custom"}, {1,L"Low"}, {2,L"Medium"},
    {3,L"High"}, {4,L"Very high"}, {5,L"Ultra"}
};
inline constexpr Choice keepQuality[]{{0,L"Keep current quality"}};
inline constexpr Choice nativeQuality[]{{0,L"Native resolution"}};
inline constexpr Choice dlssQuality[]{
    {5,L"DLAA (native resolution)"}, {0,L"Automatic"}, {1,L"Quality"},
    {2,L"Balanced (optimal)"}, {3,L"Performance"}, {4,L"Ultra performance"}
};
inline constexpr Choice fsrQuality[]{
    {4,L"Native AA"}, {0,L"Quality"}, {1,L"Balanced (optimal)"},
    {2,L"Performance"}, {3,L"Ultra performance"}
};
inline constexpr Choice xessQuality[]{
    {6,L"Native AA"}, {0,L"Ultra quality plus"}, {1,L"Ultra quality"},
    {2,L"Quality"}, {3,L"Balanced (optimal)"}, {4,L"Performance"}, {5,L"Ultra performance"}
};
struct Choices { const Choice* data; std::size_t size; };
inline Choices qualities(int tech) noexcept {
    switch(tech){
    case 0:return {nativeQuality,std::size(nativeQuality)};
    case 1:return {fsrQuality,std::size(fsrQuality)};
    case 2:return {dlssQuality,std::size(dlssQuality)};
    case 4:return {xessQuality,std::size(xessQuality)};
    default:return {keepQuality,std::size(keepQuality)};
    }
}
inline int defaultQuality(int tech) noexcept {return tech==2?1:tech==4?2:0;}
inline const char* qualityKey(int tech) noexcept {
    switch(tech){
    case 1:return "r_SuperResolution_AMD_FSR_QualityMode";
    case 2:return "r_SuperResolution_NVIDIA_DLSS_QualityMode";
    case 4:return "r_SuperResolution_INTEL_XeSS_QualityMode";
    default:return nullptr;
    }
}
inline bool valid(int tech,int quality,int preset) noexcept {
    if(preset!=-1&&(preset<1||preset>5))return false;
    bool found=false;for(const auto& t:technologies)found|=t.value==tech;
    if(!found)return false;
    const auto q=qualities(tech);for(std::size_t i=0;i<q.size;++i)if(q.data[i].value==quality)return true;
    return false;
}
inline bool settings(int tech,int quality,int preset,std::vector<std::pair<std::string,int>>& out){
    out.clear();if(!valid(tech,quality,preset))return false;
    // Write the native full preset and its own groups. Explicit previous custom
    // group assignments must not silently override the selected full preset.
    if(preset>=1){
        out.emplace_back("sys_spec_Full",preset);
        for(const char* key:{"sys_spec_ObjectDetail","sys_spec_Shading","sys_spec_GlobalIllumination",
            "sys_spec_VolumetricEffects","sys_spec_Shadows","sys_spec_Texture","sys_spec_PostProcessing",
            "sys_spec_Particles","sys_spec_Light","sys_spec_Vegetation","sys_spec_Characters"})out.emplace_back(key,preset);
    }
    if(tech>=0){
        if(const auto* key=qualityKey(tech))out.emplace_back(key,quality);
        out.emplace_back("r_SuperResolution_Mode",tech);
    }
    return true;
}
// Read only literal assignments. Unknown/custom expressions remain 'Keep'.
// Last active assignment wins, just as when user.cfg is executed in order.
inline bool readInt(const std::vector<std::string>& lines,const char* key,int& value){
    bool found=false;
    for(const auto& line:lines){
        if(!graphics_owner31::userCfgKeyR27(line,key))continue;
        found=false;auto text=line.substr(line.find('=')+1);
        const auto comment=text.find_first_of(";#");if(comment!=std::string::npos)text.resize(comment);
        const auto slash=text.find("//");if(slash!=std::string::npos)text.resize(slash);
        const auto first=text.find_first_not_of(" \t\r\n"),last=text.find_last_not_of(" \t\r\n");
        if(first==std::string::npos)continue;text=text.substr(first,last-first+1);
        if(text.size()>=2&&text.front()=='\"'&&text.back()=='\"')text=text.substr(1,text.size()-2);
        try{std::size_t used=0;const int n=std::stoi(text,&used);if(used==text.size()){value=n;found=true;}}catch(...){}
    }
    return found;
}
// D81R2: Keep is a verified no-write choice, not an alias for forcing Native.
inline bool canKeepConfig(int productionMode,const std::vector<std::string>& lines){
    int actual=-1;const bool known=readInt(lines,"r_SuperResolution_Mode",actual);
    return stereo_upscaler_d81r2::canKeep(productionMode,known,actual);
}
inline bool compatible(int productionMode,int tech,const std::vector<std::string>& lines){
    return tech==-1 ? canKeepConfig(productionMode,lines)
        : stereo_upscaler_d81r2::canSelect(productionMode,tech);
}
inline bool settingsForProduction(int productionMode,int tech,int quality,int preset,
    const std::vector<std::string>& lines,std::vector<std::pair<std::string,int>>& out){
    out.clear();return compatible(productionMode,tech,lines)&&settings(tech,quality,preset,out);
}
inline Choices technologiesForProduction(int productionMode,bool keepVerified) noexcept {
    static constexpr Choice stereoChoices[]{{0,L"Native (no upscaling)"},{2,L"NVIDIA DLSS (D82 stereo candidate)"},{1,L"AMD FSR (D82 stereo candidate)"},{4,L"Intel XeSS (D85 stereo candidate)"}};
    static constexpr Choice stereoAndKeep[]{{-1,L"Keep current supported setting"},{0,L"Native (no upscaling)"},{2,L"NVIDIA DLSS (D82 stereo candidate)"},{1,L"AMD FSR (D82 stereo candidate)"},{4,L"Intel XeSS (D85 stereo candidate)"}};
    if(!stereo_upscaler_d81r2::stereoPolicy(productionMode))return {technologies,std::size(technologies)};
    return keepVerified ? Choices{stereoAndKeep,std::size(stereoAndKeep)} : Choices{stereoChoices,std::size(stereoChoices)};
}

}
