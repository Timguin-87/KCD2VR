#include <atomic>
#include <cassert>
#include <cstdio>
#include "core/collab_test_h21.h"
namespace diagnostic_trace {void Write(const char*){}}
namespace nhp134::geom27::impulse596 {
std::atomic<float> g_scale{0},g_cap{0},g_mass{0};std::atomic<bool> g_enabled{false};std::atomic<unsigned> g_session{0};
}
#include <corpse_default_setter_h26.inc>
int main(){using namespace nhp134::geom27::impulse596;
kcd2vr::collab_h21::setPhysicsMode(0);assert(!kcd2vr::collab_h21::npcPhysics());
SetContactImpulse596(true,1.f,40.f,4.f);assert(g_enabled&&g_session==1&&g_scale==1.f&&g_cap==40.f&&g_mass==4.f);
SetContactImpulse596(true,1.8f,80.f,4.f);assert(g_enabled&&g_session==1&&g_scale==1.8f&&g_cap==80.f);
SetContactImpulse596(false,1.f,40.f,4.f);assert(!g_enabled&&g_session==2);
kcd2vr::collab_h21::setPhysicsMode(3);SetContactImpulse596(true,1.f,40.f,4.f);assert(g_enabled&&g_session==3);
kcd2vr::collab_h21::setPhysicsMode(0);SetContactImpulse596(true,1.f,40.f,4.f);assert(g_enabled&&g_session==3);
SetContactImpulse596(false,1.f,40.f,4.f);assert(!g_enabled&&g_session==4);
std::puts("H26 actual public corpse setter: default enable, session ownership,3-to0 preservation and disable PASS; native leaves not executed");}
