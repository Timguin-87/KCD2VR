namespace decap59c7e {
inline std::atomic<bool> g_enabled{false};inline std::atomic<float> g_min_energy{.6f};
inline std::atomic<std::uint64_t> g_candidates{0},g_refused{0};
// body_material2subpart.xml race 0 (human): 38 = head_neck (subpart 28, neck). 1..5 = head_up/back/front/right/left.
constexpr int kMaterialHeadNeck=38;constexpr int kSurfaceFlesh=116;
inline const char* MaterialName(int m) noexcept {
    switch(m){case 1:return "head_up";case 2:return "head_back";case 3:return "head_front";case 4:return "head_right";case 5:return "head_left";
    case 38:return "head_neck";case -1:return "UNKNOWN";default:return m>=6&&m<=37?"body_or_limb":"OUT_OF_TABLE";}
}
// All routes observe the canonical completed receipt; no sever/physics writes.
inline void Evaluate(const Pending&,const kcd2vr::contact58c1::Receipt&,int,int) noexcept;
} // namespace decap59c7e
namespace impale40n {inline void Evaluate(const Pending&,const kcd2vr::contact58c1::Receipt&,std::uint64_t) noexcept;} // C40 (impale_armour40.inl)
