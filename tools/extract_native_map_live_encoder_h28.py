from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
s=(root/'src/subsystems/vrmap/runtime/held_map_feed_t2.cpp').read_text()
a=s.index('bool Encode(');b=s.index('void ServiceReadiness',a)
Path(sys.argv[1]).write_text(s[a:b])

if len(sys.argv)>2:
 a=s.index('void Service(const native_script_t2::Context& context) noexcept {')
 b=s.index('\n}\n}\nbool Install()',a)+2
 Path(sys.argv[2]).write_text(s[a:b])
