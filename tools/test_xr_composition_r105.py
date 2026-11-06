#!/usr/bin/env python3
"""Compile actual layer assembly/end-frame code against an explicit OpenXR callback stub.

This checks the production submission call count and actual submitted-kind mask,
not a second implementation of their policy. OpenXR/D3D12 and headset optics are
not available to this host test. --revision HEAD demonstrates the R104 failures.
"""
from pathlib import Path
import argparse
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument("--revision")
p.add_argument("--sanitize", action="store_true")
args = p.parse_args()

def source(name):
    if args.revision:
        return subprocess.check_output(["git", "show", f"{args.revision}:{name}"], cwd=ROOT, text=True)
    return (ROOT / name).read_text()

cpp = source("src/xr_manager.cpp")
guard = source("src/xr_layer_guard_r104.h")
assembly = cpp[cpp.index("    namespace lgR104=xr_layer_guard_r104;"):cpp.index("    // Receipt from the actual submit gates")]
submit = cpp[cpp.index("    XrResult end_result;", cpp.index("bool XrManager::RunFrame(")):cpp.index("    (void)dice_marker_r92::presented.publish")]
old_guard = "xr_layer_guard_r104::Quarantine layer_guard_r104_{};" if "struct Quarantine" in guard else ""
fixture = r'''
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include "guard.h"
#define XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME 1
using XrResult=int;
constexpr int XR_SUCCESS=0, XR_ERROR_SESSION_LOST=-17, XR_ERROR_INSTANCE_LOST=-13;
constexpr int XR_ERROR_CALL_ORDER_INVALID=-37;
#define XR_SUCCEEDED(r) ((r)>=0)
#define XR_FAILED(r) ((r)<0)
struct Vec {float x=0,y=0,z=0;};
struct Quat {float x=0,y=0,z=0,w=1;};
struct Pose {Quat orientation{};Vec position{};};
struct Rect {struct Offset {std::int32_t x=0,y=0;} offset;struct Extent {std::int32_t width=256,height=256;} extent;};
struct SubImage {std::uintptr_t swapchain=2;Rect imageRect{};unsigned imageArrayIndex=0;};
struct XrCompositionLayerBaseHeader {};
struct XrCompositionLayerQuad {std::uintptr_t space=3;SubImage subImage{};Pose pose{};struct Size {float width=1,height=1;} size;};
struct Cylinder {std::uintptr_t space=3;SubImage subImage{};Pose pose{};float radius=2,centralAngle=1,aspectRatio=1;};
struct XrFrameEndInfo {unsigned layerCount=0;const XrCompositionLayerBaseHeader* const* layers=nullptr;};
namespace diagnostics::xr165 {enum Kind {EndFrame};struct Timer {explicit Timer(Kind){}};}
static unsigned calls=0;static int desired=0;static bool begun=false;static std::uint64_t clockMs=100000;
static std::uint64_t GetTickCount64(){return clockMs;}
static int xrEndFrame(int,const XrFrameEndInfo*){
    ++calls;if(!begun)return XR_ERROR_CALL_ORDER_INVALID;begun=false;return desired;
}
struct Receipt {int result;unsigned calls,count,kinds;};
struct Production {
    @OLD_GUARD@
    unsigned max_layer_count_r104_=13;
    std::uint64_t layer_held_r104_[xr_layer_guard_r104::Count]{};
    void NoteLayerDropR104(unsigned,unsigned){}
    void NoteLayerFixR104(unsigned,unsigned){}
    void NoteEndFrameR104(unsigned,int,bool,int,std::uint32_t,std::uint64_t){}
    void NoteEndFrameR105(unsigned,int,std::uint64_t){}
    void NoteEndFrameR105(unsigned,int,std::uint32_t,std::uint64_t){}
    Receipt frame(int result,unsigned limit=13,bool corruptHud=false,std::uint64_t advanceMs=16){
        begun=true;calls=0;desired=result;clockMs+=advanceMs;max_layer_count_r104_=limit;int session_=1;
        struct Image {std::uintptr_t swapchain=2;int width=256,height=256;} eyes_[2],menu_ui_,xr_hud;
        const bool have_layer=true,flat_game_requested=false,have_flat_game_quad=true,have_flat_game_cylinder=false;
        const bool have_flat_laser=true,have_flat_cursor=true,haveDiceR92=true,have_ui_test_laser=true;
        const bool have_hand_interaction_laser=true,have_hud_quad=true,have_ui_layer=true,use_menu_cylinder=false,have_menu_laser=true;
        XrCompositionLayerQuad projection_layer{},flat_game_quad{},flat_laser_quad{},flat_cursor_quad{},diceQuadR92{};
        XrCompositionLayerQuad ui_test_laser_quad{},hand_interaction_laser_quad{},hud_quad{},menu_quad{},menu_laser_quad{};
        Cylinder flat_game_cylinder{};
        if(corruptHud)hud_quad.pose.orientation.x=std::numeric_limits<float>::quiet_NaN();
        @ASSEMBLY@
        XrFrameEndInfo end_info{layer_count,layer_count?layers:nullptr};
        @SUBMIT@
        return {end_result,calls,layer_count,kindsR104};
    }
};
int main(){
    unsigned checks=0,failed=0;
    const auto check=[&](bool ok,const char* why){++checks;if(!ok){++failed;std::fprintf(stderr,"FAIL: %s\n",why);}};
    namespace g=xr_layer_guard_r104;
    for(int error:{-2,-22,-26,-30,-36,-37,-40,-41,-42,-43,-44,XR_ERROR_SESSION_LOST,XR_ERROR_INSTANCE_LOST}){ // not layer errors
        Production p;const auto bad=p.frame(error);
        check(bad.calls==1,"exactly one xrEndFrame after a begun frame, including an error");
        check(bad.result==error,"retain the actual runtime error");
        const auto good=p.frame(XR_SUCCESS);
        check(good.calls==1&&good.result==XR_SUCCESS,"next begun frame succeeds");
        check((good.kinds&(1u<<g::Hud))&&(good.kinds&(1u<<g::HandLaser)),"healthy HUD and hand laser eligible immediately after a non-layer error");
    }
    for(int error:{-1,-23,-24,-25,-39}){ // R106/R108: layer errors (openxr.h numbers) -> the overlays of that frame sit out 1 s, return one by one, never for the session
        Production p;const auto bad=p.frame(error);
        check(bad.calls==1,"R106: still exactly one xrEndFrame on a layer error");
        const auto next=p.frame(XR_SUCCESS);
        check(next.calls==1&&next.result==XR_SUCCESS&&next.count==1&&next.kinds==0,"R106: the frame after a layer error carries only the projection");
        const auto later=p.frame(XR_SUCCESS,13,false,1000);
        check(later.kinds!=0&&!(later.kinds&(1u<<g::MenuLaser)),"R106: overlays return one by one after 1 s");
        const auto all=p.frame(XR_SUCCESS,13,false,5000);
        check((all.kinds&(1u<<g::Hud))&&(all.kinds&(1u<<g::HandLaser))&&(all.kinds&(1u<<g::MenuLaser)),"R106: everything is back within 6 s");
        // the kind that keeps failing is isolated: 10 s, 20 s, .. 60 s, then tried again
        for(int i=0;i<6;++i){const auto again=p.frame(error,13,false,16);check(again.calls==1,"one call");
            const auto after=p.frame(XR_SUCCESS,13,false,16);check(!(after.kinds&(1u<<g::MenuLaser))||(after.kinds&(1u<<g::Hud)),"some overlay sits out");
            (void)p.frame(XR_SUCCESS,13,false,70000);}
        const auto back=p.frame(XR_SUCCESS,13,false,70000);
        check((back.kinds&(1u<<g::Hud))&&(back.kinds&(1u<<g::MenuLaser)),"R106: no permanent latch - all overlays tried again after the longest sit-out");
    }
    for(unsigned limit=1;limit<=13;++limit){
        Production p;const auto r=p.frame(XR_SUCCESS,limit);
        check(r.count<=limit,"actual layer count respects runtime capacity");
        unsigned maskCount=0;for(unsigned k=0;k<g::Count;++k)maskCount+=(r.kinds>>k)&1u;
        check(maskCount+1==r.count,"receipt mask contains only actual submitted overlays");
    }
    {Production p;const auto bad=p.frame(XR_SUCCESS,13,true);const auto good=p.frame(XR_SUCCESS);
        check(!(bad.kinds&(1u<<g::Hud))&&(bad.kinds&(1u<<g::HandLaser)),"only corrupt current HUD omitted");
        check(good.kinds&(1u<<g::Hud),"next valid HUD recovers with no backoff (a locally dropped layer is never blamed)");}
    XrCompositionLayerQuad q;unsigned fixes=0;
    q.pose.orientation={1e38f,-1e38f,1e38f,1e38f};
    check(g::fixQuad(q,256,256,&fixes)==0,"finite large quaternion can normalize safely");
    const auto& rot=q.pose.orientation;const double n=double(rot.x)*rot.x+double(rot.y)*rot.y+double(rot.z)*rot.z+double(rot.w)*rot.w;
    check(std::isfinite(n)&&std::fabs(n-1)<1e-5,"normalization cannot produce zero quaternion from overflow");
    q={};q.subImage.imageRect.offset={128,200};q.subImage.imageRect.extent={INT32_MAX,INT32_MAX};
    check(g::fixQuad(q,256,256,&fixes)==0&&q.subImage.imageRect.extent.width==128&&q.subImage.imageRect.extent.height==56,"rect clamping without signed overflow");
    q={};q.space=0;check(g::fixQuad(q,256,256,&fixes)!=0,"null layer space rejected locally");
    std::printf("composition: %u checks, %u failures; external runtime callback is stubbed\n",checks,failed);
    return failed?1:0;
}
'''.replace("@OLD_GUARD@", old_guard).replace("@ASSEMBLY@", assembly).replace("@SUBMIT@", submit)
compiler = shutil.which("clang++") or shutil.which("g++")
if not compiler:
    raise SystemExit("A C++17 compiler is required")
with tempfile.TemporaryDirectory(prefix="kcd2vr-composition-r105-") as td:
    temp=Path(td)
    (temp/"guard.h").write_text(guard)
    (temp/"test.cpp").write_text(fixture)
    flags=["-std=c++17", "-O2", "-Wall", "-Wextra", "-Wno-unused-variable"]
    if args.sanitize:
        flags += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    subprocess.run([compiler,*flags,str(temp/"test.cpp"),"-o",str(temp/"test")],check=True)
    raise SystemExit(subprocess.run([str(temp/"test")]).returncode)
