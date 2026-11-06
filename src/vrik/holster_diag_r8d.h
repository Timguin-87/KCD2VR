#pragma once
// R8d (Claude, 25 Oct): F2 rung 1 holster attempt diagnostics. "are the holsters working and why not".
// Pure observer: one per-hand attempt FSM fed once per pose update with what the holster producer already
// computed (zone gap, draw volume, nearest-zone arbitration, PhysDrawCandidate126 gate, equipment114 FSM,
// hand claims, HS2 switch, issued commands, native held key). It never feeds anything back.
// An attempt starts on a Grip rise (> .55) near a zone, on entering a zone with Grip already closed (the
// H50 10:33:48 polearm: gap .248, Grip 1, latched 0 - the FSM needs an OPEN hand in the zone first), or when
// the FSM leaves neutral / a request is issued. It ends on request + native ACK, Grip release (< .25) or 2 s.
// Stages in order: ZONE VOLUME ARBITRATION ELIGIBLE FSM_LATCH DWELL/PULL REQUEST NATIVE_ACK VISIBLE.
#include <cstdint>
#include <cstdio>
namespace kcd2vr::holster_diag_r8d {
enum class Stage : unsigned char { Zone, Volume, Arbitration, Eligible, FsmLatch, Pull, Request, NativeAck, Visible, Complete };
enum class Refuse : unsigned char {
    None, NoMount, OutsideZone, OutsideVolume, LostArbitration, DaggerSlot, ItemHeld, NotEquipped, NativeNotReady,
    HandHolds, OtherLoadoutHeld, Ambiguous, OtherSelected, PendingOther, NotTracked, BowQuiver,
    HipKnife, HandClaim, QuickTalk, Door, F9, Fist,
    GripClosedAtEntry, BlockedAfterTimeout, NoSqueeze, NoPull, SwitchWait, SwitchTimeout, NoAck, NotVisible, Count };
enum class Result : unsigned char { None, Drawn, Stowed, Refused, Timeout, ReleasedEarly };
inline constexpr double kGripHeld=.55,kGripReleased=.25,kNearSlack=.12;
inline constexpr std::uint64_t kAttemptMs=2000,kSummaryMs=5000,kVisibleMs=2000,kDwellMs=80;

inline const char* stage_name(Stage s) noexcept {
    switch(s){case Stage::Zone:return "ZONE";case Stage::Volume:return "VOLUME";case Stage::Arbitration:return "ARBITRATION";
    case Stage::Eligible:return "ELIGIBLE";case Stage::FsmLatch:return "FSM_LATCH";case Stage::Pull:return "DWELL/PULL";
    case Stage::Request:return "REQUEST";case Stage::NativeAck:return "NATIVE_ACK";case Stage::Visible:return "VISIBLE";default:return "NONE";}
}
inline const char* refuse_name(Refuse r) noexcept {
    switch(r){case Refuse::None:return "none";case Refuse::NoMount:return "NO_MOUNT";case Refuse::OutsideZone:return "OUTSIDE_ZONE";
    case Refuse::OutsideVolume:return "OUTSIDE_VOLUME";case Refuse::LostArbitration:return "LOST_ARBITRATION";
    case Refuse::DaggerSlot:return "DAGGER_SLOT";case Refuse::ItemHeld:return "ITEM_HELD";case Refuse::NotEquipped:return "NOT_EQUIPPED";
    case Refuse::NativeNotReady:return "NATIVE_NOT_READY";case Refuse::HandHolds:return "HAND_HOLDS_NATIVE_KEY";
    case Refuse::OtherLoadoutHeld:return "OTHER_LOADOUT_WHILE_HELD";case Refuse::Ambiguous:return "AMBIGUOUS_LOADOUT";
    case Refuse::OtherSelected:return "OTHER_SELECTED_ITEM";case Refuse::PendingOther:return "OTHER_DRAW_PENDING";
    case Refuse::NotTracked:return "NOT_TRACKED";case Refuse::BowQuiver:return "BOW_QUIVER_OWNS_SHOULDER";
    case Refuse::HipKnife:return "HIP_KNIFE_CLAIM";case Refuse::HandClaim:return "HAND_CLAIM_T24";case Refuse::QuickTalk:return "QUICKTALK";
    case Refuse::Door:return "DOOR";case Refuse::F9:return "F9_PHYSICS";case Refuse::Fist:return "FIST_TRIGGER_GRIP";
    case Refuse::GripClosedAtEntry:return "GRIP_CLOSED_AT_ENTRY";case Refuse::BlockedAfterTimeout:return "BLOCKED_AFTER_TIMEOUT";
    case Refuse::NoSqueeze:return "NO_SQUEEZE";case Refuse::NoPull:return "NO_PULL";case Refuse::SwitchWait:return "SWITCH_WAIT";
    case Refuse::SwitchTimeout:return "SWITCH_TIMEOUT";case Refuse::NoAck:return "NO_NATIVE_ACK";case Refuse::NotVisible:return "NOT_VISIBLE";
    default:return "?";}
}
inline const char* claim_name(Refuse r) noexcept {
    switch(r){case Refuse::QuickTalk:return "QuickTalk";case Refuse::Door:return "door";case Refuse::F9:return "F9";
    case Refuse::HipKnife:return "knife";case Refuse::HandClaim:return "hand_claim_t24";case Refuse::Fist:return "fist";default:return "none";}
}
inline const char* result_name(Result r) noexcept {
    switch(r){case Result::Drawn:return "DRAWN";case Result::Stowed:return "STOWED";case Result::Refused:return "REFUSED";
    case Result::Timeout:return "TIMEOUT";case Result::ReleasedEarly:return "RELEASED_EARLY";default:return "NONE";}
}
inline const char* class_name(int c) noexcept {
    switch(c){case 7:return "polearm";case 9:return "bow";case 8:case 17:return "shield";case 10:case 14:case 15:return "crossbow";
    case 13:return "long gun";case 3:case 5:case 6:return "hip weapon";default:return "weapon";}
}
inline const char* zone_text(unsigned z) noexcept { // ph::HolsterZone126 order
    switch(z){case 1:return "right shoulder";case 2:return "left shoulder";case 3:return "belt";case 4:return "right hip";
    case 5:return "left back";case 6:return "left chest";case 7:return "left shoulder";case 8:return "right shoulder";case 9:return "right back";default:return "holster";}
}
inline bool shoulder_zone(unsigned z) noexcept {return z==1||z==2||z==5||z==7||z==8;}

// One unheld or held equipment item as the producer saw it this update (after equipment114::update).
struct Item {
    std::uint64_t key=0;int classId=-1;unsigned zone=0;int hand=-1;
    bool held=false,handKeyMatch=false,handBusy=false,mountKnown=false,volumeOk=false,arbitrationWon=true;
    double gap=1.0e9,radius=.24;int lostToClass=-1,heldClass=-1;Refuse gate=Refuse::None;
    bool neutral=false,latched=false,pending=false,blocked=false,wantDrawn=false;std::uint64_t latchedMs=0;
    unsigned char request=0; // 1 draw, 2 holster, 3 drop (an issued pose command for this key)
};
struct HandFrame {
    double grip=0;Refuse claim=Refuse::None;unsigned char switchState=0; // 1 waiting for the stow, 2 timed out
    int count=0;Item items[8]{};
};
inline const char* fsm_name(const Item& i) noexcept {
    if(i.pending)return "requested";if(i.blocked)return "blocked";
    if(i.latched)return i.latchedMs<kDwellMs?"dwell":"pull";return i.neutral?"neutral":"closed";
}
inline bool near_zone(const Item& i) noexcept {return i.mountKnown&&!i.held&&i.gap<i.radius+kNearSlack;}
// The item an attempt is about: the attempt's own key, else an issued/latched one, else the nearest zone.
inline int pick(const HandFrame& f,std::uint64_t key) noexcept {
    int best=-1;double gap=1.0e9;
    for(int i=0;i<f.count&&i<8;++i){const auto& it=f.items[i];
        if(key){if(it.key==key)return i;continue;}
        if(it.request||(!it.held&&(it.pending||it.latched)))return i; // a drawn weapon only counts once a stow is issued
        if(near_zone(it)&&it.gap<gap){gap=it.gap;best=i;}
    }
    return best;
}
// First stage this item fails right now, with the real gate that stopped it.
inline Stage classify(const Item& it,const HandFrame& f,Refuse& why) noexcept {
    why=Refuse::None;
    if(f.switchState==2){why=Refuse::SwitchTimeout;return Stage::Request;}
    if(f.switchState==1){why=Refuse::SwitchWait;return Stage::Request;}
    if(it.held){return Stage::Request;} // stow: release Grip anywhere issues it
    if(!it.mountKnown){why=Refuse::NoMount;return Stage::Zone;}
    if(it.gap>=it.radius&&!it.latched){why=Refuse::OutsideZone;return Stage::Zone;}
    if(!it.volumeOk&&!it.latched){why=Refuse::OutsideVolume;return Stage::Volume;}
    if(!it.arbitrationWon){why=Refuse::LostArbitration;return Stage::Arbitration;}
    if(f.claim!=Refuse::None){why=f.claim;return Stage::Eligible;}
    if(it.gate!=Refuse::None){why=it.gate;return Stage::Eligible;}
    if(it.blocked){why=Refuse::BlockedAfterTimeout;return Stage::FsmLatch;}
    if(!it.latched){why=it.neutral?Refuse::NoSqueeze:(f.grip>kGripHeld?Refuse::GripClosedAtEntry:Refuse::NoSqueeze);return Stage::FsmLatch;}
    why=Refuse::NoPull;return Stage::Pull;
}
struct Attempt {
    int side=0;std::uint64_t key=0,start=0,end=0,ackMs=0;int classId=-1,lostToClass=-1;unsigned zone=0;
    double gap=1.0e9;bool volumeOk=false,arbitrationWon=true,eligible=false,acked=false;
    Refuse why=Refuse::None,claim=Refuse::None,gate=Refuse::None;const char* fsm="neutral";
    unsigned char request=0,switchState=0;Stage firstMissing=Stage::Zone;Result result=Result::None;int heldClass=-1;
};
struct Track {
    bool active=false,lastNear=false;double lastGrip=0;std::uint64_t requestTick=0;Stage best=Stage::Zone;Attempt a{};
};
inline bool refusal(Refuse r) noexcept {return r!=Refuse::None&&r!=Refuse::NoSqueeze&&r!=Refuse::NoPull;}
// Feed one update. Returns true and fills `done` when an attempt ended.
inline bool step(Track& t,const HandFrame& f,int side,std::uint64_t now,Attempt& done) noexcept {
    const bool rise=t.lastGrip<=kGripHeld&&f.grip>kGripHeld;
    const int p=pick(f,t.active?t.a.key:0);
    const Item* it=p>=0?&f.items[p]:nullptr;
    const bool nearNow=it&&near_zone(*it);
    bool ended=false;
    if(!t.active){
        const bool fsmLeft=it&&(it->request||(!it->held&&(it->pending||it->latched)));
        const bool entryClosed=nearNow&&!t.lastNear&&f.grip>kGripHeld&&!it->handBusy;
        if(it&&(fsmLeft||(rise&&nearNow)||entryClosed)){
            t=Track{};t.active=true;t.a.side=side;t.a.key=it->key;t.a.start=now;t.a.classId=it->classId;t.a.zone=it->zone;
            Refuse why{};t.best=classify(*it,f,why);t.a.why=why;
        }
    }
    if(t.active){
        auto& a=t.a;
        const bool releasing=!a.request&&!(it&&it->request)&&f.grip<kGripReleased; // keep the failing frame's fields, not the release frame's
        if(it&&!releasing){
            a.gap=it->gap;a.volumeOk=it->volumeOk;a.arbitrationWon=it->arbitrationWon;a.lostToClass=it->lostToClass;
            a.gate=it->gate;a.claim=f.claim;a.fsm=fsm_name(*it);if(f.switchState)a.switchState=f.switchState;if(it->heldClass>=0)a.heldClass=it->heldClass;
            a.eligible=it->arbitrationWon&&f.claim==Refuse::None&&it->gate==Refuse::None;
            if(it->request&&!a.request){a.request=it->request;t.requestTick=now;}
            if(a.request&&!a.acked){
                const bool draw=a.request==1;
                if(draw?it->handKeyMatch:!it->handKeyMatch){a.acked=true;a.ackMs=now-t.requestTick;}
            }
        }
        if(a.acked){
            const bool draw=a.request==1;const bool visible=it&&(draw?it->held:!it->held);
            if(visible){a.result=draw?Result::Drawn:Result::Stowed;a.firstMissing=Stage::Complete;a.why=Refuse::None;}
            else if(now-t.requestTick>kAttemptMs){a.result=Result::Refused;a.firstMissing=Stage::Visible;a.why=Refuse::NotVisible;}
            ended=a.result!=Result::None;
        }else if(a.request){
            t.best=Stage::NativeAck;a.why=Refuse::NoAck;
            if(now-t.requestTick>kAttemptMs){a.ackMs=now-t.requestTick;a.result=Result::Timeout;a.firstMissing=Stage::NativeAck;ended=true;}
        }else if(releasing){
            a.firstMissing=t.best;a.result=refusal(a.why)?Result::Refused:Result::ReleasedEarly;ended=true;
        }else{
            if(it){Refuse why{};const Stage s=classify(*it,f,why);
                if(s>t.best||(s==t.best&&a.why==Refuse::None)){t.best=s;a.why=why;}}
            if(now-a.start>kAttemptMs){a.firstMissing=t.best;a.result=refusal(a.why)?Result::Refused:Result::Timeout;ended=true;}
        }
        if(ended){a.end=now;done=a;t.active=false;}
    }
    t.lastGrip=f.grip;t.lastNear=nearNow;
    return ended;
}

inline int format_attempt(const Attempt& a,char* out,std::size_t n) noexcept {
    char arb[48]{};
    if(a.arbitrationWon)std::snprintf(arb,sizeof(arb),"WON");else std::snprintf(arb,sizeof(arb),"LOST_TO:%s",class_name(a.lostToClass));
    const char* sw=a.switchState==1?"WAIT":a.switchState==2?"TIMEOUT":"none";
    char ack[24]{};if(a.acked)std::snprintf(ack,sizeof(ack),"%llu",(unsigned long long)a.ackMs);else std::snprintf(ack,sizeof(ack),"-1");
    return std::snprintf(out,n,"R8D HOLSTER_ATTEMPT side=%c item=%016llX class=%s zone=%s gap=%.3f volumeOk=%d arbitration=%s eligible=%d refuse=%s "
        "fsm=%s request=%s ack=%d ackMs=%s switch=%s claim=%s result=%s firstMissing=%s ms=%llu",
        a.side?'L':'R',(unsigned long long)a.key,class_name(a.classId),zone_text(a.zone),a.gap<1.0e8?a.gap:-1.0,a.volumeOk,arb,a.eligible,
        refuse_name(a.why),a.fsm,a.request==1?"DRAW":a.request==2?"HOLSTER":a.request==3?"DROP":"none",a.acked,ack,sw,claim_name(a.claim),
        result_name(a.result),a.firstMissing==Stage::Complete?"none":stage_name(a.firstMissing),(unsigned long long)(a.end-a.start));
}
// One short in-game line (ASCII, no quotes, <= 96 chars).
inline void visible_text(const Attempt& a,char* out,std::size_t n) noexcept {
    const char* c=class_name(a.classId);const char* hand=a.side?"left":"right";const char* z=zone_text(a.zone);
    const int cm=a.gap<1.0e8?int(a.gap*100.0+.5):-1;
    if(a.result==Result::Drawn){std::snprintf(out,n,"%s: drawn (game ACK %llu ms)",c,(unsigned long long)a.ackMs);return;}
    if(a.result==Result::Stowed){std::snprintf(out,n,"%s: stowed (game ACK %llu ms)",c,(unsigned long long)a.ackMs);return;}
    switch(a.why){
    case Refuse::NoMount:std::snprintf(out,n,"%s: no holster point yet (body/mount not known)",c);break;
    case Refuse::OutsideZone:std::snprintf(out,n,"%s: %s hand %d cm from the %s zone",c,hand,cm,z);break;
    case Refuse::OutsideVolume:std::snprintf(out,n,a.classId==9||shoulder_zone(a.zone)?"%s: hand not above the %s":"%s: hand not in the %s space",c,z);break;
    case Refuse::LostArbitration:std::snprintf(out,n,"%s: the %s zone is nearer to the hand",c,class_name(a.lostToClass));break;
    case Refuse::HandHolds:std::snprintf(out,n,"%s: %s hand holds the %s - stow it first",c,hand,a.heldClass>=0?class_name(a.heldClass):"weapon");break;
    case Refuse::OtherLoadoutHeld:std::snprintf(out,n,"%s: other loadout while a hand holds a weapon",c);break;
    case Refuse::Ambiguous:std::snprintf(out,n,"%s: two stored loadouts claim this zone",c);break;
    case Refuse::OtherSelected:std::snprintf(out,n,"%s: the selected loadout owns this zone",c);break;
    case Refuse::ItemHeld:std::snprintf(out,n,"%s: already in hand",c);break;
    case Refuse::NotEquipped:std::snprintf(out,n,"%s: not in an equipment slot",c);break;
    case Refuse::DaggerSlot:std::snprintf(out,n,"%s: dagger slot is not a holster draw",c);break;
    case Refuse::NativeNotReady:std::snprintf(out,n,"%s: the game has not reported the hands yet",c);break;
    case Refuse::PendingOther:std::snprintf(out,n,"%s: another draw is still waiting for the game",c);break;
    case Refuse::NotTracked:std::snprintf(out,n,"%s: %s controller not tracked",c,hand);break;
    case Refuse::BowQuiver:std::snprintf(out,n,"%s: right shoulder is the quiver while a bow or crossbow is out",c);break;
    case Refuse::HipKnife:std::snprintf(out,n,"%s: right hip taken by the knife/axe grab",c);break;
    case Refuse::HandClaim:std::snprintf(out,n,"%s: %s hand claimed by another grab",c,hand);break;
    case Refuse::QuickTalk:std::snprintf(out,n,"%s: %s hand busy with quick talk",c,hand);break;
    case Refuse::Door:std::snprintf(out,n,"%s: %s hand is on a door",c,hand);break;
    case Refuse::F9:std::snprintf(out,n,"%s: hands owned by F9 physics",c);break;
    case Refuse::Fist:std::snprintf(out,n,"%s: fist (Trigger+Grip) - squeeze Grip only",c);break;
    case Refuse::GripClosedAtEntry:std::snprintf(out,n,"%s: open the hand at the %s, then squeeze",c,shoulder_zone(a.zone)?"shoulder":"zone");break;
    case Refuse::BlockedAfterTimeout:std::snprintf(out,n,"%s: last draw timed out - release and squeeze again",c);break;
    case Refuse::NoSqueeze:std::snprintf(out,n,"%s: squeeze Grip at the %s",c,z);break;
    case Refuse::NoPull:std::snprintf(out,n,"%s: grabbed - pull it out further",c);break;
    case Refuse::SwitchWait:std::snprintf(out,n,"%s: %s hand holds the %s - stowing it",c,hand,a.heldClass>=0?class_name(a.heldClass):"weapon");break;
    case Refuse::SwitchTimeout:std::snprintf(out,n,"%s: the game kept the weapon in hand - stow it first",c);break;
    case Refuse::NoAck:std::snprintf(out,n,a.request==1?"%s: the game didn't attach it (no ACK in %llu ms)":"%s: the game didn't stow it (no ACK in %llu ms)",c,(unsigned long long)a.ackMs);break;
    case Refuse::NotVisible:std::snprintf(out,n,"%s: game acked but it is not in hand",c);break;
    default:std::snprintf(out,n,"%s: %s at %s",c,result_name(a.result),stage_name(a.firstMissing));break;
    }
}

// Per class counters for the 5 s summary.
struct Summary {
    struct Row {unsigned attempts=0,drawn=0,stowed=0,refused=0,timeout=0,released=0;unsigned short why[unsigned(Refuse::Count)]{};};
    Row rows[32]{};std::uint64_t due=0;unsigned total=0;
    void add(const Attempt& a) noexcept {
        auto& r=rows[a.classId>=0&&a.classId<32?a.classId:31];++r.attempts;++total;
        if(a.result==Result::Drawn)++r.drawn;else if(a.result==Result::Stowed)++r.stowed;
        else if(a.result==Result::Refused)++r.refused;else if(a.result==Result::Timeout)++r.timeout;else ++r.released;
        if(a.result!=Result::Drawn&&a.result!=Result::Stowed&&r.why[unsigned(a.why)]<65535)++r.why[unsigned(a.why)];
    }
};
inline int format_summary(const Summary& s,char* out,std::size_t n) noexcept {
    int w=std::snprintf(out,n,"R8D HOLSTER_SUMMARY attempts=%u",s.total);
    for(int c=0;c<32;++c){const auto& r=s.rows[c];if(!r.attempts||w<0||std::size_t(w)>=n)continue;
        w+=std::snprintf(out+w,n-std::size_t(w)," | %s#%d a=%u drawn=%u stowed=%u refused=%u timeout=%u released=%u",class_name(c),c,r.attempts,r.drawn,r.stowed,r.refused,r.timeout,r.released);
        for(unsigned k=1;k<unsigned(Refuse::Count);++k)if(r.why[k]&&w>=0&&std::size_t(w)<n)
            w+=std::snprintf(out+w,n-std::size_t(w)," %s:%u",refuse_name(Refuse(k)),unsigned(r.why[k]));
    }
    return w;
}
struct Diag {Track hands[2]{};Summary summary{};std::uint64_t lastVisible=0;};
inline bool visible_due(Diag& d,std::uint64_t now) noexcept {
    if(d.lastVisible&&now>=d.lastVisible&&now-d.lastVisible<kVisibleMs)return false;d.lastVisible=now;return true;
}
} // namespace kcd2vr::holster_diag_r8d
