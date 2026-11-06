"""Halve six authored message containers in the supplied retail HUD movie.

No ActionScript, text, font, icon, fade, position, root or other clip is edited.
The native message manager attaches each new message below these containers, so
their size exists before the first notification rather than after a worker retry.
Run with --input the pinned retail hud.gfx; the default output is canonical source.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct
import zlib

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SHA256 = "85303baaeaa925ae051fe49626f15e638642908599886168682a16e402a065cd"
TARGETS = {
    (692, "infoText"): "cccursor.infoText",
    (929, "gameLog"): "tr.gameLog",
    (929, "commonEvent"): "tr.commonEvent",
    (927, "fancyEvent"): "tc.fancyEvent",
    (799, "skillCheck"): "bc.skillCheck",
    (940, "itemTransfer"): "bl.itemTransfer",
}
PARENTS = {"cccursor": 692, "tr": 929, "tc": 927, "bc": 799, "bl": 940}


class Bits:
    def __init__(self, data: bytes, offset: int):
        self.data, self.bit = data, offset * 8

    def unsigned(self, count: int) -> int:
        value = 0
        for _ in range(count):
            value = (value << 1) | ((self.data[self.bit // 8] >> (7 - self.bit % 8)) & 1)
            self.bit += 1
        return value

    def signed(self, count: int) -> int:
        value = self.unsigned(count)
        return value - (1 << count) if count and value & (1 << (count - 1)) else value


def matrix(data: bytes, offset: int):
    bits = Bits(data, offset)
    a = d = 65536
    if bits.unsigned(1):
        count = bits.unsigned(5)
        a, d = bits.signed(count), bits.signed(count)
    b = c = 0
    if bits.unsigned(1):
        count = bits.unsigned(5)
        b, c = bits.signed(count), bits.signed(count)
    count = bits.unsigned(5)
    tx, ty = bits.signed(count), bits.signed(count)
    return (a, b, c, d, tx, ty), (bits.bit + 7) // 8


def encode_matrix(value) -> bytes:
    a, b, c, d, tx, ty = value
    bits = []

    def put(number, count):
        number &= (1 << count) - 1
        bits.extend((number >> i) & 1 for i in range(count - 1, -1, -1))

    def pair(x, y):
        count = max(abs(x).bit_length(), abs(y).bit_length()) + 1
        assert count < 32
        put(count, 5)
        put(x, count)
        put(y, count)

    scaled = (a, d) != (65536, 65536)
    put(int(scaled), 1)
    if scaled:
        pair(a, d)
    rotated = bool(b or c)
    put(int(rotated), 1)
    if rotated:
        pair(b, c)
    pair(tx, ty)
    while len(bits) % 8:
        bits.append(0)
    return bytes(sum(bits[i + j] << (7 - j) for j in range(8)) for i in range(0, len(bits), 8))


def placement(data: bytes, kind: int):
    flags = data[0]
    second = data[1] if kind == 70 else 0
    offset = 4 if kind == 70 else 3
    if kind == 70 and ((second & 8) or ((second & 16) and (flags & 2))):
        offset = data.index(0, offset) + 1
    character = 0
    if flags & 2:
        character = struct.unpack_from("<H", data, offset)[0]
        offset += 2
    start = end = offset
    value = None
    if flags & 4:
        value, offset = matrix(data, offset)
        end = offset
    if flags & 8:
        bits = Bits(data, offset)
        add, multiply, count = bits.unsigned(1), bits.unsigned(1), bits.unsigned(4)
        for _ in range(4 * (add + multiply)):
            bits.signed(count)
        offset = (bits.bit + 7) // 8
    if flags & 16:
        offset += 2
    name = data[offset:data.index(0, offset)].decode("utf-8") if flags & 32 else ""
    return name, character, value, start, end


def unpack(data: bytes):
    if data[:3] != b"CFX" or data[3] != 8:
        raise ValueError("Expected the pinned compressed GFx v8 movie")
    body = zlib.decompress(data[8:])
    if len(body) + 8 != struct.unpack_from("<I", data, 4)[0]:
        raise ValueError("GFx declared length mismatch")
    rectangle_bytes = (5 + 4 * (body[0] >> 3) + 7) // 8
    header_bytes = rectangle_bytes + 4
    return body[:header_bytes], body[header_bytes:]


def parse_tags(data: bytes, sprite=0, path=()):
    result = []
    offset = 0
    ordinal = 0
    while offset + 2 <= len(data):
        begin = offset
        header = struct.unpack_from("<H", data, offset)[0]
        offset += 2
        kind, size = header >> 6, header & 63
        if size == 63:
            size = struct.unpack_from("<I", data, offset)[0]
            offset += 4
        payload = data[offset:offset + size]
        if len(payload) != size:
            raise ValueError("Truncated GFx tag")
        offset += size
        key = path + (ordinal,)
        ordinal += 1
        result.append((key, sprite, kind, payload, data[begin:offset]))
        if kind == 39:
            child = struct.unpack_from("<H", payload)[0]
            result.extend(parse_tags(payload[4:], child, key))
        if kind == 0:
            if offset != len(data):
                raise ValueError("Unexpected trailing tag bytes")
            return result
    raise ValueError("Missing end tag")


def rewrite(data: bytes, edits: dict, path=()) -> bytes:
    output = bytearray()
    offset = ordinal = 0
    while offset < len(data):
        begin = offset
        header = struct.unpack_from("<H", data, offset)[0]
        offset += 2
        kind, size = header >> 6, header & 63
        if size == 63:
            size = struct.unpack_from("<I", data, offset)[0]
            offset += 4
        payload = data[offset:offset + size]
        offset += size
        key = path + (ordinal,)
        ordinal += 1
        replacement = edits.get(key, payload)
        if kind == 39:
            replacement = payload[:4] + rewrite(payload[4:], edits, key)
        if replacement == payload:
            output.extend(data[begin:offset])
        else:
            output.extend(struct.pack("<HI", (kind << 6) | 63, len(replacement)))
            output.extend(replacement)
        if kind == 0:
            assert offset == len(data)
    return bytes(output)


def patch(source: bytes):
    if hashlib.sha256(source).hexdigest() != SOURCE_SHA256:
        raise ValueError("Retail HUD SHA256 mismatch; no output written")
    header, tags = unpack(source)
    before = parse_tags(tags)
    edits, changed, parents = {}, {}, {}
    for key, sprite, kind, payload, raw in before:
        if kind not in (26, 70):
            continue
        name, character, value, start, end = placement(payload, kind)
        if sprite == 0 and name in PARENTS:
            parents[name] = character
        target = TARGETS.get((sprite, name))
        if target is None:
            continue
        if target in changed or kind != 26 or payload[0] != 0x26 or character != 152:
            raise ValueError("Notification placement ownership mismatch")
        if value is None or value[:4] != (65536, 0, 0, 65536):
            raise ValueError("Unexpected native notification transform")
        half = (32768, 0, 0, 32768, value[4], value[5])
        edits[key] = payload[:start] + encode_matrix(half) + payload[end:]
        changed[target] = {"sprite": sprite, "before": value, "after": half, "tag_key": key}
    if parents != PARENTS or set(changed) != set(TARGETS.values()):
        raise ValueError("Exact HUD hierarchy or six targets missing")
    result_tags = rewrite(tags, edits)
    after = parse_tags(result_tags)
    if len(before) != len(after):
        raise ValueError("Tag inventory changed")
    unchanged_leaf = script_count = 0
    script_sha = hashlib.sha256()
    for left, right in zip(before, after):
        key, sprite, kind, payload, raw = left
        rkey, rsprite, rkind, rpayload, rraw = right
        assert (key, sprite, kind) == (rkey, rsprite, rkind)
        if kind == 39:
            assert payload[:4] == rpayload[:4]  # same symbol and frame count
        elif key in edits:
            assert rpayload == edits[key]
            name, character, value, start, end = placement(rpayload, kind)
            old_name, old_character, old_value, old_start, old_end = placement(payload, kind)
            assert (name, character) == (old_name, old_character)
            assert value[:4] == (32768, 0, 0, 32768) and value[4:] == old_value[4:]
            assert rpayload[:start] == payload[:old_start] and rpayload[end:] == payload[old_end:]
        else:
            assert raw == rraw  # every unmodified leaf remains byte-for-byte exact
            unchanged_leaf += 1
        if kind in (12, 59):
            assert payload == rpayload
            script_sha.update(struct.pack("<I", len(payload)) + payload)
            script_count += 1
    body = header + result_tags
    output = source[:4] + struct.pack("<I", len(body) + 8) + zlib.compress(body, 9)
    report = {"input_sha256": SOURCE_SHA256, "output_sha256": hashlib.sha256(output).hexdigest(),
              "input_bytes": len(source), "output_bytes": len(output), "changed_containers": changed,
              "unchanged_leaf_tags": unchanged_leaf, "unchanged_script_blocks": script_count,
              "script_blocks_sha256": script_sha.hexdigest(), "native_or_headset_execution": False}
    return output, report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, default=ROOT / "companion/source/Libs/UI/hud.gfx")
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args()
    output, report = patch(args.input.read_bytes())
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(output)
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
