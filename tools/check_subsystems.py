#!/usr/bin/env python3
"""Validate the explicit static build partition and frozen source patch ownership."""
from pathlib import Path
import collections,json,re,sys
r=Path(__file__).resolve().parents[1]
snapshot=json.loads((r/"cmake/module_groups_m2.json").read_text());m=json.loads((r/"subsystems.json").read_text())
g={name:(paths if name=="base" else json.loads((r/f"src/subsystems/{name}/sources.json").read_text())) for name,paths in snapshot.items()}
relocations=json.loads((r/"cmake/source_relocations_m2.json").read_text())
production=(r/"cmake/production_sources_m1.txt").read_text().splitlines()
flat=[p for paths in g.values() for p in paths]
assert len(production)==54 and len(set(production))==54
for p in production:
 canonical=relocations.get(p,p)
 assert flat.count(canonical)==1,("lost/duplicate production TU",p,canonical)
 if canonical!=p:
  assert p not in flat,("legacy and canonical TU both compiled",p,canonical)
  assert (r/p).read_text().strip()=='#include "'+canonical[4:]+'" // M2_OWNED_SOURCE',("stale compatibility TU",p)
assert len(flat)==len(set(flat)),"duplicate object source"
assert len(flat)==len({p.casefold() for p in flat}),"case-alias duplicate object source"
cm=(r/"cmake/module_sources.cmake").read_text()
for module,paths in g.items():
 if module=="base":
  text=re.search(r"kcd2vr_module\("+module+r"\n(.*?)\n\)",cm,re.S).group(1)
  assert text.split()==paths,("CMake/manifest disagreement",module)
 else:
  assert "kcd2vr_module("+module+")" in cm,("missing module target",module)
 for p in paths:
  assert isinstance(p,str) and p.startswith("src/") and "\\" not in p and ":" not in p and ".." not in p.split("/"),("unsafe source path",p)
  assert (r/p).resolve().is_relative_to((r/"src").resolve()) and (r/p).is_file(),p
  assert p.endswith(".cpp"),("production source must be cpp",p)
  if module!="base":
   assert p in m["modules"][module]["owned_files"] or p.startswith("src/subsystems/"+module+"/"),("foreign production source",module,p)
assert set(g)==set(m["modules"])
permissions={}
for module,entry in m["modules"].items():
 assert entry["target"]=="KCD2VR_"+module
 for p in entry["owned_files"]:
  assert p not in m["shared_host_bridges"],("protected host file",module,p)
  assert p not in permissions,("multiple patch owners",p,module,permissions.get(p))
  assert (r/p).is_file(),("missing owned source",p)
  permissions[p]=module
root=(r/"CMakeLists.txt").read_text()
assert "src/proxy_dinput8_29.cpp" in root and "if(KCD2VR_PROXY_DINPUT8 AND WIN32)" in root
print("PASS54 original production TUs retained exactlyonce;17 static targets; optional proxy retained; frozen patch ownership disjoint")
