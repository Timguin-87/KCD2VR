// Actual pre-native-skinning boundary. The original callback still runs once;
// revoked acquisition poses are restored BEFORE it can read the scoped bones.
// INVESTIGATE_D67R1: 5FE2F0 really writes the current 32-byte/joint palette;
// native_palette_d67r1 observes its current/previous buffers at this existing
// boundary. This is already late skinning, not a generic Update-stage solve.
// 3D4C49 puts the resulting skinData in renderObject+58 (character at +50);
// 4A34FC creates a separate remapped skinData cache for other meshes. Next proof
// is those exact draw/upload consumers and both eye submissions, not another
// global-latest-camera match. Retain the scope/restore while that lifetime is
// unresolved: extending bone writes into queued work needs actual job ownership.
if(committed&&!CoherentOwnerCurrent57(transaction)){
    transaction.Restore();committed=false;
    g_committed_pose_valid6689.store(false,std::memory_order_release);
}
