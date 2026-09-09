# V58.9 build note

Source-only Windows/MSVC candidate.

Key changes:
- F8 reduced to four mono-projection phases; native render-twice phases removed from the active ladder.
- Pre-V46.6 black-edge mapping reconstructed from surviving V46.5/V46.6 documentation; this is explicitly an approximation because the exact V46.5 source snapshot is not present in the package.
- Cyberpunk-inspired cyclopean direct-copy phase added without claiming binary-equivalent behavior.
- Touch locomotion changed to left-stick 2D movement + right-stick smooth turn.
- Left Y repurposed from temporary weapon-slot toggle to jump.
- Touch inventory short-press toggles the world-fixed comfort quad, avoiding the observed 31° inventory camera being fed into immersive mono remap.
