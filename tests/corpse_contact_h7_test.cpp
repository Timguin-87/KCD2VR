#include "subsystems/gore/corpse_contact_h7.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::corpse_contact_h7;
int main(){Evidence ok{100,100,379,379,true,true,true,true,true};assert(qualified(ok));
 auto e=ok;e.nativeDead=false;assert(!qualified(e));e=ok;e.ownerStable=false;assert(!qualified(e));
 e=ok;e.relinquished=false;assert(!qualified(e));e=ok;e.articulated=false;assert(!qualified(e));
 e=ok;e.partReadable=false;assert(!qualified(e));e=ok;e.currentMain=200;assert(!qualified(e));
 e=ok;e.rawPart=-1;assert(!qualified(e));e=ok;e.returnedPart=179;assert(!qualified(e));
 std::puts("H7 corpse contact: no living/successor/joint substitution PASS");}
