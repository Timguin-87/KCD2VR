// A metadata-only transaction. Native constraints retire in their original T4
// owner; AddItem runs once only after that exact original retirement receipt.
#include "subsystems/throw_drop/grip_stow_cb1.h"
#include "native_script_t2.h"
#include "diagnostic_trace.h"
#include "vrik/visible_failure.h"
#include "core/snapshot_mailbox.h"
#include <windows.h>
#include <atomic>
#include <cstdio>
#include <cstring>
namespace kcd2vr::grip_stow_cb1 {
namespace {
namespace gp=physics::grab_producer60;
std::atomic<bool> enabled{false},registered{false};std::atomic<int> button{0};
std::atomic<std::uint64_t> generation{1},serials{0},pending{0};std::atomic<CurrentReader> reader{nullptr};
struct Req {gp::Owner owner{};Identity identity{};std::uint64_t serial=0,tick=0,generation=0,vm=0;unsigned stage=0;bool abort=false;};
core::SnapshotMailbox<Req> inbox;Req active{};
bool Same(const Identity&a,const Identity&b) noexcept {return a.player==b.player&&a.actor==b.actor&&a.character==b.character&&a.manager==b.manager&&a.nativePlayer==b.nativePlayer&&a.body==b.body&&a.tracking==b.tracking&&a.request==b.request&&a.policy==b.policy;}
bool Current(const gp::Owner&o,std::uint64_t now,Identity& id) noexcept {
    id={};auto f=reader.load(std::memory_order_acquire);return f&&f(o,now,id)&&id.player&&id.actor==o.player&&id.character&&id.manager&&id.nativePlayer&&id.body==o.poseEpoch&&id.tracking==o.trackingEpoch;
}
bool Wanted() noexcept {return pending.load(std::memory_order_acquire)!=0;}
bool Packet(const native_script_t2::Context&ctx,const Req&r,const char*action,const char*expected=nullptr) noexcept {
    char in[160]{},out[128]{},gotAction[16]{},status[32]{};unsigned long long serial=0;int used=0;
    std::snprintf(in,sizeof(in),"%llu|%s|%u|%u|%d",(unsigned long long)r.serial,action,r.identity.player,r.owner.entity,r.owner.side);
    if(!ctx.CallPacket("kcd2vr_grip_stow_cb1",in,out,sizeof(out))){diagnostic_trace::Write("H41 STOW scriptReceipt=UNKNOWN noTransferRetry=1");return false;}
    if(std::sscanf(out,"OK|%llu|%15[^|]|%31[^|]%n",&serial,gotAction,status,&used)!=3||out[used]||serial!=r.serial||std::strcmp(action,gotAction)){diagnostic_trace::Write("H41 STOW scriptReceipt=IDENTITY_OR_PARSE_REJECTED noTransferRetry=1");return false;}
    char line[192]{};std::snprintf(line,sizeof(line),"H41 STOW serial=%llu side=%d action=%s result=%s inventory=ORIGINAL_NATIVE_ONLY",serial,r.owner.side,action,status);diagnostic_trace::Write(line);
    if(!std::strcmp(status,"NATIVE_REFUSED"))(void)visible_failure::publish("The game refused to stow this item",ctx.now);
    else if(!std::strcmp(status,"NATIVE_UNKNOWN"))(void)visible_failure::publish("Stow result unavailable",ctx.now);
    return !expected||!std::strcmp(status,expected);
}
struct PrepareContext {const native_script_t2::Context*ctx;Req*r;};
bool Prepare(void*p) noexcept {
    auto& c=*static_cast<PrepareContext*>(p);Identity current{};
    return enabled.load(std::memory_order_acquire)&&c.r->generation==generation.load(std::memory_order_acquire)&&Current(c.r->owner,c.ctx->now,current)&&Same(current,c.r->identity)&&Packet(*c.ctx,*c.r,"PREPARE","PREPARED");
}
void ClearPending(std::uint64_t serial) noexcept {auto own=serial;(void)pending.compare_exchange_strong(own,0,std::memory_order_acq_rel);}
void Done(const Req&r) noexcept {ClearPending(r.serial);active={};}
void Service(const native_script_t2::Context&ctx) noexcept {
    if(!active.serial){Req r{};if(!inbox.read(r)||!r.serial||r.serial!=pending.load(std::memory_order_acquire))return;active=r;}
    auto&r=active;
    Identity current{};const bool identity=Current(r.owner,ctx.now,current)&&Same(current,r.identity);
    const bool live=enabled.load(std::memory_order_acquire)&&r.generation==generation.load(std::memory_order_acquire)&&identity&&ctx.now>=r.tick&&ctx.now-r.tick<=1500;
    if(r.stage==0){
        if(!live){Done(r);return;}
        PrepareContext c{&ctx,&r};
        if(!gp::PrepareTransfer(r.owner,r.serial,ctx.now,&Prepare,&c)){
            // A prepare read can have run before an unknown reply: cancel only
            // our script snapshot, never AddItem or a replacement physical hold.
            (void)Packet(ctx,r,"CANCEL");(void)visible_failure::publish("Stow cancelled: original item unavailable",ctx.now);r.abort=true;r.stage=3;
            if(gp::EndTransfer(r.owner,r.serial))Done(r);return;
        }
        r.stage=1;r.vm=ctx.generation;return;
    }
    if(r.stage==3){if(gp::EndTransfer(r.owner,r.serial))Done(r);return;}
    if(!live||ctx.generation!=r.vm)r.abort=true;
    const auto progress=gp::ReadTransfer(r.owner,r.serial);
    if(progress==gp::TransferProgress::Declined){
        (void)Packet(ctx,r,"CANCEL");r.stage=3;if(gp::EndTransfer(r.owner,r.serial))Done(r);return;
    }
    if(progress!=gp::TransferProgress::Retired)return; // BUSY/unknown is NEVER release proof
    if(r.stage==1){
        r.stage=2; // spend before entering Lua; unknown/throw cannot replay AddItem
        (void)Packet(ctx,r,r.abort?"CANCEL":"COMMIT");
    }
    // Native references are already retired; preserve reservation across BUSY.
    if(gp::EndTransfer(r.owner,r.serial))Done(r);
}
}
void SetCurrentReader(CurrentReader f) noexcept {reader.store(f,std::memory_order_release);}
bool Enabled() noexcept{return enabled.load(std::memory_order_acquire);}
void SetEnabled(bool on) noexcept{if(enabled.exchange(on,std::memory_order_acq_rel)!=on)generation.fetch_add(1,std::memory_order_acq_rel);}
int Button() noexcept{return button.load(std::memory_order_acquire);}
void SetButton(int b) noexcept{const int next=b<0||b>2?0:b;if(button.exchange(next,std::memory_order_acq_rel)!=next)generation.fetch_add(1,std::memory_order_acq_rel);} // D39: an old face-button receipt cannot survive a live remap
bool Request(int side,std::uint64_t now) noexcept {
    const auto originalGeneration=generation.load(std::memory_order_acquire);
    if(!Enabled()||!registered.load(std::memory_order_acquire)||side<0||side>1||!now)return false;
    gp::Status st{};Identity id{};
    if(!gp::ReadStatus(side,st)||st.state!=gp::State::Held||!st.tick||now<st.tick||now-st.tick>80||!st.owner.requestToken||!st.owner.entity||!Current(st.owner,now,id))return false;
    if(!Enabled()||generation.load(std::memory_order_acquire)!=originalGeneration)return false;
    const auto serial=serials.fetch_add(1,std::memory_order_acq_rel)+1;if(serial>9007199254740991ull)return false;
    std::uint64_t empty=0;if(!pending.compare_exchange_strong(empty,serial,std::memory_order_acq_rel))return false;
    Req r{};r.owner=st.owner;r.identity=id;r.serial=serial;r.tick=now;r.generation=originalGeneration;
    if(!inbox.publish(r)){ClearPending(serial);return false;}
    // Publication is the ownership commit point: Service may already have
    // entered. Never retract pending or return the button to another route.
    // The stored ORIGINAL generation makes a later toggle abort/drain normally.
    return true;
}
bool Install() noexcept {
    if(registered.load(std::memory_order_acquire))return true;
    const bool ok=native_script_t2::Register(&Service,&Wanted)&&native_script_t2::Install();registered.store(ok,std::memory_order_release);return ok;
}
}
