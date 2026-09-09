# V31 — Stereo Framework Runtime Probe

V30 established that the two repeatedly consumed mode-0 `CRenderView` owners are renderer/temporal alternates, not a stable left/right eye pair. Their record matrices differ by ordinary camera motion and their dispatch gap varies; therefore V31 does not assign eye semantics to them and does not write camera/projection state.

Static analysis of the supplied `WHGame.dll` found a direct executable registration site for `g_stereoFrameworkEnable`:

- name RVA: `0x3E0EB20`
- help RVA: `0x3E0EAD0`
- registration site: `WHGame+0x1594A90`
- integer-CVar registration helper: `WHGame+0x23D8168`
- call-site default: `1`
- help text: `Enables the processing of the game stereo framework. (2=extra debug output)`

This makes a blind `g_stereoFrameworkEnable=1` patch unjustified: the shipping registration already requests default 1. The next useful question is whether the registration runs, what storage/result object it produces, and whether the runtime gate is present when KCD2VR installs.

V31 adds a read-only full-ABI observer around the integer-CVar registration helper and filters for the exact static `g_stereoFrameworkEnable` name pointer. It logs:

- `STEREO-FRAMEWORK-V31 static ...`
- `STEREO-FRAMEWORK-V31 register-enter ...`
- `STEREO-FRAMEWORK-V31 register-leave ...`
- `STEREO-FRAMEWORK-V31 summary ...`

The V29/V30 renderer/resource/differential observers remain enabled for correlation. No CVar values, cameras, projections, culling state, render targets, or D3D12 bindings are modified.

## Interpretation

1. If `register-enter/leave` appears with default 1, capture the returned/backing object next and trace its readers; the framework gate exists and is enabled by registration semantics.
2. If only the static line appears, registration likely happened before our ASI hook was installed. Then V32 should locate the registered CVar through the game console registry or hook readers of the backing object, rather than forcing a value.
3. If the hook fails, verify the target build before changing any RVA.
