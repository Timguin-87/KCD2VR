#pragma once
#include "native_door_script19c4r4.h"
#include "snapshot_mailbox.h"
#include "door_curve_signal66.h"
#include "../diagnostics/capture168.h"
#include <array>
#include <atomic>
#include <cstring>

namespace kcd2vr::door_channel19c4r4 {
using door_script19c4r4::Command;using door_script19c4r4::Ack;
inline constexpr char prefix[]="kcd2vr_door_";
inline constexpr std::uint32_t maxSerial=8388607,maxSequence=8388606,maxTarget=16777215;
struct Posted {Command command{};std::uint64_t tick=0,epoch=0;std::uintptr_t entity=0;};
using Cleanup=door_script19c4r4::CleanupIdentity;
struct Feedback {Ack ack{};std::uint64_t epoch=0;};
class Channel {
    std::atomic<bool> enabled_{false};std::atomic<std::uint64_t> epoch_{1};
    core::SnapshotMailbox<Posted> current_,history_[16];
    core::SnapshotMailbox<Cleanup> cleanup_;
    core::SnapshotMailbox<Feedback> feedback_;
    std::atomic<std::uint32_t> claimed_{0},next_{0};
    std::atomic_flag claimBusy_=ATOMIC_FLAG_INIT;
    std::atomic<bool> companion46_{false},companion69_{false};
    static bool fresh(const Posted& p,std::uint64_t now) noexcept {return p.tick&&now>=p.tick&&now-p.tick<=80;}
    bool posted(std::uint32_t sequence,std::uint32_t request,Posted& p) const noexcept {
        for(const auto& slot:history_)if(slot.read(p)&&p.command.sequence==sequence&&p.command.request==request)return true;
        return false;
    }
    static bool digits(const char*& p,std::uint32_t& value,char separator,std::uint32_t limit) noexcept {
        if(!p||*p<'0'||*p>'9')return false;
        value=0;unsigned count=0;
        while(*p>='0'&&*p<='9'){
            const auto digit=unsigned(*p-'0');
            if(++count>8||value>limit/10||(value==limit/10&&digit>limit%10))return false;
            value=value*10+unsigned(*p++-'0');
        }
        if(*p!=separator)return false;if(separator)++p;return true;
    }
public:
    void enabled(bool on) noexcept {
        if(enabled_.exchange(false))epoch_.fetch_add(1);
        if(on){epoch_.fetch_add(1);enabled_.store(true);}
    }
    bool enabled() const noexcept {return enabled_.load();}
    bool companion46() const noexcept {return companion46_.load(std::memory_order_acquire);}
    bool pending() const noexcept {return claimed_.load()!=0;}
    bool cleanup(Cleanup& out) const noexcept {
        const auto token=claimed_.load();
        return token&&cleanup_.read(out)&&out.request==token&&claimed_.load()==token;
    }
    bool retireKnownAbsent(const Cleanup& expected,std::uintptr_t currentEntity) noexcept {
        // No body access or restoration assertion. Only a successful native
        // registry observation can retire an exact old opaque identity.
        if(!expected.entity||currentEntity==expected.entity)return false;
        if(claimBusy_.test_and_set())return false;
        Cleanup live{};bool retired=false;
        if(cleanup(live)&&live.request==expected.request&&live.target==expected.target&&
            live.source==expected.source&&live.epoch==expected.epoch&&live.entity==expected.entity){
            // Invalidate old commands without changing enable intent or racing
            // a concurrent Stop back ON. Claim is serialized until retirement.
            epoch_.fetch_add(1);
            std::uint32_t token=expected.request;retired=claimed_.compare_exchange_strong(token,0);
        }
        claimBusy_.clear();return retired;
    }
    bool publish(const Command& c,std::uint64_t now,std::uintptr_t entity=0) noexcept {
        const auto epoch=epoch_.load();
        if(!enabled_.load()||!now||c.sequence<2||c.sequence>maxSequence||(c.sequence&1)||
           !c.request||c.request>maxSerial||!c.target||c.target>maxTarget||
           (c.phase!=0&&c.phase!=1&&c.phase!=2&&c.phase!=4&&c.phase!=5)||
           (c.mode69!=2&&c.mode69!=3)||(c.mode69==3&&!companion69_.load(std::memory_order_acquire))||
           !door_script19c4r4::Owner::fraction(c.fraction))return false;
        const auto held=claimed_.load();if(held&&held!=c.request)return false;
        Posted old{};if(current_.read(old)&&old.command.sequence>=c.sequence)return false;
        const Posted p{c,now,epoch,entity};
        if(!history_[next_.fetch_add(1)%16].publish(p)||!current_.publish(p))return false;
        return enabled_.load()&&epoch_.load()==epoch;
    }
    bool claim(std::uint32_t source,std::uint32_t request,std::uint64_t now) noexcept {
        if(claimBusy_.test_and_set())return false;
        Posted p{};bool granted=false;
        const auto epoch=epoch_.load();
        if(enabled_.load()&&!claimed_.load()&&posted(source,request,p)&&fresh(p,now)&&p.epoch==epoch&&p.entity&&p.command.phase==1){
            Cleanup lease{request,p.command.target,source,epoch,p.entity};
            // Publish possible-pause cleanup BEFORE the final enabled check.
            // Stop either sees this lease, or the final check denies the pause.
            if(cleanup_.publish(lease)){
                claimed_.store(request);
                Posted current{};
                if(enabled_.load()&&epoch_.load()==epoch&&current_.read(current)&&current.epoch==epoch&&
                   current.command.request==request&&current.command.target==lease.target&&current.entity==lease.entity&&current.command.phase==1)granted=true;
                else claimed_.store(0);
            }
        }
        claimBusy_.clear();return granted;
    }
    bool acknowledge(std::uint32_t source,std::uint32_t request,std::uint32_t state,std::uint32_t micro,std::uint64_t now) noexcept {
        if(!now||source<2||source>maxSequence||(source&1)||!request||request>maxSerial||state>2||micro>999500)return false;
        if(claimed_.load()!=request){
            Feedback prior{};
            return state!=1&&feedback_.read(prior)&&prior.ack.state==int(state)&&
                prior.ack.source==source&&prior.ack.request==request;
        }
        Cleanup lease{};if(!cleanup_.read(lease)||lease.request!=request)return false;
        Posted p{};std::uint64_t epoch=lease.epoch;
        if(state==1){
            if(!enabled_.load()||epoch_.load()!=epoch||!posted(source,request,p)||p.epoch!=epoch||
               p.command.target!=lease.target||!fresh(p,now)||p.command.phase==0)return false;
        }else{
            // A delayed release only retires the same immutable cleanup lease.
            // It grants no new command/hold authority and must survive menus.
            Posted last{};
            if(source<lease.source||!current_.read(last)||last.command.request!=request||source>last.command.sequence)return false;
        }
        Feedback f{{source,request,int(state),float(micro)/1000000.f,now},epoch};
        if(!feedback_.publish(f))return false;
        if(state!=1){std::uint32_t expected=request;(void)claimed_.compare_exchange_strong(expected,0);}
        return true;
    }
    bool ack(Ack& out,std::uint64_t now) const noexcept {
        Feedback f{};if(!feedback_.read(f)||!f.ack.tick||now<f.ack.tick||now-f.ack.tick>80)return false;
        if(f.ack.state==1&&(!enabled_.load()||f.epoch!=epoch_.load()))return false;
        out=f.ack;return true;
    }
    bool curve66(door_curve_signal66::Query& q,std::uint64_t now) const noexcept {
        Cleanup lease{};Posted p{};const auto epoch=epoch_.load();
        if(!enabled_.load()||!cleanup(lease)||lease.request!=q.lease.request||lease.epoch!=epoch||
           !posted(q.source,lease.request,p)||!fresh(p,now)||p.epoch!=epoch||p.command.phase==0||
           p.entity!=lease.entity||p.command.target!=lease.target)return false;
        q.lease=lease;return enabled_.load()&&epoch_.load()==epoch&&claimed_.load()==lease.request;
    }
    bool field(const char* name,std::uint64_t now,std::uint32_t& out) noexcept {
        if(!name||std::strncmp(name,prefix,sizeof(prefix)-1))return false;
        const char* key=name+sizeof(prefix)-1;out=0;
        // D48: read-only finite diagnostic state. No registered CVar, door
        // enable/claim or gameplay mutation is needed to query these fields.
        if(!std::strncmp(key,"capture_",8)){
            namespace cap=diagnostics::capture168;
            const char* rung=key+8;
            if(!std::strcmp(rung,"f2"))out=cap::active(cap::Domain::Bug,now)?1:0;
            else if(!std::strcmp(rung,"f3"))out=cap::active(cap::Domain::Combat,now)?1:0;
            else if(!std::strcmp(rung,"f5"))out=cap::active(cap::Domain::Interaction,now)?1:0;
            else if(!std::strcmp(rung,"f7"))out=cap::active(cap::Domain::Vrik,now)?1:0;
            else if(!std::strcmp(rung,"f11"))out=cap::active(cap::Domain::Reload,now)?1:0;
            return true; // unknown/private capture fields are always zero
        }
        if(!std::strncmp(key,"claim_",6)){
            const char* p=key+6;std::uint32_t source=0,request=0;
            if(digits(p,source,'_',maxSequence)&&digits(p,request,0,maxSerial))out=claim(source,request,now)?1:0;
            return true;
        }
        if(!std::strncmp(key,"ack_",4)){
            const char* p=key+4;std::uint32_t source=0,request=0,state=0,micro=0;
            if(digits(p,source,'_',maxSequence)&&digits(p,request,'_',maxSerial)&&digits(p,state,'_',2)&&digits(p,micro,0,999500))
                out=acknowledge(source,request,state,micro,now)?1:0;
            return true;
        }
        if(!enabled_.load())return true;
        if(!std::strcmp(key,"companion_69")){companion69_.store(true,std::memory_order_release);out=69;return true;}
        if(!std::strcmp(key,"companion_66")){companion46_.store(true,std::memory_order_release);out=66;return true;}
        Posted p{};const bool have=current_.read(p)&&p.epoch==epoch_.load()&&fresh(p,now);
        // Valid idle lets DoPlay capture request0 before the first accepted Use.
        const Command c=have?p.command:Command{2,0,0,0,0};
        if(!std::strcmp(key,"version"))out=1966;
        else if(!std::strcmp(key,"seq"))out=(have||!pending())?c.sequence:0;
        else if(!std::strcmp(key,"request"))out=c.request;
        else if(!std::strcmp(key,"target_hi"))out=c.target>>16;
        else if(!std::strcmp(key,"target_lo"))out=c.target&65535;
        else if(!std::strcmp(key,"mode69"))out=std::uint32_t(c.mode69);
        else if(!std::strcmp(key,"phase"))out=std::uint32_t(c.phase);
        else if(!std::strcmp(key,"fraction"))out=std::uint32_t(std::lround(c.fraction*1000000.f));
        return true;
    }
};
}
