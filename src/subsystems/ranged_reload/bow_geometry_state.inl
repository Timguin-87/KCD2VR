namespace bowgeom59 = kcd2vr::bow_geometry59c10r2;
using BowGeometry166=bowgeom59::Geometry;
static kcd2vr::core::SnapshotMailbox<BowGeometry166> g_bow_geometry166;
static kcd2vr::core::SnapshotMailbox<bowgeom59::Placement> g_bow_placement59c10r2;
static bowgeom59::Lifetimes g_bow_lifetimes59c10r2; // exact native attachment owner, under PhysTryLock6698
using BowReleaseRejected166=ph::RejectedBowRelease166;
static kcd2vr::core::SnapshotMailbox<BowReleaseRejected166> g_bow_release_rejected166;
static bool PhysApplyBowGeometry166(const BowGeometry166& b,const PhysEquipment6698& e,ph::Profile& p,std::uint64_t now) noexcept {
    bowgeom59::Placement placed{};
    if(!p.bow()||!p.primary||!e.valid||!e.tick||now<e.tick||now-e.tick>100||
       e.heldIndex[1]<0||e.heldIndex[1]>=e.count||!e.handKnown[1]||e.handKey[1]!=p.key||
       !e.items[e.heldIndex[1]].held||e.items[e.heldIndex[1]].entity!=b.entity||
       e.items[e.heldIndex[1]].profile.key!=p.key||e.items[e.heldIndex[1]].profile.generation!=p.generation||
       !g_bow_placement59c10r2.read(placed)||!bowgeom59::usable(b,placed,now)||b.key!=p.key||b.generation!=p.generation||
       b.request!=e.request||b.epoch!=e.epoch||b.character!=g_bow_char128.load()||!b.character||
       !pm::finite(b.nock)||!pm::unit(b.axis)||!std::isfinite(b.drawLength)||
       !((b.restStructural175&&b.drawLength==0)||(b.drawLength>=.12&&b.drawLength<=.95)))return false;
    p.bowGeometryRank48=b.restStructural175?(b.partialAxis48?1u:0u):2u;p.bowNockQualified40=true;p.grip[0].p=b.nock;p.axis=b.axis;p.support=true;p.bowDrawLength166=b.drawLength;return true;
}
