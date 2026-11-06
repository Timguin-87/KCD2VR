# KCD2VR gore tool: makes "cut" and "piece" variants of a game .skin by turning triangles into
# degenerate ones (same vertex three times). nothing else in the file changes (sizes, chunk table).
import struct,sys,os,re
SETS={
 'lh':lambda n:n.startswith('LeftHand') or n.startswith('LeftInHand'),
 'rh':lambda n:n.startswith('RightHand') or n.startswith('RightInHand'),
 'lf':lambda n:n in('LeftFoot','LeftToeBase','LeftHeel','LeftFoot_IKBlend','GroundAlignLeft','LeftToeBaseEnd'),
 'rf':lambda n:n in('RightFoot','RightToeBase','RightHeel','RightFoot_IKBlend','GroundAlignRight','RightToeBaseEnd'),
}
def chunks(d):
    n,to=struct.unpack_from('<II',d,8);return [struct.unpack_from('<HHIII',d,to+16*i) for i in range(n)]
def parse(d):
    if d[:4]!=b'CrCh' or struct.unpack_from('<I',d,4)[0]!=0x746:raise ValueError('not CrCh 746')
    ch=chunks(d);c={};st={};bn=[]
    for t,v,cid,sz,off in ch:
        if t==0x1016:
            fl,ty,n,esz,_=struct.unpack_from('<IIIHH',d,off);st[ty]=(off+24,n,esz)
        elif t in(0x2000,0x2004,0x2005,0x2006):c[t]=(off,sz)
    if 0x2000 in c:
        off,sz=c[0x2000];i=0
        while 344+584*i<sz:bn.append(bytes(d[off+344+584*i:off+344+584*i+64]).split(b'\0')[0].decode('latin1'));i+=1
    return st,c,bn
def dominant(d,st,nv):
    bo,n,esz=st[9];out=[]
    for i in range(nv):
        ids=struct.unpack_from('<4H',d,bo+esz*i);w=d[bo+esz*i+8:bo+esz*i+12];out.append(ids[max(range(4),key=lambda k:w[k])])
    return out
def variants(path):
    d=bytearray(open(path,'rb').read());st,c,bn=parse(d)
    if 0 not in st or 5 not in st or 9 not in st:return {},'no pos/index/bonemap'
    nv=st[0][1];dom=dominant(d,st,nv);io,ni,iesz=st[5]
    if iesz!=2:return {},'index esz %d'%iesz
    idx=list(struct.unpack_from('<%dH'%ni,d,io))
    e2i=None;iv=None
    if 0x2006 in c and 0x2004 in c:
        o,sz=c[0x2006];e2i=struct.unpack_from('<%dH'%nv,d,o) if sz>=2*nv else None
    res={}
    for key,pred in SETS.items():
        inS=[pred(bn[b]) if b<len(bn) else False for b in dom]
        if not any(inS):continue
        intIn={}
        if e2i:
            for e,i in enumerate(e2i):intIn[i]=intIn.get(i,False) or inS[e]
        for mode in('cut','piece'):
            out=bytearray(d);rem=0
            for t in range(ni//3):
                a,b_,c_=idx[3*t:3*t+3];k=(inS[a],inS[b_],inS[c_])
                drop=all(k) if mode=='cut' else not any(k)
                if drop:struct.pack_into('<3H',out,io+6*t,a,a,a);rem+=1
            if rem==0 or (mode=='piece' and rem==ni//3):continue
            if e2i:
                fo,fsz=c[0x2004]
                for t in range(fsz//6):
                    a,b_,c_=struct.unpack_from('<3H',d,fo+6*t);k=(intIn.get(a,False),intIn.get(b_,False),intIn.get(c_,False))
                    if (all(k) if mode=='cut' else not any(k)):struct.pack_into('<3H',out,fo+6*t,a,a,a)
            cen=None
            if mode=='piece':
                po=st[0][0];used=set()
                for t in range(ni//3):
                    tri=idx[3*t:3*t+3]
                    if any(inS[v] for v in tri):used.update(tri)
                sx=sy=sz=0.0
                for v in used:x,y,z=struct.unpack_from('<3f',d,po+12*v);sx+=x;sy+=y;sz+=z
                cen=(sx/len(used),sy/len(used),sz/len(used)) if used else (0,0,0)
            res[key+('' if mode=='cut' else 'p')]=(out,rem,ni//3,cen)
    return res,''
if __name__=='__main__':
    src,dst,lua=sys.argv[1],sys.argv[2],sys.argv[3]
    table={};count=0
    for root,_,files in sorted(os.walk(src)):
        for f in sorted(files):
            if not f.lower().endswith('.skin'):continue
            p=os.path.join(root,f);rel=os.path.relpath(p,src).replace('\\','/')
            try:res,why=variants(p)
            except Exception as e:print('ERR',rel,e);continue
            base=rel[:-5];m=re.match(r'(.*?)(_LOD\d)$',base,re.I)
            stem,lod=(m.group(1),m.group(2)) if m else (base,'')
            for key,(out,rem,nt,cen) in res.items():
                if lod and key.endswith('p'):continue   # flying pieces: full detail only
                q=stem+'_kcdg'+key+lod+'.skin'
                os.makedirs(os.path.dirname(os.path.join(dst,q)),exist_ok=True);open(os.path.join(dst,q),'wb').write(out);count+=1
                print('OK',rel,key,'%d/%d'%(rem,nt))
                if not lod:
                    e=table.setdefault(rel.lower(),{});e[key]=q.lower()
                    if cen:e['c_'+key]=cen
    with open(lua,'w',newline='\n') as o:
        o.write('-- KCD2VR Gore: cut copies of the game\'s own body/hand/foot/leg meshes (made by gore_split.py, triangles only).\n')
        o.write('-- key = original mesh, lh/rh/lf/rf = the mesh without that hand/foot, ..p = only that hand/foot (+ centre in model space)\n')
        o.write('KCD2VR_Gore = KCD2VR_Gore or {}\nKCD2VR_Gore.V = {\n')
        for k in sorted(table):
            parts=[]
            for kk in sorted(table[k]):
                v=table[k][kk]
                parts.append("%s={x=%.4f,y=%.4f,z=%.4f}"%(kk,*v) if kk.startswith('c_') else "%s='%s'"%(kk,v))
            o.write("  ['%s']={%s},\n"%(k,','.join(parts)))
        o.write('}\n')
    print('variants',count,'meshes',len(table))
