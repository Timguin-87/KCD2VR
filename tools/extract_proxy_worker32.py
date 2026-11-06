"""Extract the actual startup worker, leaving module/game execution as test leaves."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib

root = Path(__file__).resolve().parents[1]
source = read_source(root / "src/dllmain.cpp")
start = source.index("static DWORD WINAPI ModThreadMain(LPVOID)")
opening = source.index("{", start)
depth = 1
end = opening + 1
while depth:
    depth += (source[end] == "{") - (source[end] == "}")
    end += 1
function = source[start:end]
target = root / "tests/generated_proxy_worker32.inl"
target.write_text("// Generated from actual src/dllmain.cpp; do not edit.\n"
                  "// worker SHA256: " + hashlib.sha256(function.encode()).hexdigest() + "\n"
                  + function + "\n")
print(target)
mod = read_source(root / "src/mod.cpp")
start = mod.index("        startupRetry32.Tick(")
end = mod.index("\n#endif", start)
call = mod[start:end]
target = root / "tests/generated_proxy_retry32.inl"
target.write_text("// Generated from actual Mod::Run startup retry call; do not edit.\n"
                  "// call SHA256: " + hashlib.sha256(call.encode()).hexdigest() + "\n"
                  + call + "\n")
print(target)
