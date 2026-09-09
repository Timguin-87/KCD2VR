# V56.2 implementation notes

- Retunes source capture FOV default from 130 to 100 degrees based on runtime feedback.
- Narrows FOV slider to 80–115 degrees and changes coarse adjustment to 1 degree per step.
- Adds conservative automatic flat-UI state based on `FrameSync::LatestPublishedFovRadians()`.
- AUTO enters flat mode after 120 ms of an extreme published FOV (<0.55 rad or >2.40 rad), and exits after 450 ms back in the normal 0.85–2.10 rad region.
- F6 cycles AUTO / FORCE ON / FORCE OFF.
- No unproven WHGame menu pointers or animation-memory writes are introduced.
- F6 UI scale and UI eye-separation/convergence controls are preserved.
