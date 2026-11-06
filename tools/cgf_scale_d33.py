"""D33 chest dagger sheath: write a scaled copy of a static CGF (CryEngine CrCh 0x746, KCD2 .cgf + .cgfm pair).
Scales vertex positions per axis (model space, about the origin = the attachment pivot) in every position stream
(stream type 0 = f32 Vec3, type 15 = P3S_C4B_T2S half Vec3). Everything else (normals/tangents, mesh/node chunks,
physics proxy, material) is copied byte for byte - an attachment does not use the proxy; shading on a non-uniform
shrink is slightly off, acceptable for a small sheath. verified on meat_piece (537 verts, header bytes unchanged).
usage: python3 cgf_scale_d33.py in.cgf out.cgf sx sy sz   (out.cgfm written next to out.cgf)
"""
import struct,sys,os
def half(x): return struct.unpack('<e',struct.pack('<e',x))[0]
def chunks(d):
    sig,ver,n,off=struct.unpack_from('<4sIII',d,0)
    if sig!=b'CrCh' or ver!=0x746: raise SystemExit('not a CrCh 0x746 file')
    return [struct.unpack_from('<HHIII',d,off+i*16) for i in range(n)],off
def scale_streams(d,sx,sy,sz):
    d=bytearray(d); cs,_=chunks(d); done=0
    for t,v,cid,sz_,o in cs:
        if t!=0x1016: continue
        flags,stype,count,esize=struct.unpack_from('<IIII',d,o)[:4]
        base=o+24
        if stype==0 and esize==12:
            for i in range(count):
                x,y,z=struct.unpack_from('<3f',d,base+i*12); struct.pack_into('<3f',d,base+i*12,x*sx,y*sy,z*sz)
            done+=count
        elif stype==15 and esize==16:
            for i in range(count):
                x,y,z=struct.unpack_from('<3e',d,base+i*16); struct.pack_into('<3e',d,base+i*16,half(x*sx),half(y*sy),half(z*sz))
            done+=count
    return bytes(d),done
def main(a):
    src,dst,sx,sy,sz=a[0],a[1],float(a[2]),float(a[3]),float(a[4])
    for ext in ('','m'):
        p=src+ext
        if not os.path.exists(p): continue
        d=open(p,'rb').read(); d2,n=scale_streams(d,sx,sy,sz)
        open(dst+ext,'wb').write(d2); print(dst+ext,'scaled verts',n)
if __name__=='__main__': main(sys.argv[1:])
