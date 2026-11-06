"""Build actual old/new supply producers and T23 admission with typed leaf fakes.
No ASI/PAK/native execution or installation. Native physics retirement remains
an explicit external ACK, never inferred from our Retire call.
"""
import argparse,hashlib,json,subprocess
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
def extract_function(text,name):
    marker='static bool '+name+'('
    start=text.index(marker);begin=text.index('{',start);depth=1;end=begin+1
    while depth:
        if text[end]=='{':depth+=1
        elif text[end]=='}':depth-=1
        end+=1
    return text[start:end]+'\n'
def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--build-dir',required=True,type=Path)
    ap.add_argument('--out',required=True,type=Path)
    a=ap.parse_args();build=a.build_dir.resolve();assert build!=ROOT and ROOT not in build.parents
    build.mkdir(parents=True,exist_ok=True)
    fixture=ROOT/'tests/subsystems/throw_drop/pending_reach_t23_test.cpp'
    frozen=ROOT/'tests/subsystems/throw_drop/fixtures/pending_reach_t23/frozen_native_supply_publication.inl'
    helper=ROOT/'src/subsystems/throw_drop/holster_gestures.inl'
    inputs=[fixture,frozen,helper,ROOT/'src/subsystems/throw_drop/pending_reach_t23.h',ROOT/'src/subsystems/throw_drop/native_supply_publication.inl']
    hashes={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs}
    (build/'frozen_native_supply_publication.inl').write_bytes(frozen.read_bytes())
    (build/'draw_candidate_t23.inc').write_text(''.join(extract_function(helper.read_text(),name) for name in ['PhysMainWeaponR27','PhysAnotherMainHeldR27','PhysDrawCandidatePolicyR38','PhysDrawCandidate126']))
    producer=inputs[-1].read_text()
    guarded='''const auto observed=t::ObservePending(side,pending);
        if(observed==t::PendingState::Unknown){(void)t::PublishGrant(side,{});continue;}
        if(observed==t::PendingState::Absent)'''
    assert producer.count(guarded)==1
    # Controlled one-hunk counterexample, not a substituted timing policy.
    (build/'unknown_as_absent_t23.inc').write_text(producer.replace(guarded,'if(!t::ReadPending(side,pending))'))
    publication='''ReadCommittedPacket6695(packet)&&publicationT23&&!(publicationT23&1)&&publicationT23==frame.publicationT23&&
        publicationT23==ReadCompletedPublicationT23()&&'''
    assert producer.count(publication)==1
    (build/'publication_unjoined_t23.inc').write_text(producer.replace(publication,'ReadCommittedPacket6695(packet)&&'))
    vc='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    cmd=build/'run.cmd';cmd.write_text('@echo off\ncall "'+vc+'" >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d "'+str(build)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(ROOT/'src')+'" /I "'+str(build)+'" "'+str(fixture)+'" /Fe:test.exe\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
    r=subprocess.run(['cmd.exe','/d','/c',str(cmd)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
    (build/'RUN.log').write_text(r.stdout);print(r.stdout)
    lines=[line for line in r.stdout.splitlines() if line.startswith('{"checks":')]
    report={'native_execution':False,'ASI_built':False,'PAK_built':False,'installed':False,'source_sha256':hashes,
            'exit_code':r.returncode,'fixture':json.loads(lines[-1]) if lines else None,'all_passed':r.returncode==0 and bool(lines),
            'explicit_leaves':['existing native identity getters','ReadCommittedPacket6695 copied payload','HandWorld6694 transform','T4 atomic admission/retirement queries'],
            'actual_production':['full frozen old supply publisher','full new owned publisher','normal PhysDrawCandidate126','hip_t10::update/closest','T23 pending/source/grant API'],
            'limits':['Host native leaves are substitutes, not native execution.','Root must independently compose metadata and suspension bit8 into H36.','Cancel/request is never physical retirement ACK.']}
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
    return 0 if report['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
