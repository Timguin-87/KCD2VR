"""Search indexed archived evidence without reopening 468 original files."""
import argparse
import io
import re
import sqlite3
import zlib
from pathlib import Path
from discover_vrik_logs113 import family


def rows(db, family_like='%', path_like='%', context=False):
    # Chunk indexes narrow I/O; apply the same SQL LIKE semantics to each line.
    # Otherwise a query silently counts neighboring families in the same chunk.
    matches = re.compile('^' + ''.join('.*' if c=='%' else '.' if c=='_' else re.escape(c)
                                     for c in family_like) + '$', re.I)
    # Do not DISTINCT/sort compressed blobs after a many-to-many join: that can
    # create gigabytes of temporary I/O. Filter integer chunk IDs first instead.
    query='''SELECT c.id,l.path,c.first_line,c.data FROM chunks c
        JOIN logs l ON l.id=c.log_id WHERE l.path LIKE ?'''
    args=[path_like]
    if family_like!='%':
        query+=' AND c.id IN (SELECT chunk_id FROM chunk_families WHERE family LIKE ?)'
        args.append(family_like)
    query+=' ORDER BY c.log_id,c.first_line'
    for _,path,start,blob in db.execute(query,args):
        for offset,line in enumerate(io.BytesIO(zlib.decompress(blob))):
            if context or family_like=='%' or matches.fullmatch(family(line)):
                yield path,start+offset,line.decode('utf-8',errors='replace').rstrip('\r\n')


if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--db',required=True,type=Path)
    p.add_argument('--family',default='%')
    p.add_argument('--path',default='%')
    p.add_argument('--text',default='.')
    p.add_argument('--limit',type=int,default=40)
    p.add_argument('--context',action='store_true',help='Include neighboring lines in matching chunks')
    a=p.parse_args()
    db=sqlite3.connect(a.db.resolve().as_uri()+'?mode=ro',uri=True)
    pattern=re.compile(a.text,re.I)
    count=0
    for path,line,text in rows(db,a.family,a.path,a.context):
        if pattern.search(text):
            count+=1
            if count<=a.limit:
                print(f'{path}:{line}: {text}')
    print(f'matches={count}, displayed={min(count,a.limit)}; stored lines are observations, not ABI authorization')
