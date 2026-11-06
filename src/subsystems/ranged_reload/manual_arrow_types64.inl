// Native ammo identity is separate from equipment weapons and workstation tools.
struct ManualArrowLease64 {
    void* player=nullptr;void* model=nullptr;void* entity=nullptr;
    std::uint32_t id=0;
    std::uint64_t serial=0,key=0,ammo=0,generation=0,epoch=0,request=0,tick=0,sample=0;
};
struct ManualArrowSocket64 {
    ManualArrowLease64 owner{};void* self=nullptr;void* attachment=nullptr;void* character=nullptr;
    int joint=-1;QuatT28 relative{},nockHelper{};bool helper=false;
};
struct ManualArrowPose64 {
    ManualArrowSocket64 socket{};QuatT28 model{};V3 wrist{},bowNock{};
    bool valid=false,bowValid=false;
};
struct ManualArrowAchieved64 {
    ManualArrowLease64 owner{};std::uint64_t sample=0,tick=0;
    V3 arrowNock{},bowNock{};double gap=100.;bool valid=false;
};
static kcd2vr::core::SnapshotMailbox<ManualArrowLease64> g_manual_arrow_lease64;
static kcd2vr::core::SnapshotMailbox<ManualArrowSocket64> g_manual_arrow_socket64;
static kcd2vr::core::SnapshotMailbox<ManualArrowAchieved64> g_manual_arrow_achieved64;
static std::atomic<std::uint64_t> g_manual_arrow_sequence64{0},g_manual_arrow_active64{0};
static std::recursive_mutex g_manual_arrow_mutex64;
struct ManualBow596;struct PhysFrame6698;
static std::uint64_t ManualArrowBegin64() noexcept;
static void ManualArrowWithdraw64() noexcept;
static void ManualArrowPublish64(const ManualBow596&,const PhysFrame6698&,std::uint64_t,std::uint64_t) noexcept;
static void ManualArrowCue64(const PhysFrame6698&) noexcept;
static double ManualArrowNockGap64(const ManualBow596&,const PhysFrame6698&) noexcept;
static bool ManualArrowAttachment64(void*,void*) noexcept;
