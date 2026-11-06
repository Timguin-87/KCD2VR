"""Verify H40's exact-retail object-angle table against decoded native key data.

The offsets below belong only to SHA256 e07f3fd1... doors.dba. The two formats
are the primary CryAnimation 48/64-bit quaternion codecs; nlerp is an offline
format cross-check, not a call to the game's resident animation sampler.
No asset, runtime source, game state, or permission is changed by this verifier.
"""
from pathlib import Path
import argparse,bisect,hashlib,json,math,struct,zipfile
DBA_SHA='e07f3fd185907d4fff92b6afdc3204523fff5563fb9a8439d0fb6cf87c0d7323'
DBA_MEMBER='Animations/assets/doors/doors.dba'
# alias,end key,root rotation locator,pivot rotation locator; each locator is
# rotationFormat/count/byteOffset,timeFormat/count/byteOffset in this exact DBA.
LOCATORS=[['door_l_b_c_player', 73, [8, 2, 134044, 2, 2, 307420], [5, 31, 177540, 6, 8, 301768]], ['door_l_b_o_player', 56, [8, 2, 134044, 2, 2, 307368], [5, 31, 176912, 6, 7, 301720]], ['door_l_f_c_player', 55, [8, 2, 134044, 2, 2, 307364], [5, 32, 176376, 6, 7, 301672]], ['door_l_f_o_player', 46, [8, 2, 134044, 2, 2, 307360], [5, 27, 175876, 6, 6, 301628]], ['stealth_door_l_b_c_player', 96, [8, 2, 134044, 2, 2, 307316], [5, 41, 172156, 6, 10, 301324]], ['stealth_door_l_b_o_player', 69, [8, 2, 134044, 2, 2, 307612], [5, 41, 171348, 6, 8, 301268]], ['stealth_door_l_f_c_player', 71, [8, 2, 134044, 2, 2, 307424], [5, 44, 170680, 6, 8, 301220]], ['stealth_door_l_f_o_player', 86, [8, 2, 134044, 2, 2, 307312], [5, 49, 169956, 6, 9, 301168]], ['door_r_b_c_player', 97, [8, 2, 134044, 2, 2, 307508], [5, 32, 146388, 6, 10, 299364]], ['door_r_b_o_player', 64, [8, 2, 134044, 2, 2, 307524], [5, 42, 145548, 6, 8, 299308]], ['door_r_f_c_player', 64, [8, 2, 134044, 2, 2, 307524], [5, 41, 144920, 6, 8, 299260]], ['door_r_f_o_player', 56, [8, 2, 134044, 2, 2, 307368], [5, 33, 144328, 6, 7, 299212]], ['stealth_door_r_b_c_player', 89, [8, 2, 134044, 2, 2, 307496], [5, 42, 138168, 6, 9, 298752]], ['stealth_door_r_b_o_player', 68, [8, 2, 134044, 2, 2, 307304], [5, 42, 137376, 6, 8, 298696]], ['stealth_door_r_f_c_player', 71, [8, 2, 134044, 2, 2, 307424], [5, 46, 136692, 6, 8, 298664]], ['stealth_door_r_f_o_player', 71, [8, 2, 134044, 2, 2, 307424], [5, 38, 136032, 6, 8, 298648]]]

def track(blob,loc):
    rf,n,ro,tf,tn,to=loc
    if tf==1:times=list(struct.unpack_from('<'+'H'*tn,blob,to))
    elif tf==2:times=list(blob[to:to+tn])
    elif tf==6:
        raw=blob[to:to+tn*2];start,stop,count=struct.unpack_from('<3H',raw)
        bits=int.from_bytes(raw[6:],'little');times=[start+k for k in range(stop-start+1)if(bits>>k)&1]
        assert len(times)==count
    else:raise ValueError('unverified native time codec')
    values=[]
    assert len(times)==n and rf in(5,8)
    for k in range(n):
        size=6 if rf==5 else 8;v=int.from_bytes(blob[ro+k*size:ro+(k+1)*size],'little')
        omitted=(v>>46)&3 if rf==5 else(v>>62)&3
        if rf==5:xyz=[((v>>(15*j))&0x7fff)/23170.-.707106781186 for j in range(3)]
        else:xyz=[(v&0x1fffff)/1482909.-.707106781186,((v>>21)&0x1fffff)/1482909.-.707106781186,((v>>42)&0xfffff)/741454.-.707106781186]
        q=[];j=0
        for i in range(4):
            if i==omitted:q.append(0.)
            else:q.append(xyz[j]);j+=1
        q[omitted]=math.sqrt(max(0.,1-sum(x*x for x in q)));values.append(q)
    return times,values

def sample(tv,t):
    times,values=tv
    if t<=times[0]:return values[0]
    if t>=times[-1]:return values[-1]
    i=bisect.bisect_right(times,t);w=(t-times[i-1])/(times[i]-times[i-1]);a,b=values[i-1],values[i]
    if sum(x*y for x,y in zip(a,b))<0:b=[-x for x in b]
    q=[x*(1-w)+y*w for x,y in zip(a,b)];norm=math.sqrt(sum(x*x for x in q))
    return[x/norm for x in q]

def multiply(a,b):
    x,y,z,w=a;X,Y,Z,W=b
    return[w*X+x*W+y*Z-z*Y,w*Y-x*Z+y*W+z*X,w*Z+x*Y-y*X+z*W,w*W-x*X-y*Y-z*Z]

def yaw(q):
    x,y,z,w=q;return math.atan2(2*(w*z+x*y),1-2*(y*y+z*z))

def decoded_curves(blob):
    if hashlib.sha256(blob).hexdigest()!=DBA_SHA:raise ValueError('unverified doors.dba; no fallback table')
    result=[]
    for alias,end,r,p in LOCATORS:
        rr,pr=track(blob,r),track(blob,p)
        keys=sorted(set([0,end]+[x for x in rr[0]+pr[0]if 0<=x<=end]))
        origin=yaw(multiply(sample(rr,0),sample(pr,0)))
        def angle(key,rr=rr,pr=pr,origin=origin):
            v=yaw(multiply(sample(rr,key),sample(pr,key)))-origin
            return math.atan2(math.sin(v),math.cos(v))
        values=[angle(k)for k in keys]
        for i in range(1,len(values)):
            while values[i]-values[i-1]>math.pi:values[i]-=2*math.pi
            while values[i]-values[i-1]<-math.pi:values[i]+=2*math.pi
        result.append((alias,end,keys,values,angle))
    return result

def lua_rows(curves):
    rows=[]
    for alias,end,keys,values,_ in curves:
        entries=','.join('{'+format(k/end,'.17g')+','+format(a,'.17g')+'}'for k,a in zip(keys,values))
        rows.append("    ['"+alias+"']={"+entries+'},')
    return'\n'.join(rows)

def main():
    p=argparse.ArgumentParser();p.add_argument('--animations-pak',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);p.add_argument('--curve-output',type=Path)
    p.add_argument('--runtime',type=Path,default=Path(__file__).resolve().parents[1]/'mods/KCD2VRDoors/hand_only_h40.lua')
    p.add_argument('--interval-subdivisions',type=int,default=1024);a=p.parse_args()
    assert a.interval_subdivisions>=2
    with zipfile.ZipFile(a.animations_pak)as z:blob=z.read(DBA_MEMBER)
    curves=decoded_curves(blob)
    if a.curve_output:a.curve_output.write_text(lua_rows(curves)+'\n',encoding='utf-8')
    from test_door_clip_angle_h40 import load_helper
    _,H=load_helper(a.runtime);rows=[];checks=0;maximum=0.
    for alias,end,keys,values,angle in curves:
        error=0.
        for key,value in zip(keys,values):
            actual=H.At(alias,key/end);assert isinstance(actual,(int,float))and abs(actual-value)<1e-12;checks+=1
        for lo,hi in zip(keys,keys[1:]):
            for i in range(a.interval_subdivisions+1):
                key=lo+(hi-lo)*i/a.interval_subdivisions
                exact=angle(key);interpolated=H.At(alias,key/end)
                diff=abs(math.atan2(math.sin(interpolated-exact),math.cos(interpolated-exact)))
                error=max(error,diff);checks+=1
        rows.append({'alias':alias,'native_end_key':end,'samples':len(keys),'measured_static_interpolation_max_degrees':math.degrees(error)})
        maximum=max(maximum,error)
    a.output.parent.mkdir(parents=True,exist_ok=True)
    receipt={'status':'PASS','checks':checks,'dba_member':DBA_MEMBER,'dba_sha256':DBA_SHA,'runtime_sha256':hashlib.sha256(a.runtime.read_bytes()).hexdigest(),
      'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'subdivisions_per_native_key_interval':a.interval_subdivisions,
      'maximum_measured_static_interpolation_degrees':math.degrees(maximum),'rows':rows,
      'limits':['Decoded compressed native key data plus nlerp format reference; engine-native resident sampler not executed.',
       'Finite dense measurement, not an analytic global approximation bound or headset acceptance.',
       'Only sixteen exact retail aliases. Unsupported replacement/quest clips are not represented by a guessed fallback.']}
    a.output.write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
