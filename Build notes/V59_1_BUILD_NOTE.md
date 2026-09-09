# V59.1 build note

V59.0 showed that direct cyclopean presentation with reduced/centered OpenXR FOV asymmetry could look convincing while stationary, but head rotation caused severe world swim/warp.

V59.1 adds a shader-side rotation-only reprojection path. It computes `q_delta = inverse(renderPose) * currentPose` from the immutable XR frame ring and maps each current centered-XR ray back into the camera orientation that produced the captured KCD2 PRESENT frame before sampling the source texture. An inverse-sign diagnostic is included because quaternion/order conventions remain empirical in this black-box bridge.

The root constants were expanded from 16 to 24 floats to pass the relative quaternion and reprojection controls. AFR/native stereo behavior is not enabled by this F8 ladder.

The uploaded V43 source was checked for comparison. V43's bridge used only linear UV scale/offset with black outside the source; it did not contain pose-aware UV reprojection. V59.1 therefore does not copy an old V43 projection mechanism; it tests a new render-pose-aware correction while retaining the current comfort-quad architecture.
