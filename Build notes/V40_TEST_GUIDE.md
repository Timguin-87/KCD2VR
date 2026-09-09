# V40 test

Recommended initial config:
```
VrStereoExperiment = 3
VrSwapEyes = true
EnableLegacyStereoProbes = false
```

1. Start game and confirm `V40-LEAN legacy V29-V36 runtime probes disabled`.
2. Press Home once and compare the doubled/crossed image. Log: `V40-SWAP-EYES now=...`.
3. Press Insert to cycle modes. Mode 1 is the existing AFR proof. Modes 2/3 currently report their scaffold status and fall back safely rather than pretending to be native.
4. Compare frame rate against V39. The absence of `STEREO-GATE-V35` spam is a key expected change.
