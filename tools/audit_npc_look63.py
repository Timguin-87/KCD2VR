"""Exact retail evidence for the scoped native Henry LookAt target seam."""
import argparse, hashlib, json, struct
from pathlib import Path
from audit_ui175 import PE, EXPECTED_SHA256

def audit(path):
    pe=PE(path)
    sha=hashlib.sha256(pe.data).hexdigest()
    assert sha==EXPECTED_SHA256 and pe.timestamp==0x6A350E20
    checks=[]
    def pin(name,rva,data):
        ok=pe.read(rva,len(data))==data
        checks.append(dict(name=name,rva=hex(rva),size=len(data),passed=ok))
        assert ok,name
    pin('native resolver entry',0x59B0F0,bytes.fromhex('48895c24084889742410574883ec30'))
    pin('native wrapper payload at18 and output R8',0x59B0FF,bytes.fromhex('48837a1800498bd8488bfa488bf1'))
    pin('native successful output returns true',0x59B167,bytes.fromhex('b001'))
    pin('native missing payload returns false',0x59B179,bytes.fromhex('32c0'))
    pin('native follows payload GetEntity slot18',0x59B34C,bytes.fromhex('488b4a18488b01ff5018'))
    pin('pinned puppet getter reads entity38',0x602360,bytes.fromhex('488b4138c3'))
    for name,vft in [('C_AIPuppet',0x3A399A8),('C_SOPuppet',0x3A6B9E8)]:
        pin(name+' getter slot18',vft+0x18,struct.pack('<Q',pe.image_base+0x602360))
    return dict(sha256=sha,timestamp=hex(pe.timestamp),passed=all(c['passed'] for c in checks),checks=checks,
                signature='bool native ResolveLookTargetWorldPos(owner,wrapper,float out[3]); original always first',
                limits='Static seam/ABI evidence only. Native dialogue participant LookAt invocation and visible head-follow remain HMD-unverified; no AI task or participant ownership is changed.')

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--dll',required=True);ap.add_argument('--out',required=True)
    args=ap.parse_args();result=audit(args.dll)
    Path(args.out).write_text(json.dumps(result,indent=2)+'\n')
    print('PASS scoped Henry LookAt exact-retail %d checks'%len(result['checks']))
