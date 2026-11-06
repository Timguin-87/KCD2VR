#pragma once
#include <cmath>
#include <cstdint>
namespace kcd2vr::finisher_observation_c2 {
// LOG-ONLY evidence, not a sever/kill permission. Numeric body-material 38 and
// runtime surface 116 remain candidates until their contact domain is proven.
enum Failure : unsigned {NativeUnaccepted=1u,NotApplied=2u,FieldsUnknown=4u,
    NotBody=8u,ContinuationUnknown=16u,NotLethal=32u,NotCut=64u,
    NotNeckCandidate=128u,NotFleshSurfaceCandidate=256u,EnergyInsufficient=512u,
    DefendedOrUnknown=1024u};
struct Input {
    bool accepted=false,applied=false,fields=false,body=false,continued=false;
    int death=-1,kind=-1,bodyMaterial=-1,surface=-1;
    int blocked=-1,perfect=-1,parry=-1;
    float normalizedEnergy=0,minimum=.6f;
    int victimPhaseBefore=-1,victimPhaseAfter=-1;
};
struct Result {
    unsigned failures=0;
    bool candidate=false,postPhase32OrHigher=false;
    bool bodyMaterialDomainProven=false,severBoundaryProven=false;
};
inline Result evaluate(const Input& i) noexcept {
    Result r{};
    if(!i.accepted)r.failures|=NativeUnaccepted;
    if(!i.applied)r.failures|=NotApplied;
    if(!i.fields)r.failures|=FieldsUnknown;
    if(!i.body)r.failures|=NotBody;
    if(!i.continued)r.failures|=ContinuationUnknown;
    if(!i.fields||i.death!=1)r.failures|=NotLethal;
    if(i.kind!=0)r.failures|=NotCut;
    if(i.bodyMaterial!=38)r.failures|=NotNeckCandidate;
    if(i.surface!=116)r.failures|=NotFleshSurfaceCandidate;
    if(!std::isfinite(i.normalizedEnergy)||!std::isfinite(i.minimum)||
       i.minimum<0||i.minimum>1||i.normalizedEnergy<i.minimum)r.failures|=EnergyInsufficient;
    if(i.blocked!=0||i.perfect!=0||i.parry!=0)r.failures|=DefendedOrUnknown;
    // Post-submit phase may be death, hit reaction, recovery or native guard.
    // Keep it as an annotation; never call it an eligibility ordinal or reject
    // a lethal contact just because its native consequence changed this value.
    r.postPhase32OrHigher=i.victimPhaseAfter>=32;
    r.candidate=r.failures==0;
    return r;
}
}
