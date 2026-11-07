#define Allow(category,perSecond) \
    (::kcd2vr::diagnostics::capture168::active(::kcd2vr::diagnostics::capture168::Domain::Combat,GetTickCount64()) && \
     ::kcd2vr::receipt56::allow(([]() noexcept -> ::kcd2vr::receipt56::Site& {static ::kcd2vr::receipt56::Site site56;return site56;})(), \
        static_cast<unsigned>(perSecond),std::uint64_t(GetTickCount64())))
