"""Actual prompt lease functions, with native CVar/Flash queue leaves substituted."""
from pathlib import Path
import argparse, hashlib, json, subprocess
from host_compiler import command, executable_path
ROOT=Path(__file__).resolve().parents[1]
def function(text, signature):
    start=text.index(signature);end=text.index('{',start);depth=1;end+=1
    while depth:
        depth+=(text[end]=='{')-(text[end]=='}');end+=1
    return text[start:end]
def fixture(text, r129=True):
    names=('bool ApplyF7PromptPresentation(', 'bool RestyleF7PromptPresentationD33(')
    if r129:names=('static bool LeasePreferredInputR129(','static bool ApplyGlyphStyleR129(','static void ResendGlyphStyleOnUiChangeR129(')+names
    production='\n'.join(function(text,n) for n in names)
    # pre-R129 Run-loop edge (mod.cpp): re-send only while the lease was held.
    if not r129:production+='\nstatic void ResendGlyphStyleOnUiChangeR129(int s){if(g_f7_prompt_active_d33)(void)prompt_device_lock::RequestFlashStyleD56(s==4?4:2);}\n'
    # R129 rule: the button ART never waits for the wh_ui_PreferredInputDevice lease (mod start = no console yet).
    # The pre-R129 source fails check 1/2 (console missing -> art never queued, the saved PlayStation art lost).
    return r"""
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include "subsystems/hud_ui_controls/runtime/glyph_apply_r129.h"
namespace diagnostic_trace {void Write(const char*){} int rows=0; void WriteStatus(const char*){++rows;}}
namespace leaf {
 bool console=true,cvar=true,write=true,queue=true;int value=1,calls=0,applied=0,forced=2;std::vector<int> writes,art;
}
namespace prompt_device_lock {
 void SetForcedDevice(int d){leaf::forced=d==4?4:2;}
 int AppliedGlyphStyleR129(){return leaf::applied;}
 void NoteGlyphPathR129(int,bool){}
 bool RequestFlashStyleD56(int style){++leaf::calls;leaf::art.push_back(style);return leaf::queue;}
}
void* SafeGetCryConsole(){return leaf::console?&leaf::value:nullptr;}
void* SafeConsoleGetCVar(void*,const char*){return leaf::cvar?&leaf::value:nullptr;}
int SafeCVarGetInt(void*,int){return leaf::value;}
bool SafeCVarSetInt(void*,int v){leaf::writes.push_back(v);if(!leaf::write)return false;leaf::value=v;return true;}
bool ApplyControllerPromptPresentationV6477(int style){return prompt_device_lock::RequestFlashStyleD56(style);}
static bool g_f7_prompt_active_d33=false;static int g_f7_prompt_style_d33=2;
static bool g_f7_have_restore_r129=false;static int g_f7_restore_preferred_r129=0;
@PRODUCTION@
unsigned checks=0;
void ck(bool b){++checks;if(!b){std::printf("FAIL check %u\n",checks);std::exit(1);}}
// FlashServiceD56 stand-in: the movies took the last queued art.
void dispatched(bool ok){leaf::applied=ok&&!leaf::art.empty()?leaf::art.back():0;}
int main(){
 // 1. mod start, console not up, saved PlayStation: art queued anyway, lease not taken (retried later).
 leaf::console=false;ApplyF7PromptPresentation(true,4);ck(!g_f7_prompt_active_d33&&leaf::calls==1&&leaf::art.back()==4&&leaf::forced==4);
 dispatched(true);
 // 2. save load / HUD rebuilt before any VR menu click: the UI change re-sends the art without the lease.
 leaf::applied=0;ResendGlyphStyleOnUiChangeR129(4);ck(leaf::calls==2&&leaf::art.back()==4&&!g_f7_prompt_active_d33);dispatched(true);
 // 3. console up: the next UI change takes the lease (preferred=2) and still sends the art.
 leaf::console=true;ResendGlyphStyleOnUiChangeR129(4);ck(g_f7_prompt_active_d33&&leaf::value==2&&leaf::calls==3);dispatched(true);
 // 4. VR menu: same style already on the movies = no native work on unrelated clicks.
 ck(RestyleF7PromptPresentationD33(4));ck(leaf::calls==3);
 // 5. VR menu XBOX -> PLAYSTATION switch right away; a refused queue is retried by the next click (no latch).
 ck(RestyleF7PromptPresentationD33(2));ck(leaf::calls==4&&leaf::art.back()==2&&g_f7_prompt_style_d33==2);dispatched(true);
 leaf::queue=false;ck(!RestyleF7PromptPresentationD33(4));ck(g_f7_prompt_style_d33==2&&leaf::value==2);
 leaf::queue=true;ck(RestyleF7PromptPresentationD33(4));ck(g_f7_prompt_style_d33==4&&leaf::calls==6);dispatched(true);
 // 6. a dispatch that reached no movie (applied 0) is re-sent by the same choice.
 dispatched(false);ck(RestyleF7PromptPresentationD33(4));ck(leaf::calls==7);dispatched(true);
 // 7. exit restores the CVar we changed; restart path again.
 ck(ApplyF7PromptPresentation(false,4));ck(!g_f7_prompt_active_d33&&leaf::value==1);
 leaf::applied=0;leaf::cvar=false;ApplyF7PromptPresentation(true,4);ck(!g_f7_prompt_active_d33&&leaf::art.back()==4);
 leaf::cvar=true;ck(RestyleF7PromptPresentationD33(2));ck(g_f7_prompt_active_d33&&leaf::art.back()==2);
 ck(ApplyF7PromptPresentation(false,4));ck(leaf::value==1);
 leaf::write=false;leaf::applied=0;ApplyF7PromptPresentation(true,4);ck(!g_f7_prompt_active_d33&&leaf::art.back()==4); // CVar write refused: art still out
 leaf::write=true;
 for(int v:leaf::writes)ck(v==1||v==2); // PS art never changes native action-family semantics
 for(int v:leaf::art)ck(v==2||v==4);
 std::printf("%u prompt lease checks PASS; native leaves substituted\n",checks);
}
""".replace('@PRODUCTION@',production)
def main():
 p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
 rel='src/subsystems/hud_ui_controls/host.inl';text=(ROOT/rel).read_text();generated=out/'prompt_lease.cpp';generated.write_text(fixture(text))
 # R129: CXX (default g++, or cl from a VS shell); the R129 policy header comes from src/.
 def build(exe):
  cmd=command(generated,exe,[ROOT/'src'])
  return cmd,subprocess.run(cmd,cwd=out,capture_output=True,text=True)
 results=[]
 for label,flag in [('debug','-O0'),('optimized','-O2')]:
  exe=executable_path(out/label);cmd=command(generated,exe,[ROOT/'src'])
  if '/O2' in cmd:cmd[cmd.index('/O2')]='/Od' if flag=='-O0' else '/O2'
  else:cmd[cmd.index('-O2')]=flag
  built=subprocess.run(cmd,cwd=out,capture_output=True,text=True);(out/(label+'.build.log')).write_text(built.stdout+built.stderr);built.check_returncode()
  run=subprocess.run([str(exe)],capture_output=True,text=True);(out/(label+'.run.log')).write_text(run.stdout+run.stderr);run.check_returncode();print(run.stdout.strip());results.append(run.stdout.strip())
 # Baseline = r127_int/r128 host.inl (before R129): only its two old functions exist; check 1 must fail.
 old=subprocess.check_output(['git','show','r127_int:'+rel],cwd=ROOT,text=True);generated.write_text(fixture(old,False));exe=executable_path(out/'baseline')
 cmd,built=build(exe);(out/'baseline.build.log').write_text(built.stdout+built.stderr)
 if built.returncode==0:
  run=subprocess.run([str(exe)],capture_output=True,text=True);(out/'baseline.run.log').write_text(run.stdout+run.stderr)
  if run.returncode==0:raise AssertionError('Baseline unexpectedly passes failure reproducer')
  base=run.stdout.strip()
 else:base='baseline does not build against the R129 contract (old functions only)'
 print('Baseline expected failure:',base);generated.write_text(fixture(text))
 (out/'RESULT.json').write_text(json.dumps({'passed':True,'results':results,'baseline_expected_failure':base,'source_sha256':hashlib.sha256((ROOT/rel).read_bytes()).hexdigest(),'limits':'Actual C++ functions; native CVar and Flash queue substituted, no rendered pixels/headset/DLL.'},indent=2)+'\n')
if __name__=='__main__':main()
