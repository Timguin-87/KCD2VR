# V60.6 F8 immersive FOV-only ladder

This build removes V60.5's static homography from F8. F8 now uses the normal production OpenXR tangent mapping.

During F8 only, the LOCAL/F6 flat-world quad and inventory-flat mode are forced OFF so every phase is an immersive VR world view. The exact pre-test F6/inventory-flat state is restored when the ladder completes or is cancelled.

F8 phases (8 s each): 100, 102.5, 105, 107.5, 110 degrees. All phases use zoom 1.00, UV binocular shift 0, asymmetry 0.75, and GAME_FOV == SOURCE_FOV. OpenXR headset eye FOV is unchanged.

All ordinary functions outside F8 remain present, including F6, menu, pseudo modes, controls, culling slider, dialogue settings, and the V60.5 MONO hard gate.

Report for each phase: black edges, geometry/perspective, sharpness, warp on head rotation. Also verify F6 works again after F8.
