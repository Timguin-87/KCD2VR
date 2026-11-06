// C25: coalescing background INI writer (src/core/async_save.h).
// Host-only: models the request/worker protocol, not WritePrivateProfileStringA.
#include "../src/core/async_save.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

using kcd2vr::core::AsyncSaveCoalescer;
static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); ++failures; } } while (0)

static auto detachSpawn = [](auto worker) { std::thread(std::move(worker)).detach(); };

int main() {
    using namespace std::chrono_literals;
    { // 1. one request -> one pass, worker exits, idle afterwards
        AsyncSaveCoalescer c; std::atomic<int> saves{0};
        auto save = [&](std::uint64_t) { saves.fetch_add(1); return true; };
        CHECK(c.Request(detachSpawn, save) == AsyncSaveCoalescer::Requested::Started);
        CHECK(c.WaitIdle(2000ms));
        CHECK(saves.load() == 1); CHECK(c.CompletedGeneration() == 1); CHECK(!c.Running());
    }
    { // 2. burst while a slow write is in flight -> coalesced, final pass sees the last request
        AsyncSaveCoalescer c; std::atomic<int> saves{0}, inFlight{0}, maxInFlight{0};
        std::atomic<bool> release{false}; std::atomic<std::uint64_t> lastGen{0};
        auto save = [&](std::uint64_t gen) {
            const int now = inFlight.fetch_add(1) + 1; int m = maxInFlight.load();
            while (now > m && !maxInFlight.compare_exchange_weak(m, now)) {}
            while (!release.load()) std::this_thread::sleep_for(1ms);
            lastGen.store(gen); saves.fetch_add(1); inFlight.fetch_sub(1); return true; };
        int started = 0, coalesced = 0;
        for (int i = 0; i < 50; ++i) {
            const auto r = c.Request(detachSpawn, save);
            started += r == AsyncSaveCoalescer::Requested::Started; coalesced += r == AsyncSaveCoalescer::Requested::Coalesced;
        }
        CHECK(started == 1); CHECK(coalesced == 49);
        release.store(true);
        CHECK(c.WaitIdle(3000ms));
        CHECK(saves.load() >= 1 && saves.load() <= 2); // first pass (+ at most one coalesced pass)
        CHECK(lastGen.load() == 50); CHECK(c.CompletedGeneration() == 50);
        CHECK(maxInFlight.load() == 1);
    }
    { // 3. many producers hammering: never two writers, last request always persisted
        AsyncSaveCoalescer c; std::atomic<int> inFlight{0}, maxInFlight{0}, saves{0};
        auto save = [&](std::uint64_t) {
            const int now = inFlight.fetch_add(1) + 1; int m = maxInFlight.load();
            while (now > m && !maxInFlight.compare_exchange_weak(m, now)) {}
            std::this_thread::sleep_for(std::chrono::microseconds(200));
            inFlight.fetch_sub(1); saves.fetch_add(1); return true; };
        std::vector<std::thread> producers;
        for (int t = 0; t < 4; ++t) producers.emplace_back([&] { for (int i = 0; i < 500; ++i) { (void)c.Request(detachSpawn, save); if (i % 7 == 0) std::this_thread::yield(); } });
        for (auto& p : producers) p.join();
        CHECK(c.WaitIdle(3000ms));
        CHECK(maxInFlight.load() == 1);
        CHECK(c.CompletedGeneration() == 2000); CHECK(c.RequestedCount() == 2000);
        CHECK(saves.load() >= 1 && saves.load() < 2000);
    }
    { // 4. spawn failure -> inline write, lock released
        AsyncSaveCoalescer c; int saves = 0;
        auto save = [&](std::uint64_t) { ++saves; return false; };
        auto throwing = [](auto) { throw 1; };
        CHECK(c.Request(throwing, save) == AsyncSaveCoalescer::Requested::Inline);
        CHECK(saves == 1); CHECK(!c.Running()); CHECK(c.Failures() == 1);
    }
    { // 5. closed -> inline, no thread; WaitIdle bounded when a writer is stuck
        AsyncSaveCoalescer c; int saves = 0; bool spawned = false;
        c.Close();
        CHECK(c.Request([&](auto) { spawned = true; }, [&](std::uint64_t) { ++saves; return true; }) == AsyncSaveCoalescer::Requested::Inline);
        CHECK(!spawned && saves == 1);
        AsyncSaveCoalescer stuck; std::atomic<bool> go{false};
        (void)stuck.Request(detachSpawn, [&](std::uint64_t) { while (!go.load()) std::this_thread::sleep_for(1ms); return true; });
        const auto t0 = std::chrono::steady_clock::now();
        CHECK(!stuck.WaitIdle(50ms));
        CHECK(std::chrono::steady_clock::now() - t0 < 1000ms);
        go.store(true); CHECK(stuck.WaitIdle(2000ms));
    }
    std::printf(failures ? "async_save25: %d FAILURES\n" : "async_save25: all passed\n", failures);
    return failures ? 1 : 0;
}
