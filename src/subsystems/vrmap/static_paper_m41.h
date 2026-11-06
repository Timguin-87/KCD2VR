#pragma once
#include <cstring>
namespace kcd2vr::static_paper_m41 {
inline constexpr bool integrated=true;
inline constexpr const char* prefix="objects/kcd2vr/staticmap40/";
inline constexpr const char* artwork[]{"apolena","bohounovice","bylany","certovka","grunta","horany","klaster","klaster_interior","kutna_hora","kutnohorsko","malesov","miskovice","nebakov","nomad_camp","opatovice","pritoky","ratbor","semin","stara_kutna","suchdol","tachov","trosecko","troskovice","trosky","vysoka","zelejov"};
inline bool valid(const char* token) noexcept {
 if(!token)return false;for(const auto* name:artwork)if(!std::strcmp(name,token))return true;return false;
}
}
