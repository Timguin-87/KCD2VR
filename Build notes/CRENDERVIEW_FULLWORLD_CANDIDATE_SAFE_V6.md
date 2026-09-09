# CRenderView Full-World Candidate SAFE-V6

This is a targeted observer build based on the static WHGame.dll analysis of the engine-managed derived-view helper at `WHGame+0x4E6F1C`.

No new engine hook is added.

Retained safe hooks:
- CRenderView managed checkout/factory/constructor observers
- CSystem ViewCamera publication
- previous stable camera/CSystem/C3DEngine observers

Disabled:
- `WHGame+0x4E6924`
- `WHGame+0x4E7E64`

New correlation:
1. Classifies each mode-2 checkout by its stack family:
   - `main-4E6C`
   - `alternate-4E73`
2. Waits until the engine has finished configuring the derived CRenderView.
3. Confirms its `+0x38` parent/source CRenderView.
4. Correlates the child and source against the persistent `CSystem+0x288` ViewCamera:
   - exact pointer scan across the known 0xA770 CRenderView allocation
   - exact 0x30-byte camera matrix scan, 4-byte aligned
5. Logs one compact `CRV-CANDIDATE post-config` line per sampled derived view.

Important:
- observer only
- no custom CRenderView allocation
- no render-loop hook
- no stereo forcing
- no AER

High-value log fields:
- `route=main-4E6C` vs `route=alternate-4E73`
- `source38=...`
- `child_cam_ptr_off`
- `source_cam_ptr_off`
- `child_cam_matrix_off`
- `source_cam_matrix_off`

Offsets print as `0xFFFFFFFFFFFFFFFF` when not found.
