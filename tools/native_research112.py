"""Read-only PE research helpers. Candidate xrefs require disassembly validation."""
import bisect
import hashlib
import struct
from pathlib import Path


class PE:
    def __init__(self, path):
        self.data = Path(path).read_bytes()
        pe = struct.unpack_from('<I', self.data, 0x3c)[0]
        assert self.data[pe:pe+4] == b'PE\0\0'
        count = struct.unpack_from('<H', self.data, pe+6)[0]
        size = struct.unpack_from('<H', self.data, pe+20)[0]
        self.stamp = struct.unpack_from('<I', self.data, pe+8)[0]
        self.base = struct.unpack_from('<Q', self.data, pe+48)[0]
        self.sections = []
        for i in range(count):
            p = pe+24+size+i*40
            _, va, rawsize, raw = struct.unpack_from('<IIII', self.data, p+8)
            self.sections.append((self.data[p:p+8].rstrip(b'\0'), va, rawsize, raw))
        er, es = struct.unpack_from('<II', self.data, pe+24+112+24)
        self.functions = [struct.unpack_from('<III', self.data, self.offset(er)+i)[:2] for i in range(0, es, 12)]
        self.starts = [f[0] for f in self.functions]

    def offset(self, rva, size=1):
        for _, va, rawsize, raw in self.sections:
            if va <= rva and rva+size <= va+rawsize:
                return raw+rva-va
        raise ValueError(hex(rva))

    def read(self, rva, size):
        o = self.offset(rva, size)
        return self.data[o:o+size]

    def function(self, rva):
        i = bisect.bisect_right(self.starts, rva)-1
        return self.functions[i] if i >= 0 and rva < self.functions[i][1] else None

    def calls(self, target):
        _, va, size, raw = next(s for s in self.sections if s[0] == b'.text')
        code = self.data[raw:raw+size]
        at = -1
        while True:
            at = code.find(b'\xe8', at+1)
            if at < 0 or at+5 > len(code):
                break
            if va+at+5+struct.unpack_from('<i', code, at+1)[0] == target:
                yield va+at

    def pointers(self, target):
        needle = struct.pack('<Q', self.base+target)
        for name, va, size, raw in self.sections:
            if name == b'.text':
                continue
            section = self.data[raw:raw+size]
            at = -1
            while True:
                at = section.find(needle, at+1)
                if at < 0:
                    break
                yield va+at


if __name__ == '__main__':
    import sys
    p = PE(sys.argv[1])
    print('SHA256', hashlib.sha256(p.data).hexdigest(), 'timestamp', hex(p.stamp))
    for arg in sys.argv[2:]:
        target = int(arg, 0)
        print(hex(target), 'range', p.function(target), 'bytes', p.read(target, 24).hex())
        print('call candidates', [(hex(a), p.function(a)) for a in p.calls(target)])
        print('pointers', [hex(a) for a in p.pointers(target)])
