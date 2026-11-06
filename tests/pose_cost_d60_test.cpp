#include "diagnostics/pose_cost_d60.h"
#include <cassert>
#include <string>
#include <vector>
using namespace kcd2vr::diagnostics;
int main(){using namespace pose_cost_d60;std::vector<std::string> rows;auto sink=[&](const char*s){rows.emplace_back(s);};
const auto now=kcd2vr::hookcost178r22::tickNow();
{Scope off(ComposeDirty);assert(!off.capture.serial);}drain(now,sink);assert(rows.empty());
capture168::gate.select(capture168::Domain::Bug,true,now);
{Scope on(ComposeDirty);assert(on.capture.serial);}drain(now,sink);assert(rows.size()==1&&rows.back().find("stage=FK_DIRTY")!=std::string::npos);
rows.clear();{Scope old(ContactSweep);capture168::gate.select(capture168::Domain::Bug,false,now);capture168::gate.select(capture168::Domain::Bug,true,now);}
drain(now,sink);assert(rows.empty());
const auto serial=capture168::gate.session[unsigned(capture168::Domain::Bug)].load();record(ContactRecovery,serial,17);record(ContactRecovery,serial,9);drain(now,sink);
assert(rows.size()==1&&rows[0].find("calls=2")!=std::string::npos&&rows[0].find("totalUs=26")!=std::string::npos&&rows[0].find("maxUs=17")!=std::string::npos);
rows.clear();record(ComposeFull,serial,100);capture168::gate.select(capture168::Domain::Bug,false,now);capture168::gate.select(capture168::Domain::Bug,true,now);drain(now,sink);assert(rows.empty());
capture168::gate.select(capture168::Domain::Bug,false,now);{Scope off(ContactRecovery);assert(!off.capture.serial);}drain(now,sink);assert(rows.empty());
assert(capture168::domain("D60 F2_POSE_COST stage=HAND_SWEEP")==capture168::Domain::Bug);
}
