"""Extract the production state driver; replace only native setter/pin leaves."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import re,hashlib,json
root=Path(__file__).resolve().parents[1]
src=read_source(root/'src/diagnostics/geometry_hit178r27.inl')
a=src.index('namespace vrstate596 {\n');b=src.index('// =====================================================================================================================',a)
block=src[a:b];replaced=[]
start=block.index('// R59.16C1: native preparation');end=block.index('static bool ClockOwnerAlive(',start)
block=block[:start]+'static bool ChargeEffort5916(const void*) noexcept {++hostEffortCalls;return hostEffortOK;}\n'+block[end:]
replaced.append('native effort adapter')
for name,rep in [('Verified','static bool Verified() noexcept {return hostVerified;}'),
 ('DefenseIntentRead19c4','''static bool DefenseIntentRead19c4(std::uintptr_t p,void* out,std::size_t n) noexcept {
#ifdef KCD2VR_DIRECT_PREPARE_H21_TEST
    return HostDefenseIntentReadH21(p,out,n);
#else
    return false;
#endif
}'''),
 ('ReadPhase4Inputs19','static kcd2vr::defense_publication19::Snapshot ReadPhase4Inputs19(const void*,int,int,std::uint64_t) noexcept {return {};}'),
 ('ReadDefenseBoolDiagnostic','static const char* ReadDefenseBoolDiagnostic(const void*,std::uint32_t) noexcept {return "UNKNOWN";}'),
 ('SetInt','''static bool SetInt(const void* hs,std::uint32_t prop,int v) noexcept {
#ifdef KCD2VR_CUT_FEINT62_TEST
    HostObserveWrite62(prop,false);
#endif
    g_last_set={SetReadback::Success,prop,double(v),double(v)};
    if(hostFailProp==prop){g_last_set.result=SetReadback::Unreadable;return false;}
    if(hostExceptionProp==prop){g_setter_exception=true;g_last_set.result=SetReadback::Exception;return false;}
    HostWatchPublication(hs,prop,v);hostInts[{hs,prop}]=v;
#ifdef KCD2VR_DIRECT_PREPARE_H21_TEST
    HostPhaseNotifyH21(hs,prop,v);
#endif
    if(hostMismatchProp==prop){hostInts[{hs,prop}]=hostMismatchInt;g_last_set.result=SetReadback::ReadableMismatch;g_last_set.observed=hostMismatchInt;if(hostMismatchOnce)hostMismatchProp=~0u;return false;}
    return true;}'''),
 ('SetFloat','''static bool SetFloat(const void* hs,std::uint32_t prop,float v) noexcept {
#ifdef KCD2VR_CUT_FEINT62_TEST
    HostObserveWrite62(prop,true);
#endif
    g_last_set={SetReadback::Success,prop,double(v),double(v)};
    if(hostFailProp==prop){g_last_set.result=SetReadback::Unreadable;return false;}
    if(hostExceptionProp==prop){g_setter_exception=true;g_last_set.result=SetReadback::Exception;return false;}
    hostFloats[{hs,prop}]=v;
    if(hostMismatchProp==prop){hostFloats[{hs,prop}]=hostMismatchFloat;g_last_set.result=SetReadback::ReadableMismatch;g_last_set.observed=hostMismatchFloat;return false;}
    return true;}'''),
 ('SetDefense','static bool SetDefense(const void* hs,int value) noexcept {return SetInt(hs,kDefenseZone,value);}'),
 ('SetBool','''static bool SetBool(const void* hs,std::uint32_t prop,bool v) noexcept {
    g_last_set={SetReadback::Success,prop,double(v),double(v)};
#ifdef KCD2VR_DIRECT_PREPARE_H21_TEST
    if(prop==0x5A0)return HostDefenseIntentSetH21(hs,prop,v);
#endif
    return prop==0x480;}'''),
 ('SetClock','static bool SetClock(const void* hs,float value) noexcept {return SetFloat(hs,kHitRemaining,value);}'),
 ('ClockOwnerAlive','static bool ClockOwnerAlive(const void*) noexcept {return hostOwner;}'),
 ('ResolveDefense','static bool ResolveDefense(const void* hs,int committed,const void*& row,int& value) noexcept {return HostResolveDefense(hs,committed,row,value);}'),
 ('ObserveNpcDefense','static void ObserveNpcDefense(const void*,std::uint64_t) noexcept {}'),
 ('ReciprocalOpponent39','''static const void* ReciprocalOpponent39(const void*,const void*& opponentState,bool& reciprocal) noexcept {
#ifdef KCD2VR_CUT_FEINT62_TEST
 return HostReciprocal62(opponentState,reciprocal);
#else
 opponentState=nullptr;reciprocal=false;return nullptr;
#endif
}'''),
 ('OpponentSkill39','static int OpponentSkill39(const void*) noexcept {return -1;}'),
 ('NpcDefense39','''static bool NpcDefense39(const void* target,int& phase,int& zone,bool& henry) noexcept {
#ifdef KCD2VR_CUT_FEINT62_TEST
 return HostNpcDefense62(target,phase,zone,henry);
#else
 phase=zone=-99;henry=false;return false;
#endif
}'''),
 ('DefenseTimingSeed39','static float DefenseTimingSeed39(float seed,std::uint64_t) noexcept {return seed;}'),
 ('EmitDefenseTimingContact39','static void EmitDefenseTimingContact39(unsigned,std::uint64_t,const void*,const void*,int,std::uint64_t) noexcept {}'),
 ('ReadForwardOfferRetirement','static unsigned ReadForwardOfferRetirement(const void* hs,int& phase,int& defense,float& clock) noexcept {++hostOfferReads;phase=I(hs,kActionPhaseProp39);defense=I(hs,kDefenseZone);clock=F(hs,kHitRemaining);return hostOfferReadMask;}')]:
 m=re.search(r'static (?:bool|void|unsigned|int|float|const void\*|const char\*|kcd2vr::defense_publication19::Snapshot) '+name+r'\([^\n]*?\) noexcept \{',block);assert m,name
 i=block.index('{',m.start());j=i+1;d=1
 while d:
  d+=(block[j]=='{')-(block[j]=='}');j+=1
 replaced.append(name);block=block[:m.start()]+rep+block[j:]
# C10R18: the production forward declaration (geometry_hit178r27.inl, outside the extracted block) of Henry's actor.
block='#include \"vrik/defense_timing39.h\"\n#include \"diagnostics/native_defense_intent19c4.h\"\n#include \"diagnostics/defense_publication19.h\"\nnamespace vrstate596 {constexpr std::uintptr_t g_base=0;std::atomic<const void*> g_henry_actor{nullptr};}\n'+block
block='#include "vrik/dirblock39.h"\n'+block
# H21 adds a bounded native-memory graph in the actual Tick fixture. Other
# consumers retain the old fail-closed native leaf/base. No driver is copied.
block=block.replace('constexpr std::uintptr_t g_base=0;', '''constexpr std::uintptr_t g_base=
#ifdef KCD2VR_DIRECT_PREPARE_H21_TEST
0x180000000ull;
#else
0;
#endif
''')
(root/'tests/state_driver5912.inc').write_text('// Generated by tools/extract_state5912.py; native leaves substituted, production control flow intact.\n'+block,newline='\n')
(root/'tests/state_driver5912.provenance.json').write_text(json.dumps({'sourceSha256':hashlib.sha256(src.encode()).hexdigest(),'leaves':replaced},indent=2),newline='\n')
