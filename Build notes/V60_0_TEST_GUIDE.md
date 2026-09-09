# V60.0 Test Guide

Build ID: `TANGENT-ZOOM-CYCLO-MENU-20260905-D`

Recommended starting point:

```ini
r_Width = 1920
r_Height = 1800
```

In the VR menu:
1. Set `sys_MaxFPS` to 90 and APPLY.
2. Apply the desired KCD2 FOV. V59.8 FOV/source lock is retained.
3. Switch live between:
   - **MONO** = common centered OpenXR FOV, asymmetry 0.00
   - **PSEUDO 1 (.75)** = V59.8 phase 4
   - **PSEUDO 2 (1.0)** = V59.8 phase 5 / previous normal OpenXR eye-FOV asymmetry
4. Test tangent-space zoom in small steps: 0.95, 0.94, 0.93, 0.92, 0.91, then lower only if still geometrically comfortable. Slider range is 0.85-1.20.

Important:
- The zoom control scales the tangent-projected source sample uniformly. It does not apply headset lens distortion.
- If the requested zoom-out needs angular content outside the rendered KCD2 source frustum, no remap can recover it. Expect black/unavailable edges or cropping depending on presentation path.
- OpenXR/VirtualDesktopXR remains responsible for physical lens distortion correction.
- The three menu modes are mono/pseudo-stereo presentation modes from one KCD2 scene image. They are not true geometric stereo.
