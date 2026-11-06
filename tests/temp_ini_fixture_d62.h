#pragma once
// The retained native-INI doubles still use in-memory keys. The production D62
// guard additionally requires a real complete file before those writers run.
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
struct TempIniFixtureD62 {
 std::filesystem::path directory,path;
 std::string name;
 inline static std::atomic<unsigned> serial{0};
 explicit TempIniFixtureD62(const std::string& bytes="; D62 fixture\n[Settings]\nEnabled=true\n[Unknown.Module]\nPrivateKey=preserve me\n"){
  const auto stem=std::string("kcd2vr-ini-d62-")+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
  bool made=false;
  for(unsigned n=0;n<64&&!made;++n){
   directory=std::filesystem::temp_directory_path()/(stem+"-"+std::to_string(++serial));
   made=std::filesystem::create_directory(directory);
  }
  if(!made)throw std::runtime_error("cannot create isolated INI fixture");
  path=directory/"KCD2VR.ini";name=path.string();
  std::ofstream file(path,std::ios::binary);file.write(bytes.data(),std::streamsize(bytes.size()));file.close();
  if(!file)throw std::runtime_error("cannot write isolated INI fixture");
 }
 ~TempIniFixtureD62(){std::error_code error;std::filesystem::remove_all(directory,error);}
 TempIniFixtureD62(const TempIniFixtureD62&)=delete;
 TempIniFixtureD62& operator=(const TempIniFixtureD62&)=delete;
};
