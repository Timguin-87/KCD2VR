#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace kcd2vr::subsystems {
// A statically linked protocol, not a promise of cross-compiler/DLL ABI. Native
// pointers, engine interfaces and native write permission never enter it.
inline constexpr std::uint32_t kProtocolVersion = 1;
inline constexpr std::size_t kMaxModules = 32, kMaxDependencies = 16;
inline constexpr std::size_t kMaxSettings = 64, kMaxMenuRows = 64;
using ModuleId = std::uint32_t;

enum class ValueKind : std::uint32_t { Boolean, Integer, Real };
struct Value {
    ValueKind kind = ValueKind::Boolean;
    std::int64_t integer = 0;
    double real = 0;
    static constexpr Value Boolean(bool v) noexcept { return {ValueKind::Boolean, v ? 1 : 0, 0}; }
    static constexpr Value Integer(std::int64_t v) noexcept { return {ValueKind::Integer, v, 0}; }
    static constexpr Value Real(double v) noexcept { return {ValueKind::Real, 0, v}; }
};
struct Setting {
    const char* key = nullptr;
    Value initial{}, minimum{}, maximum{};
};
enum class ControlKind : std::uint32_t { Toggle, IntegerSlider, RealSlider, Action, ReadOnly };
struct MenuRow {
    const char* id = nullptr;
    const char* label = nullptr;
    const char* setting_key = nullptr; // Action rows have no setting binding.
    std::uint32_t page = 0, order = 0;
    ControlKind control = ControlKind::Toggle;
};
// Copied observation only. Zero generations/sample mean unknown. This is NOT a
// native scheduler callback or an entitlement to access an actor/physics/VM.
enum ContextFlag : std::uint32_t { TrackingValid = 1, GameplayAllowed = 2, MenuVisible = 4, Loading = 8 };
struct Context {
    std::uint32_t size = sizeof(Context), protocol = kProtocolVersion;
    std::uint64_t now_ms = 0, world_generation = 0, tracking_generation = 0, sample = 0;
    std::uint32_t flags = 0;
};
enum class LogLevel : std::uint32_t { Info, Warning, Error };
struct HostServices {
    std::uint32_t size = sizeof(HostServices), protocol = kProtocolVersion;
    std::uintptr_t user = 0; // Opaque HOST callback cookie; never a game pointer.
    void (*log)(std::uintptr_t, ModuleId, LogLevel, const char*) noexcept = nullptr;
    bool (*read_setting)(std::uintptr_t, ModuleId, const char*, Value*) noexcept = nullptr;
    bool (*write_setting)(std::uintptr_t, ModuleId, const char*, const Value*) noexcept = nullptr;
};
enum class StopReason : std::uint32_t { HostShutdown, InitializeFailed, ContextFailed };
enum class Lifecycle : std::uint32_t {
    HostOwned, // Catalog/menu/config only; existing native behavior remains host owned.
    Managed   // Actual initialize/stop callbacks; context callback is optional.
};
struct ModuleDescriptor {
    std::uint32_t size = sizeof(ModuleDescriptor), protocol = kProtocolVersion;
    ModuleId id = 0;
    std::uint32_t module_version = 1;
    const char* name = nullptr;
    const ModuleId* dependencies = nullptr;
    std::uint32_t dependency_count = 0;
    std::uint64_t exclusive_outputs = 0; // Host-defined manifest domains, not native authority.
    const Setting* settings = nullptr;
    std::uint32_t setting_count = 0;
    const MenuRow* menu = nullptr;
    std::uint32_t menu_count = 0;
    Lifecycle lifecycle = Lifecycle::HostOwned;
    bool (*initialize)(const HostServices*) noexcept = nullptr;
    bool (*on_context)(const HostServices*, const Context*) noexcept = nullptr;
    void (*stop)(const HostServices*, StopReason) noexcept = nullptr;
};
enum class Error : std::uint32_t {
    Ok, InvalidDescriptor, ProtocolMismatch, Capacity, DuplicateModule,
    MissingDependency, DependencyCycle, OwnershipConflict, SettingConflict,
    MenuConflict, InvalidMenuBinding, InvalidValue, UnknownModule, UnknownSetting,
    HostServiceUnavailable, NotStarted, AlreadyFrozen, Reentrant, CallbackFailed
};
struct Status {
    Error error = Error::Ok;
    ModuleId module = 0, other = 0;
    const char* detail = nullptr; // Descriptor-owned/static text, never temporary.
    explicit constexpr operator bool() const noexcept { return error == Error::Ok; }
};
const char* ErrorName(Error) noexcept;

// Host-serialized CONTROL lifetime only. Never call from concurrent game hooks.
// Descriptors/arrays/strings and callback code must remain immutable and alive
// for process lifetime. Registry copies descriptors, not their pointed-to data.
// Register before Start. No late registration, restart, DLL load/unload or tick
// worker. Managed startup is dependency ordered; teardown is reverse ordered.
class Registry {
public:
    Registry() noexcept = default;
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;
    Registry(Registry&&) = delete;
    Registry& operator=(Registry&&) = delete;
    Status Add(const ModuleDescriptor&) noexcept;
    Status Validate() const noexcept;
    Status Start(const HostServices&) noexcept;
    Status Dispatch(const Context&) noexcept;
    Status Stop() noexcept;
    bool NeedsContext() const noexcept { return running_ && needs_context_; }
    bool Running() const noexcept { return running_; }
    std::size_t ModuleCount() const noexcept { return count_; }
    const ModuleDescriptor* ModuleAt(std::size_t index) const noexcept;
    std::size_t MenuCount() const noexcept;
    // Enumeration follows registration order; page/order are the layout keys.
    const MenuRow* MenuAt(std::size_t index, ModuleId* owner = nullptr) const noexcept;
    Status ReadSetting(ModuleId, const char* key, Value&) noexcept;
    Status WriteSetting(ModuleId, const char* key, const Value&) noexcept;

private:
    Status ValidateOrder(std::array<std::size_t, kMaxModules>&) const noexcept;
    void StopManaged(StopReason) noexcept;
    const ModuleDescriptor* Find(ModuleId) const noexcept;
    const Setting* FindSetting(ModuleId, const char*) const noexcept;
    std::array<ModuleDescriptor, kMaxModules> modules_{};
    std::array<std::size_t, kMaxModules> order_{};
    std::array<bool, kMaxModules> started_{};
    HostServices host_{};
    std::size_t count_ = 0;
    bool frozen_ = false, running_ = false, needs_context_ = false, entered_ = false;
};
} // namespace kcd2vr::subsystems
