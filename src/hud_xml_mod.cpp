#include "hud_xml_mod.h"
#include "debug_trace.h"

#include <windows.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace kcd2vr::hud_xml_mod {
namespace {

constexpr int kCanvasW = 1872;
constexpr int kCanvasH = 2016;
constexpr const char* kPakEntry = "Libs/UI/UIElements/HUD.xml";
constexpr const char* kModId = "KCD2VR_HUD_Runtime";

const char* BaseHudXml() noexcept {
    static constexpr const char* kXml =
#include "hud_xml_base.inc"
    ;
    return kXml;
}

std::uint32_t Crc32(const unsigned char* data, std::size_t size) noexcept {
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b)
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}

void Put16(std::ofstream& f, std::uint16_t v) {
    const unsigned char b[2]{static_cast<unsigned char>(v), static_cast<unsigned char>(v >> 8)};
    f.write(reinterpret_cast<const char*>(b), 2);
}
void Put32(std::ofstream& f, std::uint32_t v) {
    const unsigned char b[4]{static_cast<unsigned char>(v), static_cast<unsigned char>(v >> 8), static_cast<unsigned char>(v >> 16), static_cast<unsigned char>(v >> 24)};
    f.write(reinterpret_cast<const char*>(b), 4);
}

bool WriteStoredSingleFileZip(const std::string& path, const char* entry, const std::string& payload) noexcept {
    try {
        std::ofstream f(path, std::ios::binary | std::ios::trunc);
        if (!f) return false;
        const std::uint32_t crc = Crc32(reinterpret_cast<const unsigned char*>(payload.data()), payload.size());
        const std::uint32_t size = static_cast<std::uint32_t>(payload.size());
        const std::uint16_t nlen = static_cast<std::uint16_t>(std::strlen(entry));
        // local file header
        Put32(f, 0x04034B50u); Put16(f, 20); Put16(f, 0); Put16(f, 0); // STORE
        Put16(f, 0); Put16(f, 0); Put32(f, crc); Put32(f, size); Put32(f, size);
        Put16(f, nlen); Put16(f, 0); f.write(entry, nlen); f.write(payload.data(), payload.size());
        const std::uint32_t cd_off = static_cast<std::uint32_t>(static_cast<std::streamoff>(f.tellp()));
        // central directory
        Put32(f, 0x02014B50u); Put16(f, 20); Put16(f, 20); Put16(f, 0); Put16(f, 0);
        Put16(f, 0); Put16(f, 0); Put32(f, crc); Put32(f, size); Put32(f, size);
        Put16(f, nlen); Put16(f, 0); Put16(f, 0); Put16(f, 0); Put16(f, 0); Put32(f, 0); Put32(f, 0);
        f.write(entry, nlen);
        const std::uint32_t cd_end = static_cast<std::uint32_t>(static_cast<std::streamoff>(f.tellp()));
        const std::uint32_t cd_size = cd_end - cd_off;
        // EOCD
        Put32(f, 0x06054B50u); Put16(f, 0); Put16(f, 0); Put16(f, 1); Put16(f, 1);
        Put32(f, cd_size); Put32(f, cd_off); Put16(f, 0);
        return static_cast<bool>(f);
    } catch (...) { return false; }
}

bool GetGameRoot(std::string& out) noexcept {
    HMODULE self = nullptr;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCSTR>(&GetGameRoot), &self)) return false;
    char p[MAX_PATH * 2]{};
    const DWORD n = GetModuleFileNameA(self, p, static_cast<DWORD>(sizeof(p)));
    if (!n || n >= sizeof(p)) return false;
    std::string s(p, n);
    auto slash = s.find_last_of("\\/"); if (slash == std::string::npos) return false; s.resize(slash); // Win64...
    slash = s.find_last_of("\\/"); if (slash == std::string::npos) return false; s.resize(slash); // Bin
    slash = s.find_last_of("\\/"); if (slash == std::string::npos) return false; s.resize(slash); // game root
    out = s;
    return true;
}

bool EnsureDir(const std::string& p) noexcept {
    if (CreateDirectoryA(p.c_str(), nullptr)) return true;
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

std::string MakeHudXml(const Settings& in) {
    Settings s = in;
    s.scale = std::clamp(s.scale, 0.50f, 1.10f);
    s.offset_x = std::clamp(s.offset_x, -700, 700);
    s.offset_y = std::clamp(s.offset_y, -700, 700);
    const int w = static_cast<int>(kCanvasW * s.scale + 0.5f);
    const int h = static_cast<int>(kCanvasH * s.scale + 0.5f);
    // V61.22: X/Y now mean actual screen-space origin (left/top), not an
    // offset from a centered placement. Scale 1.0/X 0/Y 0 reproduces the
    // original fullscreen HUD exactly; Scale 0.60/X 0/Y 0 anchors the
    // smaller HUD at the top-left corner instead of hundreds of pixels
    // into the canvas. Previous formula was (canvas-w)/2 + offset, which
    // is why a "0,0" input still landed at top=403 left=374 for scale=0.60.
    const int left = s.offset_x;
    const int top = s.offset_y;

    std::string xml(BaseHudXml());
    const std::string old = "      <Constraints>\n        <Align mode=\"fullscreen\" />\n      </Constraints>";
    char repl[256]{};
    std::snprintf(repl, sizeof(repl),
        "      <Constraints>\n        <Align mode=\"fixed\" />\n        <Position top=\"%d\" left=\"%d\" width=\"%d\" height=\"%d\" />\n      </Constraints>",
        top, left, w, h);
    const auto pos = xml.find(old);
    if (pos == std::string::npos) return {};
    xml.replace(pos, old.size(), repl);
    return xml;
}

bool WriteTextFile(const std::string& path, const char* text) noexcept {
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f.write(text, std::strlen(text));
    return static_cast<bool>(f);
}

} // namespace

bool WriteRuntimeHudMod(const Settings& settings) noexcept {
    std::string root;
    if (!GetGameRoot(root)) { debug_trace::Write("V61.20-HUD-XML ERROR game root unresolved"); return false; }
    const std::string mods = root + "\\Mods";
    const std::string mod = mods + "\\" + kModId;
    const std::string data = mod + "\\Data";
    if (!EnsureDir(mods) || !EnsureDir(mod) || !EnsureDir(data)) {
        debug_trace::Write("V61.20-HUD-XML ERROR failed to create Mods/KCD2VR_HUD_Runtime/Data"); return false;
    }
    const std::string xml = MakeHudXml(settings);
    if (xml.empty()) { debug_trace::Write("V61.20-HUD-XML ERROR base HUD constraint patch failed"); return false; }
    const std::string pak = data + "\\KCD2VR_HUD_Runtime.pak";
    const char* manifest =
        "<?xml version=\"1.0\" encoding=\"us-ascii\"?>\r\n"
        "<kcd_mod>\r\n <info>\r\n  <name>KCD2VR HUD Runtime</name>\r\n"
        "  <modid>KCD2VR_HUD_Runtime</modid>\r\n"
        "  <description>HUD rectangle generated by KCD2VR. Restart required after changes.</description>\r\n"
        "  <author>KCD2VR</author>\r\n  <version>61.20</version>\r\n"
        "  <created_on>2026-09-06</created_on>\r\n  <modifies_level>false</modifies_level>\r\n"
        " </info>\r\n</kcd_mod>\r\n";
    const bool ok = WriteStoredSingleFileZip(pak, kPakEntry, xml) && WriteTextFile(mod + "\\mod.manifest", manifest);
    char msg[512]{};
    const int w = static_cast<int>(kCanvasW * std::clamp(settings.scale,0.50f,1.10f) + 0.5f);
    const int h = static_cast<int>(kCanvasH * std::clamp(settings.scale,0.50f,1.10f) + 0.5f);
    const int left = std::clamp(settings.offset_x,-700,700);
    const int top = std::clamp(settings.offset_y,-700,700);
    std::snprintf(msg,sizeof(msg),"V61.20-HUD-XML %s scale=%.3f x=%d y=%d -> Position top=%d left=%d width=%d height=%d path='%s' RESTART_REQUIRED",
        ok?"WRITE-OK":"WRITE-FAILED",static_cast<double>(settings.scale),settings.offset_x,settings.offset_y,top,left,w,h,pak.c_str());
    debug_trace::Write(msg);
    return ok;
}

bool ResetRuntimeHudMod() noexcept {
    std::string root;
    if (!GetGameRoot(root)) return false;
    const std::string mod = root + "\\Mods\\" + kModId;
    const std::string pak = mod + "\\Data\\KCD2VR_HUD_Runtime.pak";
    const std::string manifest = mod + "\\mod.manifest";
    const bool pak_ok = DeleteFileA(pak.c_str()) || GetLastError() == ERROR_FILE_NOT_FOUND;
    const bool man_ok = DeleteFileA(manifest.c_str()) || GetLastError() == ERROR_FILE_NOT_FOUND;
    RemoveDirectoryA((mod + "\\Data").c_str());
    RemoveDirectoryA(mod.c_str());
    debug_trace::Write((pak_ok && man_ok) ? "V61.20-HUD-XML RESET-OK generated HUD mod removed; restart required" : "V61.20-HUD-XML RESET-FAILED could not fully remove generated HUD mod");
    return pak_ok && man_ok;
}

} // namespace kcd2vr::hud_xml_mod
