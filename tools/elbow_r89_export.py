#!/usr/bin/env python3
# R89 elbow: export the solver inputs of F7 VRIK rows (ELBOW_INPUT + ELBOW + TORSO + ARM) to a CSV for
# tools/elbow_r89_replay_test.cpp.  usage: elbow_r89_export.py KCD2VR.log out.csv
# only frames where the VRIK arm solver ran (solveOk=1) are written, sorted per session/side/sample.
import re, sys
vec = re.compile(r'(\w+)=\(([-0-9.e,]+)\)')
sca = re.compile(r'(\w+)=([-0-9.e]+)(?=\s|$)')
kinds = ('ELBOW_INPUT', 'ELBOW', 'TORSO', 'ARM')
rows = {}
with open(sys.argv[1], 'rb') as fh:
    for raw in fh:
        if b'VRIK_F7 ' not in raw:
            continue
        line = raw.decode('ascii', 'replace')
        m = re.search(r'VRIK_F7 (\w+) ', line)
        if not m or m.group(1) not in kinds:
            continue
        d = {k: [float(x) for x in v.split(',')] for k, v in vec.findall(line)}
        for k, v in sca.findall(line):
            d.setdefault(k, float(v))
        try:
            key = (int(d['session']), int(d['side']), int(d['sample']))
        except KeyError:
            continue
        rows.setdefault(key, {})[m.group(1)] = d
cols = ['session', 'side', 'sample', 'tick', 'timeNs', 'epoch', 'bow', 'frikLab',
        'sx', 'sy', 'sz', 'gx', 'gy', 'gz', 'upper', 'fore', 'rx', 'ry', 'rz', 'bx', 'by', 'bz',
        'px', 'py', 'pz', 'ax', 'ay', 'az', 'tx', 'ty', 'tz', 'radius', 'qx', 'qy', 'qz', 'qw', 'fx', 'fy', 'fz']
n = 0
with open(sys.argv[2], 'w') as out:
    out.write(','.join(cols) + '\n')
    for key in sorted(rows):
        r = rows[key]
        if len(r) < 4 or not r['ELBOW_INPUT'].get('solveOk'):
            continue
        e, t, a = r['ELBOW_INPUT'], r['TORSO'], r['ARM']
        v = [key[0], key[1], key[2], int(e['sourceTick']), int(e['timeNs']), int(e['epoch']), int(e['bowPolicy']), int(e['frikLab'])]
        v += t['shoulder'] + t['goal'] + t['lengths'] + t['rawPole'] + e['bodyBackward'] + t['finalPreference']
        v += t['torsoA'] + t['torsoB'] + [t['radius']] + a['wristQuat'] + e['filteredPole']
        out.write(','.join(repr(x) for x in v) + '\n')
        n += 1
print('rows', n)
