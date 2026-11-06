#include "../src/subsystems/base/startup_setup_s15.inl"
#include <cassert>
#include <iostream>
using namespace kcd2vr::startup_s15;
static std::string bytes(const fs::path& p){std::ifstream in(p,std::ios::binary);return {std::istreambuf_iterator<char>(in),{}};}
static void put(const fs::path& p,const std::string& text){std::ofstream out(p,std::ios::binary);out<<text;}
int main(){
 wchar_t temp[MAX_PATH]{};GetTempPathW(MAX_PATH,temp);
 const auto root=fs::path(temp)/(L"KCD2VR_LAUNCH_OPTIONS_"+std::to_wstring(GetCurrentProcessId()));
 fs::create_directory(root);struct Clean{fs::path path;~Clean(){fs::remove_all(path);}} clean{root};
 State s{};s.root=root;s.ini=root/L"KCD2VR.ini";
 const std::string ini="; preserve this comment\r\n[Settings]\r\nVrStereoExperiment=0\r\nVrGameFov=101\r\nVrDeveloperMode=false\r\nCustomSetting=42\r\n[Calibration]\r\nHeight=1.83\r\n";
 put(s.ini,ini);for(const auto& f:fields)s.values.push_back(Read(s.ini,f));
 const auto cfg=root/L"user.cfg";
 put(cfg,"; r_SuperResolution_Mode = 0\r\nr_SuperResolution_Mode=1\r\ncustom_key=7\r\nsys_spec_Shading=1\r\n");
 s.hasLaunchOptions=true;s.upscaler=4;s.quality=6;s.graphics=5;s.developer=true;s.balanced=false;
 assert(SaveSource(s,true));const auto graphics=bytes(cfg);
 assert(graphics.find("r_SuperResolution_INTEL_XeSS_QualityMode = 6")!=std::string::npos);
 assert(graphics.find("r_SuperResolution_Mode = 4")!=std::string::npos);
 assert(graphics.find("sys_spec_Shading = 5")!=std::string::npos&&graphics.find("sys_spec_Shading=1")==std::string::npos);
 assert(graphics.find("custom_key=7")!=std::string::npos&&graphics.find("; r_SuperResolution_Mode = 0")!=std::string::npos);
 assert(SaveIni(s,true)&&bytes(s.ini)==ini); // only next-start profile while game is running
 assert(WritePrivateProfileStringW(L"Calibration",L"NewFineTune",L"0.123",s.ini.c_str()));
 assert(ApplyPending(s.ini));
 const auto merged=bytes(s.ini);assert(merged.find("Height=1.83")!=std::string::npos&&merged.find("NewFineTune=0.123")!=std::string::npos);
 assert(merged.find("CustomSetting=42")!=std::string::npos&&merged.find("preserve this comment")!=std::string::npos);
 assert(ReadBool(s.ini,L"VrDeveloperMode",false)&&!ReadBool(s.ini,L"EnableVrBalancedGraphics",true));

 // A later VR-menu save wins over the launcher's old pending graphics mirror.
 s.upscaler=1;s.quality=1;assert(SaveIni(s,true));
 put(cfg,"r_SuperResolution_Mode=2\r\nr_SuperResolution_NVIDIA_DLSS_QualityMode=5\r\n");
 State reopened{};reopened.root=root;reopened.ini=s.ini;
 LoadLaunchOptions(reopened,PendingPath(s.ini),true);
 assert(reopened.developer&&reopened.upscaler==2&&reopened.quality==5);
 put(cfg,"sys_spec_Full=3\r\nsys_spec_Shading=custom_value\r\n");
 LoadLaunchOptions(reopened,s.ini,false);assert(reopened.graphics==-1);
 put(cfg,"r_SuperResolution_Mode=3\r\n");LoadLaunchOptions(reopened,s.ini,false);
 assert(reopened.upscaler==-1); // Unsupported engine modes remain unchanged, never mislabelled OFF.

 // Keep ignores edited launch controls; malformed choices never touch config.
 const auto before=bytes(cfg);s.upscaler=99;s.quality=99;
 assert(!SaveSource(s,true)&&bytes(cfg)==before);assert(SaveSource(s,false));
 assert(bytes(cfg).find("r_SuperResolution_Mode=3")!=std::string::npos);
 s.developer=false;assert(SaveIni(s,false)&&ApplyPending(s.ini));assert(ReadBool(s.ini,L"VrDeveloperMode",false));

 namespace g=kcd2vr::startup_graphics_d80;
 std::vector<std::pair<std::string,int>> values;
 assert(g::settings(2,5,-1,values)&&values.front()==std::make_pair(std::string("r_SuperResolution_NVIDIA_DLSS_QualityMode"),5));
 assert(g::settings(1,0,-1,values)&&values.front()==std::make_pair(std::string("r_SuperResolution_AMD_FSR_QualityMode"),0));
 assert(g::settings(4,2,-1,values)&&values.front()==std::make_pair(std::string("r_SuperResolution_INTEL_XeSS_QualityMode"),2));
 assert(g::settings(-1,0,-1,values)&&values.empty());
 assert(!g::settings(4,7,-1,values)&&!g::settings(3,0,-1,values)&&!g::settings(2,1,6,values));
 // Supported per-eye DLSS may be selected and kept in the stereo profile.
 fs::create_directory(root/L"Data");fs::create_directory(root/L"Bin");
 assert(WritePrivateProfileStringW(L"Settings",L"VrStereoExperiment",L"3",s.ini.c_str()));
 s.hasLaunchOptions=true;s.upscaler=2;s.quality=5;s.graphics=-1;
 put(cfg,"r_SuperResolution_Mode=2\r\nr_AntialiasingMode=3\r\ncustom_key=7\r\n");
 const auto unchangedIni=bytes(s.ini);
 assert(SaveSource(s,true));
 assert(bytes(cfg).find("r_SuperResolution_Mode = 2")!=std::string::npos);
 assert(bytes(cfg).find("r_SuperResolution_NVIDIA_DLSS_QualityMode = 5")!=std::string::npos);
 assert(bytes(cfg).find("r_AntialiasingMode=3")!=std::string::npos);
 assert(bytes(cfg).find("custom_key=7")!=std::string::npos);
 const auto supported=bytes(cfg);
 assert(SaveSource(s,false)&&bytes(cfg)==supported);
 assert(SaveIni(s,true)&&bytes(s.ini)==unchangedIni);
 assert(SaveIni(s,false)&&bytes(s.ini)==unchangedIni);
 s.upscaler=0;s.quality=0;assert(SaveSource(s,true));
 assert(bytes(cfg).find("r_SuperResolution_Mode = 0")!=std::string::npos);
 assert(bytes(cfg).find("r_AntialiasingMode=3")!=std::string::npos);
 assert(SaveIni(s,true)&&ApplyPending(s.ini));
 assert(bytes(s.ini).find("Height=1.83")!=std::string::npos&&bytes(s.ini).find("NewFineTune=0.123")!=std::string::npos);
 s.upscaler=-1;assert(SaveIni(s,true)&&ApplyPending(s.ini));
 assert(SaveIni(s,false)&&ApplyPending(s.ini));
 // A pending supported Mono DLSS choice remains valid after selecting stereo.
 assert(WritePrivateProfileStringW(L"Settings",L"VrStereoExperiment",L"0",s.ini.c_str()));
 s.upscaler=2;s.quality=5;assert(SaveIni(s,true));
 assert(WritePrivateProfileStringW(L"Settings",L"VrStereoExperiment",L"3",s.ini.c_str()));
 assert(ApplyPending(s.ini)&&!fs::exists(PendingPath(s.ini)));
 assert(GetPrivateProfileIntW(L"Settings",L"VrStereoExperiment",-1,s.ini.c_str())==3);
 assert(bytes(s.ini).find("Height=1.83")!=std::string::npos&&bytes(s.ini).find("NewFineTune=0.123")!=std::string::npos);
 // Unsupported/unknown engine modes are still rejected without writing either file.
 put(cfg,"r_SuperResolution_Mode=3\r\ncustom_key=7\r\n");
 s.upscaler=3;s.quality=0;const auto unsupported=bytes(cfg), unchangedStereoIni=bytes(s.ini);
 assert(!SaveSource(s,true)&&!SaveSource(s,false)&&bytes(cfg)==unsupported);
 assert(!SaveIni(s,true)&&!SaveIni(s,false)&&bytes(s.ini)==unchangedStereoIni);
 s.upscaler=2;s.quality=5;assert(SaveIni(s,true));
 assert(WritePrivateProfileStringW(L"Settings",L"StartupUpscaler",L"99",PendingPath(s.ini).c_str()));
 const auto beforeRefusal=bytes(s.ini);assert(!ApplyPending(s.ini)&&bytes(s.ini)==beforeRefusal);
 assert(fs::exists(PendingPath(s.ini)));
 std::cout<<"Startup graphics/pending ownership, stereo compatibility, explicit Native and calibration preservation PASS\n";
}
