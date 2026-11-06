# host test for cgf_scale_d33: synthetic CrCh 0x746 file with one f32 and one P3S position stream
import struct,os,sys,tempfile
sys.path.insert(0,os.path.dirname(__file__)); import cgf_scale_d33 as t
def build():
    pos=[(1.0,2.0,3.0),(-0.5,0.25,0.125)]
    s0=struct.pack('<IIIIHH',0,0,len(pos),12,0,0)+b'\0'*4+b''.join(struct.pack('<3f',*p) for p in pos)
    s15=struct.pack('<IIIIHH',0,15,len(pos),16,0,0)+b'\0'*4+b''.join(struct.pack('<3e',*p)+b'\0\0'+b'\x11\x22\x33\x44'+b'\0'*4 for p in pos)
    n=2; off=16; data_off=off+n*16
    hdr=struct.pack('<4sIII',b'CrCh',0x746,n,off)
    tab=struct.pack('<HHIII',0x1016,0x800,1,len(s0),data_off)+struct.pack('<HHIII',0x1016,0x800,2,len(s15),data_off+len(s0))
    return hdr+tab+s0+s15,pos
d,pos=build(); out,n=t.scale_streams(d,2.0,0.5,-1.0); fails=0
def chk(c,m):
    global fails
    if not c: fails+=1; print('FAIL',m)
chk(n==4,'4 verts scaled')
base0=16+32+24
for i,(x,y,z) in enumerate(pos):
    X,Y,Z=struct.unpack_from('<3f',out,base0+i*12); chk((X,Y,Z)==(2*x,0.5*y,-z),'f32 vert %d'%i)
base15=base0+2*12+24
for i,(x,y,z) in enumerate(pos):
    X,Y,Z=struct.unpack_from('<3e',out,base15+i*16); chk(abs(X-2*x)<1e-3 and abs(Y-0.5*y)<1e-3 and abs(Z+z)<1e-3,'half vert %d'%i)
    chk(out[base15+i*16+8:base15+i*16+16]==d[base15+i*16+8:base15+i*16+16],'colour/uv untouched %d'%i)
chk(out[:base0]==d[:base0],'header/table untouched')
try: t.scale_streams(b'XXXX'+b'\0'*12,1,1,1); chk(False,'bad magic refused')
except SystemExit: pass
print('cgf_scale_d33 checks done, fails=%d'%fails); sys.exit(1 if fails else 0)
