# C50: tools/gore_split.py on a tiny synthetic CrCh 0x746 skin (bones Spine/LeftHand, 5 vertices, 3 triangles)
import struct,os,tempfile,importlib.util
spec=importlib.util.spec_from_file_location('gs',os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','tools','gore_split.py'))
gs=importlib.util.module_from_spec(spec);spec.loader.exec_module(gs)
import unittest
class GoreSplit50(unittest.TestCase):
    def test_cut_and_piece(self):
        bones=bytearray(32+584*2)
        for i,n in enumerate([b'Spine',b'LeftHand']):bones[344+584*i:344+584*i+len(n)]=n
        def stream(ty,n,esz,data):return struct.pack('<IIIHH',0,ty,n,esz,0)+b'\0'*8+data
        pos=struct.pack('<18f',0,0,1, 0,1,1, -.5,0,1, -.6,.1,1, -.6,-.1,1, .2,0,1)
        bone=(0,0,1,1,1,0)                                # vertices 0,1,5 spine; 2,3,4 left hand
        tris=[(0,1,5),(2,3,4),(1,2,3)]                   # all spine, all hand, mixed (seam)
        idx=struct.pack('<9H',*[v for t in tris for v in t])
        bm=b''.join(struct.pack('<4H4B',b,0,0,0,255,0,0,0) for b in bone)
        faces=struct.pack('<9H',*[v for t in tris for v in t]);e2i=struct.pack('<6H',0,1,2,3,4,5)
        chs=[(0x2000,bytes(bones)),(0x2004,faces),(0x2006,e2i),(0x1016,stream(0,6,12,pos)),(0x1016,stream(5,9,2,idx)),(0x1016,stream(9,6,12,bm))]
        off=16+16*len(chs);body=b'';tab=b''
        for i,(t,data) in enumerate(chs):tab+=struct.pack('<HHIII',t,0x800,i+1,len(data),off+len(body));body+=data
        f=b'CrCh'+struct.pack('<III',0x746,len(chs),16)+tab+body
        p=os.path.join(tempfile.mkdtemp(),'x.skin');open(p,'wb').write(f)
        res,why=gs.variants(p)
        assert set(res)=={'lh','lhp'},(res.keys(),why)
        out,rem,nt,cen=res['lh'];assert (rem,nt)==(1,3) and len(out)==len(f)
        io=off+sum(len(d) for _,d in chs[:4])+24;got=struct.unpack_from('<9H',out,io)
        assert got==(0,1,5, 2,2,2, 1,2,3),got                   # only the all-hand triangle became degenerate
        fo=off+len(chs[0][1]);assert struct.unpack_from('<9H',out,fo)==(0,1,5, 2,2,2, 1,2,3)  # int faces too
        out,rem,nt,cen=res['lhp'];assert (rem,nt)==(1,3)                         # the piece = hand + seam triangle
        assert struct.unpack_from('<9H',out,io)==(0,0,0, 2,3,4, 1,2,3)
        assert abs(cen[0]-(-0.425))<1e-6,cen                                     # centre of vertices 1..4 used by the piece

if __name__=='__main__':unittest.main()
