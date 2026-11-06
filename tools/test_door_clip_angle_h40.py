"""Execute the actual H40 pure angle helper in Lua5.1; no engine calls modeled.
Expose an existing lupa.lua51 installation through PYTHONPATH. The helper/data
section is extracted verbatim; operation/animation/quest callbacks are not copied.
"""
from pathlib import Path
import argparse,hashlib,json,math
try:
    from lupa.lua51 import LuaRuntime
except ImportError as e:
    raise SystemExit('This test requires lupa.lua51; expose the installed package via PYTHONPATH.')from e
ROOT=Path(__file__).resolve().parents[1]
BEGIN='-- H40_CLIP_ANGLE_BEGIN:'
END='-- H40_CLIP_ANGLE_END'
PARENT_WITHOUT_HELPER='e263eda252776b240146676705356db87eed5ec1267d7cfdb461fc60abe502c8'

def load_helper(runtime):
    text=Path(runtime).read_text(encoding='utf-8-sig')
    if BEGIN not in text or END not in text:raise AssertionError('MISSING_PRODUCTION_H40_CLIP_ANGLE_HELPER')
    start=text.index(BEGIN);stop=text.index(END,start)+len(END)
    helper=text[start:stop]
    L=LuaRuntime(unpack_returned_tuples=True)
    # Any incidental native/script IO would fail, rather than silently no-op.
    L.execute("D={};local noio=setmetatable({},{__index=function()error('pure helper performed IO')end});System=noio;Script=noio;Game=noio")
    L.execute(helper)
    return L,L.globals().D.ClipAngleH40

def values(value):return value if isinstance(value,tuple)else(value,)
def scalar(value):return values(value)[0]
def vec(L,p):return L.table_from({'x':p[0],'y':p[1],'z':p[2]})
def dot(a,b):return sum(x*y for x,y in zip(a,b))
def cross(a,b):return(a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0])
def unit(a):n=math.sqrt(dot(a,a));return tuple(x/n for x in a)
def point(root,axis,u,v,angle,radius,height):return tuple(root[i]+radius*(u[i]*math.cos(angle)+v[i]*math.sin(angle))+axis[i]*height for i in range(3))

def main():
    a=argparse.ArgumentParser();a.add_argument('--runtime',type=Path,default=ROOT/'mods/KCD2VRDoors/hand_only_h40.lua');a.add_argument('--output',type=Path,required=True)
    a.add_argument('--require-unmodified-lifecycle',action='store_true',help='Use only for the original helper-only addition, before wiring the operation consumer.');args=a.parse_args()
    from verify_door_clip_angle_h40 import LOCATORS,DBA_SHA
    L,H=load_helper(args.runtime);checks=0;scenarios={}
    def check(ok,name):
        nonlocal checks
        checks+=1;scenarios[name]=scenarios.get(name,0)+1
        if not ok:raise AssertionError(name)
    text=args.runtime.read_text();start=text.index(BEGIN);stop=text.index(END,start)+len(END)
    stripped=text[:start]+text[stop+2:]
    lifecycle_sha=hashlib.sha256(stripped.encode()).hexdigest()
    if args.require_unmodified_lifecycle:
        check(lifecycle_sha==PARENT_WITHOUT_HELPER,'all_existing_H40_lifecycle_bytes_unchanged')
    check(H.assetSha256==DBA_SHA and H.version==1,'exact_asset_identity')
    aliases=[x[0]for x in LOCATORS]
    check(len(set(aliases))==16,'sixteen_native_object_aliases')
    for alias in aliases:
        ys=[scalar(H.At(alias,i/512))for i in range(513)];lo,hi=min(ys),max(ys)
        for p in [0,.1,.125,.25,.5,.75,.9,1]:
            target=scalar(H.At(alias,p));actual,err=H.Inverse(alias,target,p)
            check(abs(actual-p)<1e-12 and err<1e-12,'stationary_join_plateau_and_branch_retention')
            for _ in range(10):actual,err=H.Inverse(alias,target,actual)
            check(abs(actual-p)<1e-12,'unchanged_target_no_accumulation')
        prior=0
        for i in list(range(201))+list(range(200,-1,-1)):
            target=lo+(hi-lo)*i/200;actual,err=H.Inverse(alias,target,prior)
            check(0<=actual<=1 and abs(scalar(H.At(alias,actual))-target)<1e-10 and err<1e-10,'both_directions_all_available_angles_including_overshoot')
            prior=actual
        for native_phase,wanted_phase in [(.1,.65),(.45,.65),(.7,.4),(.9,.3)]:
            native_angle=scalar(H.At(alias,native_phase));wanted=scalar(H.At(alias,wanted_phase));delta=wanted-native_angle
            for a in [(0,0,1),(1,0,0),unit((.3,-.4,.8660254038))]:
                u=unit(cross(a,(0,1,0)if abs(a[1])<.9 else(1,0,0)));v=cross(a,u)
                root=(101.,-32.,7.);pivot=tuple(root[i]+a[i]*1.7 for i in range(3))
                for angle in [math.radians(179),math.radians(-179),0.]:
                    for radius in [.001,.6,2.]:
                        hand0=point(root,a,u,v,angle,radius,.73);hand1=point(root,a,u,v,angle+delta,radius,-.24)
                        join=H.Join(alias,native_phase,vec(L,root),vec(L,pivot),vec(L,hand0))
                        check(not isinstance(join,tuple),'join_exact_current_hinge_axis')
                        actual,err,target=H.Map(join,native_phase,vec(L,root),vec(L,pivot),vec(L,hand1))
                        check(abs(target-wanted)<1e-10 and err<1e-10 and abs(scalar(H.At(alias,actual))-wanted)<1e-10,'fixed_join_cross_dot_world_wrap_tilt_translation_radius_and_height')
                        expected=scalar(H.Inverse(alias,wanted,native_phase))
                        if abs(actual-expected)>=1e-7:raise AssertionError(('same_selected_clip_nearest_prior_continuity',alias,native_phase,wanted_phase,a,angle,radius,actual,expected,target,wanted))
                        check(True,'same_selected_clip_nearest_prior_continuity')
                        returned=H.Map(join,actual,vec(L,root),vec(L,pivot),vec(L,hand0))
                        check(abs(values(returned)[2]-native_angle)<1e-10,'returning_hand_uses_fixed_join')
    check(scalar(H.At('stealth_door_l_b_c_player',1))>math.pi/2-.001,'selected_close_curve_direction_retained')
    check(min(scalar(H.At('stealth_door_l_b_c_player',i/1024))for i in range(1025))<0,'native_stealth_anticipation_not_erased')
    check(max(scalar(H.At('stealth_door_l_b_c_player',i/1024))for i in range(1025))>math.pi/2,'stealth_overshoot_not_clamped_at_90degrees')
    zero=vec(L,(0,0,0));up=vec(L,(0,0,1));side=vec(L,(1,0,0))
    for result in [H.At('missing_clip',.5),H.At(aliases[0],float('nan')),H.Inverse(aliases[0],float('inf'),.5),
                   H.Join(aliases[0],.5,zero,zero,side),H.Join(aliases[0],.5,zero,up,up),H.Map(None,.5,zero,up,side)]:
        check(scalar(result)is None,'undefined_math_returns_unavailable_without_native_action_or_fallback')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    receipt={'status':'PASS','checks':checks,'scenarios':scenarios,'runtime_sha256':hashlib.sha256(args.runtime.read_bytes()).hexdigest(),'test_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
      'helper_only':True,'lifecycle_sha256':lifecycle_sha,'unmodified_lifecycle_required':args.require_unmodified_lifecycle,'lua':'Lua5.1 via lupa.lua51','limits':['This test executes only the actual pure helper; operation consumers are verified separately by test_hand_only_doors_h40.py.',
      'No native calls, animation ownership, release/partial-stop, retail engine or headset acceptance is asserted.',
      'Current root/pivot world points are explicit pure inputs; caller still supplies exact current native door/clip identity.',
      'Fixed world join radial assumes the ordinary door hinge frame does not independently rotate about its own axis during one acquired operation.']}
    args.output.write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
