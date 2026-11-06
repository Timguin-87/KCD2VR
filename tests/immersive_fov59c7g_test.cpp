#include "../src/immersive_fov59c7g.h"
#include <cassert>
#include <limits>
#include <initializer_list>
using namespace kcd2vr::immersive_fov59c7g;
int main(){Policy p{100,1.7f,true};
 for(float native:{.4f,1.2f,1.7f,2.2f})assert(select(p,110,native,true,false)==1.7f);
 assert(select(p,181,.4f,true,false)==.4f);assert(select(p,99,.4f,true,false)==.4f);
 assert(select(p,110,.4f,false,false)==.4f);assert(select(p,110,.4f,true,true)==.4f);
 p.immersive=false;assert(select(p,110,.4f,true,false)==.4f); // cinematic/full-menu pass-through
 p.immersive=true;p.radians=0;assert(select(p,110,.4f,true,false)==.4f);
 p.radians=1.7f;const auto nan=std::numeric_limits<float>::quiet_NaN();assert(std::isnan(select(p,110,nan,true,false)));
}
