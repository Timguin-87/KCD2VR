#include "subsystems/catalog.h"
#include <cassert>
#include <cstdio>
#include <cstring>
int main(){using namespace kcd2vr::subsystems;HostServices h{};assert(StartCatalog(h));auto& r=Catalog();assert(r.ModuleCount()==17);assert(!r.NeedsContext());bool vrik=false;for(std::size_t i=0;i<r.ModuleCount();++i){const auto* d=r.ModuleAt(i);assert(d&&d->lifecycle==Lifecycle::HostOwned&&!d->on_context);if(!std::strcmp(d->name,"VRIK"))vrik=true;}assert(vrik);assert(StopCatalog());assert(!r.Running());std::puts("actual17-module catalog validates, VRIK naming, host lifetimes and no added context polling PASS");}
