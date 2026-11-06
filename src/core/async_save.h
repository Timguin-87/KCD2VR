#pragma once
// C25: event-driven, coalescing single-writer for slow persistence (KCD2VR.ini).
//
// Request() is called on a latency-critical thread (the XR/tracking loop). It only
// bumps a counter and, when no writer owns the slot, hands ONE worker to `spawn`.
// The worker saves the latest values, then re-checks the counter; requests that
// arrived while it was writing are folded into one more pass. No timers, no polling
// while idle, at most one writer at any time (running_ is the writer lock).
// WaitIdle() is the only waiting call; it is bounded and intended for shutdown.
#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

namespace kcd2vr::core {

class AsyncSaveCoalescer {
public:
    enum class Requested { Coalesced, Started, Inline };

    // spawn(worker) must start `worker` on another thread or throw. If it throws,
    // the work runs inline on the caller (still under the single-writer lock).
    // save(generation) performs one full write and returns success.
    template<class Spawn, class Save>
    Requested Request(Spawn&& spawn, Save save) {
        requested_.fetch_add(1, std::memory_order_acq_rel);
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
            return Requested::Coalesced; // the running writer will pick this request up
        if (!closed_.load(std::memory_order_acquire)) {
            threads_.fetch_add(1, std::memory_order_acq_rel);
            try {
                // threads_ drop is the worker's LAST touch of *this: WaitIdle() only
                // reports idle after it, so shutdown never races the tail re-check.
                spawn([this, save]() mutable { RunWorker(save); threads_.fetch_sub(1, std::memory_order_acq_rel); });
                return Requested::Started;
            } catch (...) { threads_.fetch_sub(1, std::memory_order_acq_rel); }
        }
        RunWorker(save);
        return Requested::Inline;
    }

    // Writer body. Precondition: running_ was claimed by Request().
    template<class Save>
    void RunWorker(Save& save) {
        for (;;) {
            const std::uint64_t target = requested_.load(std::memory_order_acquire);
            const bool ok = save(target);
            passes_.fetch_add(1, std::memory_order_acq_rel);
            if (!ok) failures_.fetch_add(1, std::memory_order_acq_rel);
            completed_.store(target, std::memory_order_release);
            running_.store(false, std::memory_order_release);
            if (requested_.load(std::memory_order_acquire) == target) return;
            bool expected = false; // newer request: reclaim unless its requester already did
            if (!running_.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) return;
        }
    }

    // Bounded shutdown wait. Returns true when no writer runs and every worker thread
    // has finished touching this object.
    // Spin-free: sleeps in small steps; no mutex (a writer killed by process exit
    // cannot leave a lock that would hang the caller).
    bool WaitIdle(std::chrono::milliseconds timeout) const {
        const auto deadline = std::chrono::steady_clock::now() + timeout;
        while (running_.load(std::memory_order_acquire) || threads_.load(std::memory_order_acquire) != 0) {
            if (std::chrono::steady_clock::now() >= deadline) return false;
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        return true;
    }

    // After Close(), new requests run inline (no new threads during unload).
    void Close() { closed_.store(true, std::memory_order_release); }

    bool Running() const { return running_.load(std::memory_order_acquire); }
    int Threads() const { return threads_.load(std::memory_order_acquire); }
    std::uint64_t RequestedCount() const { return requested_.load(std::memory_order_acquire); }
    std::uint64_t CompletedGeneration() const { return completed_.load(std::memory_order_acquire); }
    std::uint64_t Failures() const { return failures_.load(std::memory_order_acquire); }

private:
    std::atomic<std::uint64_t> requested_{0}, completed_{0}, passes_{0}, failures_{0};
    std::atomic<int> threads_{0};
    std::atomic<bool> running_{false}, closed_{false};
};

} // namespace kcd2vr::core
