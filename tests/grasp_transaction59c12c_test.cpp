#include "../src/core/grasp_transaction59c12c.h"
#include <cassert>
#include <limits>
using namespace kcd2vr::grasp_transaction59c12c;
int main() {
 assert(SafeHoldAt(.30f)==.47f);
 assert(SafeHoldAt(.70f)==.55f);
 assert(SafeHoldAt(std::numeric_limits<float>::quiet_NaN())==.50f);
 assert(Decide(1.f,.5f,.5f,.10f,.35f)==Intent::KeepHolding);
 assert(Decide(0.f,.5f,.5f,.10f,.35f)==Intent::Drop);
 assert(Decide(1.f,.29f,.5f,.10f,.35f)==Intent::Stow);
 assert(Decide(0.f,.29f,.5f,.10f,.35f)==Intent::Drop); // drop priority
 assert(Decide(1.f,std::numeric_limits<float>::quiet_NaN(),.5f,.10f,.35f)==Intent::KeepHolding);
}
