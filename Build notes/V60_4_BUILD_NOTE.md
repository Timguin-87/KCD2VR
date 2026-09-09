# V60.4 Build Note

This build is intentionally focused on the four reported blockers:

1. peripheral KCD2 culling already visible on the monitor,
2. unresolved FOV/remap calibration,
3. PS3 direct double vision,
4. bow/weapon control ergonomics.

The culling change is reversible per frustum call: it expands the tangent of the
camera FOV for visibility-plane construction and restores the original field
immediately after the original frustum function returns.

The FOV change removes automatic coupling between `cl_fov` and the compositor
source-FOV model. The user can now tune the actual game render FOV and the
compositor calibration independently.

PS6 is deliberately conservative: asymmetry 0.875 with no UV shift, halfway
between the two successful V59.8 cyclopean settings.
