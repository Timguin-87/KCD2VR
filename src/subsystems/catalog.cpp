#include "catalog.h"
#include "hybrid/module.h"
#include "direct/module.h"
#include "masterstrike_combos/module.h"
#include "physics/module.h"
#include "interactions/module.h"
#include "alchemy/module.h"
#include "smithing/module.h"
#include "doors/module.h"
#include "dialogues/module.h"
#include "hud_ui_controls/module.h"
#include "vrik/module.h"
#include "vrmap/module.h"
#include "gore/module.h"
#include "throw_drop/module.h"
#include "ranged_reload/module.h"
#include "experimental/module.h"
namespace kcd2vr::subsystems {
Registry& Catalog() noexcept { static Registry r; return r; }
Status StartCatalog(const HostServices& h) noexcept {
 auto& r=Catalog(); Status s{};
 ModuleDescriptor base{};base.id=1;base.name="KCD2VR Base"; if(!(s=r.Add(base)))return s;
 if(!(s=r.Add(hybrid::Descriptor())))return s;
 if(!(s=r.Add(direct::Descriptor())))return s;
 if(!(s=r.Add(masterstrike_combos::Descriptor())))return s;
 if(!(s=r.Add(physics::Descriptor())))return s;
 if(!(s=r.Add(interactions::Descriptor())))return s;
 if(!(s=r.Add(alchemy::Descriptor())))return s;
 if(!(s=r.Add(smithing::Descriptor())))return s;
 if(!(s=r.Add(doors::Descriptor())))return s;
 if(!(s=r.Add(dialogues::Descriptor())))return s;
 if(!(s=r.Add(hud_ui_controls::Descriptor())))return s;
 if(!(s=r.Add(vrik::Descriptor())))return s;
 if(!(s=r.Add(vrmap::Descriptor())))return s;
 if(!(s=r.Add(gore::Descriptor())))return s;
 if(!(s=r.Add(throw_drop::Descriptor())))return s;
 if(!(s=r.Add(ranged_reload::Descriptor())))return s;
 if(!(s=r.Add(experimental::Descriptor())))return s;
 return r.Start(h);
}
Status StopCatalog() noexcept {return Catalog().Stop();}
}
