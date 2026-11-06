"""Extract current native instant adapter; mock only native leaves for host proof."""
from pathlib import Path
import hashlib
import json
import re

root = Path(__file__).resolve().parents[1]
p = root / 'src/subsystems/alchemy/instant_native_d72.inl'
s = p.read_text(encoding='utf-8')
s = s[:s.index('static bool InstallInstantAlchemyD72()')]
s = re.sub(r'^static std::optional<DetourModKit::hook::Hook>.*\n', '', s, flags=re.M)

def replace_function(name, body):
    global s
    start = s.index('static bool ' + name + '(')
    opening = s.index('{', start)
    level = 1
    end = opening + 1
    while level:
        level += (s[end] == '{') - (s[end] == '}')
        end += 1
    s = s[:opening + 1] + body + s[end - 1:]

for name, body in {
    'AlchemyMortarHeldD75': 'return HostMortarHeldD75(s);',
    'AlchemyInvokeSemanticD72': 'return HostSemanticD75(s);',
    'AlchemyHeldReadbackD72': 'return HostOrdinaryHeldD75(s,held);',
    'AlchemyCompleteD72': 'return HostCompleteD75(s,success);',
}.items():
    replace_function(name, body)

out = root / 'tests/generated'
out.mkdir(exist_ok=True)
(out / 'instant_mortar_d75.inc').write_text(s, encoding='utf-8')
(out / 'instant_mortar_d75_provenance.json').write_text(json.dumps({
    'source': str(p.relative_to(root)), 'sha256': hashlib.sha256(p.read_bytes()).hexdigest(),
    'mocked': ['four native call leaves', 'hook installation excluded'],
    'actual': ['prepare', 'identity', 'source', 'director/start wrappers', 'effect readback', 'tick/completion receipt'],
}, indent=2) + '\n', encoding='utf-8')
print('Extracted current D75 instant mortar adapter; native ABI calls remain mocked.')
