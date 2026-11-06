#!/usr/bin/env python3
"""D61C: which reaction clip the VICTIM plays for each masterstrike, from the retail ADB join (no new decoding).

Input: data/research/combat/AUTHORED_EVENT_JOIN_D60.json (Codex D60, 601 clips joined to retail fragments).
Model (CryMannequin tag query, most specific option wins):
  attacker fragment  CombatMasterStrikeGen / CombatMasterStrikeDeathGen
  victim fragment    CombatMasterStrikeHitGen / CombatMasterStrikeHitDeathGen, same death kind
  victim needs of attacker: opp<Tag> -> attacker FragTag <tag>; ol_/or_<w> -> attacker l_/r_<w>
  attacker needs of victim: ol_/or_<w> in the attacker fragment -> victim l_/r_<w>
  victim own equipment (l_/r_ tags of its fragment) must be what the victim carries.
Output: one row per (attacker fragment, victim equipment) that the data allows. Exit 1 if any is ambiguous.
Usage: ms_reaction_table_d61c.py [join.json] [out.json] [out.csv]
"""
import collections, csv, json, re, sys
from pathlib import Path
src = sys.argv[1] if len(sys.argv) > 1 else str(Path(__file__).resolve().parents[1] / 'data/research/combat/AUTHORED_EVENT_JOIN_D60.json')
out_json = sys.argv[2] if len(sys.argv) > 2 else 'ms_reaction_table_d61c.json'
out_csv = sys.argv[3] if len(sys.argv) > 3 else 'ms_reaction_table_d61c.csv'
data = json.load(open(src))
F = []
for clip in data['clips']:
    for f in clip.get('retailFragments') or []:
        if 'MasterStrike' not in f['group']: continue
        fr = f['fragment']
        blends = [p.get('precedingBlendRaw', {}) for p in f.get('procedures', []) if p.get('layer') == 0]
        F.append(dict(clip=clip['clip'], group=f['group'], guid=fr.get('GUID'),
                      tags={t for t in fr.get('Tags', '').split('+') if t},
                      ft={t for t in fr.get('FragTags', '').split('+') if t},
                      seconds=round(clip.get('authoredFrameSpanSeconds') or 0, 4),
                      blendOut=fr.get('BlendOutDuration'),
                      layer0ExitTimes=[b.get('ExitTime') for b in blends]))
MASTER = ('CombatMasterStrikeGen', 'CombatMasterStrikeDeathGen')
HIT = ('CombatMasterStrikeHitGen', 'CombatMasterStrikeHitDeathGen')
M = [f for f in F if f['group'] in MASTER]
S = [f for f in F if f['group'] in HIT]
low1 = lambda s: s[0].lower() + s[1:]
own = lambda f: {t for t in (f['tags'] | f['ft']) if re.match(r'^(l|r)_', t)}
def victim_needs(s):
    r = set()
    for t in s['ft']:
        if t.startswith('opp') and t != 'oppMale': r.add(('ft', low1(t[3:])))
        elif t.startswith('ol_'): r.add(('w', 'l_' + t[3:]))
        elif t.startswith('or_'): r.add(('w', 'r_' + t[3:]))
    return r
def attacker_has(m, need):
    kind, v = need
    return v in m['ft'] if kind == 'ft' else (v in m['tags'] or v in m['ft'])
def attacker_needs(m):
    return {('l_' if t.startswith('ol_') else 'r_') + t[3:] for t in (m['tags'] | m['ft']) if t.startswith(('ol_', 'or_'))}
equipment = sorted({frozenset(own(s)) for s in S}, key=lambda e: (len(e), sorted(e)))
rows, counts, ambiguous = [], collections.Counter(), []
for m in M:
    death = 'Death' in m['group']
    for E in equipment:
        if not attacker_needs(m) <= set(E): continue
        cands = [s for s in S if ('Death' in s['group']) == death and own(s) <= set(E)
                 and all(attacker_has(m, n) for n in victim_needs(s))]
        if not cands: counts['no_reaction_for_this_equipment'] += 1; continue
        best = max(len(own(s)) + len(s['ft']) for s in cands)
        top = [s for s in cands if len(own(s)) + len(s['ft']) == best]
        counts['unique' if len(top) == 1 else 'ambiguous'] += 1
        if len(top) > 1: ambiguous.append((m['clip'], sorted(E), [s['clip'] for s in top]))
        s = top[0]
        rows.append(dict(attacker=m['clip'], attackerGroup=m['group'], attackerGuid=m['guid'], attackerSeconds=m['seconds'],
                         victimEquipment='+'.join(sorted(E)) or '(none)', reaction=s['clip'], reactionGroup=s['group'],
                         reactionGuid=s['guid'], reactionSeconds=s['seconds'], reactionBlendOut=s['blendOut'],
                         reactionLayer0ExitTimes=s['layer0ExitTimes'], candidates=len(cands), unique=len(top) == 1))
json.dump(dict(source=src, model=__doc__.split('Model')[1].split('Output')[0].strip(), counts=counts,
               attackerFragments=len(M), reactionFragments=len(S), rows=rows), open(out_json, 'w'), indent=1)
with open(out_csv, 'w', newline='') as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0].keys())); w.writeheader(); w.writerows(rows)
print('attacker fragments', len(M), 'reaction fragments', len(S), dict(counts))
for a in ambiguous[:10]: print('AMBIGUOUS', a)
sys.exit(1 if ambiguous else 0)
