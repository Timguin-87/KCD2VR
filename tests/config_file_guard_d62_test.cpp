#include "../src/core/config_file_guard_d62.h"
#include "temp_ini_fixture_d62.h"
#include <cstdlib>
#include <cstdio>
#include <future>
#include <iterator>
#include <stdexcept>
#include <thread>
namespace guard=kcd2vr::core::config_file_guard_d62;
namespace fs=std::filesystem;
static unsigned checks=0;
static void check(bool value,const char* why){++checks;if(!value){std::fprintf(stderr,"FAIL D62 config guard %u: %s\n",checks,why);std::abort();}}
static void write(const fs::path& path,const std::string& text){std::ofstream out(path,std::ios::binary|std::ios::trunc);out.write(text.data(),std::streamsize(text.size()));out.close();if(!out)throw std::runtime_error("fixture write failed");}
static std::string read(const fs::path& path){std::ifstream in(path,std::ios::binary);return {std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};}
static fs::path backup(const fs::path& path){auto b=path;b+=".last-good.d62.bak";return b;}
static void truncateBeforeOpen(const fs::path& path){fs::resize_file(path,1);}
static void removeBeforeOpen(const fs::path& path){fs::remove(path);}
static void appendAfterRead(const fs::path& path){std::ofstream out(path,std::ios::app|std::ios::binary);out<<"changed";}
static void stampAfterRead(const fs::path& path){fs::last_write_time(path,fs::last_write_time(path)+std::chrono::seconds(2));}
static void throwBeforeOpen(const fs::path&){throw std::runtime_error("observed I/O fault");}
int main(){
 const std::string profile="; preserve comment and original CRLF\r\n[Settings]\r\nEnabled=true\r\nVrFreeTarget=true\r\n[OtherMod]\r\nUnknownSetting=keep exactly\r\n[VRIK.Fingers.42]\r\nBank1=personal calibration bytes\r\n";
 TempIniFixtureD62 file(profile);guard::Snapshot snapshot;
 check(guard::readComplete(file.path,snapshot)==guard::Status::Ready,"a complete current INI is admitted");
 check(snapshot.bytes==profile,"complete snapshot preserves every byte and unknown/calibration sections");
 {
  guard::Lease current(file.name.c_str());
  check(current.valid()&&current.checkpointed(),"normal startup/save lease checkpoints a readable file");
  check(current.path().is_absolute()&&current.original().bytes==profile,"lease pins the same absolute path and bytes");
  check(read(file.path)==profile&&read(backup(file.path))==profile,"checkpoint leaves original untouched and stores exact bytes");
 }
 const auto savedBackup=read(backup(file.path));fs::remove(file.path);
 {
  guard::Lease absent(file.name.c_str());
  check(!absent.valid()&&!absent.checkpointed(),"missing current INI cannot enter a writer");
 }
 check(!fs::exists(file.path)&&read(backup(file.path))==savedBackup,"missing-file refusal never restores or overwrites the old checkpoint");
 write(file.path,"");{
  guard::Lease empty(file.name.c_str());check(!empty.valid()&&empty.status()==guard::Status::Empty,"empty profile is refused before migration defaults");
 }
 check(fs::file_size(file.path)==0&&read(backup(file.path))==savedBackup,"empty original and old checkpoint are untouched");
 fs::remove(file.path);fs::create_directory(file.path);
 {
  guard::Lease directory(file.name.c_str());check(!directory.valid(),"a directory cannot masquerade as a configuration file");
 }
 fs::remove(file.path);write(file.path,profile);fs::resize_file(file.path,16u*1024u*1024u+1u);
 {
  guard::Lease oversized(file.name.c_str());check(!oversized.valid()&&oversized.status()==guard::Status::TooLarge,"bounded reader refuses an unrelated oversized file without rewriting it");
 }
 check(fs::file_size(file.path)==16u*1024u*1024u+1u,"oversized profile is retained exactly");
 write(file.path,profile);snapshot.bytes="old successful observation";
 check(guard::readComplete(file.path,snapshot,{truncateBeforeOpen,nullptr})==guard::Status::ReadFailed&&snapshot.bytes.empty(),"real short read clears stale snapshot and fails");
 write(file.path,profile);
 check(guard::readComplete(file.path,snapshot,{removeBeforeOpen,nullptr})==guard::Status::OpenFailed&&snapshot.bytes.empty(),"real open failure after metadata observation fails");
 write(file.path,profile);
 check(guard::readComplete(file.path,snapshot,{nullptr,appendAfterRead})==guard::Status::Changed&&snapshot.bytes.empty(),"file growth between read and validation revokes the snapshot");
 write(file.path,profile);
 check(guard::readComplete(file.path,snapshot,{nullptr,stampAfterRead})==guard::Status::Changed&&snapshot.bytes.empty(),"same-size file with changed timestamp is rejected");
 write(file.path,profile);
 check(guard::readComplete(file.path,snapshot,{throwBeforeOpen,nullptr})==guard::Status::Exception&&snapshot.bytes.empty(),"I/O exception cannot admit a stale snapshot");
 {
  guard::Lease nullPath(nullptr);check(!nullPath.valid(),"null path is refused");
 }
 {
  guard::Lease emptyPath("");check(!emptyPath.valid(),"empty path is refused");
 }
 // Rename failure is deterministic even when tests run as an elevated user.
 // A checkpoint failure must NEVER make a readable full configuration unusable.
 write(file.path,profile);fs::remove(backup(file.path));fs::create_directory(backup(file.path));
 write(backup(file.path)/"unrelated","retain");
 {
  guard::Lease noBackup(file.name.c_str());check(noBackup.valid()&&!noBackup.checkpointed(),"best-effort checkpoint failure still admits a valid startup/save");
  check(noBackup.original().bytes==profile&&read(file.path)==profile,"failed checkpoint cannot mutate the live original");
 }
 check(read(backup(file.path)/"unrelated")=="retain","failed replacement leaves the existing backup destination alone");
 unsigned tempCount=0;for(const auto& entry:fs::directory_iterator(file.directory))if(entry.path().extension()==".tmp")++tempCount;
 check(tempCount==0,"failed checkpoint cleans only its own temporary file");
 fs::remove_all(backup(file.path));
 // No UTF or INI syntax interpretation is attempted; the DetourModKit parser
 // has no read/parse success ACK. Checkpoint bytes remain exact, including BOM.
 const std::string encoded("\xff\xfe[\0S\0]\0",8);
 write(file.path,encoded);
 {
  guard::Lease binary(file.name.c_str());check(binary.valid()&&read(backup(file.path))==encoded,"complete byte-level checkpoint does not re-encode a profile");
 }
 {
  TempIniFixtureD62 other(profile);const fs::path unicodePath=other.directory/fs::path(u8"settings_漢_č.ini");
  fs::rename(other.path,unicodePath);guard::Lease nativePath(unicodePath);
  check(nativePath.valid()&&nativePath.path()==fs::absolute(unicodePath),"native filesystem path overload retains a non-ASCII filename without a narrow conversion");
  check(nativePath.checkpointed()&&read(backup(unicodePath))==profile,"native-path checkpoint preserves the original bytes");
 }
 // The menu's snapshot must follow the worker's completed calibration write.
 // Native writer behavior is a file leaf here; the shared lease/mutex is real.
 write(file.path,profile);
 std::promise<void> firstLocked,secondWaiting,releaseFirst;
 std::promise<std::string> observedBySecond;
 auto firstReady=firstLocked.get_future();auto waiting=secondWaiting.get_future();auto release=releaseFirst.get_future();auto observed=observedBySecond.get_future();
 const std::string calibrated=profile+"\r\n[VRIK.Head]\r\nStandingEyeHeightM=1.7312\r\n";
 std::atomic<bool> secondAcquired{false};
 std::thread calibration([&]{guard::Lease lease(file.name.c_str());if(!lease.valid())std::abort();firstLocked.set_value();release.wait();write(file.path,calibrated);});
 firstReady.wait();
 std::thread menu([&]{secondWaiting.set_value();guard::Lease lease(file.name.c_str());secondAcquired=true;observedBySecond.set_value(lease.original().bytes);write(file.path,lease.original().bytes+"\r\n[Settings]\r\nMenuChoice=2\r\n");});
 waiting.wait();
 check(!secondAcquired.load(),"a calibration lease excludes concurrent menu writers");
 releaseFirst.set_value();calibration.join();menu.join();
 check(observed.get()==calibrated,"menu snapshots current calibration, never the earlier cached configuration");
 check(read(backup(file.path))==calibrated,"menu checkpoint includes the immediately preceding calibration save");
 check(read(file.path).find(profile)==0&&read(file.path).find("StandingEyeHeightM=1.7312")!=std::string::npos,"serialized per-key writer leaf retains unknown keys and calibration");
 std::printf("config_file_guard_d62: %u checks PASS; real file I/O; native Win32 profile/parser not emulated\n",checks);
}
