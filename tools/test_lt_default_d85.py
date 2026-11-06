#!/usr/bin/env python3
"""D85 craft: the smithing LT anvil assist (VrSmithSemiAnchorsD67) has ONE default, ON.

Executes the literal one-time INI migrations from src/mod.cpp in their source
order (D81 repair, then D72 candidate profile) on simulated INI files, and
reads the atomic default and the live INI reader default. Win32 INI IO is the
only substituted leaf. Usage: test_lt_default_d85.py <root> [--expect-fail]
"""
import re, sys
from pathlib import Path
root = Path(sys.argv[1]); expect_fail = '--expect-fail' in sys.argv
mod = (root/'src/mod.cpp').read_text(errors='replace')
d81 = mod.index('"VrRepairDefaultsD81",0)<1')
d72 = mod.index('"VrCandidateDefaultsD72",0)==0')
d81_block = mod[d81:mod.index('VrRepairDefaultsD81","1"', d81)]
d81_changes = dict(re.findall(r'\{"(\w+)","([^"]*)"\}', d81_block))
cand = (root/'src/vrik/candidate_defaults_d72.h').read_text()
d72_changes = dict(re.findall(r'\{"(\w+)","([^"]*)"\}', cand))
opts = (root/'src/subsystems/crafting/options_d67.h').read_text()
atomic = re.search(r'smithSemiAnchors\{(\w+)\}', opts).group(1)
live = (root/'src/subsystems/experimental/live_runtime_d40.inl').read_text()
reader = re.search(r'"VrSmithSemiAnchorsD67",(\w+)\)\)', live).group(1)
def migrate(ini):
    ini = dict(ini)
    steps = [(d81, 'd81'), (d72, 'd72')]
    for _, name in sorted(steps):
        if name == 'd81' and int(ini.get('VrRepairDefaultsD81', '0')) < 1:
            ini.update(d81_changes); ini['VrRepairDefaultsD81'] = '1'
        if name == 'd72' and int(ini.get('VrCandidateDefaultsD72', '0')) == 0:
            ini.update(d72_changes); ini['VrCandidateDefaultsD72'] = '1'
    return ini
def effective(ini):  # live reader: explicit INI value, else the reader default
    v = ini.get('VrSmithSemiAnchorsD67')
    return (v.lower() in ('1', 'true', 'yes', 'on')) if v is not None else reader == 'true'
cases = {
  'fresh INI (no migration markers)': ({}, True),
  'D81 already applied, D72 never': ({'VrRepairDefaultsD81': '1'}, True),
  'D72 already applied, D81 never': ({'VrCandidateDefaultsD72': '1'}, True),
  'both applied, user chose OFF in menu': ({'VrRepairDefaultsD81': '1', 'VrCandidateDefaultsD72': '1', 'VrSmithSemiAnchorsD67': 'false'}, False),
  'both applied, key missing': ({'VrRepairDefaultsD81': '1', 'VrCandidateDefaultsD72': '1'}, True),
}
fails = []
for name, (ini, want) in cases.items():
    got = effective(migrate(ini))
    if got != want: fails.append(f'{name}: LT assist {"ON" if got else "OFF"}, want {"ON" if want else "OFF"}')
if atomic != 'true': fails.append(f'options_d67 atomic default is {atomic} (before INI load / unit default)')
if reader != 'true': fails.append(f'live INI reader default is {reader}')
defaults = {d81_changes.get('VrSmithSemiAnchorsD67'), d72_changes.get('VrSmithSemiAnchorsD67'), atomic, reader}
if defaults != {'true'}: fails.append(f'more than one default: {sorted(x for x in defaults if x)}')
ok = not fails
for f in fails: print('  ' + f)
print(f'D85 LT assist single default ON: {"PASS" if ok else "FAIL"} ({len(cases)+3} checks)' + (' [expected failure on baseline]' if expect_fail else ''))
sys.exit(0 if ok != expect_fail else 1)
