# V61.0 build note

Derived from V60.9 HFOV-CULLSYNC-NORMALVIEW.

The stereo work intentionally reuses proven plumbing instead of adding another blind renderer hook: frame-locked AFR from V42/V46.5 and the Normal CRenderView selector dual-pass diagnostic from V54/V58.8. The V60.7-B matched source/submitted FOV mapping is used for AFR presentation. The recovered CryEngine stereo CVars are enabled only in DUAL phases/manual DUAL mode and restored on test completion/cancel.

KCD2Tools-derived TPV camera semantics are now represented directly in the VR menu as local X/Y/Z offsets. No TPV DLL is required.
