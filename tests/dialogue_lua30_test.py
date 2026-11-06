"""Execute actual patched OnTalk methods in Lua5.1; native RequestDialog is a recorded leaf."""
from pathlib import Path
import os,sys,unittest,zipfile
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import build_dialogue_lua30 as builder
SCRIPTS=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Scripts.pak')
class NativeLuaTalk30(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  with zipfile.ZipFile(SCRIPTS) as z:cls.inputs={n:z.read(n) for n in builder.INPUTS}
 def test_exact_retail_refusal(self):
  for name,data in self.inputs.items():
   with self.subTest(name=name):
    with self.assertRaises(ValueError):builder.patch(name,data+b' ')
    with self.assertRaises(ValueError):builder.patch('Other.lua',data)
 def test_full_file_syntax_and_byte_preservation(self):
  for name,data in self.inputs.items():
   result=builder.patch(name,data);text=result.decode();old=data.decode()
   lua=LuaRuntime();self.assertIsNotNone(lua.eval('function(s) return assert(loadstring(s)) end')(text))
   at=old.index(builder.CALL);start=old.rfind('\n',0,at)+1;indent=old[start:at]
   block=builder.BODY.format(indent=indent)
   if b'\r\n' in data:block=block.replace('\n','\r\n')
   self.assertEqual(text.count(block),1)
   self.assertEqual(text.replace(block,builder.CALL).encode(),data)
 def test_actual_methods_native_request_once_and_scope(self):
  count=0
  for name,data in self.inputs.items():
   text=(data if os.environ.get('KCD2VR_BASELINE_LUA30')=='1' else builder.patch(name,data)).decode();owner='Dog' if name.endswith('Dog.lua') else 'BasicAIActions'
   begin=text.index('function '+owner+':OnTalk(');end=text.index('\nend',begin)+4
   method=text[begin:end]
   for cameras in (None,0,1,2):
    for pov in (None,0,1,2):
     for kind in ('player','npc','no_actor','no_method'):
      count+=1
      with self.subTest(name=name,cameras=cameras,pov=pov,kind=kind):
       lua=LuaRuntime();lua.execute(owner+' = {}; DebugUtils={Log=function(...)end}; EntityUtils={GetName=function(x)return "test" end}; System={}; user={id=42}; calls={}; cvarReads=0')
       if kind!='no_actor':lua.execute('user.actor={}')
       if kind=='player':lua.execute('user.actor.IsPlayer=function(self)return 1 end')
       elif kind=='npc':lua.execute('user.actor.IsPlayer=function(self)return nil end')
       lua.globals().noCamera=cameras;lua.globals().pov=pov
       lua.execute("System.GetCVar=function(n) cvarReads=cvarReads+1; if n=='wh_dlg_NoCameras' then return noCamera elseif n=='wh_dlg_ForcePOV' then return pov else error('unexpected CVar') end end")
       lua.execute(method)
       lua.execute("self={actor={RequestDialog=function(actor,id,roles,third,fourth) calls[#calls+1]={actor,id,roles,third,fourth} end}}; "+owner+".OnTalk(self,user,99)")
       calls=lua.globals().calls;self.assertEqual(len(calls),1)
       call=calls[1];self.assertTrue(lua.eval('calls[1][1] == self.actor'))
       self.assertEqual(call[2],42);self.assertEqual(call[3],'');self.assertIs(call[4],False)
       scoped=kind=='player' and cameras==1 and pov==1
       self.assertEqual(call[5],not scoped)
       if kind!='player':self.assertEqual(lua.globals().cvarReads,0)
  self.assertEqual(count,128)
if __name__=='__main__':unittest.main()
