#!/usr/bin/env python3
"""Source-only subsystem overlay. Never installs binaries or executes patch code."""
from pathlib import Path, PurePosixPath
import argparse, base64, hashlib, json, os, subprocess, sys, tempfile, zipfile

class PatchError(Exception): pass

def sha(data): return hashlib.sha256(data).hexdigest() if data is not None else None

def content_sha(data):
    # Git may store LF while a Windows working tree has CRLF. Compare source
    # semantics under explicit UTF8/LF policy; payload integrity remains raw SHA.
    if data is None:return None
    try:return sha(data.decode("utf-8").replace("\r\n","\n").encode("utf-8"))
    except UnicodeDecodeError:return sha(data)

def safe(root, name):
    p=PurePosixPath(name)
    if not name or "\\" in name or p.is_absolute() or ":" in name or any(x in ("", ".", "..") for x in name.split("/")):
        raise PatchError("unsafe source path: "+name)
    target=root.joinpath(*p.parts)
    if not target.resolve().is_relative_to(root.resolve()): raise PatchError("source path escapes checkout")
    for parent in [target,*target.parents]:
        if parent==root.parent: break
        if parent.is_symlink(): raise PatchError("symlink source path refused")
    return target

def read_manifest(root):
    raw=(root/"subsystems.json").read_bytes();m=json.loads(raw)
    if m.get("schema")!=1: raise PatchError("unsupported ownership schema")
    for name,expected in m.get("base_files",{}).items():
        path=safe(root,name)
        if not path.is_file() or content_sha(path.read_bytes())!=expected:
            raise PatchError("frozen base source changed: "+name)
    return m,content_sha(raw)

def allowed(m, module, name):
    entry=m["modules"].get(module)
    if not entry or module=="base": return False
    if name in m.get("shared_host_bridges",[]): return False
    # Explicit file/prefix permissions are frozen by the base, not payload supplied.
    return name in entry["owned_files"] or any(name.startswith(prefix) for prefix in entry.get("allowed_prefixes",[entry["implementation_prefix"]]))

def git(root,*args):
    result=subprocess.run(["git","-C",str(root),*args],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    if result.returncode: raise PatchError("git "+args[0]+" failed: "+result.stderr.decode(errors="replace"))
    return result.stdout

def create(root,module,base,out):
    root=Path(root).resolve();m,mhash=read_manifest(root)
    if module=="base" or module not in m["modules"]: raise PatchError("select an owned subsystem, not the base")
    git(root,"rev-parse","--verify",base+"^{commit}")
    changed=git(root,"diff","--name-only",base,"--").decode().splitlines()
    for prefix in m["modules"][module].get("allowed_prefixes",[m["modules"][module]["implementation_prefix"]]):
        changed+=git(root,"ls-files","--others","--exclude-standard","--",prefix).decode().splitlines()
    entries=[];payload={}
    for name in sorted(set(changed)):
        if not allowed(m,module,name): continue
        path=safe(root,name)
        prior=subprocess.run(["git","-C",str(root),"show",base+":"+name],stdout=subprocess.PIPE,stderr=subprocess.DEVNULL)
        before=prior.stdout if prior.returncode==0 else None
        after=path.read_bytes() if path.exists() else None
        if before==after: continue
        if after is not None and len(after)>20*1024*1024: raise PatchError("source payload too large")
        entries.append({"path":name,"before":content_sha(before),"after":sha(after)})
        if after is not None: payload[name]=after
    if not entries: raise PatchError("no owned source changes to package")
    meta={"schema":1,"module":module,"base_contract":m["base_contract"],"manifest_sha256":mhash,"from_commit":base,"comparison":"utf8-lf-v1","files":entries}
    with zipfile.ZipFile(out,"w",zipfile.ZIP_DEFLATED) as z:
        z.writestr("patch.json",json.dumps(meta,indent=2)+"\n")
        for n,data in payload.items(): z.writestr("source/"+n,data)
    return meta

def verify(root,archive):
    root=Path(root).resolve();m,mhash=read_manifest(root)
    with zipfile.ZipFile(archive) as z:
        if len(z.infolist())>4096: raise PatchError("too many source members")
        if len({n.lower() for n in z.namelist()})!=len(z.namelist()): raise PatchError("duplicate archive paths")
        meta=json.loads(z.read("patch.json"))
        if meta.get("comparison")!="utf8-lf-v1" or meta.get("schema")!=1 or meta.get("base_contract")!=m["base_contract"] or meta.get("manifest_sha256")!=mhash:
            raise PatchError("incompatible frozen base contract")
        module=meta.get("module");entries=meta.get("files",[])
        if not entries: raise PatchError("empty source patch")
        names={"patch.json"};seen=set();plan=[]
        for entry in entries:
            n=entry["path"]
            if n.lower() in seen: raise PatchError("duplicate source path")
            seen.add(n.lower());path=safe(root,n)
            if not allowed(m,module,n): raise PatchError("patch crosses subsystem ownership: "+n)
            data=None
            if entry["after"] is not None:
                member="source/"+n;names.add(member)
                if z.getinfo(member).file_size>20*1024*1024: raise PatchError("source payload too large")
                data=z.read(member)
                if sha(data)!=entry["after"]: raise PatchError("corrupt source payload")
            current=path.read_bytes() if path.exists() else None
            if sha(current)==entry["after"] or (data is not None and content_sha(current)==content_sha(data)): continue # idempotent, preserve checkout newlines
            if content_sha(current)!=entry["before"]: raise PatchError("same-file conflict: "+n)
            plan.append((path,current,data))
        if set(z.namelist())!=names: raise PatchError("unexpected archive member")
        return meta,plan

def apply(root,archive):
    meta,plan=verify(root,archive);written=[]
    try:
        for path,before,after in plan:
            actual=path.read_bytes() if path.exists() else None
            if actual!=before:raise PatchError("source changed after verification: "+str(path))
            path.parent.mkdir(parents=True,exist_ok=True)
            if after is None: path.unlink()
            else:
                fd,temp=tempfile.mkstemp(prefix=".module-",dir=path.parent)
                try:
                    with os.fdopen(fd,"wb") as f:f.write(after)
                    os.replace(temp,path)
                finally:
                    if os.path.exists(temp):os.unlink(temp)
            written.append((path,before))
    except Exception:
        for path,before in reversed(written):
            if before is None:path.unlink(missing_ok=True)
            else:path.write_bytes(before)
        raise
    return meta,len(plan)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument("command",choices=["create","verify","apply"]);p.add_argument("--root",type=Path,default=Path(__file__).resolve().parents[1]);p.add_argument("--module");p.add_argument("--base");p.add_argument("--output",type=Path);p.add_argument("--patch",type=Path)
    a=p.parse_args()
    try:
        if a.command=="create":
            if not all((a.module,a.base,a.output)):p.error("create needs --module --base --output")
            result=create(a.root,a.module,a.base,a.output);print("Packaged",len(result["files"]),"owned source changes for",a.module)
        else:
            if not a.patch:p.error("verify/apply needs --patch")
            if a.command=="apply":meta,count=apply(a.root,a.patch);print("Applied",count,"source changes for",meta["module"])
            else:meta,plan=verify(a.root,a.patch);print("Compatible source patch for",meta["module"],"pending",len(plan))
    except (PatchError,ValueError,KeyError,OSError,zipfile.BadZipFile) as e:print("REFUSED:",e,file=sys.stderr);return 1
    return 0
if __name__=="__main__":sys.exit(main())
