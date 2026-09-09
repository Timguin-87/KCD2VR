# V29 Pipeline Resource Probe

Read-only renderer probe. Old camera-offset experiments are disabled.

Static mapping from WHGame.dll:
- CGraphicsPipeline RTTI/vtable intact; vtable +0x28 -> WHGame+0x57278C.
- That dispatcher passes CRenderView+0x9B30/count to WHGame+0x653940, then executes active graphics-pipeline stages.
- CSceneGBufferStage RTTI/vtable intact; stage vtable +0x20 -> WHGame+0x7BB6B0.
- G-buffer execution calls WHGame+0x7BC700, which configures four resource inputs plus a separate depth resource for the scene pass.

Runtime markers:
- PIPELINE-V29
- GBUFFER-V29
- GBUFFER-MRT-V29
- RENDER-CONSUME-V29
- XR-RUNTIME-V29

Goal: correlate active CRenderView/source owners with concrete G-buffer resources, then trace those resources to CryEngine texture/D3D12 backing objects and ultimately OpenXR eye submission.
