"""Execute the actual build.bat PowerShell/exit block with only build_release mocked."""
from pathlib import Path
import argparse,json,os,subprocess,sys,tempfile
ROOT=Path(__file__).resolve().parents[1]

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
 source=(ROOT/'build.bat').read_text();lines=source.splitlines();at=next(i for i,l in enumerate(lines) if l.startswith('powershell -NoProfile -Command '))
 block='\r\n'.join(lines[at:at+2]);assert lines[at+1].startswith('if errorlevel 1 ')
 rows=[]
 for proxy in ('0','1'):
  for result in (0,7):
   with tempfile.TemporaryDirectory(prefix='wrapper31-',dir=a.out) as folder:
    d=Path(folder);sdk=str(d/'SDK with spaces'/'OpenXR')
    (d/'record.py').write_text('import sys,json;open("args.json","w").write(json.dumps(sys.argv[1:]))')
    (d/'build_release.bat').write_text('@echo off\n"'+sys.executable+'" "'+str(d/'record.py')+'" %*\necho MOCK_NATIVE_BUILD\nexit /b %KCD2VR_MOCK_EXIT%\n')
    (d/'driver.cmd').write_bytes(('@echo off\r\n'+block+'\r\necho SUCCESS>success.txt\r\nexit /b 0\r\n').encode())
    env=os.environ.copy();env.update(OPENXR_DIR=sdk,KCD2VR_BUILD_PROXY31=proxy,KCD2VR_MOCK_EXIT=str(result))
    run=subprocess.run([env.get('ComSpec','cmd.exe'),'/d','/c',str(d/'driver.cmd')],cwd=d,env=env,capture_output=True,text=True)
    args=json.loads((d/'args.json').read_text());rawlog=(d/'build.log').read_bytes();log=rawlog.decode('utf-16' if rawlog.startswith(b'\xff\xfe') else 'utf-8-sig')
    assert args==['-DOpenXR_DIR='+sdk]+(['-DKCD2VR_PROXY_DINPUT8=ON'] if proxy=='1' else []),args
    assert run.returncode==(0 if result==0 else 1),(run.returncode,run.stdout,run.stderr)
    assert (d/'success.txt').exists()==(result==0)
    assert 'EXIT='+str(result) in log and 'MOCK_NATIVE_BUILD' in log,log
    rows.append({'proxy':proxy,'native_exit':result,'wrapper_exit':run.returncode,'args':args,'log':log,'stdout':run.stdout,'stderr':run.stderr})
 (a.out/'results.json').write_text(json.dumps({'actual_block':block,'runs':rows,'passed':True,'installer_executed':False,'native_build_leaf':'mock batch only'},indent=2)+'\n')
 print('Actual wrapper31: 4/4 process/exit/space-path cases PASS; no installer or native build executed')
if __name__=='__main__':main()
