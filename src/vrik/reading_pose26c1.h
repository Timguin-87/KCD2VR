#pragma once
#include "../ui_menu_router.h"

namespace kcd2vr::reading_pose26c1 {
// Visual permission only. Never grants input, tool, weapon or interaction authority.
inline bool allow(bool witnessed,bool visualSelected,bool theater,bool matrix,
                  bool vrMenu,bool dialogue,bool cinematic,bool nativeReference,
                  const ui_menu_router::Decision& ui) noexcept {
    const bool ownUi=ui.owner==ui_menu_router::Owner::None||
        ui.owner==ui_menu_router::Owner::NativeContext;
    return witnessed&&visualSelected&&!theater&&!matrix&&!vrMenu&&!dialogue&&
        !cinematic&&!nativeReference&&ownUi&&ui.uncertain==0;
}
}
