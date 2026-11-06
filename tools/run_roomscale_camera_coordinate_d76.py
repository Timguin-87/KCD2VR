from pathlib import Path
import argparse,hashlib,json,os,subprocess
ap=argparse.ArgumentParser();ap.add_argument('--out',required=True);ap.add_argument('--asan',action='store_true');ap.add_argument('--extract-only',action='store_true');a=ap.parse_args()
s=Path(__file__).resolve().parents[1];out=Path(a.out).resolve();out.mkdir(parents=True,exist_ok=True)
c=(s/'src/camera_hook.cpp').read_text()
def block(start,end):
 i=c.index(start);j=c.index(end,i);return c[i:j]
math=block('static Quatf Normalize(Quatf q)','static Quatf SlerpShortest')+block('static Vec3f Rotate(Quatf q, Vec3f v)','// C47:')
old=block('    {\n        // V60.8: tracked translation','    CameraHook::PublishVisualForwardFromCameraMatrix(matrix);')
new=block('        {\n            Vec3f cry_local{0.0f,0.0f,0.0f};','        // V65.09 F5: publish position')
theater=block('        if(!s_apply_rotation.load(std::memory_order_relaxed) && !s_apply_position.load(std::memory_order_relaxed)){','        // V64.19 F5 dialogue takeover lab.')
# Helpers and translation blocks are extracted verbatim from the production TU.
gen='// Generated from camera_hook.cpp by run_roomscale_camera_coordinate_d76.py\n'+math
gen+=block('static void CopyCurrentPoseAndOrigin(', '// Keep RAII out of the SEH-bearing camera function')
selected=block('        float frameX51=0,frameY51=0,frameZ51=0,frameHx55=0,frameHy55=0;','        const Vec3f basis[3]={base_right,base_forward,base_up};')
gen+='\nnr::Origin SelectTrackedOrigin(nr::Origin canonicalOriginD76,std::uint64_t roomRevisionD76,bool&trackedBase48){nr::Origin roomOriginD76=canonicalOriginD76;const bool dialogueEye59c10r2=false,activityEye59c19=false;Vec3f base_pos{},base_forward{0,1,0},base_right{1,0,0},base_up{0,0,1};\n'+selected+'return roomOriginD76;}\n'
for name,body in [('OldCamera',old),('PublishedCamera',theater+new)]:
 gen+='\nbool '+name+'(Matrix34f* matrix,XrPose pose,XrPose origin,Vec3f base_pos,Quatf origin_q,const Vec3f* visual_basis,float world_scale=1,bool record_render_identity=false){\n'
 gen+='const Vec3f base_right=visual_basis[0],base_forward=visual_basis[1],base_up=visual_basis[2];float displayCenterD75[3]{};\n'
 gen+='const bool apply_position=s_apply_position.load(),hmd_final_orientation_authority=true;const int marriage_mode=0;const std::uint64_t pose_frame=1;\n'
 gen+='kcd2vr::native_roomscale_d76::Origin roomOriginD76{};kcd2vr::native_roomscale_d76::ReadTrackingOriginD76(roomOriginD76);\n'+body+'for(int i=0;i<3;++i)lastNameCenterD76R1[i]=displayCenterD75[i];return true;\n}\n'
p=s/'tests/generated/roomscale_camera_coordinate_d76.inc';p.parent.mkdir(exist_ok=True);tmp=p.with_suffix('.tmp');tmp.write_bytes(gen.replace('\r\n','\n').replace('\n','\r\n').encode());os.replace(tmp,p)
if a.extract_only:
 print(str(p));raise SystemExit(0)
exe=out/('coordinate_asan.exe'if a.asan else 'coordinate.exe');obj=out/(exe.stem+'.obj')
cmd='call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncl /nologo /std:c++17 /EHsc /Od /Zi '+('/fsanitize=address 'if a.asan else '')+'"'+str(s/'tests/roomscale_camera_coordinate_d76_test.cpp')+'" /Fe:"'+str(exe)+'" /Fo:"'+str(obj)+'"\nif errorlevel 1 exit /b 1\n"'+str(exe)+'"\n'
bat=out/('run_asan.cmd'if a.asan else 'run.cmd');bat.write_text(cmd)
r=subprocess.run(['cmd','/c',str(bat)],capture_output=True,text=True);(out/('asan.log'if a.asan else 'debug.log')).write_text(r.stdout+r.stderr);print(r.stdout+r.stderr)
(out/'INPUT_HASHES.json').write_text(json.dumps({str(f.relative_to(s)):hashlib.sha256(f.read_bytes()).hexdigest()for f in [s/'src/camera_hook.cpp',s/'src/subsystems/roomscale/coordinate_d76.h',s/'src/subsystems/roomscale/receipt_ledger_d76.h',s/'src/subsystems/roomscale/native_roomscale_d76.cpp',p,s/'tests/roomscale_camera_coordinate_d76_test.cpp']},indent=2));raise SystemExit(r.returncode)
