"""Actual production Lua5.1 stow protocol; native inventory leaves modeled."""
from pathlib import Path
import sys,hashlib,json,argparse
sys.path.insert(0,r'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'mods/KCD2VRStow/zzkcd2vr_grip_stow_cb1.lua'
LEAVES=r"""
checks=0;adds=0;world={};System={GetEntity=function(id)if getThrows then error('query unknown')end;if id==10 then return player end;return world[id]end}
local inv={AddItem=function(self,id)adds=adds+1;lastAdded=id;if throwAdd then error('native side effect then throw')end;if refuse then return nil end;return id end}
player={id=10,inventory=inv}
originalPlayer=player
function item(id)local e={item={GetId=function(self)return self.id end,id=id}};return e end
function ck(x)checks=checks+1;assert(x,'check '..checks)end
function packet(n,a) return kcd2vr_grip_stow_cb1(n..'|'..a..'|10|100|0')end
function setup(id)world[100]=item(id);return world[100]end
"""
TEST=r"""
setup('original');ck(packet(1,'PREPARE')=='OK|1|PREPARE|PREPARED');ck(adds==0)
ck(packet(1,'PREPARE')=='OK|1|PREPARE|PREPARED');ck(packet(1,'COMMIT')=='OK|1|COMMIT|NATIVE_RETURNED');ck(adds==1 and lastAdded=='original')
ck(packet(1,'COMMIT')=='OK|1|COMMIT|NATIVE_RETURNED');ck(adds==1);ck(packet(1,'PREPARE')=='OK|1|PREPARE|STALE')
-- The old draft looked up whatever now occupied the entity ID. The original
-- entity/item/WUID triple must instead survive PREPARE -> retirement -> COMMIT.
setup('old');ck(packet(2,'PREPARE')=='OK|2|PREPARE|PREPARED');setup('replacement');ck(packet(2,'COMMIT')=='OK|2|COMMIT|IDENTITY_CHANGED');ck(adds==1)
setup('wuid-old');ck(packet(3,'PREPARE')=='OK|3|PREPARE|PREPARED');world[100].item.id='wuid-new';ck(packet(3,'COMMIT')=='OK|3|COMMIT|IDENTITY_CHANGED');ck(adds==1)
setup('same');ck(packet(4,'PREPARE')=='OK|4|PREPARE|PREPARED');world[100].item=item('other').item;ck(packet(4,'COMMIT')=='OK|4|COMMIT|IDENTITY_CHANGED');ck(adds==1)
setup('actor');ck(packet(5,'PREPARE')=='OK|5|PREPARE|PREPARED');player={id=10,inventory=originalPlayer.inventory};ck(packet(5,'COMMIT')=='OK|5|COMMIT|IDENTITY_CHANGED');ck(adds==1);player=originalPlayer
setup('inventory');ck(packet(6,'PREPARE')=='OK|6|PREPARE|PREPARED');local saved=player.inventory;player.inventory={};ck(packet(6,'COMMIT')=='OK|6|COMMIT|IDENTITY_CHANGED');ck(adds==1);player.inventory=saved
setup('cancel');ck(packet(7,'PREPARE')=='OK|7|PREPARE|PREPARED');ck(packet(7,'CANCEL')=='OK|7|CANCEL|CANCELLED');ck(packet(7,'COMMIT')=='OK|7|COMMIT|NO_PREPARE');ck(adds==1)
setup('throw');ck(packet(8,'PREPARE')=='OK|8|PREPARE|PREPARED');throwAdd=true;ck(packet(8,'COMMIT')=='OK|8|COMMIT|NATIVE_UNKNOWN');throwAdd=false;ck(packet(8,'COMMIT')=='OK|8|COMMIT|NATIVE_UNKNOWN');ck(adds==2)
setup('refuse');ck(packet(9,'PREPARE')=='OK|9|PREPARE|PREPARED');refuse=true;ck(packet(9,'COMMIT')=='OK|9|COMMIT|NATIVE_REFUSED');refuse=false;ck(packet(9,'COMMIT')=='OK|9|COMMIT|NATIVE_REFUSED');ck(adds==3)
setup('abandoned');ck(packet(10,'PREPARE')=='OK|10|PREPARE|PREPARED');setup('new');ck(packet(11,'PREPARE')=='OK|11|PREPARE|PREPARED');ck(packet(10,'COMMIT')=='OK|10|COMMIT|STALE');ck(packet(11,'COMMIT')=='OK|11|COMMIT|NATIVE_RETURNED');ck(adds==4 and lastAdded=='new')
ck(kcd2vr_grip_stow_cb1('12|COMMIT|10|100|0junk')=='BAD|0');ck(kcd2vr_grip_stow_cb1('12|ANY|10|100|0')=='BAD|0');ck(kcd2vr_grip_stow_cb1('0|PREPARE|10|100|0')=='BAD|0');ck(kcd2vr_grip_stow_cb1('9007199254740992|PREPARE|10|100|0')=='BAD|0')
ck(KCD2VR_Stow==nil);ck(adds==4)
-- Retail Entity.id may be an opaque ScriptHandle. Numeric protocol IDs rejoin
-- through System.GetEntity instead of comparing unlike identity types.
player.id={opaque=true};setup('opaque-actor');ck(packet(12,'PREPARE')=='OK|12|PREPARE|PREPARED');ck(packet(12,'COMMIT')=='OK|12|COMMIT|NATIVE_RETURNED');ck(adds==5 and lastAdded=='opaque-actor')
"""
def main():
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 lua=LuaRuntime(unpack_returned_tuples=True);lua.execute(LEAVES);lua.execute(SOURCE.read_text());lua.execute(TEST)
 result={'source':str(SOURCE),'sha256':hashlib.sha256(SOURCE.read_bytes()).hexdigest(),'lua':'5.1','checks':lua.globals().checks,'native_add_calls_modeled':lua.globals().adds,'PASS':True,'limits':'Native inventory leaves modeled; no game/headset execution or inventory delivery guarantee.'}
 a.output.mkdir(parents=True,exist_ok=True);(a.output/'LUA_RESULT.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
if __name__=='__main__':main()
