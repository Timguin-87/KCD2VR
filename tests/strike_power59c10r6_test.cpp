#include "core/strike_power_rule59c10r6.h"
#include <cstdio>
#include <limits>
using kcd2vr::strike_power59c10r6::Final;
static int fails=0;static void ck(bool b,const char* m){std::printf("%s %s\n",b?"PASS":"FAIL",m);fails+=!b;}
int main(){
  const float mn=.30f,mx=1.20f;
  ck(Final(1.10f*1.25f,mn,mx)==1.20f,"two-handed full swing (1.375) capped at 120 %");
  ck(Final(1.375f*1.5f,mn,mx)==1.20f,"riposte window two-handed (2.06) capped at 120 %");
  ck(Final(.30f*.25f,mn,mx)==.30f,"3rd repeat of the weakest stab (0.075) floored at 30 %");
  ck(Final(.85f,mn,mx)==.85f,"in-range values unchanged");
  ck(Final(1.f,.05f,2.f)>=.30f&&Final(5.f,.05f,2.f)==1.20f,"INI cannot widen the rule (min<30 % / max>120 %)");
  ck(Final(std::numeric_limits<float>::quiet_NaN(),mn,mx)==1.f,"non-finite input = a normal swing");
  ck(Final(1.f,.9f,.5f)==.9f,"max below min collapses to min");
  std::printf("%d failures\n",fails);return fails;}
