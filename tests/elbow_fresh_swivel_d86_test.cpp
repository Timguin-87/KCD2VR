// Exact fresh-history F7 row, KCD2VR.log line566735,2026-11-03 00:08:04.417.
#include "vrik/frik_elbow_lab.h"
#include <cstdio>
using namespace kcd2vr::pose_core;
int main(){
 ElbowContinuity s{};s.ready=true;s.epoch=6;s.sample=179510;s.tick=333423562;s.timeNs=333423597684600;
 s.axis={.7449412786841465,.59931007596260921,.2930698281333658};
 s.bend={-.1104925348386632,-.32239504568330457,.9401344766913865};
 const Vec3 axis{.18823954463005066,.16179347038269043,.07295072078704834};
 const Vec3 anatomical{.054403280104020145,.3555095629720007,-.92884581932807275};
 const Vec3 fallback{-.55257054239343961,.77785840589270872,-.29933609214572482};
 const auto transported=transported_elbow_bend(s,*unit(axis));
 const auto pole=kcd2vr::frik_elbow_lab::continuous_elbow_lab(s,axis,anatomical,fallback,.49017225688884419,6,179514,333423578,333423608731300);
 if(!pole||!transported)return 2;
 const double angle=std::acos(std::clamp(dot(*pole,*transported),-1.,1.))*180/3.141592653589793;
 const auto count=s.updates;
 const auto duplicate=kcd2vr::frik_elbow_lab::continuous_elbow_lab(s,axis,anatomical,fallback,.49017225688884419,6,179514,333423578,333423608731300);
 const bool pass=angle<=2.532&&duplicate&&length(*duplicate-*pole)<1e-12&&s.updates==count;
 std::printf("recorded_fresh_swivel_deg=%.9f dt_ms=11.0467 same_sample_updates=%llu %s\n",angle,(unsigned long long)(s.updates-count),pass?"PASS":"FAIL");
 return pass?0:1;
}
