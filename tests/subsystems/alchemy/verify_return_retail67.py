"""Verify production local-return byte gates against the exact retail PE, read-only."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import sys

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools"))
from smith_native_probe46 import PE, EXPECTED


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("dll", type=Path)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    image = PE(args.dll)
    source = ROOT / "src/subsystems/alchemy/native_return67.inl"
    text = source.read_text()
    rows = []
    for name, rva in (("cancel", 0x15FCED0), ("retire", 0x9CC9D4),
                      ("detach", 0x10853CC), ("home", 0x6C531C), ("caller", 0x6C2A53)):
        match = re.search(r"constexpr unsigned char " + name + r"\[\]\{([^}]+)\};", text)
        assert match, name
        expected = bytes(int(x, 16) for x in re.findall(r"0x([0-9A-Fa-f]{2})", match[1]))
        actual = image.read(rva, len(expected))
        assert actual == expected, (name, hex(rva), expected.hex(), actual.hex())
        rows.append(dict(name=name, rva=hex(rva), bytes=actual.hex(), length=len(actual), verified=True))
    result = dict(result="PASS", image_sha256=EXPECTED, timestamp=hex(image.stamp),
                  source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(), pins=rows,
                  method="Read-only current production arrays against exact retail PE; no game execution")
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2) + "\n")
    print("return67 exact retail pins:", len(rows), "PASS")


if __name__ == "__main__":
    main()
