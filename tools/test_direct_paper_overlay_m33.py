"""Exercise the production map overlay planner; no archive output."""
import json
from build_direct_paper_m31 import plan


def main():
    base={'Scripts/NativeGameplay.lua':b'keep all native action semantics','Objects/Other.cdf':b'unchanged'}
    after,changed=plan(base);assert len(changed)==4
    again,changed=plan(after);assert not changed and again==after
    partial=dict(after);partial['Scripts/KCD2VR/map_presentation_m31.lua']=b'older paper code'
    result,changed=plan(partial);assert changed==['Scripts/KCD2VR/map_presentation_m31.lua'] and result==after
    assert all(after[n]==v for n,v in base.items())
    for name in ['Libs/UI/ApseInventoryList.gfx','Libs/UI/UIElements/ApseInventoryList.xml','scripts/kcd2vr/map_presentation_m31.lua']:
        bad=dict(base);bad[name]=b'conflicting adapter'
        try:plan(bad)
        except ValueError:pass
        else:raise AssertionError('conflict silently accepted '+name)
    print(json.dumps({'suite':'M33 map overlay planner','passed':True,
       'checks':['fresh','idempotent','partial prior update','unrelated bytes','both Inventory adapters refused','path alias refused'],
       'PAK_built':False,'game_or_headset':False},indent=2))


if __name__=='__main__':main()
