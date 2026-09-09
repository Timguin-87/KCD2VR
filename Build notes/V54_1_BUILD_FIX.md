# V54.1 build fix

Fixes the V54 VRIK bridge compile error caused by referring to nonexistent `XrPose::rot_x/rot_y/rot_z/rot_w` members.
`XrPose` uses `qx/qy/qz/qw`.

The intended controller-to-KCD2 axis remap is preserved:
- position: `(x, z, y)`
- orientation components: `(qx, qz, qy, qw)`

No tracking/stereo test behavior was otherwise changed.
