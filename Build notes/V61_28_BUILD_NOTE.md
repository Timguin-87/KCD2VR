# V61.28 build note

Purpose: one-launch hypothesis test replacing isolated builds.

Hotkeys: F6 corrected exact-gameplay-CView FOV + XR-derived Center-Y; F8 passive StereoL/R producer pair archaeology; F9 temporary-before-final-skinning CPoseData ownership pulse; F7 0.8x VR menu + Touch-ray pointer.

Known-good V61.12 gameplay-only cull synchronization, GOOD MONO OpenXR submission, 1920x1800 source baseline, dialogue settings and A->E interaction are retained. Legacy +0x818/+0x828 stereo probe, native-eye writes, AFR Insert/Home controls and mode-2/shadow eye hypotheses stay retired.

Source only; Windows/MSVC compilation not verified in this environment.

## ViGEmBus / virtual gamepad note

Not included in V61.28. A future optional virtual-XInput backend could map OpenXR Touch sticks/buttons into KCD2's native Xbox-controller path, which may avoid mouse-look/menu conflicts and reduce keyboard/mouse injection. It does not solve final skeleton ownership or native stereo. ViGEmBus itself is retired/EOL, so any production input backend should be optional and should evaluate a maintained virtual-gamepad alternative before adding a driver dependency.
