# V28 — CryEngine Stereo Lineage Probe

Static WHGame.dll analysis found that Warhorse's KCD2 build still contains substantial CryEngine stereo framework material.

Confirmed strings in WHGame.dll include:
- r_stereoScaleCoefficient
- r_StereoStrength
- r_StereoEyeDist
- r_StereoScreenDist
- r_StereoEnableMgpu
- r_StereoFlipEyes
- r_StereoNearGeoScale
- r_StereoHudScreenDist
- r_StereoGammaAdjustment
- r_VolumetricCloudsStereoReprojection
- sys_flash_stereo_maxparallax
- $VolCloudLeftEye
- g_stereoFrameworkEnable
- g_stereoIronsightEyeDistance
- g_stereoIronsightWeaponDistance
- demo_use_hmd_rotation
- Stereo:StereoParameters / Stereo:ReadStereoParameters

Interpretation:
This is no longer merely historical CryEngine documentation. The shipping KCD2 WHGame.dll itself retains stereo-related CryEngine framework code/data. The next runtime work should therefore prioritize locating the registration/xrefs and state objects behind these retained CVars/framework paths before reconstructing native stereo at raw D3D12 level.

V28 is intentionally a non-destructive lineage/static-analysis package. Do not use the old V27 parent-isolation experiment as the next required test.
