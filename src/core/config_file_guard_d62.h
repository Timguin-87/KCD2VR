#pragma once
// Startup and file-I/O workers only. This guards the existing INI writers; it
// is not a replacement INI parser and never restores defaults or an old file.
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <system_error>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace kcd2vr::core::config_file_guard_d62 {
namespace fs=std::filesystem;
enum class Status {Ready,MissingOrNotRegular,Empty,TooLarge,OpenFailed,ReadFailed,Changed,Exception};
inline const char* name(Status value) noexcept {
 switch(value){case Status::Ready:return "READY";case Status::MissingOrNotRegular:return "MISSING_OR_NOT_REGULAR";
 case Status::Empty:return "EMPTY";case Status::TooLarge:return "TOO_LARGE";case Status::OpenFailed:return "OPEN_FAILED";
 case Status::ReadFailed:return "READ_FAILED";case Status::Changed:return "CHANGED_DURING_READ";default:return "IO_EXCEPTION";}
}
struct Snapshot {std::string bytes;};
// File-observation seams exercise real open/read/change failures in portable
// tests. Production passes no hooks and still executes the same file reader.
struct ReadHooks {void (*beforeOpen)(const fs::path&)=nullptr;void (*afterRead)(const fs::path&)=nullptr;};
inline Status readComplete(const fs::path& path,Snapshot& out,ReadHooks hooks={}) noexcept {
 out={};try{
  std::error_code error;if(!fs::is_regular_file(path,error)||error)return Status::MissingOrNotRegular;
  const auto size=fs::file_size(path,error);if(error)return Status::ReadFailed;
  if(!size)return Status::Empty;
  // Above ordinary INI + all bounded calibration banks, while preventing an
  // accidental unrelated multi-gigabyte file from exhausting the mod worker.
  if(size>16u*1024u*1024u)return Status::TooLarge;
  const auto modified=fs::last_write_time(path,error);if(error)return Status::ReadFailed;
  if(hooks.beforeOpen)hooks.beforeOpen(path);
  std::ifstream stream(path,std::ios::binary);if(!stream.is_open())return Status::OpenFailed;
  std::string bytes(std::size_t(size),'\0');stream.read(bytes.data(),std::streamsize(size));
  if(!stream||stream.gcount()!=std::streamsize(size))return Status::ReadFailed;
  char extra=0;if(stream.get(extra))return Status::Changed;
  if(!stream.eof()||stream.bad())return Status::ReadFailed;
  stream.close();if(hooks.afterRead)hooks.afterRead(path);
  const auto finalSize=fs::file_size(path,error);if(error||finalSize!=size)return Status::Changed;
  const auto finalModified=fs::last_write_time(path,error);if(error||finalModified!=modified)return Status::Changed;
  out.bytes=std::move(bytes);return Status::Ready;
 }catch(...){return Status::Exception;}
}
inline std::mutex writerMutex;
inline std::atomic<std::uint64_t> checkpointSerial{0};
inline std::atomic<bool> loadFailureShown{false},checkpointFailureShown{false};
// R96: the last-good checkpoint stays IN MEMORY only. The user's rule is one
// log file and no extra files in the game folder, so no .last-good.*.bak, no
// temporary checkpoint file. Recovery data = the exact bytes of the last
// readable KCD2VR.ini seen by a lease in this game session (lastGoodBytes()).
inline std::mutex lastGoodMutex;
inline fs::path lastGoodPath;inline std::string lastGoodText;
inline bool checkpoint(const fs::path& source,const Snapshot& snapshot) noexcept {
 try{
  std::lock_guard lock(lastGoodMutex);
  lastGoodPath=source;lastGoodText=snapshot.bytes;
  checkpointSerial.fetch_add(1,std::memory_order_relaxed);
  return true;
 }catch(...){return false;}
}
inline std::string lastGoodBytes() noexcept {
 try{std::lock_guard lock(lastGoodMutex);return lastGoodText;}catch(...){return {};}
}
// All mod menu/migration/calibration writes share this lease. It reads the
// CURRENT file under the same lock, so a menu save never checkpoints a stale
// copy from before calibration. Existing Win32 per-key merge writers remain;
// there is no whole-file snapshot replay or automatic restore. Other processes
// can still change this file, and DetourModKit exposes no parser success ACK.
class Lease {
 std::unique_lock<std::mutex> lock_{writerMutex,std::defer_lock};
 fs::path path_;Snapshot original_;Status status_=Status::Exception;bool checkpointed_=false;
 void acquire(const fs::path& path,bool makeCheckpoint,ReadHooks hooks) noexcept {
  try{
   lock_.lock();if(path.empty()){status_=Status::MissingOrNotRegular;return;}
   std::error_code error;path_=fs::absolute(path,error);if(error)return;
   status_=readComplete(path_,original_,hooks);
   if(status_==Status::Ready&&makeCheckpoint)checkpointed_=checkpoint(path_,original_);
  }catch(...){status_=Status::Exception;}
 }
public:
 explicit Lease(const fs::path& path,bool makeCheckpoint=true,ReadHooks hooks={}) noexcept {
  acquire(path,makeCheckpoint,hooks);
 }
 explicit Lease(const char* path,bool makeCheckpoint=true,ReadHooks hooks={}) noexcept {
  // Keep narrow-path construction inside the catch boundary; the startup
  // setup path overload above preserves its existing native wide filename.
  try{acquire(path?fs::path(path):fs::path{},makeCheckpoint,hooks);}catch(...){status_=Status::Exception;}
 }
 ~Lease()=default;
 Lease(const Lease&)=delete;Lease& operator=(const Lease&)=delete;
 bool valid()const noexcept{return status_==Status::Ready;}
 Status status()const noexcept{return status_;}
 bool checkpointed()const noexcept{return checkpointed_;}
 const fs::path& path()const noexcept{return path_;}
 const Snapshot& original()const noexcept{return original_;}
};
}
