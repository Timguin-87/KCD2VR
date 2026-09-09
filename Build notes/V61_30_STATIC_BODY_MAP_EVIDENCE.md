# V61.30 static full-body map evidence (WHGame.dll supplied build)

This note records the static facts used by the F9 FULLBODY safety gate.

## Mapper

At `WHGame+0xB7EB04`, the retail function resolves skeleton joints and stores each result as a 16-bit value:

```asm
movzx edx, ax
mov   rax, [rbp+8]
mov   word ptr [rax+r15*2+0x17C], dx
```

The index in `r15` comes from the semantic BoneID parser at `WHGame+0xB7EBFC`.

Therefore the populated table layout used by V61.30 is read-only:

`jointId = *(uint16_t*)(owner + 0x17C + 2*BoneID)`

V61.30 does not call either function; it only reads a candidate table already populated by the game.

## BoneIDs used by the runtime proof

The parser's string comparisons and return values establish:

- `BONEID_PELVIS = 2`
- `BONEID_SPINE = 3`
- `BONEID_SPINE1 = 4`
- `BONEID_SPINE2 = 5`
- `BONEID_SPINE3 = 6`
- `BONEID_NECK = 9`
- `BONEID_HEAD = 10`
- `BONEID_HAND_L = 18`
- `BONEID_HAND_R = 19`
- `BONEID_FOOT_L = 22`
- `BONEID_FOOT_R = 23`
- `BONEID_CLAVICLE_L = 32`
- `BONEID_CLAVICLE_R = 33`
- `BONEID_UPPERARM_L/R = 36/37`
- `BONEID_FOREARM_L/R = 40/41`
- `BONEID_ELBOW_L/R = 42/43`

## Runtime acceptance rule

A candidate semantic table is **not trusted from shape/range alone**. F9 FULLBODY accepts it only when its semantic `HAND_L` and `HAND_R` joint IDs equal the two live Henry joints independently captured through `WeaponSolver::SetIKTargets` (order-insensitive).

If that cross-check fails, V61.30 logs `V61.30-FULLBODY MAP-WAIT` and performs no body write.
