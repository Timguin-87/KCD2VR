#pragma once
#include "alchemy_semi46.h"
#include "../diagnostics/interaction_receipt59c10.h"
#include "../diagnostics/capture168.h"
#include "../diagnostic_trace.h"
#include <cstdio>
#include "semi_smith46.h"
#include "../subsystems/smithing/bridge67.h"
#include "../subsystems/smithing/assist_d80.h"
#include "../subsystems/smithing/instant_native_d72.h"
#include "../subsystems/alchemy/input67.h"
#include "../subsystems/alchemy/capture_d42.h"
#include "../subsystems/alchemy/native_return67.h"
#include "craft_attempt_w4.h"
#include "craft_recorder_r8b.h"
#include "visible_failure.h"
#include "station_reposition_d53.h"
#include "workstation178r20c1.h"

namespace kcd2vr::semi_workstations46 {
// Values copied by Mod from its single native input/presentation decision.
// This adapter never queries a game object or infers a minigame from a texture.
struct Context {
    bool enabled=false,qualified=false,alchemyTracked=false,smithTracked=false,smithGestures=true;
    std::uintptr_t alchemy=0,smith=0;
    std::uint64_t lifetime=0;
    int smithPhase=-1;
    bool guided=false;interaction_alignment::HandCandidate touch{};
    bool alchemyGaze63=false; // current native gaze selection; never a synthetic hand candidate
    bool gestureAlchemy67=false,gestureSmith67=false;
    bool alchemyTransferD59=false;
    std::array<subsystems::alchemy::gesture67::WorldPalm,2> world67{};
    std::uintptr_t worldPlayer67[2]{};
    std::array<subsystems::alchemy::near67::AimH21,2> aimH21{};
    bool moveQualifiedD53=false;float moveXD53=0,moveYD53=0;
    bool whyVisibleW4=false; // W4: F5 rung 3 -> one visible line per failed squeeze
};
struct State {
    diagnostics::interaction_receipt59c10::CraftGate50 alchemyTrace50,smithTrace50;
    semi_smith46::Controller smith;
    semi_smith46::Output lastSmith{};
    subsystems::smithing::stroke_receipt_d57::Tracker smithStrokeD57;
    subsystems::smithing::source_h42::Controller smithSourceGripH42;
    subsystems::smithing::source_h42::ForgePump smithForgePumpR17;
    subsystems::smithing::source_h42::Output lastSmithSourceH42{};
    // Diagnostic request identity; never read by input or placement ownership.
    struct SmithRequestReceiptH42 {subsystems::smithing::source_h42::Source source{};
        std::uint64_t tick=0,life=0,mode=0;int phase=-1;bool pulse=false;} smithReceiptH42[2];
    std::uint64_t smithReceiptSessionH42=0;
    diagnostics::interaction_receipt59c10::Gate smithJoinTraceH42[2];
    guided_crafting55::Alchemy guidedAlchemy;guided_crafting55::Smith guidedSmith;
    guided_crafting55::Status lastGuided=guided_crafting55::Status::Off;
    subsystems::alchemy::input67::State alchemy67;
    subsystems::alchemy::input67::Output lastAlchemy67{};
    subsystems::alchemy::capture_d42::Alchemy alchemyStagesD42;
    subsystems::alchemy::capture_d42::Gate forgeStagesD42;
    subsystems::alchemy::capture_d42::Sources alchemySourcesD42,smithSourcesD42;
    subsystems::alchemy::return67::Request pendingReturn67{};
    std::uint64_t returnedSerial67=0,mortarSent69=0,mortarWithdrawSentD78R2=0;
    std::uint64_t smithInputModeEpochH12=0;bool smithGestureInputH12=false;
    // D83R2 smith: one "why no sideways slide" log line per station visit.
    std::uintptr_t sidewaysNoteInstanceD83R2=0;std::uint64_t sidewaysNoteLifetimeD83R2=0,sidewaysUnknownSinceD83R2=0;
    bool idlePublishedD47=false;
    craft_attempt_w4::Tracker alchemyAttemptsW4,smithAttemptsW4;
    // R8b crafting recorder (F5 rung 3 = Context.whyVisibleW4, only while F5 records). off = one bool test per frame.
    craft_recorder_r8b::Recorder recR8b;
    static void handR8b(craft_recorder_r8b::HandIn& h,const workstation_sample46::Hand& v) noexcept {
        h.valid=v.valid;for(int i=0;i<3;++i)h.pos[i]=v.position[i];for(int i=0;i<4;++i)h.rot[i]=v.rotation[i];h.grip=v.grip;h.trigger=v.trigger;
    }
    void recordR8b(craft_recorder_r8b::Sample& s,const workstation_sample46::Frame& frame,int mode,std::uint64_t now) noexcept {
        namespace cr=craft_recorder_r8b;char line[2048]{};
        const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
        if(recR8b.active&&recR8b.session!=session&&recR8b.stop(now,"F5_SESSION_CHANGED",line,sizeof(line)))diagnostic_trace::Write(line);
        recR8b.newSession(session);
        if(recR8b.active&&recR8b.instance!=s.instance&&recR8b.stop(now,"STATION_CHANGED",line,sizeof(line)))diagnostic_trace::Write(line);
        if(recR8b.begin(s.kind,s.instance,minigame_hands45::label(mode),now,line,sizeof(line)))diagnostic_trace::Write(line);
        if(!recR8b.due(now))return;
        s.mode=minigame_hands45::label(mode);s.headValid=frame.headValid;for(int i=0;i<3;++i)s.head[i]=frame.head[i];
        for(int h=0;h<2;++h)handR8b(s.hand[h],frame.hand[h]);
        cr::HandOut out[2]{};if(recR8b.sample(s,now,line,sizeof(line),out))diagnostic_trace::Write(line);
    }
    void stopR8b(const char* reason,std::uint64_t now) noexcept {
        char line[512]{};if(recR8b.stop(now,reason,line,sizeof(line)))diagnostic_trace::Write(line);
    }
    // W4: one F5_ATTEMPT line per finished squeeze (only while F5 records) + the WHY line on F5 rung 3.
    void emitAttemptW4(const craft_attempt_w4::Tracker& t,const char* kind,bool enabled,int mode,const craft_attempt_w4::Report& r,const char* selector,
                       bool visible,std::uintptr_t instance,std::uint64_t now) noexcept {
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
        namespace ca=craft_attempt_w4;char why[112]{};
        if(!enabled)std::snprintf(why,sizeof(why),"%s %c: mode %s, VR hands do not grab here",kind,r.side?'L':'R',minigame_hands45::label(mode));
        else ca::why(r,kind,selector,why,sizeof(why));
        std::uint64_t accepted=0,refused=0;
        for(unsigned i=0;i<unsigned(subsystems::alchemy::bridge67::Why68::Count);++i){
            const auto v=subsystems::alchemy::bridge67::why68[i].load(std::memory_order_relaxed);if(i)refused+=v;else accepted+=v;}
        char line[720]{};std::snprintf(line,sizeof(line),
            "F5_ATTEMPT kind=%s mode=%s modeActive=%d id=%llu side=%c durationMs=%llu reached=0x%03X firstMissing=%s nominated=%u/%d gap=%.4f native=%u/%d selector=%s selectorCounters=ALCHEMY_ONLY selectorAccepted=%llu selectorRefused=%llu steer=%llu/%llu/%llu timedOut=%d instance=%llX finished=%llu complete=%llu why=\"%s\"",
            kind,minigame_hands45::label(mode),enabled?1:0,(unsigned long long)r.id,r.side?'L':'R',(unsigned long long)(r.endTick-r.startTick),r.reached,
            ca::name(r.firstMissing),r.nominatedEntity,r.nominatedSlot,r.gap,r.nativeEntity,r.nativeSlot,selector?selector:"-",
            (unsigned long long)accepted,(unsigned long long)refused,
            (unsigned long long)subsystems::alchemy::bridge67::steerCandidates68.load(std::memory_order_relaxed), // R8b candidate/forced/filtered
            (unsigned long long)subsystems::alchemy::bridge67::steerForced68.load(std::memory_order_relaxed),
            (unsigned long long)subsystems::alchemy::bridge67::steerFiltered68.load(std::memory_order_relaxed),r.timedOut?1:0,(unsigned long long)instance,
            (unsigned long long)t.finished,
            (unsigned long long)t.complete,why);
        diagnostic_trace::Write(line);
        if(visible&&r.firstMissing!=ca::Stage::Count)(void)visible_failure::publishDiagnosticD74(why,now,diagnostics::capture168::Domain::Bug);
    }
    template<class Pad>
    void apply(const Context& c,const workstation_sample46::Frame& frame,std::uint64_t now,Pad& pad) noexcept {
        const bool f5D42=diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now);
        const auto sessionD42=f5D42?diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load(std::memory_order_acquire):0;
        const auto writeD42=[](const char* line) noexcept {diagnostic_trace::Write(line);};
        if(!f5D42||!c.alchemy)alchemyStagesD42.stop(now,!f5D42?"CAPTURE_ENDED":"LEFT_STATION",writeD42);
        if(!f5D42||!c.smith||(forgeStagesD42.session&&forgeStagesD42.session!=sessionD42)){
            char line[512]{};if(forgeStagesD42.close(now,"SMITH_BELLOWS",!f5D42?"CAPTURE_ENDED":!c.smith?"LEFT_STATION":"SESSION_CHANGED",line,sizeof(line)))writeD42(line);
        }
        // Disabling the mode or leaving native XI publishes revocation too.
        // No pending motion or button can survive in an unserviced mailbox.
        const bool active=c.enabled&&c.qualified&&(!c.guided||guided_crafting55::active(now));
        // Raw left-stick input is copied before native page/workpiece mappings.
        // Rejoin the current typed station here, including the nested book.
        // Published input is a request only; the completed pose owns motion.
        station_reposition_d53::Command moveD53{};
        workstation178r20c1::WorkstationOwnerH51 moveOwnerD53{};
        if(c.enabled&&c.moveQualifiedD53&&!c.guided&&bool(c.alchemy)!=bool(c.smith)&&
           workstation178r20c1::workstationOwnerH51(now,moveOwnerD53)&&
           moveOwnerD53.instance==(c.alchemy?c.alchemy:c.smith)&&moveOwnerD53.lifetime==c.lifetime&&
           moveOwnerD53.kind==unsigned(c.alchemy?minigame_hands45::Alchemy:minigame_hands45::Smithing)&&
           minigame_hands45::modeFor(moveOwnerD53.kind)==minigame_hands45::SixDofVrik&&
           (c.alchemy?c.alchemyTracked:c.smithTracked)&&frame.allowed&&frame.hand[1].valid&&
           workstation_sample46::fresh(frame,now)&&workstation_sample46::finite(frame.hand[1])&&
           std::isfinite(c.moveXD53)&&std::isfinite(c.moveYD53)&&std::fabs(c.moveXD53)<=1.001f&&std::fabs(c.moveYD53)<=1.001f){
            moveD53={moveOwnerD53.instance,frame.poseEpoch,frame.sourceEpoch,moveOwnerD53.modeEpoch,
                moveOwnerD53.stationLifetimeH46,frame.sequence,frame.tick,frame.timeNs,moveOwnerD53.kind,c.moveXD53,c.moveYD53,true};
            pad.lx=pad.ly=0; // station axes cannot also move native iron/selection
        }
        (void)station_reposition_d53::command.publish(moveD53);
        const bool recorderR8b=c.whyVisibleW4&&(c.alchemy||c.smith)&&
            diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now);
        if(recR8b.active&&!recorderR8b)stopR8b(!c.alchemy&&!c.smith?"LEFT_STATION":"RECORDER_OFF",now);
        // Publish revocation once on station exit. Idle gameplay must not copy
        // two native tool tables and run both station adapters every XR tick.
        if(!c.alchemy&&!c.smith){if(idlePublishedD47)return;idlePublishedD47=true;}
        else idlePublishedD47=false;
        const bool guided=active&&c.guided&&!c.gestureAlchemy67&&!c.gestureSmith67&&guided_crafting55::active(now);
        const bool gestureSmithH12=active&&c.gestureSmith67&&c.smithGestures&&c.smithTracked&&c.smith&&!c.alchemy;
        const auto smithModeEpochH12=minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
        if(smithInputModeEpochH12!=smithModeEpochH12||smithGestureInputH12!=gestureSmithH12){
            guidedSmith.reset();smithInputModeEpochH12=smithModeEpochH12;smithGestureInputH12=gestureSmithH12;
        }
        const bool sourceSmithH42=active&&c.smithTracked&&c.smith&&!c.alchemy&&
            minigame_hands45::modeFor(minigame_hands45::Smithing)!=minigame_hands45::Native;
        const bool smithCaptureH42=f5D42;
        const auto smithSessionH42=smithCaptureH42?sessionD42:0;
        if(smithSessionH42!=smithReceiptSessionH42){smithReceiptH42[0]={};smithReceiptH42[1]={};smithReceiptSessionH42=smithSessionH42;smithAttemptsW4.reset();}
        bool forgePumpRTR17=false,instantForgePumpD72=false;
        // Selection must precede attachment. Use the original tracked palms;
        // the attached-tool filter below remains ONLY work/pose authorization.
        {namespace source=subsystems::smithing::source_h42;
            source::Input in{};in.enabled=sourceSmithH42;in.now=now;in.instance=c.smith;
            in.lifetime=c.lifetime;in.modeEpoch=smithModeEpochH12;in.phase=c.smithPhase;
            in.manualA=pad.a;in.cancel=pad.b;
            (void)subsystems::smithing::bridge67::sourcesH42.read(in.frame);
            for(int side=0;side<2;++side){const auto& p=c.world67[side];
                in.palm[side].point={p.point[0],p.point[1],p.point[2]};in.palm[side].grip=frame.hand[side].grip;
                in.palm[side].valid=p.valid&&frame.allowed&&frame.hand[side].valid&&
                    workstation_sample46::fresh(frame,now)&&workstation_sample46::finite(frame.hand[side])&&
                    p.tick&&now>=p.tick&&now-p.tick<=80&&p.poseEpoch==frame.poseEpoch&&p.sourceEpoch==frame.sourceEpoch&&
                    in.frame.playerEntity&&c.worldPlayer67[side]==in.frame.playerEntity;
            }
            source::Trace traceH42{};
            lastSmithSourceH42=smithSourceGripH42.step(in,smithCaptureH42?&traceH42:nullptr);
            // The forge pole has a source entity, not a held hammer Binding.
            // Native A/cancel wins before any pump request; the original hand
            // and copied WORLD identity are used before the tool-only filter.
            source::PumpTraceD42 pumpD42{};
            instantForgePumpD72=sourceSmithH42&&c.smithPhase==8&&subsystems::smithing::instant_d72::route();
            forgePumpRTR17=smithForgePumpR17.step(in,frame,pad.rt,lastSmithSourceH42.a,(f5D42&&c.smith)||instantForgePumpD72?&pumpD42:nullptr);
            subsystems::smithing::instant_d72::Pump semanticPumpD72{};
            semanticPumpD72.handle=in.frame.source[2];semanticPumpD72.lifetime=c.lifetime;
            semanticPumpD72.poseEpoch=frame.poseEpoch;semanticPumpD72.sourceEpoch=frame.sourceEpoch;
            semanticPumpD72.modeEpoch=smithModeEpochH12;semanticPumpD72.sample=frame.sequence;semanticPumpD72.tick=now;
            semanticPumpD72.manual=std::isfinite(pad.rt)&&pad.rt>.10f;
            semanticPumpD72.admitted=instantForgePumpD72&&pumpD42.current&&pumpD42.sampled&&pumpD42.acquired;
            semanticPumpD72.pressed=forgePumpRTR17;
            (void)subsystems::smithing::instant_d72::pump.publish(semanticPumpD72);
            if(f5D42&&c.smith&&sessionD42){
                subsystems::alchemy::capture_d42::Key key{};const auto& handle=in.frame.source[2];
                key.value={c.smith,c.lifetime,in.modeEpoch,frame.poseEpoch,frame.sourceEpoch,handle.entity,handle.holder,
                    unsigned(c.smithPhase),unsigned(handle.geometryType),unsigned(pumpD42.reason),
                    unsigned((pumpD42.current?1:0)|(pumpD42.sampled?2:0)|(pumpD42.edge?4:0)|(pumpD42.acquired?8:0)|
                        (pumpD42.armed?16:0)|(pumpD42.pressed?32:0)|(pumpD42.returned?64:0)),unsigned(forgePumpRTR17)};
                const auto event=forgeStagesD42.observe(sessionD42,now,key);
                if(event){char line[1600]{};
                    if(event==2)std::snprintf(line,sizeof(line),"D42_F5 kind=SMITH_BELLOWS event=BUDGET_EXHAUSTED session=%llu tick=%llu emitted=%llu budget=%u furtherStates=COUNTED_NOT_RECORDED",
                        (unsigned long long)sessionD42,(unsigned long long)now,(unsigned long long)forgeStagesD42.emitted,forgeStagesD42.limit);
                    else std::snprintf(line,sizeof(line),"D42_F5 kind=SMITH_BELLOWS event=STAGE session=%llu serial=%llu tick=%llu instance=%llX life=%llu sample=%llu pose=%llu sourceEpoch=%llu modeEpoch=%llu phase=%d handle=%u holder=%llX geometryType=%u reason=%s sampled=%d sourceCurrent=%d edge=%d acquired=%d armed=%d returned=%d gap=%.6f travel=%.6f relativeTravel=%.6f lastMotion=%llu requestedRT=%d manualRT=%.4f sourceAgeMs=%llu sourcePlayer=%llX palmWorld=%.6f,%.6f,%.6f handleWorld=%.6f,%.6f,%.6f gapKnown=%d nativeHeat=UNOBSERVED",
                        (unsigned long long)sessionD42,(unsigned long long)forgeStagesD42.emitted,(unsigned long long)now,(unsigned long long)c.smith,(unsigned long long)c.lifetime,
                        (unsigned long long)frame.sequence,(unsigned long long)frame.poseEpoch,(unsigned long long)frame.sourceEpoch,(unsigned long long)in.modeEpoch,
                        c.smithPhase,handle.entity,(unsigned long long)handle.holder,unsigned(handle.geometryType),source::pumpNameD42(pumpD42.reason),pumpD42.sampled,pumpD42.current,pumpD42.edge,pumpD42.acquired,pumpD42.armed,pumpD42.returned,
                        pumpD42.gap,double(pumpD42.travel),double(pumpD42.relativeTravel),(unsigned long long)pumpD42.lastMotion,forgePumpRTR17,double(pad.rt),
                        (unsigned long long)diagnostics::interaction_receipt59c10::age(now,in.frame.tick),(unsigned long long)in.frame.player,
                        in.palm[0].point.x,in.palm[0].point.y,in.palm[0].point.z,handle.slotWorld.x,handle.slotWorld.y,handle.slotWorld.z,pumpD42.gap>=0);
                    writeD42(line);
                }
            }
            if(smithCaptureH42)for(int h=0;h<2;++h){auto& receipt=smithReceiptH42[h];
                if(receipt.life!=c.lifetime||receipt.mode!=smithModeEpochH12||receipt.source.instance!=c.smith||
                   !active||in.cancel||in.manualA||(receipt.tick&&(now<receipt.tick||now-receipt.tick>4000)))receipt={};
                const bool pulse=lastSmithSourceH42.a&&lastSmithSourceH42.side==h;
                if(pulse&&!receipt.pulse&&lastSmithSourceH42.slot>=0&&lastSmithSourceH42.slot<4){
                    receipt.source=in.frame.source[unsigned(lastSmithSourceH42.slot)];receipt.tick=now;
                    receipt.life=c.lifetime;receipt.mode=smithModeEpochH12;receipt.phase=c.smithPhase;}
                receipt.pulse=pulse;
            }
            if(c.smith&&smithCaptureH42){
                const auto session=smithSessionH42;
                const bool sourceFresh=in.frame.tick&&now>=in.frame.tick&&now-in.frame.tick<=80;
                const unsigned bits=(in.enabled?1u:0u)|(c.qualified?2u:0u)|(frame.allowed?4u:0u)|(sourceFresh?8u:0u)|
                    (in.palm[0].valid?16u:0u)|(in.palm[1].valid?32u:0u)|(lastSmithSourceH42.a?64u:0u)|(in.manualA?128u:0u)|(in.cancel?256u:0u);
                if(smithTrace50.observe(session,now,c.smith,c.lifetime,frame.hand[0].grip,frame.hand[1].grip,bits)){
                    char line[1400]{};
                    std::snprintf(line,sizeof(line),"C71 F5_CRAFT_INPUT session=%llu tick=%llu kind=SMITHING instance=%llX life=%llu sample=%llu poseEpoch=%llu sourceEpoch=%llu mode=%d phase=%d bits=%u attemptR=%llu attemptL=%llu grip=%.3f,%.3f palmWorldR=%.6f,%.6f,%.6f palmWorldL=%.6f,%.6f,%.6f sourceSequence=%llu sourceAgeMs=%llu requestEntity=%u requestSlot=%d requestSide=%d nativeContextA=%d action=%u sourceCurrent=%d sourceOwner=%d sampledMask=%u squeezeEdgeMask=%u armedMask=%u pendingUntil=%llu sourceFramePhase=%d sourceFrameLife=%llu sourceFrameMode=%llu accepted=UNOBSERVED",
                        (unsigned long long)session,(unsigned long long)now,(unsigned long long)c.smith,(unsigned long long)c.lifetime,
                        (unsigned long long)frame.sequence,(unsigned long long)frame.poseEpoch,(unsigned long long)frame.sourceEpoch,
                        int(minigame_hands45::modeFor(minigame_hands45::Smithing)),c.smithPhase,bits,
                        (unsigned long long)smithTrace50.attempt[0],(unsigned long long)smithTrace50.attempt[1],frame.hand[0].grip,frame.hand[1].grip,
                        in.palm[0].point.x,in.palm[0].point.y,in.palm[0].point.z,in.palm[1].point.x,in.palm[1].point.y,in.palm[1].point.z,
                        (unsigned long long)in.frame.sequence,(unsigned long long)diagnostics::interaction_receipt59c10::age(now,in.frame.tick),
                        lastSmithSourceH42.entity,lastSmithSourceH42.slot,lastSmithSourceH42.side,lastSmithSourceH42.a,unsigned(lastSmithSourceH42.action),traceH42.current,traceH42.owner,traceH42.sampled,traceH42.edge,traceH42.armed,(unsigned long long)traceH42.pulseUntil,in.frame.phase,(unsigned long long)in.frame.lifetime,(unsigned long long)in.frame.modeEpoch);
                    diagnostic_trace::Write(line);
                    for(const auto& tool:in.frame.source){if(tool.slot<0)continue;
                        if(!smithSourcesD42.due(session,now,unsigned(tool.slot),tool.entity,
                            (tool.geometryKnown?1u:0u)|(unsigned(tool.geometryType)<<1)|(unsigned(tool.attached)<<8),c.smith,c.lifetime))continue;
                        const auto right=source::distance(tool,in.palm[0].point);
                        const auto left=source::distance(tool,in.palm[1].point);
                        const auto sourceWorld=(tool.geometryType==source::Geometry::NativeSlotPlacement||tool.geometryType==source::Geometry::ForgeHandle)?tool.slotWorld:
                            source::geometry::Point{tool.bounds.world[3],tool.bounds.world[7],tool.bounds.world[11]};
                        std::snprintf(line,sizeof(line),"C71 F5_CRAFT_SOURCE session=%llu tick=%llu kind=SMITHING instance=%llX life=%llu entity=%u slot=%d side=%d phase=%d holder=%llX attached=%u geometry=%d geometryType=%u geometryEntity=%u contextualAction=%u gapR=%.6f gapL=%.6f sourceWorld=%.6f,%.6f,%.6f boundsLocal=%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                            (unsigned long long)session,(unsigned long long)now,(unsigned long long)c.smith,(unsigned long long)c.lifetime,tool.entity,tool.slot,tool.side,tool.phase,(unsigned long long)tool.holder,unsigned(tool.attached),tool.geometryKnown,unsigned(tool.geometryType),tool.geometryEntity,unsigned(source::action(in.phase,tool.slot)),
                            tool.geometryKnown&&right.valid?right.distance:-1.,tool.geometryKnown&&left.valid?left.distance:-1.,sourceWorld.x,sourceWorld.y,sourceWorld.z,tool.bounds.local[0],tool.bounds.local[1],tool.bounds.local[2],tool.bounds.local[3],tool.bounds.local[4],tool.bounds.local[5]);diagnostic_trace::Write(line);
                    }
                }
            }

        }
        auto smithFrame=frame;bool smithToolsH12[2]{},smithAssistToolD80=false;
        if(c.gestureSmith67||(active&&c.smith&&!c.alchemy&&
           minigame_hands45::modeFor(minigame_hands45::Smithing)==minigame_hands45::SemiPassenger)){
            for(int side=0;side<2;++side){subsystems::smithing::bridge67::NativeTool t{};
                smithToolsH12[side]=subsystems::smithing::bridge67::tools[side].read(t)&&
                    subsystems::smithing::bridge67::qualified(t,now,side,c.smith,c.lifetime,frame.poseEpoch,smithModeEpochH12);
                smithFrame.hand[side].valid=smithFrame.hand[side].valid&&smithToolsH12[side];
                if(side==1)smithAssistToolD80=smithToolsH12[side]&&t.binding.phase==c.smithPhase;
                if(c.smith&&smithTrace50.last==now&&smithCaptureH42){
                    const auto& b=t.binding;char line[1000]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_TOOL session=%llu tick=%llu kind=SMITHING instance=%llX life=%llu side=%d entity=%u tool=%d joined=%d qualified=%d toolAgeMs=%llu holder=%llX attachment=%llX object=%llX key=%llX character=%llX toolPhase=%d poseEpoch=%llu modeEpoch=%llu",
                        (unsigned long long)smithTrace50.session,(unsigned long long)now,(unsigned long long)c.smith,(unsigned long long)c.lifetime,side,b.sourceEntity,b.tool,t.joined,smithToolsH12[side],
                        (unsigned long long)diagnostics::interaction_receipt59c10::age(now,t.tick),(unsigned long long)b.holder,(unsigned long long)b.attachment,(unsigned long long)b.object,(unsigned long long)b.key,(unsigned long long)b.character,b.phase,(unsigned long long)t.poseEpoch,(unsigned long long)t.modeEpoch);diagnostic_trace::Write(line);
                }

            }
        }
        guided_crafting55::AlchemyOutput ga{};guided_crafting55::SmithOutput gs{};
        alchemy_semi46::WorkWitness work{};(void)alchemy_semi46::work.read(work);
        const bool currentWork=work.instance==c.alchemy&&work.lifetime==c.lifetime&&alchemy_semi46::fresh(work.tick,now);
        {namespace craft=subsystems::alchemy;
            craft::input67::Input in{};in.enabled=active&&c.gestureAlchemy67&&c.alchemyTracked&&c.alchemy&&!c.smith;
            in.gestureTransferD59=c.alchemyTransferD59;
            in.qualified=c.qualified;in.now=now;in.nativeA=pad.a;in.nativeBellowsD47=pad.lt>.5f;in.cancel=pad.b;
            in.proximityCraft1=in.enabled||craft::reach_craft1::enabled.load(); // Saved FREE VRIK selects its implemented near-palm policy; no second trial switch
            in.handsOnlyD53=in.enabled&&minigame_hands45::modeFor(minigame_hands45::Alchemy)==minigame_hands45::SixDofVrik;
            in.pickupReachD39=craft::reach_craft1::getPickupReachD39();
            in.controllerMortar70=alchemy_semi46::mortarReady70.load(std::memory_order_acquire);
            in.modeEpoch=minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
            (void)craft::bridge67::completed.read(in.nativeFrame);
            if(in.enabled)(void)craft::bridge67::pestlePresentationD47.read(in.pestlePresentationD47);
            const auto& t=in.nativeFrame.table;
            in.context={c.alchemy,t.player,c.lifetime,frame.poseEpoch,frame.sourceEpoch,in.modeEpoch,c.lifetime};
            in.world=c.world67;in.work=work;in.aimH21=c.aimH21;in.palmRayD70=in.handsOnlyD53;
            (void)craft::mortar69::result.read(in.mortarResult69);
            for(int side=0;side<2;++side){const auto& w=c.world67[side];
                in.palm[side]={{w.point[0],w.point[1],w.point[2]},w.tick,w.poseEpoch,w.sourceEpoch,in.modeEpoch,
                    side,frame.hand[side].grip,w.valid&&c.worldPlayer67[side]==t.playerEntity&&frame.hand[side].valid};
                (void)craft::bridge67::tools[side].read(in.tool[side]);
            }
            alchemy_input19c4::Selection selected{};
            in.nativeSelectedKnown=alchemy_input19c4::lookupSelected(c.alchemy,c.lifetime,now,in.nativeSelected,selected);
            in.nativeSelectionTick=in.nativeSelectedKnown?selected.tick:0; // same recipient/hint transaction, never a second selection read
            alchemy_input19c4::Selection typeOnly{};
            in.nativeSelectedTypeOnly=alchemy_input19c4::selectedTypeOnly(c.alchemy,c.lifetime,now,typeOnly);
            in.nativeTypeOnlyTick=in.nativeSelectedTypeOnly<21u?typeOnly.tick:0;
            in.action={t.instance,t.action,t.lifetime,t.tick,t.actionKnown,t.placeMortar69};
            lastAlchemy67=alchemy67.step(in,frame);
            if(c.alchemy&&f5D42&&sessionD42){
                craft::return67::Result ret{};(void)craft::return67::result.read(ret);
                if(ret.instance!=c.alchemy||ret.lifetime!=c.lifetime)ret={};
                alchemyStagesD42.record(sessionD42,in,lastAlchemy67,alchemy67,frame,ret,writeD42,craft::bridge67::lastWhy68.load(std::memory_order_relaxed));
            }
            if(c.alchemy){ // W4 attempt chain, every frame (cheap); lines only while F5 records
                namespace ca=craft_attempt_w4;const auto& d=lastAlchemy67.nomination.decision;
                const int nomSide=d.source==craft::near67::Source::Left?1:d.source==craft::near67::Source::Right?0:-1;
                const bool sampleFresh=workstation_sample46::fresh(frame,now);
                const bool workNow=work.instance==c.alchemy&&work.lifetime==c.lifetime&&work.parent&&alchemy_semi46::fresh(work.tick,now);
                const int modeW4=int(minigame_hands45::modeFor(minigame_hands45::Alchemy));
                const auto whyW4=subsystems::alchemy::bridge67::lastWhy68.load(std::memory_order_relaxed);
                for(int h=0;h<2;++h){ca::Observation o{};o.grip=frame.hand[h].grip;o.sample=in.enabled&&in.palm[h].valid&&sampleFresh;
                    o.nominated=nomSide==h&&craft::near67::valid(d.ref);o.nominatedEntity=d.ref.entity;o.nominatedSlot=d.ref.slot;o.gap=d.gap;
                    o.nativeKnown=in.nativeSelectedKnown;o.nativeEntity=in.nativeSelected.entity;o.nativeSlot=int(in.nativeSelected.type);
                    if(!o.nativeKnown&&in.nativeSelectedTypeOnly==18u&&in.nativeFrame.table.refs[18].entity){ // R8b mounted bellows
                        o.nativeKnown=true;o.nativeEntity=in.nativeFrame.table.refs[18].entity;o.nativeSlot=18;}
                    const bool mine=lastAlchemy67.admitted&&alchemy67.admittedSide==h;
                    o.dispatched=mine&&lastAlchemy67.use;o.action=mine&&in.action.valid&&in.action.parent!=0;
                    o.attached=mine&&((craft::bridge67::currentToolD67(in.tool[h],in.context,now,h)&&in.tool[h].entity==alchemy67.admittedRef.entity)||
                        lastAlchemy67.logicalIngredientHeldD47);o.work=mine&&workNow;
                    ca::Report r{};if(alchemyAttemptsW4.step(h,o,now,r)){
                        // R8b: say what the native selector did with the hand's item (steering receipts, bridge67)
                        namespace b67=subsystems::alchemy::bridge67;char selW4[80]{};
                        const auto steer=b67::steerWhy68(r.nominatedEntity,b67::steerNominee68.load(std::memory_order_relaxed),
                            b67::steerCandidateTick68.load(std::memory_order_relaxed),b67::steerForcedTick68.load(std::memory_order_relaxed),now);
                        std::snprintf(selW4,sizeof(selW4),"%s%s%s",b67::name(b67::Why68(whyW4)),steer!=b67::Steer68::Unknown?"; ":"",
                            steer!=b67::Steer68::Unknown?b67::name(steer):"");
                        emitAttemptW4(alchemyAttemptsW4,"alchemy",in.enabled,modeW4,r,selW4,c.whyVisibleW4,c.alchemy,now);
                    }
                }
                if(recorderR8b&&!c.smith){ // R8b recorder: alchemy visit sample (formatting only when due)
                    craft_recorder_r8b::Sample s{};s.kind="alchemy";s.instance=c.alchemy;
                    s.stage=alchemy_input19c4::currentStage37(c.alchemy,now);
                    s.nominee=d.ref.entity;s.nomineeSlot=d.ref.slot;s.nomineeSide=nomSide;s.nomineeGap=float(d.gap);
                    s.selected=in.nativeSelected.entity;s.selectedSlot=in.nativeSelectedKnown?int(in.nativeSelected.type):-1;
                    if(!in.nativeSelectedKnown&&in.nativeSelectedTypeOnly<21u){s.selectedSlot=int(in.nativeSelectedTypeOnly);
                        s.selected=in.nativeFrame.table.refs[in.nativeSelectedTypeOnly].entity;}
                    s.admitted=lastAlchemy67.admitted;s.admittedSide=lastAlchemy67.admitted?alchemy67.admittedSide:-1;s.nativeA=lastAlchemy67.use;
                    s.action=in.action.valid?in.action.parent:0;s.workParent=workNow?work.parent:0;
                    const bool slotOk=d.ref.slot>=0&&d.ref.slot<int(craft::near67::Capacity);
                    const auto* offer=slotOk?&in.nativeFrame.offers[unsigned(d.ref.slot)]:nullptr;
                    if(offer&&offer->valid){
                        if(offer->boundsValid){s.nomineePoint[0]=float(offer->bounds.world[3]);s.nomineePoint[1]=float(offer->bounds.world[7]);
                            s.nomineePoint[2]=float(offer->bounds.world[11]);}
                        else{s.nomineePoint[0]=float(offer->point.x);s.nomineePoint[1]=float(offer->point.y);s.nomineePoint[2]=float(offer->point.z);}
                        s.nomineePointValid=true;}
                    for(int h=0;h<2;++h){
                        if(craft::bridge67::currentToolD67(in.tool[h],in.context,now,h)){s.hand[h].held=in.tool[h].entity;s.hand[h].heldSlot=in.tool[h].type;}
                        if(offer&&offer->valid&&in.palm[h].valid){const auto cl=craft::near67::closest(*offer,in.palm[h].point);
                            if(cl.valid)s.hand[h].targetM=float(cl.distance);}
                    }
                    recordR8b(s,frame,modeW4,now);
                }
            }else alchemyAttemptsW4.reset();
            if(c.alchemy&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
                const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
                const auto& n=lastAlchemy67.nomination.decision;
                const unsigned bits=(in.enabled?1u:0u)|(in.qualified?2u:0u)|(frame.allowed?4u:0u)|
                    (workstation_sample46::fresh(frame,now)?8u:0u)|(craft::near67::fresh(t.tick,now)?16u:0u)|
                    (in.nativeSelectedKnown?32u:0u)|(craft::near67::valid(n.ref)?64u:0u)|(lastAlchemy67.admitted?128u:0u)|
                    (lastAlchemy67.use?256u:0u)|(lastAlchemy67.passenger?512u:0u)|(in.nativeA?1024u:0u)|(in.cancel?2048u:0u);
                if(alchemyTrace50.observe(session,now,c.alchemy,c.lifetime,frame.hand[0].grip,frame.hand[1].grip,bits)){
                    char line[1750]{};
                    std::snprintf(line,sizeof(line),"C71 F5_CRAFT_INPUT session=%llu tick=%llu kind=ALCHEMY instance=%llX life=%llu sample=%llu poseEpoch=%llu sourceEpoch=%llu mode=%d bits=%u attemptR=%llu attemptL=%llu grip=%.3f,%.3f palmWorldR=%.6f,%.6f,%.6f palmWorldL=%.6f,%.6f,%.6f palmValid=%d,%d tableSequence=%llu tableAgeMs=%llu nominated=%u nominatedSlot=%d nominatedSource=%u gap=%.6f gripping=%d admissionReady=%d nativeSelected=%u selectedType=%u selectionAgeMs=%llu admitted=%d admittedEntity=%u admittedSide=%d use=%d nativeA=%d actionKnown=%d action=%llX workParent=%llX workChild=%llX workToken=%u workKind=%d status=%u quarantined=%d,%d latched=%d carriedState=%d,%d controlHeld=%d,%d nativeBellowsLT=%d clockAdmission=%d logicalIngredient=%d nativeHandType=%u,%u outcome=NATIVE_ACTION_AND_ATTACHMENT_REQUIRED",
                        (unsigned long long)session,(unsigned long long)now,(unsigned long long)c.alchemy,(unsigned long long)c.lifetime,(unsigned long long)frame.sequence,(unsigned long long)frame.poseEpoch,(unsigned long long)frame.sourceEpoch,
                        int(minigame_hands45::modeFor(minigame_hands45::Alchemy)),bits,(unsigned long long)alchemyTrace50.attempt[0],(unsigned long long)alchemyTrace50.attempt[1],frame.hand[0].grip,frame.hand[1].grip,
                        in.palm[0].point.x,in.palm[0].point.y,in.palm[0].point.z,in.palm[1].point.x,in.palm[1].point.y,in.palm[1].point.z,in.palm[0].valid,in.palm[1].valid,
                        (unsigned long long)t.sequence,(unsigned long long)diagnostics::interaction_receipt59c10::age(now,t.tick),n.ref.entity,n.ref.slot,unsigned(n.source),n.gap,n.gripping,n.admissionReady,in.nativeSelected.entity,unsigned(in.nativeSelected.type),
                        (unsigned long long)diagnostics::interaction_receipt59c10::age(now,in.nativeSelectionTick),lastAlchemy67.admitted,alchemy67.admittedRef.entity,alchemy67.admittedSide,lastAlchemy67.use,in.nativeA,in.action.valid,(unsigned long long)in.action.parent,
                        (unsigned long long)work.parent,(unsigned long long)work.child,work.token,int(work.kind),unsigned(lastAlchemy67.gesture.status),alchemy67.selection.quarantined[0],alchemy67.selection.quarantined[1],alchemy67.selection.latched,alchemy67.ordinaryCarried,alchemy67.mortarPair69,lastAlchemy67.control.held[0],lastAlchemy67.control.held[1],in.nativeBellowsD47,lastAlchemy67.control.clockAdmissionD47,lastAlchemy67.logicalIngredientHeldD47,in.nativeFrame.table.handType68[0],in.nativeFrame.table.handType68[1]);diagnostic_trace::Write(line);
                    // Copied input/native witnesses only; these fields describe the join,
                    // not native acceptance, final placement or recipe outcomes. Formatting
                    // remains inside the existing F5 capture + CraftGate50 admission.
                    bool toolContextJoin[2]{};
                    for(unsigned side=0;side<2;++side){const auto& tool=in.tool[side];const auto& binding=tool.binding;
                        toolContextJoin[side]=tool.joined&&tool.side==int(side)&&tool.instance==c.alchemy&&
                            tool.lifetime==c.lifetime&&tool.poseEpoch==frame.poseEpoch&&craft::near67::fresh(tool.tick,now)&&
                            binding&&binding.instance==tool.instance&&binding.player==t.player&&binding.character==tool.character&&
                            binding.lifetime==tool.lifetime&&binding.entity==tool.entity&&binding.side==int(side)&&binding.type==tool.type&&
                            binding.attachment&&binding.object&&tool.type>=0&&tool.type<int(craft::near67::Capacity)&&
                            t.refs[unsigned(tool.type)].entity==tool.entity;
                    }
                    const bool workContextJoin=currentWork&&work.parent&&work.instance==in.action.instance&&
                        work.lifetime==in.action.lifetime&&in.action.valid&&craft::near67::fresh(in.action.tick,now)&&
                        work.parent==in.action.parent;
                    // Keep this separate from INPUT: the journal retains at most 2048
                    // bytes per record. Both rows carry the same exact sample keys.
                    std::snprintf(line,sizeof(line),"C71 F5_CRAFT_JOIN session=%llu tick=%llu kind=ALCHEMY instance=%llX life=%llu sample=%llu poseEpoch=%llu sourceEpoch=%llu inputRoute=%s frameAllowed=%d headValid=%d frameAgeMs=%llu palmAgeMs=%llu,%llu palmPlayerMatch=%d,%d pendingNativeA=%d passenger=%d presentationRequested=%d controlHeld=%d,%d workPhase=%d workReady=%d workRunning=%d workAgeMs=%llu workContextJoin=%d gestureClockManual=%d gestureCreditSeconds=%.6f toolContextJoin=%d,%d bindingParent=%llX,%llX bindingClass=%llX,%llX bindingObject=%llX,%llX outcome=NATIVE_ACTION_AND_ATTACHMENT_REQUIRED",
                        (unsigned long long)session,(unsigned long long)now,(unsigned long long)c.alchemy,(unsigned long long)c.lifetime,
                        (unsigned long long)frame.sequence,(unsigned long long)frame.poseEpoch,(unsigned long long)frame.sourceEpoch,
                        c.gestureAlchemy67?"FREE_NEAR_PALM":guided?"GUIDED_NATIVE":active?"SEMI_NATIVE":"NATIVE",frame.allowed,frame.headValid,(unsigned long long)diagnostics::interaction_receipt59c10::age(now,frame.tick),
                        (unsigned long long)diagnostics::interaction_receipt59c10::age(now,in.palm[0].tick),(unsigned long long)diagnostics::interaction_receipt59c10::age(now,in.palm[1].tick),
                        in.palm[0].valid&&c.worldPlayer67[0]==t.playerEntity,in.palm[1].valid&&c.worldPlayer67[1]==t.playerEntity,
                        alchemy67.pendingA,lastAlchemy67.passenger,lastAlchemy67.control.trackedPresentation71,lastAlchemy67.control.held[0],lastAlchemy67.control.held[1],
                        work.phase,work.ready,work.working,(unsigned long long)diagnostics::interaction_receipt59c10::age(now,work.tick),workContextJoin,
                        lastAlchemy67.semiControl.manualUse,lastAlchemy67.semiControl.totalSeconds,toolContextJoin[0],toolContextJoin[1],
                        (unsigned long long)in.tool[0].binding.action,(unsigned long long)in.tool[1].binding.action,
                        (unsigned long long)in.tool[0].binding.actionClass,(unsigned long long)in.tool[1].binding.actionClass,
                        (unsigned long long)in.tool[0].binding.object,(unsigned long long)in.tool[1].binding.object);diagnostic_trace::Write(line);
                    for(unsigned i=0;i<craft::near67::Capacity;++i){if(!t.refs[i].entity&&!alchemySourcesD42.identity[i])continue;const auto& offer=in.nativeFrame.offers[i];
                        if(!alchemySourcesD42.due(session,now,i,t.refs[i].entity,
                            (offer.boundsValid?1u:0u)|(offer.valid?2u:0u)|(craft::near67::fresh(offer.tick,now)?4u:0u),c.alchemy,c.lifetime))continue;
                        const auto right=craft::near67::closest(offer,in.palm[0].point),left=craft::near67::closest(offer,in.palm[1].point);
                        std::snprintf(line,sizeof(line),"C71 F5_CRAFT_SOURCE session=%llu tick=%llu kind=ALCHEMY instance=%llX life=%llu entity=%u slot=%u geometry=%d offerValid=%d offerAgeMs=%llu gapR=%.6f gapL=%.6f allowedR=%d allowedL=%d enterR=%.6f enterL=%.6f sourceWorld=%.6f,%.6f,%.6f boundsLocal=%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",
                            (unsigned long long)session,(unsigned long long)now,(unsigned long long)c.alchemy,(unsigned long long)c.lifetime,t.refs[i].entity,i,offer.boundsValid,offer.valid,(unsigned long long)diagnostics::interaction_receipt59c10::age(now,offer.tick),
                            offer.boundsValid&&right.valid?right.distance:-1.,offer.boundsValid&&left.valid?left.distance:-1.,
                            in.proximityCraft1?craft::reach_craft1::allowed(i,0):craft::input67::sourceHandAllowedH5(i,0),in.proximityCraft1?craft::reach_craft1::allowed(i,1):craft::input67::sourceHandAllowedH5(i,1),
                            in.proximityCraft1?(in.handsOnlyD53?craft::reach_craft1::handsRadiusD53(i,0):n.sourceAcquisitionD39?craft::reach_craft1::pickupRadiusD39(i,0,in.pickupReachD39):craft::reach_craft1::radius(i,0))+craft::reach_craft1::Tolerance:craft::near67::Enter,in.proximityCraft1?(in.handsOnlyD53?craft::reach_craft1::handsRadiusD53(i,1):n.sourceAcquisitionD39?craft::reach_craft1::pickupRadiusD39(i,1,in.pickupReachD39):craft::reach_craft1::radius(i,1))+craft::reach_craft1::Tolerance:craft::near67::Enter,offer.bounds.world[3],offer.bounds.world[7],offer.bounds.world[11],offer.bounds.local[0],offer.bounds.local[1],offer.bounds.local[2],offer.bounds.local[3],offer.bounds.local[4],offer.bounds.local[5]);diagnostic_trace::Write(line);
                    }
                    for(unsigned side=0;side<2;++side){const auto& tool=in.tool[side];
                        std::snprintf(line,sizeof(line),"C71 F5_CRAFT_TOOL session=%llu tick=%llu kind=ALCHEMY instance=%llX life=%llu side=%u entity=%u type=%d joined=%d toolInstance=%llX toolLife=%llu toolAgeMs=%llu toolParent=%llX character=%llX poseEpoch=%llu nativeHandType=%u handTypesKnown=%d",
                            (unsigned long long)session,(unsigned long long)now,(unsigned long long)c.alchemy,(unsigned long long)c.lifetime,side,tool.entity,tool.type,tool.joined,(unsigned long long)tool.instance,(unsigned long long)tool.lifetime,
                            (unsigned long long)diagnostics::interaction_receipt59c10::age(now,tool.tick),(unsigned long long)tool.parent,(unsigned long long)tool.character,(unsigned long long)tool.poseEpoch,t.handType68[side],t.handTypesKnown68);diagnostic_trace::Write(line);
                    }
                }
            }

            if(lastAlchemy67.mortarStop69.serial&&
               (lastAlchemy67.mortarStop69.serial!=mortarSent69||lastAlchemy67.mortarStop69.withdrawTickD78R2!=mortarWithdrawSentD78R2)&&
               craft::mortar69::request.publish(lastAlchemy67.mortarStop69)){
                mortarSent69=lastAlchemy67.mortarStop69.serial;mortarWithdrawSentD78R2=lastAlchemy67.mortarStop69.withdrawTickD78R2;
                craft::mortar69::interested.store(true,std::memory_order_release);
            }
            (void)craft::bridge67::nomination.publish(lastAlchemy67.nomination);
            (void)craft::bridge67::control.publish(lastAlchemy67.control);
            if(lastAlchemy67.gesture.status==craft::gesture67::Status::ReleaseAwaitingNativeReturn&&
               work.kind==alchemy_semi46::Resource&&alchemy67.admittedTick&&alchemy67.admittedTick!=returnedSerial67){
                const auto& tool=in.tool[1];
                if(!pendingReturn67.serial&&tool.joined&&tool.binding&&tool.binding.action==work.parent&&
                   tool.instance==c.alchemy&&tool.lifetime==c.lifetime&&tool.poseEpoch==frame.poseEpoch&&
                   craft::near67::fresh(tool.tick,now)){
                    pendingReturn67={tool.binding,work.child,work.token,alchemy67.admittedTick,now,frame.poseEpoch,frame.sourceEpoch,in.modeEpoch};
                }
                if(pendingReturn67.serial&&craft::return67::request.publish(pendingReturn67)){
                    returnedSerial67=pendingReturn67.serial;pendingReturn67={};craft::return67::interested.store(true,std::memory_order_release);
                }
            }
            if(!in.enabled)pendingReturn67={};
        }
        ga=guidedAlchemy.step(guided&&c.alchemyTracked&&c.alchemy&&!c.smith,c.alchemy,c.lifetime,frame,now,c.touch,pad.a,pad.b,currentWork?work.parent:0,c.alchemyGaze63);
        // One existing flip FSM supplies native X in both guided and Gesture
        // Smithing. semi_smith46 owns axes/hammer RT/work-clock; the exact
        // phase8 forge-handle gesture separately supplies native forge-bag RT.
        gs=guidedSmith.step((guided||gestureSmithH12)&&c.smithTracked&&c.smith&&!c.alchemy,c.smith,c.lifetime,c.smithPhase,
            c.gestureSmith67?smithFrame:frame,now,pad.a,pad.b,pad.x,c.gestureSmith67?smithToolsH12:nullptr);
        lastGuided=(guided||gestureSmithH12)?(c.alchemy?ga.status:gs.status):guided_crafting55::Status::Off;
        int workHand=ga.side;
        if(currentWork&&work.kind==alchemy_semi46::Resource)workHand=1;
        if(currentWork&&(work.kind==alchemy_semi46::Mortar||work.kind==alchemy_semi46::Stir))workHand=0;
        if(c.gestureAlchemy67){
            (void)alchemy_semi46::control.publish(lastAlchemy67.semiControl);
            if(lastAlchemy67.use){if(lastAlchemy67.control.type==18)pad.lt=1.f;else pad.a=true;}
        }else alchemy_semi46::publish(frame,active&&c.alchemyTracked&&c.alchemy&&!c.smith&&(!guided||(ga.admitted&&ga.clockEligible)),
                               c.alchemy,c.lifetime,pad.a||(currentWork&&work.kind==alchemy_semi46::Bellows&&pad.lt>.5f),pad.b,guided?workHand:-1,!guided||ga.work,!guided);
        semi_smith46::Context sc{active&&c.smithTracked&&c.smithGestures&&!c.alchemy,c.qualified,
            semi_smith46::clockReady.load(std::memory_order_acquire),c.smith,c.lifetime,c.smithPhase,
            minigame_hands45::modeFor(minigame_hands45::Smithing)!=minigame_hands45::Native};
        // Saved tracked modes already require the exact native held tool.
        // F5 recording/guidance must not add the retired native-wrist gap to a free hammer.
        if(guided&&!sc.nativeHeight){smithFrame.hand[0].valid=smithFrame.hand[0].valid&&gs.right;smithFrame.hand[1].valid=smithFrame.hand[1].valid&&gs.left;}
        // Station6DOF: hand motion is the work source, never the locomotion
        // stick. Native phase/tool joins above remain mandatory. Consume the
        // physical stick only for this exact active station mode; no change to
        // Native/Semi/Free controls or unrelated gameplay.
        const bool stationSmithH41=gestureSmithH12&&
            int(minigame_hands45::modeFor(minigame_hands45::Smithing))==3;
        if(stationSmithH41)pad.lx=pad.ly=0.f;
        lastSmith=smith.step(sc,smithFrame,now,pad.lx,pad.ly,pad.rt);
        semi_smith46::clockInterested.store((!sc.nativeHeight&&sc.enabled&&sc.nativeControls&&sc.instance)||
            lastSmith.clock.fallD81.request!=0,std::memory_order_release);
        const bool smithAssistD80=subsystems::smithing::assist_d80::held(
            crafting_d67::smithSemiAnchors.load(std::memory_order_acquire),
            int(minigame_hands45::modeFor(minigame_hands45::Smithing)),c.smithPhase,
            sc.enabled&&sc.nativeControls&&smithAssistToolD80&&smithFrame.allowed&&
                smithFrame.hand[1].valid&&workstation_sample46::fresh(smithFrame,now)&&
                workstation_sample46::finite(smithFrame.hand[1]),smithFrame.hand[1].trigger);
        if(smithAssistD80){
            // D83R1: held LT keeps the native workpiece movement active. The
            // final wrist follows that CURRENT native tool result, so the
            // visible workpiece and native hammer evaluation stay aligned.
            // The existing fore/aft hand policy below still excludes hand X;
            // this never adds a separate lateral presentation offset.
            pad.lt=0.f; // this exact held-LT anvil request cannot also be native LT
        }
        if(c.smith&&!c.alchemy&&smithCaptureH42){ // captured request and actual native binding remain distinct
            namespace ca=craft_attempt_w4;
            const int modeW4=int(minigame_hands45::modeFor(minigame_hands45::Smithing));
            for(int h=0;h<2;++h){const auto& receipt=smithReceiptH42[h];const auto& requested=receipt.source;
                subsystems::smithing::bridge67::NativeTool native{};
                const bool requestedNow=receipt.tick&&receipt.life==c.lifetime&&receipt.mode==smithModeEpochH12&&requested.instance==c.smith;
                const bool nativeJoined=subsystems::smithing::bridge67::tools[h].read(native)&&
                    subsystems::smithing::bridge67::qualified(native,now,h,c.smith,c.lifetime,frame.poseEpoch,smithModeEpochH12);
                const auto& binding=native.binding;
                const bool exactSource=requestedNow&&nativeJoined&&native.tick>=receipt.tick&&
                    binding.sourceEntity==requested.entity&&binding.holder==requested.holder&&binding.model==requested.model&&
                    (requested.slot>=2||binding.key==requested.key);
                const bool phaseJoined=exactSource&&binding.phase==c.smithPhase;
                const bool phaseChanged=phaseJoined&&c.smithPhase!=receipt.phase;
                const bool nativeWork=phaseJoined&&(c.smithPhase==14||c.smithPhase==16||c.smithPhase==17);
                ca::Observation o{};o.grip=frame.hand[h].grip;
                o.sample=sourceSmithH42&&frame.hand[h].valid&&workstation_sample46::fresh(frame,now);
                o.nominated=requestedNow;o.nominatedEntity=requested.entity;o.nominatedSlot=requested.slot;
                o.nativeKnown=exactSource;o.nativeEntity=exactSource?binding.sourceEntity:0;o.nativeSlot=exactSource?requested.slot:-1;
                // A was emitted earlier; a fresh exact binding can acknowledge selection later.
                o.dispatched=requestedNow;o.action=phaseChanged;o.attached=exactSource;o.work=nativeWork;
                ca::Report report{};if(smithAttemptsW4.step(h,o,now,report))emitAttemptW4(smithAttemptsW4,"smithing",sourceSmithH42,modeW4,report,"EXACT_NATIVE_BINDING",c.whyVisibleW4,c.smith,now);
                const unsigned bits=(requestedNow?1u:0u)|(nativeJoined?2u:0u)|(exactSource?4u:0u)|
                    (phaseJoined?8u:0u)|(phaseChanged?16u:0u)|(nativeWork?32u:0u)|(lastSmith.requestRT?64u:0u)|(lastSmith.axesOwned?128u:0u);
                if(requestedNow&&smithJoinTraceH42[h].observe(smithSessionH42,now,{c.smith,requested.entity,h,int(bits)},100,4096,500)){
                    char line[1100]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_JOIN kind=SMITHING session=%llu tick=%llu side=%d requestTick=%llu requestEntity=%u requestSlot=%d requestPhase=%d phaseNow=%d nativeEntity=%u nativeTool=%d nativePhase=%d nativeAge=%llu bits=%u nativeSourceMatch=%d nativePhaseChanged=%d nativeWorkPhase=%d heightRTInput=%d axesInput=%d result=%s",
                        (unsigned long long)smithSessionH42,(unsigned long long)now,h,(unsigned long long)receipt.tick,requested.entity,requested.slot,receipt.phase,c.smithPhase,
                        binding.sourceEntity,binding.tool,binding.phase,(unsigned long long)diagnostics::interaction_receipt59c10::age(now,native.tick),bits,exactSource,phaseChanged,nativeWork,lastSmith.requestRT,lastSmith.axesOwned,
                        phaseChanged?"MATCHING_BINDING_AND_PHASE_OBSERVED":exactSource?"MATCHING_BINDING_OBSERVED":"NATIVE_RESULT_NOT_YET_OBSERVED");diagnostic_trace::Write(line);
                }
            }
            if(recorderR8b){const auto& s=lastSmithSourceH42; // R8b recorder: smithing visit sample
                craft_recorder_r8b::Sample rs{};rs.kind="smithing";rs.instance=c.smith;rs.smithPhase=c.smithPhase;
                rs.nominee=s.entity;rs.nomineeSlot=s.slot;rs.nomineeSide=s.side;rs.nativeA=s.a;
                rs.axisX=lastSmith.x;rs.axisY=lastSmith.y;rs.axesOwned=lastSmith.axesOwned;
                for(int h=0;h<2;++h)if(smithToolsH12[h]){rs.hand[h].held=1;rs.hand[h].heldSlot=-1;} // tool joined (entity not kept here)
                recordR8b(rs,frame,modeW4,now);
            }
        }else smithAttemptsW4.reset();
        {
            namespace sd=subsystems::smithing::stroke_receipt_d57;
            sd::Input d{};
            d.capture=f5D42&&c.smith; // the existing finite F5 Interaction lease
            if(d.capture){
                d.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load(std::memory_order_acquire);
                d.eligible=sc.enabled&&sc.nativeControls;d.sampleValid=smithFrame.allowed&&smithFrame.hand[0].valid&&workstation_sample46::fresh(smithFrame,now);
                d.instance=c.smith;d.lifetime=c.lifetime;d.pose=smithFrame.poseEpoch;d.source=smithFrame.sourceEpoch;
                d.modeEpoch=minigame_hands45::modeEpoch67.load(std::memory_order_acquire);d.mode=int(minigame_hands45::modeFor(minigame_hands45::Smithing));
                d.sample=smithFrame.sequence;d.sampleTick=smithFrame.tick;d.now=now;d.phase=c.smithPhase;d.nativeHeight=sc.nativeHeight;
                d.start=lastSmith.strokeStarted&&lastSmith.reason==semi_smith46::Reason::RaiseRequest;
                d.release=lastSmith.strokeReleased&&lastSmith.reason==semi_smith46::Reason::FinishRequest;
                using R=semi_smith46::Reason;
                d.abort=lastSmith.reason==R::ManualStroke||lastSmith.reason==R::Tracking||lastSmith.reason==R::Rebased||
                    lastSmith.reason==R::Discontinuity||lastSmith.reason==R::PressNotAccepted||lastSmith.reason==R::FinishNotAccepted||
                    lastSmith.reason==R::ClockNotOwned||lastSmith.reason==R::Inactive||lastSmith.reason==R::NativeComplete;
            }
            lastSmith.clock.diagnosticD57=smithStrokeD57.step(d);
        }
        (void)semi_smith46::clockControl.publish(lastSmith.clock);
        if(!smithAssistD80||!lastSmith.axesOwned)sidewaysUnknownSinceD83R2=0; // D83R2 smith
        if((guided||gestureSmithH12)&&(!stationSmithH41||kcd2vr::crafting_d67::smithSemiAnchors.load(std::memory_order_acquire))&&lastSmith.axesOwned){
            // Guided and Free Smithing own fore/aft only. A native secondary axis
            // for axes/horseshoes remains on the real stick, never sideways reach.
            // D83R2 smith: except while LT is held at the anvil on a workpiece
            // whose own native map has a second axis (axe, horseshoe). Then the
            // left hand's sideways rate feeds native lx exactly like fore/aft
            // feeds ly; the native mover clamps it and the held-LT placement
            // follows the native result. Blades (MapSize y=0) stay fore/aft only.
            namespace sb=subsystems::smithing::bridge67;
            int sideways=-1;const char* why="NOT_READ";
            if(smithAssistD80){
                sb::WorkpieceMapD83R2 map{};
                if(sb::workpieceMapD83R2.read(map))sideways=sb::sidewaysD83R2(map,c.smith,c.lifetime,why);
                else why="NOT_READ";
                // The tool and map are published by the same callback; only a
                // map still missing after 300 ms of held LT is reported.
                if(sideways>=0)sidewaysUnknownSinceD83R2=0;
                else if(!sidewaysUnknownSinceD83R2||now<sidewaysUnknownSinceD83R2)sidewaysUnknownSinceD83R2=now;
                if(sideways<0&&now-sidewaysUnknownSinceD83R2>=300&&
                   (sidewaysNoteInstanceD83R2!=c.smith||sidewaysNoteLifetimeD83R2!=c.lifetime)){
                    char note[visible_failure::kMaxText+1]{};
                    std::snprintf(note,sizeof(note),"Smithing: no sideways hand slide, workpiece type unknown (%s)",why);
                    visible_failure::publishLogOnlyD80(note,now);
                    // The shared log limiter can refuse inside 2 s; retry next frame.
                    if(visible_failure::lastLogPublishD80.load(std::memory_order_acquire)==now){
                        sidewaysNoteInstanceD83R2=c.smith;sidewaysNoteLifetimeD83R2=c.lifetime;}
                }
            }
            if(!(smithAssistD80&&sideways==1)&&std::fabs(pad.lx)<=.20f)lastSmith.x=0.f;
        }
        semi_smith46::apply(lastSmith,pad);
        if(forgePumpRTR17&&!instantForgePumpD72)pad.rt=std::max(pad.rt,1.f);
        // D72's exact process callback runs the actual bellows property leaf;
        // the physical request cannot also queue the legacy trigger child.
        if(guided)pad.a=pad.a||ga.use;
        pad.a=pad.a||lastSmithSourceH42.a; // one native xi_a press/hold/release, not a second direct action
        if(guided||gestureSmithH12)pad.x=pad.x||gs.flip;
        if(active&&c.alchemyTracked&&c.alchemy&&!c.smith)
            alchemy_semi46::apply(now,pad.a,pad.lt);
        if((c.alchemy||c.smith)&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
            static diagnostics::interaction_receipt59c10::Gate padTrace;
            const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
            const auto& strokeD57=lastSmith.clock.diagnosticD57;
            const auto& fallD84=lastSmith.clock.fallD81;
            const int bits=(pad.a?1:0)|(pad.b?2:0)|(pad.x?4:0)|(lastSmith.clock.enabled?8:0)|
                (strokeD57.releaseTick?16:0)|(int(strokeD57.id)<<5);
            if(padTrace.observe(session,now,{c.alchemy?c.alchemy:c.smith,unsigned(c.lifetime),c.smithPhase,bits},0,16384,500)){
                char line[1900]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_OUTPUT session=%llu tick=%llu instance=%llX life=%llu active=%d qualified=%d guided=%d gestureAlchemy=%d gestureSmith=%d a=%d b=%d x=%d rt=%.5f lx=%.5f ly=%.5f smithReason=%s smithClockEnabled=%d smithClockRequest=%llu raiseWork=%.6f finishWork=%.6f strokeIdD57=%llu strokeProducer=%s strokeMode=%d strokeStart=%llu strokeRelease=%llu strokeSample=%llu "
                    "fallReceiptD84=%llu fallNativePhase=%d fallReleased=%llu fallSample=%llu fallFresh=%d smithLTNativeAlign=%d nativeConsumption=UNOBSERVED",
                    (unsigned long long)session,(unsigned long long)now,(unsigned long long)(c.alchemy?c.alchemy:c.smith),(unsigned long long)c.lifetime,active,c.qualified,guided,c.gestureAlchemy67,c.gestureSmith67,
                    pad.a,pad.b,pad.x,pad.rt,pad.lx,pad.ly,semi_smith46::name(lastSmith.reason),lastSmith.clock.enabled,(unsigned long long)lastSmith.clock.request,lastSmith.clock.raiseWork,lastSmith.clock.finishWork,
                    (unsigned long long)strokeD57.id,strokeD57.id?(strokeD57.nativeHeight?"VR_NATIVE_HEIGHT":"VR_WORK_CLOCK"):"NONE_NATIVE_OR_MANUAL",strokeD57.mode,
                    (unsigned long long)strokeD57.startTick,(unsigned long long)strokeD57.releaseTick,(unsigned long long)strokeD57.sample,
                    (unsigned long long)fallD84.request,fallD84.phase,(unsigned long long)fallD84.released,(unsigned long long)fallD84.sequence,
                    subsystems::smithing::fall_d81::fresh(fallD84,now),smithAssistD80);diagnostic_trace::Write(line);
            }
        }

    }
    void revoke(std::uint64_t now) noexcept {
        struct Pad {bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;} pad;
        apply(Context{},workstation_sample46::Frame{},now,pad);
    }
};
}
