# Working on KCD2VR

Make the smallest change that fixes the actual broken transition. Keep the source,
DLL, active settings and companion identity together when comparing recordings.

Native interfaces are valid only for the supported game fingerprint. Do not guess
a virtual slot, transplant another engine's ABI or remove a fingerprint check to
make a hook install.

The game owns combat outcomes and interaction legality. VR movement must not
fabricate accepted hits, damage, recipes, inventory costs or combo progression.
Keep ordinary HMD camera ownership and stereo scheduling intact. Do not hide a
bad pose with a long tracking freeze, extra hand lag or stretched bones.

Keep experiments switchable and diagnostics bounded. A new recorder should answer
a specific missing question. Existing recordings cover a lot already.

After changing source, run the relevant actual-code fixtures and the Windows
release build. Report unavailable checks separately. Preserve the useful negative
controls: the old code should fail for the intended reason, not a compiler error.

Use a new branch for repairs. Preserved local source snapshots may share hardlinks;
replace changed files atomically or make an independent checkout before editing.
Never modify a frozen build's files in place.
