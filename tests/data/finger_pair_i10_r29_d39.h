#pragma once
// Literal full-finger reconstruction from the supplied R29 review handoff.
// finger_pair_repro.cpp SHA256: 485627d99984f092b6d77fb0321c2e316fceeee5737cbaf3d87ecb10a123854b
// I10 2026-10-26 20:53:14, samples 76354/76358. Static recipes are the
// surrounding exact native-finger packets; wrist orientations/goals are the
// selected F2/F7 samples. Source rounding is retained without fitted values.
#include "vrik/hand_shape59c7f.h"
#include <array>
namespace pair_i10_d39 {
namespace pc=kcd2vr::pose_core;namespace hs=kcd2vr::hand_shape59c7f;
struct Sample {hs::Shape hand[2]{};pc::Vec3 requested[2]{};};
inline std::array<Sample,2> samples(){std::array<Sample,2> result{};unsigned n=0;
{hs::Shape a[2]{}; pc::Vec3 p[2]{{0.07121,0.16055,1.30537},{-0.11249,0.16386,1.30583}};
{pc::FingerCurve103 curves[5]{}; float input[5]{};
curves[0].count=4;
curves[0].offset[0]={-0.01569,0.00557,-0.0197};curves[0].open[0]=curves[0].closed[0]={0.466421,-0.279386,0.086057,0.834859};
curves[0].offset[1]={-0.04798,0,-0};curves[0].open[1]=curves[0].closed[1]={0.007061,0.007294,-0.210246,0.977596};
curves[0].offset[2]={-0.03202,0,0};curves[0].open[2]=curves[0].closed[2]={0.000631,-0.004812,-0.10188,0.994785};
curves[0].offset[3]={-0.03111,-0,-0};curves[0].open[3]=curves[0].closed[3]={-0.000116,0.014367,-0.025941,0.99956};
curves[1].count=4;
curves[1].offset[0]={-0.08105,-0.01928,-0.02061};curves[1].open[0]=curves[1].closed[0]={-0.011612,-0.05019,-0.084917,0.995055};
curves[1].offset[1]={-0.04279,-0,0};curves[1].open[1]=curves[1].closed[1]={-0.00168,0.000893,-0.13243,0.991191};
curves[1].offset[2]={-0.02576,-0,0};curves[1].open[2]=curves[1].closed[2]={-0.000111,0.000117,-0.096028,0.995379};
curves[1].offset[3]={-0.02582,-0,-0};curves[1].open[3]=curves[1].closed[3]={1.4e-05,-2.9e-05,0.032859,0.99946};
curves[2].count=4;
curves[2].offset[0]={-0.08096,-0.01886,0.0033};curves[2].open[0]=curves[2].closed[0]={-0.084887,0.040208,-0.11179,0.989283};
curves[2].offset[1]={-0.04711,0,-0};curves[2].open[1]=curves[2].closed[1]={-0.000397,-0.000364,-0.130701,0.991422};
curves[2].offset[2]={-0.02745,0,0};curves[2].open[2]=curves[2].closed[2]={3.3e-05,0.000372,-0.097388,0.995246};
curves[2].offset[3]={-0.02696,-0,0};curves[2].open[3]=curves[2].closed[3]={-0.000955,0.000277,0.041026,0.999158};
curves[3].count=5;
curves[3].offset[0]={-0.01495,-0.01191,0.00657};curves[3].open[0]=curves[3].closed[0]={-0.201151,0.126245,0.027437,0.971003};
curves[3].offset[1]={-0.06351,0,0};curves[3].open[1]=curves[3].closed[1]={0.083039,0.057809,-0.141434,0.984764};
curves[3].offset[2]={-0.04429,0,0};curves[3].open[2]=curves[3].closed[2]={-0.001237,0.002278,-0.135127,0.990825};
curves[3].offset[3]={-0.02512,-0,-0};curves[3].open[3]=curves[3].closed[3]={0.000496,0.001249,-0.098541,0.995132};
curves[3].offset[4]={-0.02648,0,0};curves[3].open[4]=curves[3].closed[4]={1.4e-05,0.000101,0.013442,0.99991};
curves[4].count=5;
curves[4].offset[0]={-0.01226,-0.00714,0.01697};curves[4].open[0]=curves[4].closed[0]={-0.255747,0.192572,-0.015766,0.947239};
curves[4].offset[1]={-0.05685,-0,0};curves[4].open[1]=curves[4].closed[1]={0.090176,0.078927,-0.081182,0.989469};
curves[4].offset[2]={-0.03672,0,-0};curves[4].open[2]=curves[4].closed[2]={-0.001474,0.001192,-0.136572,0.990628};
curves[4].offset[3]={-0.0209,-0,-0};curves[4].open[3]=curves[4].closed[3]={0.000726,-0.000991,-0.103472,0.994632};
curves[4].offset[4]={-0.02205,0,-0};curves[4].open[4]=curves[4].closed[4]={0.009227,-0.032207,0.0714,0.996885};
a[0]=hs::make(curves,input,pc::Quat{0.8840473,-0.3335339,-0.2399405,0.2228096});
}
{pc::FingerCurve103 curves[5]{}; float input[5]{};
curves[0].count=4;
curves[0].offset[0]={0.01569,-0.00557,0.0197};curves[0].open[0]=curves[0].closed[0]={0.454337,-0.28379,0.076896,0.840909};
curves[0].offset[1]={0.04798,-0,-0};curves[0].open[1]=curves[0].closed[1]={0.009924,0.015259,-0.203826,0.978838};
curves[0].offset[2]={0.03202,0,0};curves[0].open[2]=curves[0].closed[2]={0.000865,0.001242,-0.098764,0.99511};
curves[0].offset[3]={0.03111,0,-0};curves[0].open[3]=curves[0].closed[3]={-0.000288,0.023708,-0.036747,0.999043};
curves[1].count=4;
curves[1].offset[0]={0.08105,0.01928,0.02061};curves[1].open[0]=curves[1].closed[0]={-0.012343,-0.051268,-0.088198,0.994706};
curves[1].offset[1]={0.04279,-0,-0};curves[1].open[1]=curves[1].closed[1]={-0.002036,0.000733,-0.131596,0.991301};
curves[1].offset[2]={0.02576,0,-0};curves[1].open[2]=curves[1].closed[2]={-0.000149,-0.000167,-0.094044,0.995568};
curves[1].offset[3]={0.02582,-0,0};curves[1].open[3]=curves[1].closed[3]={0.000192,-0.001194,0.018562,0.999827};
curves[2].count=4;
curves[2].offset[0]={0.08097,0.01886,-0.0033};curves[2].open[0]=curves[2].closed[0]={-0.087717,0.039877,-0.111747,0.989054};
curves[2].offset[1]={0.04711,0,0};curves[2].open[1]=curves[2].closed[1]={-0.000122,0.000122,-0.132987,0.991118};
curves[2].offset[2]={0.02745,-0,-0};curves[2].open[2]=curves[2].closed[2]={0.000206,0.00092,-0.095088,0.995468};
curves[2].offset[3]={0.02696,-0,0};curves[2].open[3]=curves[2].closed[3]={-0.001121,0.001768,0.024445,0.999699};
curves[3].count=5;
curves[3].offset[0]={0.01495,0.01191,-0.00657};curves[3].open[0]=curves[3].closed[0]={-0.20088,0.125158,0.029954,0.971126};
curves[3].offset[1]={0.06351,-0,0};curves[3].open[1]=curves[3].closed[1]={0.077857,0.05896,-0.141216,0.98515};
curves[3].offset[2]={0.04429,-0,-0};curves[3].open[2]=curves[3].closed[2]={-0.000351,0.00399,-0.134291,0.990934};
curves[3].offset[3]={0.02512,0,0};curves[3].open[3]=curves[3].closed[3]={0.000822,0.002869,-0.093181,0.995645};
curves[3].offset[4]={0.02648,-0,-0};curves[3].open[4]=curves[3].closed[4]={-0.000155,0.00135,0.005767,0.999982};
curves[4].count=5;
curves[4].offset[0]={0.01225,0.00714,-0.01697};curves[4].open[0]=curves[4].closed[0]={-0.255595,0.190122,-0.014298,0.947798};
curves[4].offset[1]={0.05686,0,0};curves[4].open[1]=curves[4].closed[1]={0.083339,0.079702,-0.079204,0.990166};
curves[4].offset[2]={0.03672,-0,-0};curves[4].open[2]=curves[4].closed[2]={-7.4e-05,0.004058,-0.136539,0.990626};
curves[4].offset[3]={0.0209,-0,0};curves[4].open[3]=curves[4].closed[3]={0.001641,0.00231,-0.101394,0.994842};
curves[4].offset[4]={0.02205,0,-0};curves[4].open[4]=curves[4].closed[4]={0.00508,-0.01729,0.045816,0.998787};
a[1]=hs::make(curves,input,pc::Quat{-0.3433305,-0.2885391,0.2910815,0.8450686});
}
result[n++]={{a[0],a[1]},{p[0],p[1]}};
}
{hs::Shape a[2]{}; pc::Vec3 p[2]{{0.07175,0.16214,1.30414},{-0.11255,0.16288,1.30669}};
{pc::FingerCurve103 curves[5]{}; float input[5]{};
curves[0].count=4;
curves[0].offset[0]={-0.01569,0.00557,-0.0197};curves[0].open[0]=curves[0].closed[0]={0.466421,-0.279386,0.086057,0.834859};
curves[0].offset[1]={-0.04798,0,-0};curves[0].open[1]=curves[0].closed[1]={0.007061,0.007294,-0.210246,0.977596};
curves[0].offset[2]={-0.03202,0,0};curves[0].open[2]=curves[0].closed[2]={0.000631,-0.004812,-0.10188,0.994785};
curves[0].offset[3]={-0.03111,-0,-0};curves[0].open[3]=curves[0].closed[3]={-0.000116,0.014367,-0.025941,0.99956};
curves[1].count=4;
curves[1].offset[0]={-0.08105,-0.01928,-0.02061};curves[1].open[0]=curves[1].closed[0]={-0.011612,-0.05019,-0.084917,0.995055};
curves[1].offset[1]={-0.04279,-0,0};curves[1].open[1]=curves[1].closed[1]={-0.00168,0.000893,-0.13243,0.991191};
curves[1].offset[2]={-0.02576,-0,0};curves[1].open[2]=curves[1].closed[2]={-0.000111,0.000117,-0.096028,0.995379};
curves[1].offset[3]={-0.02582,-0,-0};curves[1].open[3]=curves[1].closed[3]={1.4e-05,-2.9e-05,0.032859,0.99946};
curves[2].count=4;
curves[2].offset[0]={-0.08096,-0.01886,0.0033};curves[2].open[0]=curves[2].closed[0]={-0.084887,0.040208,-0.11179,0.989283};
curves[2].offset[1]={-0.04711,0,-0};curves[2].open[1]=curves[2].closed[1]={-0.000397,-0.000364,-0.130701,0.991422};
curves[2].offset[2]={-0.02745,0,0};curves[2].open[2]=curves[2].closed[2]={3.3e-05,0.000372,-0.097388,0.995246};
curves[2].offset[3]={-0.02696,-0,0};curves[2].open[3]=curves[2].closed[3]={-0.000955,0.000277,0.041026,0.999158};
curves[3].count=5;
curves[3].offset[0]={-0.01495,-0.01191,0.00657};curves[3].open[0]=curves[3].closed[0]={-0.201151,0.126245,0.027437,0.971003};
curves[3].offset[1]={-0.06351,0,0};curves[3].open[1]=curves[3].closed[1]={0.083039,0.057809,-0.141434,0.984764};
curves[3].offset[2]={-0.04429,0,0};curves[3].open[2]=curves[3].closed[2]={-0.001237,0.002278,-0.135127,0.990825};
curves[3].offset[3]={-0.02512,-0,-0};curves[3].open[3]=curves[3].closed[3]={0.000496,0.001249,-0.098541,0.995132};
curves[3].offset[4]={-0.02648,0,0};curves[3].open[4]=curves[3].closed[4]={1.4e-05,0.000101,0.013442,0.99991};
curves[4].count=5;
curves[4].offset[0]={-0.01226,-0.00714,0.01697};curves[4].open[0]=curves[4].closed[0]={-0.255747,0.192572,-0.015766,0.947239};
curves[4].offset[1]={-0.05685,-0,0};curves[4].open[1]=curves[4].closed[1]={0.090176,0.078927,-0.081182,0.989469};
curves[4].offset[2]={-0.03672,0,-0};curves[4].open[2]=curves[4].closed[2]={-0.001474,0.001192,-0.136572,0.990628};
curves[4].offset[3]={-0.0209,-0,-0};curves[4].open[3]=curves[4].closed[3]={0.000726,-0.000991,-0.103472,0.994632};
curves[4].offset[4]={-0.02205,0,-0};curves[4].open[4]=curves[4].closed[4]={0.009227,-0.032207,0.0714,0.996885};
a[0]=hs::make(curves,input,pc::Quat{0.8824157,-0.3398565,-0.2381403,0.2216514});
}
{pc::FingerCurve103 curves[5]{}; float input[5]{};
curves[0].count=4;
curves[0].offset[0]={0.01569,-0.00557,0.0197};curves[0].open[0]=curves[0].closed[0]={0.454337,-0.28379,0.076896,0.840909};
curves[0].offset[1]={0.04798,-0,-0};curves[0].open[1]=curves[0].closed[1]={0.009924,0.015259,-0.203826,0.978838};
curves[0].offset[2]={0.03202,0,0};curves[0].open[2]=curves[0].closed[2]={0.000865,0.001242,-0.098764,0.99511};
curves[0].offset[3]={0.03111,0,-0};curves[0].open[3]=curves[0].closed[3]={-0.000288,0.023708,-0.036747,0.999043};
curves[1].count=4;
curves[1].offset[0]={0.08105,0.01928,0.02061};curves[1].open[0]=curves[1].closed[0]={-0.012343,-0.051268,-0.088198,0.994706};
curves[1].offset[1]={0.04279,-0,-0};curves[1].open[1]=curves[1].closed[1]={-0.002036,0.000733,-0.131596,0.991301};
curves[1].offset[2]={0.02576,0,-0};curves[1].open[2]=curves[1].closed[2]={-0.000149,-0.000167,-0.094044,0.995568};
curves[1].offset[3]={0.02582,-0,0};curves[1].open[3]=curves[1].closed[3]={0.000192,-0.001194,0.018562,0.999827};
curves[2].count=4;
curves[2].offset[0]={0.08097,0.01886,-0.0033};curves[2].open[0]=curves[2].closed[0]={-0.087717,0.039877,-0.111747,0.989054};
curves[2].offset[1]={0.04711,0,0};curves[2].open[1]=curves[2].closed[1]={-0.000122,0.000122,-0.132987,0.991118};
curves[2].offset[2]={0.02745,-0,-0};curves[2].open[2]=curves[2].closed[2]={0.000206,0.00092,-0.095088,0.995468};
curves[2].offset[3]={0.02696,-0,0};curves[2].open[3]=curves[2].closed[3]={-0.001121,0.001768,0.024445,0.999699};
curves[3].count=5;
curves[3].offset[0]={0.01495,0.01191,-0.00657};curves[3].open[0]=curves[3].closed[0]={-0.20088,0.125158,0.029954,0.971126};
curves[3].offset[1]={0.06351,-0,0};curves[3].open[1]=curves[3].closed[1]={0.077857,0.05896,-0.141216,0.98515};
curves[3].offset[2]={0.04429,-0,-0};curves[3].open[2]=curves[3].closed[2]={-0.000351,0.00399,-0.134291,0.990934};
curves[3].offset[3]={0.02512,0,0};curves[3].open[3]=curves[3].closed[3]={0.000822,0.002869,-0.093181,0.995645};
curves[3].offset[4]={0.02648,-0,-0};curves[3].open[4]=curves[3].closed[4]={-0.000155,0.00135,0.005767,0.999982};
curves[4].count=5;
curves[4].offset[0]={0.01225,0.00714,-0.01697};curves[4].open[0]=curves[4].closed[0]={-0.255595,0.190122,-0.014298,0.947798};
curves[4].offset[1]={0.05686,0,0};curves[4].open[1]=curves[4].closed[1]={0.083339,0.079702,-0.079204,0.990166};
curves[4].offset[2]={0.03672,-0,-0};curves[4].open[2]=curves[4].closed[2]={-7.4e-05,0.004058,-0.136539,0.990626};
curves[4].offset[3]={0.0209,-0,0};curves[4].open[3]=curves[4].closed[3]={0.001641,0.00231,-0.101394,0.994842};
curves[4].offset[4]={0.02205,0,-0};curves[4].open[4]=curves[4].closed[4]={0.00508,-0.01729,0.045816,0.998787};
a[1]=hs::make(curves,input,pc::Quat{-0.3441531,-0.289261,0.2890076,0.8451989});
}
result[n++]={{a[0],a[1]},{p[0],p[1]}};
}
return result;
}
}
