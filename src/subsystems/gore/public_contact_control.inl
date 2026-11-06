void SetDecapitation59c7e(bool on,float minEnergy) noexcept {nhp134::geom27::decap59c7e::g_enabled.store(on);nhp134::geom27::decap59c7e::g_min_energy.store(std::clamp(minEnergy,.05f,1.f));}
