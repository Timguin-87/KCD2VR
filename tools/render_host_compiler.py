"""Host-only C++ replay compilation; MSVC on Windows, g++ elsewhere."""
import os
from pathlib import Path

def binary(path):
    path=Path(path)
    return path.with_suffix('.exe') if os.name=='nt' else path

def command(args):
    if os.name!='nt':return args
    out=[os.environ.get('CXX','cl'),'/nologo','/std:c++20','/EHsc','/W4','/D_CRT_SECURE_NO_WARNINGS']
    target=None;i=1
    while i<len(args):
        a=args[i];i+=1
        if a=='-o':
            target=Path(args[i]);i+=1;out+=['/Fe:'+str(target),'/Fo:'+str(target.parent)+os.sep]
        elif a.startswith('-I'):out.append('/I'+a[2:])
        elif a.startswith('-D'):out.append('/D'+a[2:])
        elif a in ('-O1','-O2'):out.append('/O2')
        elif a in ('-O0','-g'):out.append('/Od' if a=='-O0' else '/Zi')
        elif a in ('-std=c++20','-pthread','-Wall','-Wextra','-Wno-unused-parameter','-Wno-unused-variable','-Wno-unused-function'):pass
        elif a.startswith('-'):raise RuntimeError('Unsupported MSVC replay option '+a+'; use --skip-sanitizers on Windows')
        else:out.append(a)
    return out
