"""Actual T24 source consumers; source-only protected proposal + MSVC ASAN.

Never modifies protected source or executes a game/native DLL. The optional
syntax check compiles an external, explicitly patched copy of the actual TU.
"""
import argparse,difflib,hashlib,json,subprocess,shutil
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

def one(s,a,b):
    assert s.count(a)==1,(a[:90],s.count(a))
    return s.replace(a,b)

def function(s,name):
    start=s.index(name);start=s.rfind('\n',0,start)+1
    begin=s.index('{',start);depth=1;at=begin+1
    while depth:
        depth+=(s[at]=='{')-(s[at]=='}');at+=1
    return s[start:at]+'\n'

def proposal():
    changes={}
    def src(p):return (ROOT/p).read_text()
    p='src/frik_pose_probe.cpp';s=src(p)
    s=one(s,'#include "subsystems/interactions/hand_item_state.inl" // M2_OWNED_SOURCE',
        'bool HandClaimBlocksT24(int side) noexcept {\n'
        '    return HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),g_pose_epoch6689.load(),g_weapon_request_epoch6694.load());\n'
        '}\n#include "subsystems/interactions/hand_item_state.inl" // M2_OWNED_SOURCE')
    changes[p]=s
    p='src/frik_pose_probe.h';s=src(p)
    s=one(s,'int ReadPhysicalHandHeld55(int side,std::uint64_t now) noexcept;',
        'int ReadPhysicalHandHeld55(int side,std::uint64_t now) noexcept;\n'
        '// T24: per-hand competing utility exclusion; no native getter or hand-key write.\n'
        'bool HandClaimBlocksT24(int side) noexcept;')
    changes[p]=s
    p='src/vrik/fist_native.inl';s=src(p)
    s=one(s,'    f.player=owner.player;f.sample.empty[0]=f.sample.empty[1]=f.sample.valid;',
        '    f.player=owner.player;f.sample.empty[0]=f.sample.empty[1]=f.sample.valid;\n'
        '    // T24_FIST_CLAIM_BEGIN\n'
        '    for(int sideT24=0;sideT24<2;++sideT24){\n'
        '        const bool claimedT24=HandClaimBlockedT24(sideT24,packet.stamp.actor,packet.stamp.epoch,g_weapon_request_epoch6694.load());\n'
        '        f.sample.claimedT24[sideT24]=claimedT24;\n'
        '        contactH11.valid[sideT24]=contactH11.valid[sideT24]&&!claimedT24;\n'
        '    }\n    // T24_FIST_CLAIM_END')
    s=one(s,'now-n.pendingTick>150||!frame.sample.empty[0]||!frame.sample.empty[1]||',
        'now-n.pendingTick>150||n.side<0||n.side>1||!frame.sample.empty[n.side]||frame.sample.claimedT24[n.side]||\n'
        '           HandClaimBlockedT24(n.side,frame.sample.actor,n.epoch,g_weapon_request_epoch6694.load())||')
    s=one(s,'(!frame.sample.empty[0]||!frame.sample.empty[1])?fp3::PendingOccupied:fp3::GripReleased;',
        '(n.side<0||n.side>1||!frame.sample.empty[n.side])?fp3::PendingOccupied:fp3::GripReleased;')
    s=one(s,'    if(frame.sample.responsiveCombatH27&&!owner.responsiveCombatH27)',
        '    if(frame.punch.valid&&HandClaimBlockedT24(frame.punch.side,frame.sample.actor,frame.sample.epoch,g_weapon_request_epoch6694.load()))frame.punch={};\n'
        '    if(frame.sample.responsiveCombatH27&&!owner.responsiveCombatH27)')
    # Recheck after restoring a formerly unclaimed punch parked for native
    # guard release. The early frame.punch check cannot cover this restoration.
    s=one(s,'!fm::held(frame.sample,punch.side)){FistCancel101();return;}',
        '!fm::held(frame.sample,punch.side)||HandClaimBlockedT24(punch.side,frame.sample.actor,frame.sample.epoch,g_weapon_request_epoch6694.load())){FistCancel101();return;}')
    s=one(s,'    if(!f.sample.valid||pending.epoch!=f.sample.epoch||pending.actor!=f.sample.actor||',
        '    if(!f.sample.valid||pending.side<0||pending.side>1||(!f.sample.empty[pending.side]||f.sample.claimedT24[pending.side])||pending.epoch!=f.sample.epoch||pending.actor!=f.sample.actor||')
    s=one(s,'frame.sample.grip[n.side],frame.sample.trigger[n.side]);FistCancel101(why);return;}',
        '(n.side>=0&&n.side<2?frame.sample.grip[n.side]:0.f),(n.side>=0&&n.side<2?frame.sample.trigger[n.side]:0.f));FistCancel101(why);return;}')
    changes[p]=s
    p='src/vrik/fist_motion.h';s=src(p)
    s=one(s,'    bool valid=false,empty[2]{},blocked[2]{};',
        '    bool valid=false,empty[2]{},blocked[2]{};\n    bool claimedT24[2]{}; // independent mod owner; native empty-only eligibility is unchanged')
    s=one(s,'side>=0&&side<2&&in.grip[side]', 'side>=0&&side<2&&!in.claimedT24[side]&&in.grip[side]')
    s=one(s,'    if(s.sequence==in.sequence&&s.epoch==in.epoch&&s.actor==in.actor)',
        '    for(int side=0;side<2;++side)if(in.claimedT24[side])s.hand[side]={};\n    if(s.sequence==in.sequence&&s.epoch==in.epoch&&s.actor==in.actor)')
    s=one(s,'        auto& h=s.hand[side];const auto relative=',
        '        if(in.claimedT24[side]){if(trace){trace->hand[side].reject=Invalid;trace->hand[side].after=Phase::Release;}continue;}\n        auto& h=s.hand[side];const auto relative=')
    changes[p]=s
    p='src/vrik/coherent_pose.inl';s=src(p)
    s=one(s,'        const bool occupiedH24=heldFingerKey[side]||',
        '        const bool occupiedH24=HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,g_weapon_request_epoch6694.load())||heldFingerKey[side]||')
    s=one(s,'(nativeFingerCapture?8u:0u)|(item<0?16u:0u)|(bowString?32u:0u)|(propHeld5916c1[side]?64u:0u)|(nativePassengerHand46[side]?128u:0u);',
        '(nativeFingerCapture?8u:0u)|(item<0?16u:0u)|(bowString?32u:0u)|(propHeld5916c1[side]?64u:0u)|(nativePassengerHand46[side]?128u:0u)|\n'
        '            (HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,g_weapon_request_epoch6694.load())?256u:0u);')
    s=one(s,'        bool objectFingers=!nativeFingerCapture&&(readingVisual32||nativePassengerHand46[side]);',
        '        if(!HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,g_weapon_request_epoch6694.load())){ // T24: only competing finger writers yield; arm composition stays below.\n'
        '        bool objectFingers=!nativeFingerCapture&&(readingVisual32||nativePassengerHand46[side]);')
    s=one(s,'        if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);\n        if(std::fabs(Len(Sub(FromT(s.work[f])',
        '        } // T24 native preview keeps evaluated locals on this hand.\n        if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);\n        if(std::fabs(Len(Sub(FromT(s.work[f])')
    changes[p]=s
    p='src/mod.cpp';s=src(p)
    s=one(s,'                .quick_talk_open54=quickTalkOpenH19',
        '                .quick_talk_open54=quickTalkOpenH19,\n'
        '                .hand_claim_blocked_t24={frik_pose_probe::HandClaimBlocksT24(0),frik_pose_probe::HandClaimBlocksT24(1)}')
    changes[p]=s
    p='src/action_time_owner59c11.cpp';s=src(p)
    s=one(s,'        input.grip[use.side]>=g_cancel_grip.load(std::memory_order_acquire);',
        '        input.grip[use.side]>=g_cancel_grip.load(std::memory_order_acquire)&&\n'
        '        !frik_pose_probe::HandClaimBlocksT24(use.side);')
    changes[p]=s
    return changes

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--syntax',action='store_true');a=ap.parse_args();out=a.out.resolve()
    if out==ROOT or ROOT in out.parents:raise ValueError('external output required')
    out.mkdir(parents=True,exist_ok=True);changes=proposal();diff=[]
    for p,s in changes.items():
        diff.extend(difflib.unified_diff((ROOT/p).read_text().splitlines(True),s.splitlines(True),fromfile='a/'+p,tofile='b/'+p))
        f=out/'proposal'/p;f.parent.mkdir(parents=True,exist_ok=True);f.write_text(s)
    (out/'T24_PROTECTED_CONSUMERS_PROPOSED.patch').write_text(''.join(diff))
    # Full actual map/manual readers; the fist statements are copied verbatim
    # from the proposed production publisher, not a mirrored policy wrapper.
    fist=changes['src/vrik/fist_native.inl'].split('// T24_FIST_CLAIM_BEGIN\n',1)[1].split('// T24_FIST_CLAIM_END',1)[0]
    pending_trace='fp3::native(fp3::Stage::Pending,why'+changes['src/vrik/fist_native.inl'].split('fp3::native(fp3::Stage::Pending,why',1)[1].split(';FistCancel101',1)[0]+';'
    consumers=(function((ROOT/'src/subsystems/vrmap/committed_pose_service.inl').read_text(),'bool ReadHeldMapLeftPoseM1(')+
        function((ROOT/'src/subsystems/ranged_reload/manual_reload_attachment19c4r4.inl').read_text(),'static bool ManualHandAttached19c4r4(')+
        'static void ActualFistClaimBlockT24(){\n'+fist+'}\n'+
        'static void ActualPendingTraceT24(){const unsigned why=0;const auto now=std::uint64_t(GetTickCount64());'+pending_trace+'}\n')
    for red in [False,True]:
        target=out/('red' if red else 'green');target.mkdir(exist_ok=True)
        (target/'actual_fist_motion_t24.h').write_text(changes['src/vrik/fist_motion.h'])
        (target/'actual_consumers_t24.inc').write_text(consumers.replace('HandClaimBlockedT24(','NoClaimT24(') if red else consumers)
    vc=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
    xr=Path('D:/KCD2VR-review-builds/T15_GHOST_THROW_HOLSTERS_20261021/windows/_deps/openxr_sdk_source-build/include')
    test=ROOT/'tests/subsystems/throw_drop/hand_claim_t24_test.cpp';runs={}
    for red in [False,True]:
        target=out/('red' if red else 'green');cmd=target/'run.cmd'
        cmd.write_text('@echo off\ncall "'+str(vc)+'" >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d "'+str(target)+'"\n'
            'cl /nologo /std:c++latest /EHsc /Od /Zi /UNDEBUG /DWIN32_LEAN_AND_MEAN /DNOMINMAX /I "E:/backup/DetourModKit/include" /fsanitize=address /DT24_CAUSAL_RED='+str(int(red))+
            ' /I "'+str(ROOT/'src')+'" /I "'+str(ROOT/'src/vrik')+'" /I "'+str(xr)+'" /I "'+str(target)+'" "'+str(test)+'" /Fe:test.exe\n'
            'if errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
        run=subprocess.run(['cmd.exe','/d','/c',str(cmd)],capture_output=True,text=True)
        log=run.stdout+run.stderr;(target/'RUN.log').write_text(log)
        rows=[x for x in log.splitlines() if x.startswith('{"checks":')]
        runs['red' if red else 'green']={'exit_code':run.returncode,'fixture':json.loads(rows[-1]) if rows else None}
        print(('RED' if red else 'GREEN'),log)
    syntax={}
    if a.syntax:
        staged=out/'syntax_source'
        shutil.copytree(ROOT/'src',staged/'src',dirs_exist_ok=True)
        for p,s in changes.items():(staged/p).write_text(s)
        inc=[staged/'src',staged/'src/vrik',xr,Path('D:/KCD2VR-review-builds/M31_DIRECT_PARCHMENT_20261022/windows/_deps/openxr_sdk_source-build/include'),Path('E:/backup/DetourModKit/include'),Path('E:/backup/DetourModKit/include/DirectXMath')]
        for tu in ['src/frik_pose_probe.cpp','src/action_time_owner59c11.cpp','src/mod.cpp']:
            cmd=out/('syntax_'+Path(tu).stem+'.cmd')
            cmd.write_text('@echo off\ncall "'+str(vc)+'" >nul\ncd /d "'+str(ROOT)+'"\n'
                'cl /nologo /Zs /std:c++latest /EHsc /bigobj /DWIN32_LEAN_AND_MEAN /DNOMINMAX /DKCD2VR_PROXY_DINPUT8=1 '+
                ' '.join('/I"'+str(p)+'"' for p in inc)+' "'+str(staged/tu)+'"\nexit /b %errorlevel%\n')
            run=subprocess.run(['cmd.exe','/d','/c',str(cmd)],capture_output=True,text=True)
            (out/('SYNTAX_'+Path(tu).stem+'.log')).write_text(run.stdout+run.stderr);syntax[tu]=run.returncode
            print('SYNTAX',tu,run.returncode)
    controls={};inherited_controls={}
    if a.syntax:
        (staged/'tests').mkdir(exist_ok=True)
        for name in ['fist_motion_test.cpp','fist_responsive_h27_test.cpp','fist_qualification_h29_test.cpp','quantized_fists_test.cpp']:
            shutil.copyfile(ROOT/'tests'/name,staged/'tests'/name)
            cmd=out/('control_'+Path(name).stem+'.cmd')
            cmd.write_text('@echo off\ncall "'+str(vc)+'" >nul\ncd /d "'+str(out)+'"\n'
                'cl /nologo /std:c++latest /EHsc /Od /Zi /UNDEBUG /DNOMINMAX /fsanitize=address /I "'+str(staged/'src')+'" "'+str(staged/'tests'/name)+'" /Fe:control.exe\n'
                'if errorlevel 1 exit /b %errorlevel%\ncontrol.exe\nexit /b %errorlevel%\n')
            run=subprocess.run(['cmd.exe','/d','/c',str(cmd)],capture_output=True,text=True)
            (out/('CONTROL_'+Path(name).stem+'.log')).write_text(run.stdout+run.stderr);controls[name]=run.returncode
            print('CONTROL',name,run.returncode)
            if name=='fist_responsive_h27_test.cpp' and run.returncode:
                baseline=out/'control_parent.cmd'
                baseline.write_text('@echo off\ncall "'+str(vc)+'" >nul\ncd /d "'+str(out)+'"\n'
                    'cl /nologo /std:c++latest /EHsc /Od /Zi /UNDEBUG /DNOMINMAX /fsanitize=address /I "'+str(ROOT/'src')+'" "'+str(ROOT/'tests'/name)+'" /Fe:parent_control.exe\n'
                    'if errorlevel 1 exit /b %errorlevel%\nparent_control.exe\nexit /b %errorlevel%\n')
                parent=subprocess.run(['cmd.exe','/d','/c',str(baseline)],capture_output=True,text=True)
                (out/'CONTROL_PARENT_fist_responsive_h27_test.log').write_text(parent.stdout+parent.stderr)
                marker='FAIL 2 actual combat Grip arms without120ms quiet or Trigger'
                inherited_controls[name]={'parent_exit':parent.returncode,'proposal_exit':run.returncode,
                    'exact_same_assertion':parent.returncode==run.returncode==2 and marker in parent.stderr and marker in run.stderr,
                    'reason':'Retired H27 Grip-only expectation; current H29 requires both analogs >60%. No behavior changed to satisfy obsolete expectation.'}

    report={'native_execution':False,'installed':False,'protected_source_modified':False,'protected_consumers':'PROPOSED_UNAPPLIED',
        'runs':runs,'syntax':syntax,'unchanged_fist_controls':controls,'inherited_control_failures':inherited_controls,'source_hashes':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [ROOT/'src/subsystems/throw_drop/hand_claim_t24.h',test]},
        'passed':runs['green']['exit_code']==0 and runs['red']['exit_code']==3 and runs['red']['fixture']['failures']==8 and all(v==0 for v in syntax.values()) and all(v==0 or inherited_controls.get(k,{}).get('exact_same_assertion',False) for k,v in controls.items()),
        'limits':['Native identity/pose/helper leaves are substituted; actual policy, actual map/manual readers, utility TU and proposed fist publisher statements are compiled.',
                  'Protected source proposal must be reviewed/applied by root; current H37 utility policy must be retained during merge. No headset/native capture claim.']}
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');return 0 if report['passed'] else 1

if __name__=='__main__':raise SystemExit(main())
