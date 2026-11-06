#include "vrik/default_migration111.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::defaults_migration111;
static int n=0;static void ck(bool v,const char*m){++n;if(!v){std::fprintf(stderr,"FAIL %s\n",m);std::exit(1);}}
int main(){
 ck(LegacyCameraHeight(0.0f),"legacy zero height migrates");
 ck(!LegacyCameraHeight(0.035f),"new eye clearance stays");
 ck(!LegacyCameraHeight(0.020f),"custom height preserved");
 ck(LegacyMergedCanvas(true,1.0f,0.78f,0.78f),"exact merged canvas default migrates");
 ck(!LegacyMergedCanvas(false,1.0f,0.78f,0.78f),"already semantic stays");
 ck(!LegacyMergedCanvas(true,0.9f,0.78f,0.78f),"custom size preserved");
 ck(!LegacyMergedCanvas(true,1.0f,0.70f,0.78f),"custom spread preserved");
 std::printf("PASS %d default migration checks\n",n);return 0;
}
