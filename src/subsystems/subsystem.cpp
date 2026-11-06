#include "subsystem.h"
#include <cmath>

namespace kcd2vr::subsystems {
namespace {
bool Text(const char* p, std::size_t limit, bool identifier) noexcept {
    if (!p || !*p) return false;
    for (std::size_t n = 0; n < limit; ++n) {
        const unsigned char c = static_cast<unsigned char>(p[n]);
        if (!c) return true;
        if (identifier && !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '-')) return false;
        if (c < 32) return false;
    }
    return false;
}
bool Same(const char* a, const char* b) noexcept {
    if (!a || !b) return a == b;
    for (std::size_t n = 0; n < 128; ++n) {
        auto x = static_cast<unsigned char>(a[n]), y = static_cast<unsigned char>(b[n]);
        if (x >= 'A' && x <= 'Z') x += 'a' - 'A';
        if (y >= 'A' && y <= 'Z') y += 'a' - 'A';
        if (x != y) return false;
        if (!x) return true;
    }
    return false;
}
bool ValidValue(const Value& v) noexcept {
    switch (v.kind) {
    case ValueKind::Boolean: return (v.integer == 0 || v.integer == 1) && v.real == 0;
    case ValueKind::Integer: return v.real == 0;
    case ValueKind::Real: return v.integer == 0 && std::isfinite(v.real);
    default: return false;
    }
}
bool InRange(const Setting& s, const Value& v) noexcept {
    if (!ValidValue(v) || v.kind != s.initial.kind) return false;
    return v.kind == ValueKind::Real ? v.real >= s.minimum.real && v.real <= s.maximum.real :
        v.integer >= s.minimum.integer && v.integer <= s.maximum.integer;
}
Status Shape(const ModuleDescriptor& m) noexcept {
    if (m.protocol != kProtocolVersion || m.size < sizeof(ModuleDescriptor)) return {Error::ProtocolMismatch, m.id};
    if (!m.id || !m.module_version || !Text(m.name, 64, false) ||
        m.dependency_count > kMaxDependencies || m.setting_count > kMaxSettings || m.menu_count > kMaxMenuRows ||
        (m.dependency_count && !m.dependencies) || (m.setting_count && !m.settings) || (m.menu_count && !m.menu))
        return {Error::InvalidDescriptor, m.id};
    if (m.lifecycle == Lifecycle::HostOwned) {
        if (m.initialize || m.on_context || m.stop) return {Error::InvalidDescriptor, m.id, 0, "host-owned callbacks"};
    } else if (m.lifecycle != Lifecycle::Managed || !m.initialize || !m.stop)
        return {Error::InvalidDescriptor, m.id, 0, "managed lifecycle"};
    for (std::size_t i = 0; i < m.dependency_count; ++i) {
        if (!m.dependencies[i] || m.dependencies[i] == m.id) return {Error::DependencyCycle, m.id, m.dependencies[i]};
        for (std::size_t j = 0; j < i; ++j) if (m.dependencies[i] == m.dependencies[j])
            return {Error::InvalidDescriptor, m.id, m.dependencies[i], "duplicate dependency"};
    }
    for (std::size_t i = 0; i < m.setting_count; ++i) {
        const auto& s = m.settings[i];
        if (!Text(s.key, 64, true) || !ValidValue(s.initial) || !ValidValue(s.minimum) || !ValidValue(s.maximum) ||
            s.initial.kind != s.minimum.kind || s.initial.kind != s.maximum.kind || !InRange(s, s.initial))
            return {Error::InvalidValue, m.id, 0, s.key};
        for (std::size_t j = 0; j < i; ++j) if (Same(s.key, m.settings[j].key)) return {Error::SettingConflict, m.id, m.id, s.key};
    }
    for (std::size_t i = 0; i < m.menu_count; ++i) {
        const auto& row = m.menu[i];
        if (!Text(row.id, 64, true) || !Text(row.label, 128, false)) return {Error::InvalidDescriptor, m.id, 0, row.id};
        if (row.control == ControlKind::Action) {
            if (row.setting_key && *row.setting_key) return {Error::InvalidMenuBinding, m.id, 0, row.id};
        } else {
            if (!Text(row.setting_key, 64, true)) return {Error::InvalidMenuBinding, m.id, 0, row.id};
            const Setting* setting = nullptr;
            for (std::size_t j = 0; j < m.setting_count; ++j) if (Same(row.setting_key, m.settings[j].key)) setting = &m.settings[j];
            if (!setting || (row.control == ControlKind::Toggle && setting->initial.kind != ValueKind::Boolean) ||
                (row.control == ControlKind::IntegerSlider && setting->initial.kind != ValueKind::Integer) ||
                (row.control == ControlKind::RealSlider && setting->initial.kind != ValueKind::Real) ||
                (row.control != ControlKind::Toggle && row.control != ControlKind::IntegerSlider &&
                 row.control != ControlKind::RealSlider && row.control != ControlKind::ReadOnly))
                return {Error::InvalidMenuBinding, m.id, 0, row.id};
        }
        for (std::size_t j = 0; j < i; ++j) if (Same(row.id, m.menu[j].id) ||
            (row.page == m.menu[j].page && row.order == m.menu[j].order)) return {Error::MenuConflict, m.id, m.id, row.id};
    }
    return {};
}
struct Enter {
    bool& entered;
    explicit Enter(bool& v) noexcept : entered(v) { entered = true; }
    ~Enter() { entered = false; }
};
}
const char* ErrorName(Error e) noexcept {
    switch (e) {
#define SUBSYSTEM_ERROR(x) case Error::x: return #x;
    SUBSYSTEM_ERROR(Ok) SUBSYSTEM_ERROR(InvalidDescriptor) SUBSYSTEM_ERROR(ProtocolMismatch)
    SUBSYSTEM_ERROR(Capacity) SUBSYSTEM_ERROR(DuplicateModule) SUBSYSTEM_ERROR(MissingDependency)
    SUBSYSTEM_ERROR(DependencyCycle) SUBSYSTEM_ERROR(OwnershipConflict) SUBSYSTEM_ERROR(SettingConflict)
    SUBSYSTEM_ERROR(MenuConflict) SUBSYSTEM_ERROR(InvalidMenuBinding) SUBSYSTEM_ERROR(InvalidValue)
    SUBSYSTEM_ERROR(UnknownModule) SUBSYSTEM_ERROR(UnknownSetting) SUBSYSTEM_ERROR(HostServiceUnavailable)
    SUBSYSTEM_ERROR(NotStarted) SUBSYSTEM_ERROR(AlreadyFrozen) SUBSYSTEM_ERROR(Reentrant) SUBSYSTEM_ERROR(CallbackFailed)
#undef SUBSYSTEM_ERROR
    default: return "UnknownError";
    }
}
const ModuleDescriptor* Registry::Find(ModuleId id) const noexcept {
    for (std::size_t i = 0; i < count_; ++i) if (modules_[i].id == id) return &modules_[i];
    return nullptr;
}
const Setting* Registry::FindSetting(ModuleId id, const char* key) const noexcept {
    const auto* m = Find(id);
    if (m && Text(key, 64, true)) for (std::size_t i = 0; i < m->setting_count; ++i) if (Same(key, m->settings[i].key)) return &m->settings[i];
    return nullptr;
}
Status Registry::Add(const ModuleDescriptor& m) noexcept {
    if (entered_) return {Error::Reentrant, m.id};
    if (frozen_) return {Error::AlreadyFrozen, m.id};
    if (const auto status = Shape(m); !status) return status;
    if (Find(m.id)) return {Error::DuplicateModule, m.id};
    if (count_ == modules_.size()) return {Error::Capacity, m.id};
    modules_[count_++] = m;
    return {};
}
Status Registry::ValidateOrder(std::array<std::size_t, kMaxModules>& order) const noexcept {
    std::array<std::uint32_t, kMaxModules> degree{};
    std::array<bool, kMaxModules> emitted{};
    for (std::size_t i = 0; i < count_; ++i) {
        const auto& a = modules_[i];
        if (const auto status = Shape(a); !status) return status;
        degree[i] = a.dependency_count;
        for (std::size_t d = 0; d < a.dependency_count; ++d) if (!Find(a.dependencies[d])) return {Error::MissingDependency, a.id, a.dependencies[d]};
        for (std::size_t j = 0; j < i; ++j) {
            const auto& b = modules_[j];
            if (a.exclusive_outputs & b.exclusive_outputs) return {Error::OwnershipConflict, a.id, b.id};
            for (std::size_t x = 0; x < a.setting_count; ++x) for (std::size_t y = 0; y < b.setting_count; ++y)
                if (Same(a.settings[x].key, b.settings[y].key)) return {Error::SettingConflict, a.id, b.id, a.settings[x].key};
            for (std::size_t x = 0; x < a.menu_count; ++x) for (std::size_t y = 0; y < b.menu_count; ++y)
                if (Same(a.menu[x].id, b.menu[y].id) || (a.menu[x].page == b.menu[y].page && a.menu[x].order == b.menu[y].order))
                    return {Error::MenuConflict, a.id, b.id, a.menu[x].id};
        }
    }
    for (std::size_t n = 0; n < count_; ++n) {
        std::size_t i = 0;
        while (i < count_ && (emitted[i] || degree[i])) ++i;
        if (i == count_) return {Error::DependencyCycle};
        order[n] = i; emitted[i] = true;
        for (std::size_t j = 0; j < count_; ++j) if (!emitted[j]) for (std::size_t d = 0; d < modules_[j].dependency_count; ++d)
            if (modules_[j].dependencies[d] == modules_[i].id) --degree[j];
    }
    return {};
}
Status Registry::Validate() const noexcept {
    if (entered_) return {Error::Reentrant};
    std::array<std::size_t, kMaxModules> order{};
    return ValidateOrder(order);
}
Status Registry::Start(const HostServices& host) noexcept {
    if (entered_) return {Error::Reentrant};
    if (frozen_) return {Error::AlreadyFrozen};
    if (host.protocol != kProtocolVersion || host.size < sizeof(HostServices)) return {Error::ProtocolMismatch};
    if (const auto status = ValidateOrder(order_); !status) return status;
    Enter guard(entered_); frozen_ = true; host_ = host;
    for (std::size_t n = 0; n < count_; ++n) {
        const auto i = order_[n]; const auto& m = modules_[i];
        if (m.lifecycle != Lifecycle::Managed) continue;
        started_[i] = true; // failed partial initialize must also be cleaned up.
        if (!m.initialize(&host_)) { StopManaged(StopReason::InitializeFailed); return {Error::CallbackFailed, m.id}; }
        needs_context_ = needs_context_ || m.on_context != nullptr;
    }
    running_ = true;
    return {};
}
void Registry::StopManaged(StopReason reason) noexcept {
    for (std::size_t n = count_; n > 0; --n) {
        const auto i = order_[n - 1];
        if (started_[i]) { started_[i] = false; modules_[i].stop(&host_, reason); }
    }
    running_ = false; needs_context_ = false;
}
Status Registry::Dispatch(const Context& context) noexcept {
    if (entered_) return {Error::Reentrant};
    if (!running_) return {Error::NotStarted};
    if (context.protocol != kProtocolVersion || context.size < sizeof(Context)) return {Error::ProtocolMismatch};
    if (!needs_context_) return {};
    Enter guard(entered_);
    for (std::size_t n = 0; n < count_; ++n) {
        const auto& m = modules_[order_[n]];
        if (m.on_context && !m.on_context(&host_, &context)) { StopManaged(StopReason::ContextFailed); return {Error::CallbackFailed, m.id}; }
    }
    return {};
}
Status Registry::Stop() noexcept {
    if (entered_) return {Error::Reentrant};
    Enter guard(entered_); frozen_ = true;
    if (running_) StopManaged(StopReason::HostShutdown);
    return {};
}
const ModuleDescriptor* Registry::ModuleAt(std::size_t i) const noexcept { return i < count_ ? &modules_[i] : nullptr; }
std::size_t Registry::MenuCount() const noexcept {
    std::size_t result = 0; for (std::size_t i = 0; i < count_; ++i) result += modules_[i].menu_count; return result;
}
const MenuRow* Registry::MenuAt(std::size_t index, ModuleId* owner) const noexcept {
    if (owner) *owner = 0;
    for (std::size_t i = 0; i < count_; ++i) {
        if (index < modules_[i].menu_count) { if (owner) *owner = modules_[i].id; return &modules_[i].menu[index]; }
        index -= modules_[i].menu_count;
    }
    return nullptr;
}
Status Registry::ReadSetting(ModuleId module, const char* key, Value& out) noexcept {
    if (entered_) return {Error::Reentrant, module};
    if (!running_) return {Error::NotStarted, module};
    if (!Find(module)) return {Error::UnknownModule, module};
    const auto* s = FindSetting(module, key);
    if (!s) return {Error::UnknownSetting, module};
    Enter guard(entered_); Value value = s->initial, candidate = s->initial;
    if (host_.read_setting && host_.read_setting(host_.user, module, s->key, &candidate)) {
        if (!InRange(*s, candidate)) return {Error::InvalidValue, module, 0, s->key};
        value = candidate;
    }
    out = value; return {};
}
Status Registry::WriteSetting(ModuleId module, const char* key, const Value& value) noexcept {
    if (entered_) return {Error::Reentrant, module};
    if (!running_) return {Error::NotStarted, module};
    if (!Find(module)) return {Error::UnknownModule, module};
    const auto* s = FindSetting(module, key);
    if (!s) return {Error::UnknownSetting, module};
    if (!InRange(*s, value)) return {Error::InvalidValue, module, 0, s->key};
    if (!host_.write_setting) return {Error::HostServiceUnavailable, module};
    Enter guard(entered_);
    return host_.write_setting(host_.user, module, s->key, &value) ? Status{} : Status{Error::HostServiceUnavailable, module};
}
} // namespace kcd2vr::subsystems
