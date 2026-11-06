#include "module.h"
namespace kcd2vr::subsystems::doors {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=9; d.name="Doors"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}

bool TickService(ServiceState& state,ServiceAdmission admission,std::uint64_t now,
                 const ServiceLeaves& leaves) noexcept {
 if(!leaves.ready||!leaves.set_enabled||!leaves.start||!leaves.stop||!leaves.write)return false;
 if(!admission.wanted){
  (void)leaves.stop(); // Native/Lua owners retain cleanup, including a refused stop.
  state.retry_after=0;state.reported_bridge=-1;
 }else if(leaves.ready())leaves.start();
 else if(!state.retry_after||now>=state.retry_after){
  const bool ready=leaves.set_enabled(true);
  if(ready)leaves.start();
  state.retry_after=now+1000;
  if(state.reported_bridge!=int(ready)){
   state.reported_bridge=int(ready);
   leaves.write(ready?
       "C46 DOOR_SERVICE bridge=READY source=NORMAL_CALIBRATED_HANDLING companionAck=REQUIRED":
       "C46 DOOR_SERVICE bridge=UNAVAILABLE exactPinsOrHook=REFUSED nativeControls=AVAILABLE");
  }
 }
 if(state.previous_wanted&&!admission.wanted)
  leaves.write("C46 DOOR_SERVICE mode=REVOKED cleanup=NATIVE_AND_LUA_OWNERS");
 state.previous_wanted=admission.wanted;
 return true;
}

bool TickService(ServiceAdmission admission,std::uint64_t now,const ServiceLeaves& leaves) noexcept {
 static ServiceState state;
 return TickService(state,admission,now,leaves);
}
}
