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
def fixture(text):
    production='\n'.join(function(text,n) for n in ('bool ApplyF7PromptPresentation(', 'bool RestyleF7PromptPresentationD33('))
    return r"""
#include <cstdio>
#include <cstdlib>
#include <vector>
namespace diagnostic_trace {void Write(const char*){}}
namespace leaf {
 bool console=true,cvar=true,write=true,queue=true;int value=1,calls=0;std::vector<int> writes,art;
}
void* SafeGetCryConsole(){return leaf::console?&leaf::value:nullptr;}
void* SafeConsoleGetCVar(void*,const char*){return leaf::cvar?&leaf::value:nullptr;}
int SafeCVarGetInt(void*,int){return leaf::value;}
bool SafeCVarSetInt(void*,int v){leaf::writes.push_back(v);if(!leaf::write)return false;leaf::value=v;return true;}
bool ApplyControllerPromptPresentationV6477(int style){++leaf::calls;leaf::art.push_back(style);return leaf::queue;}
static bool g_f7_prompt_active_d33=false;static int g_f7_prompt_style_d33=2;
@PRODUCTION@
unsigned checks=0;
void ck(bool b){++checks;if(!b){std::printf("FAIL check %u\n",checks);std::exit(1);}}
int main(){
 leaf::console=false;ck(!ApplyF7PromptPresentation(true,2));ck(!g_f7_prompt_active_d33&&leaf::calls==0);
 leaf::console=true;ck(RestyleF7PromptPresentationD33(4));
 ck(g_f7_prompt_active_d33&&leaf::value==2&&leaf::calls==1&&leaf::art.back()==4&&g_f7_prompt_style_d33==4);
 ck(RestyleF7PromptPresentationD33(4));ck(leaf::calls==1); // stable choice has no repeated native work
 leaf::queue=false;ck(!RestyleF7PromptPresentationD33(2));ck(g_f7_prompt_style_d33==4&&leaf::value==2);
 leaf::queue=true;ck(RestyleF7PromptPresentationD33(2));ck(g_f7_prompt_style_d33==2&&leaf::calls==3);
 leaf::write=false;ck(!RestyleF7PromptPresentationD33(4));ck(g_f7_prompt_style_d33==2);
 leaf::write=true;ck(RestyleF7PromptPresentationD33(4));ck(g_f7_prompt_style_d33==4);
 ck(ApplyF7PromptPresentation(false,4));ck(!g_f7_prompt_active_d33&&leaf::value==1);
 leaf::queue=false;ck(!ApplyF7PromptPresentation(true,4));
 ck(g_f7_prompt_active_d33&&g_f7_prompt_style_d33!=4&&leaf::value==2); // retain valid native-family lease
 leaf::queue=true;ck(RestyleF7PromptPresentationD33(4));ck(g_f7_prompt_style_d33==4);
 ck(ApplyF7PromptPresentation(false,4));ck(leaf::value==1&&!g_f7_prompt_active_d33);
 leaf::cvar=false;ck(!RestyleF7PromptPresentationD33(4));ck(!g_f7_prompt_active_d33);
 leaf::cvar=true;ck(RestyleF7PromptPresentationD33(4));ck(g_f7_prompt_style_d33==4);
 ck(ApplyF7PromptPresentation(false,4));ck(leaf::value==1);
 for(int v:leaf::writes)ck(v==1||v==2); // PS art never changes native action-family semantics
 for(int v:leaf::art)ck(v==2||v==4);
 std::printf("%u prompt lease checks PASS; native leaves substituted\n",checks);
}
""".replace('@PRODUCTION@',production)
def main():
 p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
 rel='src/subsystems/hud_ui_controls/host.inl';text=(ROOT/rel).read_text();generated=out/'prompt_lease.cpp';generated.write_text(fixture(text))
 results=[]
 for label,flag in [('debug','/Od'),('optimized','/O2')]:
  exe=executable_path(out/label);cmd=command(generated,exe,[],'cl');cmd[cmd.index('/O2')]=flag
  built=subprocess.run(cmd,cwd=out,capture_output=True,text=True);(out/(label+'.build.log')).write_text(built.stdout+built.stderr);built.check_returncode()
  run=subprocess.run([str(exe)],capture_output=True,text=True);(out/(label+'.run.log')).write_text(run.stdout+run.stderr);run.check_returncode();print(run.stdout.strip());results.append(run.stdout.strip())
 old=subprocess.check_output(['git','show','HEAD:'+rel],cwd=ROOT,text=True);generated.write_text(fixture(old));exe=executable_path(out/'baseline');cmd=command(generated,exe,[],'cl')
 built=subprocess.run(cmd,cwd=out,capture_output=True,text=True);(out/'baseline.build.log').write_text(built.stdout+built.stderr);built.check_returncode()
 run=subprocess.run([str(exe)],capture_output=True,text=True);(out/'baseline.run.log').write_text(run.stdout+run.stderr)
 if run.returncode==0:raise AssertionError('Baseline unexpectedly passes failure reproducer')
 print('Baseline expected failure:',run.stdout.strip());generated.write_text(fixture(text))
 (out/'RESULT.json').write_text(json.dumps({'passed':True,'results':results,'baseline_expected_failure':run.stdout.strip(),'source_sha256':hashlib.sha256((ROOT/rel).read_bytes()).hexdigest(),'limits':'Actual C++ functions; native CVar and Flash queue substituted, no rendered pixels/headset/DLL.'},indent=2)+'\n')
if __name__=='__main__':main()
