"""Patch a COPY of an existing optional gore PAK; never synthesize missing meshes."""
from pathlib import Path
import argparse, hashlib, json, os, zipfile
MEMBER = 'scripts/startup/zzkcd2vr_gore.lua'
def sha(data): return hashlib.sha256(data).hexdigest()
def patch(source, output, script):
    source, output, script = map(lambda p: Path(p).resolve(), (source, output, script))
    if source == output: raise ValueError('Output must be a new COPY, never the installed source PAK')
    if output.exists(): raise ValueError('Output already exists; choose a new file')
    payload = script.read_bytes()
    if not any(v in payload for v in (b"G.version = '59.1'", b"G.version = '61.0-T3-H15'")) or b'bPhysicalize = true' not in payload:
      raise ValueError('Not a reviewed C59 or H15 gore script')
    output.parent.mkdir(parents=True, exist_ok=True)
    temp = output.with_name(output.name + '.tmp')
    if temp.exists(): raise ValueError('Temporary output already exists')
    changed=[]; preserved=[]
    try:
      with zipfile.ZipFile(source) as src, zipfile.ZipFile(temp, 'w') as dst:
        names=[x for x in src.infolist() if x.filename.replace(chr(92),'/').lower()==MEMBER]
        if len(names)!=1: raise ValueError('Source must contain exactly one original gore script')
        for info in src.infolist():
          old=src.read(info); current=payload if info is names[0] else old
          dst.writestr(info, current)
          item={'name':info.filename,'sha256':sha(current),'size':len(current)}
          if info is names[0]: item['previous_sha256']=sha(old);changed.append(item)
          else: preserved.append(item)
      with zipfile.ZipFile(temp) as check:
        for entry in preserved:
          if sha(check.read(entry['name']))!=entry['sha256']: raise ValueError('Preserved member verification failed')
      os.replace(temp, output)
    except Exception:
      if temp.exists(): temp.unlink()
      raise
    report={'source':str(source),'source_sha256':sha(source.read_bytes()),'output':str(output),
      'output_sha256':sha(output.read_bytes()),'changed':changed,'preserved_count':len(preserved),
      'preserved':preserved,'source_archive_modified':False}
    output.with_name(output.name+'.provenance.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    return report
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source',required=True);p.add_argument('--output',required=True)
    p.add_argument('--script',default=str(Path(__file__).resolve().parents[1]/'gore_pak/Scripts/Startup/zzkcd2vr_gore.lua'))
    a=p.parse_args(); r=patch(a.source,a.output,a.script)
    print(json.dumps({k:v for k,v in r.items() if k!='preserved'},indent=2))
