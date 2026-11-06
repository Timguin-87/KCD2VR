#!/usr/bin/env python3
"""Run production finite-recording helpers for reload, knife stock and combo evidence.

Native/game actions are not executed. Original helpers and capture mailboxes
run against a controlled clock and an explicit log sink; no extra log files are
introduced into the game. Outputs and host executable stay in --out.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
from test_crossbow_mechanism_d78 import function

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--out', type=Path, required=True)
a = p.parse_args()
out = a.out.resolve()
out.mkdir(parents=True, exist_ok=True)
carrier = ROOT / 'src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl'
service = ROOT / 'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl'
combo = ROOT / 'src/subsystems/masterstrike_combos/native_combo_grace.inl'
extracted = '\n'.join([
    function(carrier.read_text(), 'static void AmmoNoteT18('),
    function(carrier.read_text(), 'static std::uint64_t AmmoNoteSessionK39('),
    function(service.read_text(), 'static void AmmoNoteRefreshD86('),
    function(combo.read_text(), 'static void NoteF1('),
])
host = r'''
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include "subsystems/throw_drop/ammo_readiness_k32.h"
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
namespace cap=kcd2vr::diagnostics::capture168;
namespace rd=kcd2vr::reload_diagnostics_d48;
namespace xc=kcd2vr::crossbow_native_xr34;
static std::uint64_t now=1000;
static std::uint64_t GetTickCount64(){return now;}
static std::vector<std::string> rows;
namespace diagnostic_trace {static void WriteFor(cap::Domain d,const char* line){if(cap::active(d,now))rows.emplace_back(line);}}
struct AmmoReservationReplyT22 {std::uint64_t key=0;int daggers=0,axes=0;char status[24]{};};
static std::atomic<unsigned> g_note_count_f1{0},g_last_result_f1{0},g_last_query_f1{0};
'''
cases = r'''
static unsigned checks=0;
static void check(bool result,const char* why){++checks;if(!result){std::fprintf(stderr,"FAIL_CAPTURE_D86 %s\n",why);std::exit(1);}}
static void off(){for(auto d:{cap::Domain::Interaction,cap::Domain::Combat,cap::Domain::Reload,cap::Domain::Bug,cap::Domain::Vrik})cap::gate.select(d,false,now);}
static void drain(){rd::drain(now,[](cap::Domain,std::uint64_t,const char* line){rows.emplace_back(line);});}
int main(){
 AmmoReservationReplyT22 reply{};std::snprintf(reply.status,sizeof(reply.status),"READY");
 xc::Status st{};st.owner.serial=7;st.owner.source.sample=99;st.owner.source.tick=now;st.owner.source.primary=1;st.owner.source.classId=10;st.clockOwned=true;
 for(int i=0;i<40;++i){AmmoNoteRefreshD86(reply,true,true,1);NoteF1(5,0,7,.5f,1.5f);rd::recordMechanismD86(st,"NO_NATIVE_POSE_WITNESS",now,20,18,4);}
 drain();check(rows.empty(),"all recorders OFF produce no rows");
 check(g_note_count_f1.load()==0&&rd::next.load()==0,"OFF cannot spend combo budget or enqueue reload evidence");
 cap::gate.select(cap::Domain::Interaction,true,now);
 AmmoNoteRefreshD86(reply,true,true,1);
 check(rows.size()==1&&rows.back().find("drawStock=NO_SUPPORTED_STOCK")!=std::string::npos,"accepted zero stock is explicit, not generic ready");
 check(rows.back().find("mount=LEFT_HIP input=LEFT_GRIP")!=std::string::npos,"actual pouch and hand documented");
 AmmoNoteRefreshD86(reply,true,true,1);check(rows.size()==1,"stable inventory reply is change-only");
 reply.daggers=3;AmmoNoteRefreshD86(reply,true,false,1);
 check(rows.back().find("drawStock=UNCONFIRMED")!=std::string::npos,"foreign/changed owner reply cannot advertise stock");
 AmmoNoteRefreshD86(reply,true,true,1);
 check(rows.back().find("drawStock=AVAILABLE")!=std::string::npos,"accepted positive finite stock remains available");
 const auto before=rows.size();NoteF1(5,0,7,.5f,1.5f);check(rows.size()==before,"F5 alone does not enable combat notes");
 rd::recordMechanismD86(st,"NO_NATIVE_POSE_WITNESS",now,20,18,4);
 rd::recordMechanismD86(st,"NO_NATIVE_POSE_WITNESS",now,22,18,4);
 drain();check(rows.size()==before+1,"source publication churn does not spam same mechanism refusal");
 check(rows.back().find("sourcePublication=20 passengerFramePublication=18 nativePublication=4")!=std::string::npos,"mechanism keeps causal publications");
 rd::recordMechanismD86(st,"PUBLISHED",now,22,18,4);drain();
 check(rows.back().find("result=PUBLISHED")!=std::string::npos,"mechanism success transition is observable");
 cap::gate.select(cap::Domain::Vrik,true,now);cap::gate.select(cap::Domain::Reload,true,now);
 const auto multi=rows.size();rd::recordMechanismD86(st,"NATIVE_CLOCK_OR_OWNER",now);drain();
 check(rows.size()==multi+1,"simultaneous F5/F7/F11 produce one receipt");
 off();cap::gate.select(cap::Domain::Combat,true,now);
 const auto combat=rows.size();AmmoNoteRefreshD86(reply,true,true,1);
 check(rows.size()==combat+1,"F3 alone receives current knife stock decision");
 for(int i=0;i<50;++i)NoteF1(1+i%7,0,7,.5f,1.5f);
 check(rows.size()==combat+17,"combo callback notes bounded to 16 per F3 session");
 off();++now;cap::gate.select(cap::Domain::Combat,true,now);
 const auto newSession=rows.size();NoteF1(5,0,7,.5f,1.5f);AmmoNoteRefreshD86(reply,true,true,1);
 check(rows.size()==newSession+2,"new F3 session receives its own first combo and stock decisions");
 const auto limited=rows.size();for(int i=0;i<40;++i){reply.daggers=i;AmmoNoteRefreshD86(reply,true,true,1);}
 check(rows.size()==limited+15,"knife refresh bounded to 16 changes in a finite session");
 off();const auto stopped=rows.size();NoteF1(7,0);AmmoNoteRefreshD86(reply,true,true,1);rd::recordMechanismD86(st,"PUBLISHED",now);drain();
 check(rows.size()==stopped,"stopping capture does not produce later ordinary rows");
 cap::gate.select(cap::Domain::Vrik,true,now);rd::recordMechanismD86(st,"PUBLISHED",now);drain();
 check(rows.size()==stopped+1&&rows.back().find("recorder=VRIK")!=std::string::npos,"F7 alone receives mechanism evidence");
 std::printf("D86 production capture helpers: %u checks PASS; native actions NOT_RUN\n",checks);
}
'''
unit = out / 'combat_capture_d86.cpp'
unit.write_text(host + extracted + cases)
exe = out / 'combat_capture_d86'
command = ['g++', '-std=c++20', '-O1', '-g', '-pthread', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-no-pie', '-I', str(ROOT / 'src'), str(unit), '-o', str(exe)]
build = subprocess.run(command, text=True, capture_output=True)
(out / 'build.log').write_text(build.stdout + build.stderr)
result = {'command': command, 'compile_rc': build.returncode,
          'source_sha256': {str(v.relative_to(ROOT)): hashlib.sha256(v.read_bytes()).hexdigest() for v in [carrier, service, combo]},
          'extracted_sha256': hashlib.sha256(extracted.encode()).hexdigest()}
if build.returncode == 0:
    run = subprocess.run([str(exe)], text=True, capture_output=True, timeout=30,
                         env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'))
    result.update(run_rc=run.returncode, output=run.stdout + run.stderr)
    (out / 'run.log').write_text(run.stdout + run.stderr)
    print(run.stdout + run.stderr)
else:
    print(build.stderr[-6000:])
exe.unlink(missing_ok=True)
(out / 'RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
raise SystemExit(result.get('run_rc', build.returncode))
