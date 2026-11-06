#include "../src/subsystems/subsystem.h"
#include <cassert>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>
#include <cstdio>

static unsigned checks = 0;
#define CHECK(condition) do { ++checks; assert(condition); } while (false)
namespace ss = kcd2vr::subsystems;
static_assert(std::is_trivially_copyable<ss::Context>::value, "copied observation");
static_assert(std::is_trivially_copyable<ss::HostServices>::value, "static callback table");
static_assert(std::is_trivially_copyable<ss::ModuleDescriptor>::value, "static descriptor");
static_assert(!std::is_copy_constructible<ss::Registry>::value, "no duplicate lifecycle owner");
namespace {
struct Fixture {
    std::vector<std::string> events;
    ss::Registry* registry = nullptr;
    ss::ModuleId fail_init = 0, fail_context = 0;
    bool found = false, write_ok = true, mutate_absent = false;
    ss::Value value = ss::Value::Boolean(false);
    unsigned reads = 0, writes = 0;
    ss::StopReason last_stop = ss::StopReason::HostShutdown;
};
Fixture& Data(const ss::HostServices* h) noexcept { return *reinterpret_cast<Fixture*>(h->user); }
bool Read(std::uintptr_t token, ss::ModuleId id, const char* key, ss::Value* out) noexcept {
    auto& f = *reinterpret_cast<Fixture*>(token); ++f.reads;
    CHECK(id == 1 && !std::strcmp(key, "VrOwned"));
    if (f.found || f.mutate_absent) *out = f.value;
    return f.found;
}
bool Write(std::uintptr_t token, ss::ModuleId id, const char* key, const ss::Value* value) noexcept {
    auto& f = *reinterpret_cast<Fixture*>(token); ++f.writes;
    CHECK(id == 1 && !std::strcmp(key, "VrOwned")); f.value = *value;
    return f.write_ok;
}
ss::HostServices Host(Fixture& f) {
    ss::HostServices h{}; h.user = reinterpret_cast<std::uintptr_t>(&f); h.read_setting = Read; h.write_setting = Write; return h;
}
template<ss::ModuleId id> bool Init(const ss::HostServices* h) noexcept {
    auto& f = Data(h); f.events.push_back("I" + std::to_string(id));
    if (f.registry) CHECK(f.registry->Stop().error == ss::Error::Reentrant);
    return f.fail_init != id;
}
template<ss::ModuleId id> bool Context(const ss::HostServices* h, const ss::Context* c) noexcept {
    auto& f = Data(h); CHECK(c->now_ms == 100 && c->sample == 7); f.events.push_back("C" + std::to_string(id)); return f.fail_context != id;
}
template<ss::ModuleId id> void Stop(const ss::HostServices* h, ss::StopReason reason) noexcept {
    auto& f = Data(h); f.events.push_back("S" + std::to_string(id)); f.last_stop = reason;
    if (f.registry) CHECK(f.registry->Stop().error == ss::Error::Reentrant);
}
ss::ModuleDescriptor Catalog(ss::ModuleId id) {
    ss::ModuleDescriptor m{}; m.id = id; m.name = "host owned"; return m;
}
template<ss::ModuleId id> ss::ModuleDescriptor Managed(bool context = true) {
    auto m = Catalog(id); m.name = "managed adapter"; m.lifecycle = ss::Lifecycle::Managed;
    m.initialize = Init<id>; m.stop = Stop<id>; if (context) m.on_context = Context<id>; return m;
}
const ss::Setting boolean_setting[]{ {"VrOwned", ss::Value::Boolean(true), ss::Value::Boolean(false), ss::Value::Boolean(true)} };
const ss::MenuRow boolean_menu[]{ {"owned.enable", "OWNED", "VrOwned", 2, 1, ss::ControlKind::Toggle} };
}
int main() {
    // Catalog-only manifests never pretend to have initialized native behavior.
    {
        ss::Registry r; auto m = Catalog(1); m.settings = boolean_setting; m.setting_count = 1; m.menu = boolean_menu; m.menu_count = 1;
        CHECK(r.Add(m) && r.Validate()); CHECK(r.ModuleCount() == 1 && r.MenuCount() == 1);
        ss::ModuleId owner = 0; CHECK(r.MenuAt(0, &owner) == boolean_menu && owner == 1);
        CHECK(!r.MenuAt(1, &owner) && owner == 0 && !r.ModuleAt(1));
        ss::Value v = ss::Value::Integer(99); CHECK(r.ReadSetting(1, "VrOwned", v).error == ss::Error::NotStarted);
        CHECK(r.Start({}) && !r.NeedsContext() && r.Running());
        CHECK(r.ReadSetting(1, "vrowned", v) && v.kind == ss::ValueKind::Boolean && v.integer == 1);
        CHECK(r.WriteSetting(1, "VrOwned", ss::Value::Boolean(false)).error == ss::Error::HostServiceUnavailable);
        ss::Context c{}; CHECK(r.Dispatch(c)); CHECK(r.Add(Catalog(2)).error == ss::Error::AlreadyFrozen);
        CHECK(r.Stop() && r.Stop() && !r.NeedsContext() && !r.Running()); CHECK(r.Start({}).error == ss::Error::AlreadyFrozen);
    }
    // Dependency order and reverse teardown execute the actual registered callbacks.
    {
        ss::Registry r; Fixture f; f.registry = &r; const ss::ModuleId dependency[]{1};
        auto b = Managed<2>(); b.dependencies = dependency; b.dependency_count = 1;
        CHECK(r.Add(b) && r.Add(Managed<1>()) && r.Start(Host(f)) && r.NeedsContext());
        CHECK((f.events == std::vector<std::string>{"I1", "I2"}));
        ss::Context c{}; c.now_ms = 100; c.sample = 7; CHECK(r.Dispatch(c));
        CHECK((f.events == std::vector<std::string>{"I1", "I2", "C1", "C2"}));
        CHECK(r.Stop() && r.Stop()); CHECK((f.events == std::vector<std::string>{"I1", "I2", "C1", "C2", "S2", "S1"}));
    }
    for (bool initialize_failure : {true, false}) {
        ss::Registry r; Fixture f; if (initialize_failure) f.fail_init = 2; else f.fail_context = 2;
        CHECK(r.Add(Managed<1>()) && r.Add(Managed<2>()));
        const auto started = r.Start(Host(f));
        if (initialize_failure) CHECK(started.error == ss::Error::CallbackFailed && started.module == 2);
        else { CHECK(started); ss::Context c{}; c.now_ms = 100; c.sample = 7; CHECK(r.Dispatch(c).error == ss::Error::CallbackFailed); }
        CHECK(f.events[f.events.size() - 2] == "S2" && f.events.back() == "S1");
        CHECK(f.last_stop == (initialize_failure ? ss::StopReason::InitializeFailed : ss::StopReason::ContextFailed));
        const auto n = f.events.size(); CHECK(r.Stop() && f.events.size() == n && !r.Running());
        CHECK(r.Start(Host(f)).error == ss::Error::AlreadyFrozen);
    }
    // Bad graphs and ownership collisions are rejected BEFORE any initialize.
    {
        ss::Registry r; Fixture f; const ss::ModuleId missing[]{9}; auto m = Managed<1>(); m.dependencies = missing; m.dependency_count = 1;
        CHECK(r.Add(m)); CHECK(r.Start(Host(f)).error == ss::Error::MissingDependency && f.events.empty());
    }
    {
        ss::Registry r; const ss::ModuleId a_dep[]{2}, b_dep[]{1}; auto a = Catalog(1), b = Catalog(2);
        a.dependencies = a_dep; a.dependency_count = 1; b.dependencies = b_dep; b.dependency_count = 1;
        CHECK(r.Add(a) && r.Add(b) && r.Validate().error == ss::Error::DependencyCycle);
    }
    {
        ss::Registry r; auto a = Catalog(1), b = Catalog(2); a.exclusive_outputs = b.exclusive_outputs = 4;
        CHECK(r.Add(a) && r.Add(b) && r.Start({}).error == ss::Error::OwnershipConflict);
    }
    // Case-insensitive keys match the existing Windows INI behavior.
    {
        ss::Registry r; auto a = Catalog(1), b = Catalog(2); const ss::Setting lower[]{ {"vrowned", ss::Value::Boolean(false), ss::Value::Boolean(false), ss::Value::Boolean(true)} };
        a.settings = boolean_setting; a.setting_count = 1; b.settings = lower; b.setting_count = 1;
        CHECK(r.Add(a) && r.Add(b) && r.Validate().error == ss::Error::SettingConflict);
    }
    for (bool same_id : {false, true}) {
        ss::Registry r; auto a = Catalog(1), b = Catalog(2);
        const ss::MenuRow ma[]{ {"a.go", "A", nullptr, 3, 1, ss::ControlKind::Action} };
        const ss::MenuRow mb[]{ {same_id ? "A.GO" : "b.go", "B", nullptr, same_id ? 4u : 3u, 1, ss::ControlKind::Action} };
        a.menu = ma; a.menu_count = 1; b.menu = mb; b.menu_count = 1;
        CHECK(r.Add(a) && r.Add(b) && r.Validate().error == ss::Error::MenuConflict);
    }
    {
        auto m = Catalog(1); m.menu = boolean_menu; m.menu_count = 1; ss::Registry r;
        CHECK(r.Add(m).error == ss::Error::InvalidMenuBinding); // cannot bind another module's setting
        auto n = Managed<1>(); n.stop = nullptr; CHECK(r.Add(n).error == ss::Error::InvalidDescriptor);
        n = Catalog(1); n.initialize = Init<1>; CHECK(r.Add(n).error == ss::Error::InvalidDescriptor);
        n = Catalog(1); n.protocol = 99; CHECK(r.Add(n).error == ss::Error::ProtocolMismatch);
        n = Catalog(1); n.size = 1; CHECK(r.Add(n).error == ss::Error::ProtocolMismatch);
        n = Catalog(1); n.dependencies = nullptr; n.dependency_count = 1; CHECK(r.Add(n).error == ss::Error::InvalidDescriptor);
    }
    {
        ss::Registry r; Fixture f; auto m = Catalog(1); m.settings = boolean_setting; m.setting_count = 1;
        CHECK(r.Add(m) && r.Add(Catalog(2))); CHECK(r.Add(m).error == ss::Error::DuplicateModule);
        auto h = Host(f); h.protocol = 99; CHECK(r.Start(h).error == ss::Error::ProtocolMismatch); CHECK(r.Start(Host(f)));
        ss::Value out = ss::Value::Integer(77); f.mutate_absent = true; f.value = ss::Value::Real(std::numeric_limits<double>::quiet_NaN());
        CHECK(r.ReadSetting(1, "VrOwned", out) && out.kind == ss::ValueKind::Boolean && out.integer == 1); // absent callback output is ignored
        f.found = true; CHECK(r.ReadSetting(1, "VrOwned", out).error == ss::Error::InvalidValue && out.integer == 1);
        const auto reads = f.reads; CHECK(r.ReadSetting(2, "VrOwned", out).error == ss::Error::UnknownSetting && f.reads == reads);
        CHECK(r.WriteSetting(2, "VrOwned", ss::Value::Boolean(true)).error == ss::Error::UnknownSetting && f.writes == 0);
        CHECK(r.WriteSetting(1, "VrOwned", ss::Value::Integer(1)).error == ss::Error::InvalidValue && f.writes == 0);
        CHECK(r.WriteSetting(1, "vrowned", ss::Value::Boolean(false)) && f.writes == 1 && f.value.integer == 0);
        f.write_ok = false; CHECK(r.WriteSetting(1, "VrOwned", ss::Value::Boolean(true)).error == ss::Error::HostServiceUnavailable);
        ss::Context bad{}; bad.size = 0; CHECK(r.Dispatch(bad).error == ss::Error::ProtocolMismatch);
        CHECK(r.Stop()); CHECK(r.Dispatch({}).error == ss::Error::NotStarted);
    }
    {
        ss::Registry r; const ss::Setting bad[]{ {"VrBad", ss::Value::Real(std::numeric_limits<double>::quiet_NaN()), ss::Value::Real(0), ss::Value::Real(1)} };
        auto m = Catalog(1); m.settings = bad; m.setting_count = 1; CHECK(r.Add(m).error == ss::Error::InvalidValue);
        for (std::size_t i = 0; i < ss::kMaxModules; ++i) CHECK(r.Add(Catalog(static_cast<ss::ModuleId>(i + 1))));
        CHECK(r.Add(Catalog(100)).error == ss::Error::Capacity);
    }
    std::printf("PASS %u actual subsystem contract checks: lifecycle rollback/order, no idle context polling, dependency/owner/menu/config conflicts, typed service routing, immutable freeze and bounded rejection\n", checks);
}
