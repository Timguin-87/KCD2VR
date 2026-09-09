# V59.9 Test Guide

Use the known stable game setup first:
- `r_Width = 1920`
- `r_Height = 1800`
- set `sys_MaxFPS = 90` (menu Apply is fine)
- choose the FOV you want to evaluate before starting F8; F8 re-reads `cl_fov` and locks SOURCE_FOV to the same value.

Press F8 once. The complete ladder lasts about 77 seconds. Do not touch FOV, FPS, zoom, Insert/Home, or the MIS menu during the run.

For phases A-C report framing, edge distortion, sharpness, and whether lower zoom finally works after the FOV-sync fix.
For D-H report depth impression, eye strain, world swimming on head rotation, and whether the effect feels reversed.
For J/K report any real binocular parallax versus shadow/state smearing. These phases are diagnostic and may look wrong.

After the run, send the normal + debug logs and your preferred phase numbers.
