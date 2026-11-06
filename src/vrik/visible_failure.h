#pragma once
#include "../core/snapshot_mailbox.h"
#include "../diagnostics/capture168.h"
#include <atomic>
#include <cstdint>
// Mod status is log-only during quiet play. On-screen diagnostics carry a
// finite recorder lease; native game prompts do not use this mailbox.
namespace kcd2vr::visible_failure {
inline constexpr std::size_t kMaxText=96;
inline constexpr std::uint64_t kMinIntervalMs=2000;
inline constexpr std::uint64_t kMaxQueueAgeMs=5000;
struct Message {char text[kMaxText+1]{};std::uint64_t tick=0;std::uint32_t serial=0;
    // Diagnostic messages retain their finite recorder lease across the mailbox.
    // A zero session is log-only and can never authorize an on-screen line.
    std::uint64_t diagnosticSessionD74=0;
    diagnostics::capture168::Domain diagnosticDomainD74=diagnostics::capture168::Domain::Bug;};
inline core::SnapshotMailbox<Message> mailbox;
inline core::SnapshotMailbox<Message> logMailboxD80;
inline std::atomic<std::uint64_t> lastLogPublishD80{0};
inline std::atomic<std::uint32_t> nextSerial{0};
inline std::atomic<std::uint32_t> lastDrainedSerial{0};
inline std::atomic<std::uint64_t> lastDispatched{0}; // last SUCCESSFUL native dispatch (the limiter)
inline bool safe(const char* s) noexcept {
    if(!s||!*s)return false;
    std::size_t n=0;
    for(const char* p=s;*p;++p){
        if(++n>kMaxText)return false; // fixed documented bound: never oversized buffers
        const unsigned char c=static_cast<unsigned char>(*p);
        if(!(c>=32&&c<127)||c=='"'||c=='\\')return false;
    }
    return true;
}
inline bool due(std::uint64_t now) noexcept {
    const auto last=lastDispatched.load(std::memory_order_acquire);
    return !last||now<last||now-last>=kMinIntervalMs; // clock moved backwards: admit
}
// Bounded copied status receipt. Only the existing mod worker writes the log.
inline void publishLogOnlyD80(const char* text,std::uint64_t now) noexcept {
    if(!safe(text)||!now)return;
    auto previous=lastLogPublishD80.load(std::memory_order_acquire);
    if(previous&&now>=previous&&now-previous<kMinIntervalMs)return;
    if(!lastLogPublishD80.compare_exchange_strong(previous,now,std::memory_order_acq_rel))return;
    Message m{};std::size_t i=0;for(;text[i];++i)m.text[i]=text[i];
    m.tick=now;m.serial=nextSerial.fetch_add(1,std::memory_order_relaxed)+1;
    (void)logMailboxD80.publish(m);
}
// R90: user order "no screen messages without F2". Every on-screen line (this
// mailbox and any direct Game.SendInfoText) needs the F2 recorder (Bug domain)
// active right now. F3/F5/F7/F9/F11 captures still log, but never draw text.
// The log row (publishLogOnlyD80 -> "VR status: ...") is unchanged.
inline std::uint64_t screenSessionR90(std::uint64_t now) noexcept {
    namespace cap=diagnostics::capture168;
    if(!cap::active(cap::Domain::Bug,now))return 0;
    return cap::gate.session[unsigned(cap::Domain::Bug)].load(std::memory_order_acquire);
}
inline bool screenAllowedR90(std::uint64_t now) noexcept {return screenSessionR90(now)!=0;}
// Thread-safe publication (any thread). Never calls native code.
inline bool publish(const char* text,std::uint64_t now,
    std::uint64_t diagnosticSessionD74=0,diagnostics::capture168::Domain diagnosticDomainD74=diagnostics::capture168::Domain::Bug) noexcept {
    publishLogOnlyD80(text,now);
    if(!diagnosticSessionD74||!safe(text)||!due(now))return false;
    namespace cap=diagnostics::capture168;
    // The caller's own lease must still be live (no delayed line from an ended capture) ...
    if(!cap::active(diagnosticDomainD74,now)||cap::gate.session[unsigned(cap::canonical(diagnosticDomainD74))].load(std::memory_order_acquire)!=diagnosticSessionD74)return false;
    // ... and R90: the screen itself belongs to F2 only. The line carries the F2 session.
    const auto screen=screenSessionR90(now);
    if(!screen)return false;
    Message m{};std::size_t i=0;for(const char* p=text;*p&&i<kMaxText;++p)m.text[i++]=*p;
    m.text[i]=0;m.tick=now;m.serial=nextSerial.fetch_add(1,std::memory_order_relaxed)+1;
    m.diagnosticSessionD74=screen;m.diagnosticDomainD74=cap::Domain::Bug;
    return mailbox.publish(m);
}
inline bool diagnosticAllowedD74(const Message& m,std::uint64_t now) noexcept {
    if(!m.diagnosticSessionD74)return false;
    namespace cap=diagnostics::capture168;
    // R90: drained only while the same F2 session is still active.
    return cap::canonical(m.diagnosticDomainD74)==cap::Domain::Bug&&screenSessionR90(now)==m.diagnosticSessionD74;
}
inline bool publishDiagnosticD74(const char* text,std::uint64_t now,diagnostics::capture168::Domain domain) noexcept {
    namespace cap=diagnostics::capture168;
    domain=cap::canonical(domain);
    if(!cap::active(domain,now))return false;
    const auto session=cap::gate.session[unsigned(domain)].load(std::memory_order_acquire);
    return session&&publish(text,now,session,domain);
}
}
