# V43 OpenXR asymmetric FOV projection proof

Mode 1 disables AFR/IPD camera translation and temporarily replaces only the consumed render record projection coefficients m00/m11/m20/m21 at CRenderView+0x9B30+0x290 using the latest OpenXR XrFovf. KCD2 depth/near/far terms are preserved and the record is restored immediately after CStandardGraphicsPipeline::Execute.

Marker: `V43-XR-FOV ... applied=true ...`

Success criteria: visible zoom/asymmetry change and improved per-eye alignment. If applied=true but no image change, +0x290 is not consumed by the final visible scene path and is retired.
