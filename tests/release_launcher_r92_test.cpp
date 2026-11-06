#include "subsystems/base/startup_setup_s15.inl"
#include <cassert>
#include <iostream>
using namespace kcd2vr::startup_s15;
static std::string bytes(const fs::path& p){std::ifstream in(p,std::ios::binary);return {std::istreambuf_iterator<char>(in),{}};}
static void put(const fs::path& p,const std::string& text){std::ofstream out(p,std::ios::binary);out<<text;}
int main(){
 wchar_t temp[MAX_PATH]{};GetTempPathW(MAX_PATH,temp);
 const auto root=fs::path(temp)/(L"KCD2VR_RELEASE_R92_"+std::to_wstring(GetCurrentProcessId()));fs::create_directory(root);
 struct Clean{fs::path p;~Clean(){fs::remove_all(p);}}clean{root};fs::create_directory(root/L"Data");fs::create_directory(root/L"Bin");State s{};s.root=root;s.ini=root/L"Bin"/L"KCD2VR.ini";
 const std::string ini="; retain comments\r\n[Settings]\r\nVrStereoExperiment=3\r\nVrDeveloperMode=false\r\nCustomOption=42\r\n[Calibration]\r\nHeight=1.83\r\n";
 put(s.ini,ini);for(const auto& f:fields)s.values.push_back(Read(s.ini,f));
 put(root/L"user.cfg","r_SuperResolution_Mode=0\r\ncustom_key=7\r\nsys_spec_Shading=1\r\n");
 s.hasLaunchOptions=true;s.upscaler=2;s.quality=1;s.graphics=3;s.developer=true;s.balanced=false;
 assert(SaveSource(s,true));assert(SaveIni(s,true));assert(bytes(s.ini)==ini);assert(ApplyPending(s.ini));
 assert(ReadBool(s.ini,L"VrDeveloperMode",false));auto saved=bytes(s.ini);assert(saved.find("Height=1.83")!=std::string::npos&&saved.find("CustomOption=42")!=std::string::npos);
 auto cfg=bytes(root/L"user.cfg");assert(cfg.find("sys_spec_Shading = 3")!=std::string::npos&&cfg.find("custom_key=7")!=std::string::npos);
 assert(cfg.find("r_SuperResolution_NVIDIA_DLSS_QualityMode = 1")!=std::string::npos);
 s.developer=false;s.upscaler=-1;s.quality=0;s.graphics=-1;assert(SaveIni(s,true));assert(ApplyPending(s.ini));assert(!ReadBool(s.ini,L"VrDeveloperMode",true));
 saved=bytes(s.ini);assert(saved.find("Height=1.83")!=std::string::npos&&saved.find("CustomOption=42")!=std::string::npos);
 std::cout<<"PASS launcher DeveloperMode / shared preset / existing per-eye DLSS choice / calibration preservation\n";
}
