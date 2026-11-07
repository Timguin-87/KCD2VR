// Included in action_time_owner59c11's private owner namespace. D72 qualifies a
// genuine native director admission, omits only its authored child, then runs
// the class's typed semantic helper at the next exact Henry controller callback.
// Native completion remains AFTER director installation and its caller's final
// notification. Never synthesize an event or complete inside concrete Start.
namespace ai72=kcd2vr::subsystems::alchemy::instant_d72;
namespace ab72=kcd2vr::subsystems::alchemy::bridge67;
namespace ip72=kcd2vr::instant_d72;
using AlchemyDirectorD72=bool(__fastcall*)(void*,void*,int,bool);
using AlchemyStartD72=bool(__fastcall*)(void*);
struct AlchemyClassD72 {std::uintptr_t vft,start,end;ai72::Action action;};
static constexpr AlchemyClassD72 kAlchemyClassesD72[]{
    {0x3A5CCD0,0x2E15B78,0x2E15F4C,ai72::Action::TakeObject},
    {0x3C23010,0x2E15A24,0x2E15E64,ai72::Action::ReturnObject},
    {0x3A5C308,0x6C5B48,0x6C57CC,ai72::Action::TakeIngredient},
    {0x3B87A38,0x2E158AC,0x2E15E64,ai72::Action::ReturnIngredient},
    {0x3A5C4C0,0x6C59E8,0x6C56C4,ai72::Action::PlaceIngredient},
    {0x3F60940,0x2E15718,0x6C56C4,ai72::Action::PlaceBowl},
    {0x3A5BB90,0x6C3AC0,0xA97910,ai72::Action::PourMortarD75}
};
static AlchemyDirectorD72 g_alchemy_director_d72=nullptr;
static AlchemyStartD72 g_alchemy_start_d72[7]{};
static std::atomic<bool> g_alchemy_instant_ready_d72{false},g_alchemy_instant_stop_d72{true};
static std::atomic<unsigned> g_alchemy_instant_inflight_d72{0};
static std::uint64_t g_alchemy_instant_serial_d72=0;
struct AlchemyInstantOwnerD72 {
    ai72::Receipt receipt{};ab72::Control input{};
    std::uintptr_t player=0,manager=0,controller=0,vft=0;
    int selected=-1,handBefore=-1,sourceCount=0,destinationCount=0,counter=0;
    unsigned mortarEntityD75=0,pestleEntityD75=0;
    bool active=false,admitted=false,attempted=false,completionAttempted=false;
    bool semanticReturnedD80=false;std::uint64_t semanticTickD80=0;
};
static AlchemyInstantOwnerD72 g_alchemy_instant_d72{};
static thread_local AlchemyInstantOwnerD72* g_alchemy_start_scope_d72=nullptr;
struct AlchemyInstantFlightD72 {
    AlchemyInstantFlightD72(){g_alchemy_instant_inflight_d72.fetch_add(1,std::memory_order_acq_rel);}
    ~AlchemyInstantFlightD72(){g_alchemy_instant_inflight_d72.fetch_sub(1,std::memory_order_acq_rel);}
};
static bool AlchemyCurrentSlotD72(std::uintptr_t instance,std::uintptr_t& parent) noexcept {
    auto* p=reinterpret_cast<void*>(instance);const auto first=Rd<std::uintptr_t>(p,0xB0,0),last=Rd<std::uintptr_t>(p,0xB8,0);
    parent=0;return first&&last>=first+16&&last-first<=256&&!((last-first)%8)&&
        SehCopy(reinterpret_cast<void*>(first+8),&parent,sizeof(parent));
}
static bool AlchemyContentsD72(std::uintptr_t instance,int type,int& count) noexcept {
    count=0;if(type<0||type>20)return false;
    auto* p=reinterpret_cast<void*>(instance+0x470+24*type);
    const auto begin=Rd<std::uintptr_t>(p,0,~std::uintptr_t(0)),end=Rd<std::uintptr_t>(p,8,0),cap=Rd<std::uintptr_t>(p,16,0);
    if(end<begin||cap<end||end-begin>1024||(end-begin)%8||cap-begin>4096||(!begin&&(end||cap)))return false;
    count=int((end-begin)/8);return true;
}
static bool AlchemyInstantIdentityD72(const AlchemyInstantOwnerD72& s,bool installed) noexcept {
    namespace act=kcd2vr::activity_input163;const auto& w=act::witness175;const auto& r=s.receipt;
    const auto b=g_base.load(std::memory_order_acquire);auto* instance=reinterpret_cast<void*>(r.instance);
    auto* player=reinterpret_cast<void*>(s.player);auto* ctrl=reinterpret_cast<void*>(s.controller);
    std::uintptr_t current=0;
    if(!b||!retail_build112::Ready(b)||!w.ready.load()||w.transitionDepth175.load()||
       w.lifetime175.load()!=r.lifetime||w.active[unsigned(act::Kind175::Alchemy)].load()!=r.instance||
       !ctrl||ctrl!=g_henry_ctrl.load()||!IsHenryController(ctrl)||
       Rd<std::uintptr_t>(instance,0,0)!=b+0x3F60B08||Rd<unsigned char>(instance,0x2C,0)!=1||
       Rd<std::uintptr_t>(instance,0x18,0)!=s.player||Rd<std::uintptr_t>(player,0,0)!=b+0x3AA4B98||
       !Rd<unsigned>(player,0x30,0)||Rd<unsigned>(player,0x30,0)!=Rd<unsigned>(ctrl,kCtrlEntityId,0)||
       Rd<std::uintptr_t>(player,0x9D0,0)!=s.manager||Rd<std::uintptr_t>(reinterpret_cast<void*>(s.manager),0,0)!=b+0x3EC9958||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(s.manager),0x38,0)!=s.player||
       Rd<std::uintptr_t>(instance,0x78,0)!=b+0x3BFA5B0||!AlchemyCurrentSlotD72(r.instance,current)||
       (installed?current!=r.parent:current!=0))return false;
    auto* parent=reinterpret_cast<void*>(r.parent);
    if(Rd<std::uintptr_t>(parent,0,0)!=s.vft||Rd<std::uintptr_t>(parent,0x58,0)!=r.instance||
       Rd<std::uintptr_t>(parent,0x70,1)!=0)return false;
    if(installed&&(Rd<std::uintptr_t>(parent,0x40,0)!=r.instance+0x78||
       Rd<int>(parent,0x3C,-1)!=1||Rd<unsigned char>(parent,0x28,0)!=1))return false;
    return true;
}
// Same exact retail held-entity helper already used by ordinary instant
// attachment readback. Mortar logically occupies right type2 while its actual
// props are entity_left2 and entity_right3; never equate those two layouts.
static bool AlchemyMortarHeldD75(const AlchemyInstantOwnerD72& s) noexcept {return HostMortarHeldD75(s);}
static bool AlchemyInstantSourceD72(const AlchemyInstantOwnerD72& s,std::uint64_t now) noexcept {
    const auto& r=s.receipt;ab72::Control c{};kcd2vr::workstation_sample46::Frame f{};
    const int gestureSideR93=s.input.plateBindingR93?s.input.plateGestureSideR93:r.side;
    if(gestureSideR93<0||gestureSideR93>1)return false;
    if(!ab72::control.read(c)||!c.enabled||c.instance!=r.instance||c.lifetime!=r.lifetime||
       c.admissionTickR22!=r.admission||c.poseEpoch!=r.poseEpoch||c.sourceEpoch!=r.sourceEpoch||
       c.modeEpoch!=r.modeEpoch||c.type!=s.input.type||c.side!=s.input.side||!c.tick||now<c.tick||now-c.tick>80||
       (c.passenger&&!c.trackedPresentation71&&!(r.action==ai72::Action::PourMortarD75&&c.heldMortarPourD75))||c.entity!=s.input.entity||
       r.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load()||
       !kcd2vr::workstation178r20c1::fullVrAlchemy63(now)||
       !kcd2vr::frik_pose_probe::WorkstationFrame46(f)||!f.allowed||
       !kcd2vr::workstation_sample46::fresh(f,now)||f.poseEpoch!=r.poseEpoch||f.sourceEpoch!=r.sourceEpoch||
       !f.hand[gestureSideR93].valid||!kcd2vr::workstation_sample46::finite(f.hand[gestureSideR93]))return false;
    if(s.input.plateBindingR93){ab72::NativeTable table{};ab72::NativeTool held{},other{};
        if(!kcd2vr::alchemy_tool18::same(s.input.plateBindingR93,c.plateBindingR93)||
           !ab72::table.read(table)||!ab72::tools[0].read(held)||!ab72::tools[1].read(other)||
           !ab72::plateControlCurrentR93(c,table,held,other,now,r.poseEpoch))return false;}
    auto* instance=reinterpret_cast<void*>(r.instance);
    if(r.action==ai72::Action::PourMortarD75&&(!c.heldMortarPourD75||
       Rd<unsigned>(instance,0x158,0)!=s.mortarEntityD75||Rd<unsigned>(instance,0x15C,0)!=s.pestleEntityD75||!AlchemyMortarHeldD75(s)))return false;
    return Rd<unsigned>(instance,0x150+4*s.input.type,0)==c.entity&&Rd<int>(instance,0x304,-1)==s.selected;
}
static bool AlchemyInstantPrepareD72(void* director,void* reference,int slot,AlchemyInstantOwnerD72& s) noexcept {
    if(slot!=1||!director||!reference||!ip72::Crafting()||g_alchemy_instant_stop_d72.load()||
       !g_alchemy_instant_ready_d72.load()||g_alchemy_instant_d72.active||g_owner.state!=Idle||
       g_semi_alchemy46.written||g_alchemy_stage19c4.written)return false;
    ab72::Control c{};if(!ab72::control.read(c)||!c.enabled||!c.admissionTickR22||c.side<0||c.side>1||c.type<0||c.type>12||
       !c.entity||reinterpret_cast<std::uintptr_t>(director)!=c.instance+0x78)return false;
    s={};s.input=c;s.receipt.instance=c.instance;s.receipt.parent=Rd<std::uintptr_t>(reference,0,0);
    if(!s.receipt.parent)return false;
    s.vft=Rd<std::uintptr_t>(reinterpret_cast<void*>(s.receipt.parent),0,0);const auto b=g_base.load();
    for(const auto& k:kAlchemyClassesD72)if(s.vft==b+k.vft){s.receipt.action=k.action;break;}
    if(!ai72::semanticOnlyAllowed(s.receipt.action))return false;
    auto& r=s.receipt;r.lifetime=c.lifetime;r.admission=c.admissionTickR22;r.poseEpoch=c.poseEpoch;
    r.sourceEpoch=c.sourceEpoch;r.modeEpoch=c.modeEpoch;r.source=c.type;r.side=c.side;
    s.player=Rd<std::uintptr_t>(reinterpret_cast<void*>(c.instance),0x18,0);
    s.manager=Rd<std::uintptr_t>(reinterpret_cast<void*>(s.player),0x9D0,0);
    s.controller=reinterpret_cast<std::uintptr_t>(g_henry_ctrl.load());
    auto* instance=reinterpret_cast<void*>(c.instance);auto* parent=reinterpret_cast<void*>(r.parent);
    s.selected=Rd<int>(instance,0x304,-1);r.destination=s.selected;
    if(r.action==ai72::Action::PourMortarD75){
        if(!c.heldMortarPourD75||(c.type!=2&&c.type!=3))return false;
        r.source=2;r.side=1;s.mortarEntityD75=Rd<unsigned>(instance,0x158,0);s.pestleEntityD75=Rd<unsigned>(instance,0x15C,0);
    }
    s.handBefore=Rd<int>(instance,0x2A0+4*r.side,-1);
    if(!AlchemyInstantIdentityD72(s,false)||!AlchemyInstantSourceD72(s,GetTickCount64()))return false;
    // D75: the recorded left flask uses the same native Take/ReturnObject
    // helpers. Its ActionEnd668 flags only admit replacement while the child
    // parent remains installed; normal completion clears them and empties slot1.
    // Retain that exact completion/readback, never fabricate the event or flags.
    const bool object=r.source==0||r.source==1||r.source==4||(r.source==5&&r.side==1);
    const bool ingredient=r.source>=7&&r.source<=12;
    const bool destination=s.selected==0||s.selected==2||s.selected==3||s.selected==6;
    switch(r.action){
    case ai72::Action::TakeObject:
        return object&&s.selected==r.source&&s.handBefore==21&&Rd<int>(parent,0x50,-1)==r.side;
    case ai72::Action::ReturnObject:
        return object&&s.handBefore==r.source&&Rd<int>(parent,0x50,-1)==r.side&&
            Rd<unsigned char>(parent,0x54,2)==unsigned(r.side==1)&&Rd<unsigned char>(parent,0x55,2)==unsigned(r.side==0);
    case ai72::Action::TakeIngredient:
        if(!ingredient||r.side!=0||s.selected!=r.source||s.handBefore!=21||
           !AlchemyContentsD72(r.instance,r.source,s.sourceCount)||s.sourceCount)return false;
        s.counter=Rd<int>(instance,r.source<=9?0x368:0x380,-1);return s.counter>=0&&s.counter<100000;
    case ai72::Action::ReturnIngredient:
        if(!ingredient||r.side!=0||s.handBefore!=r.source||
           !AlchemyContentsD72(r.instance,r.source,s.sourceCount)||s.sourceCount!=1)return false;
        s.counter=Rd<int>(instance,r.source<=9?0x368:0x380,-1);return s.counter>0&&s.counter<100000;
    case ai72::Action::PlaceIngredient:
        // R90 (R88 craft): instant omits the authored child, and with it the CAF
        // effect event (herb_to_pot: WH_Particels.other.herbs_throw, D66 log).
        // Into the pot (slot 6) the native clip plays instead, sped by the D81
        // transfer clock like mortar_to_bowl. PlaceBowl is already native (R2).
        if(s.selected==6)return false;
        return ingredient&&r.side==0&&s.handBefore==r.source&&destination&&s.selected!=r.source&&
            AlchemyContentsD72(r.instance,r.source,s.sourceCount)&&s.sourceCount>0&&
            AlchemyContentsD72(r.instance,s.selected,s.destinationCount);
    case ai72::Action::PourMortarD75:
        // Genuine PlaceMortar admission, retained pair, and a current held tilt.
        // The native Use branch transfers contents only; release/home is later.
        return s.selected==6&&Rd<int>(instance,0x2A0,-1)==2&&s.handBefore==21&&
            AlchemyContentsD72(r.instance,2,s.sourceCount)&&s.sourceCount>0&&
            AlchemyContentsD72(r.instance,6,s.destinationCount);
    case ai72::Action::PlaceBowl:
        // The exact pot route needs no mortar-resource child attachment.
        return r.source==0&&r.side==0&&s.handBefore==0&&s.selected==6&&
            AlchemyContentsD72(r.instance,0,s.sourceCount)&&s.sourceCount>0&&
            AlchemyContentsD72(r.instance,6,s.destinationCount);
    default:return false;
    }
}
template<unsigned Index> static bool __fastcall AlchemyStartDetourD72(void* parent) noexcept {
    AlchemyInstantFlightD72 flight;auto* s=g_alchemy_start_scope_d72;
    if(s&&!s->admitted&&s->receipt.parent==reinterpret_cast<std::uintptr_t>(parent)&&
       s->receipt.action==kAlchemyClassesD72[Index].action&&g_owner_transaction_depth19c4&&
       Rd<std::uintptr_t>(parent,0,0)==s->vft&&Rd<std::uintptr_t>(parent,0x58,0)==s->receipt.instance&&
       Rd<std::uintptr_t>(parent,0x40,0)==s->receipt.instance+0x78&&Rd<int>(parent,0x3C,-1)==1&&
       Rd<std::uintptr_t>(parent,0x70,1)==0){s->admitted=true;return true;}
    return g_alchemy_start_d72[Index]?g_alchemy_start_d72[Index](parent):false;
}
static bool __fastcall AlchemyDirectorDetourD72(void* director,void* reference,int slot,bool resume) noexcept {
    AlchemyInstantFlightD72 flight;auto original=g_alchemy_director_d72;if(!original)return false;
    OwnerTransaction19c4 transaction;AlchemyInstantOwnerD72 candidate{};
    if(!transaction||g_alchemy_start_scope_d72||!AlchemyInstantPrepareD72(director,reference,slot,candidate))
        return original(director,reference,slot,resume);
    auto* previous=g_alchemy_start_scope_d72;g_alchemy_start_scope_d72=&candidate;
    const bool accepted=original(director,reference,slot,resume);g_alchemy_start_scope_d72=previous;
    if(accepted&&candidate.admitted){
        candidate.active=true;candidate.receipt.serial=++g_alchemy_instant_serial_d72;
        candidate.receipt.tick=GetTickCount64();g_alchemy_instant_d72=candidate;
        ip72::Publish(ip72::Family::Crafting,ip72::State::Active);
    }
    return accepted;
}
static bool AlchemyInvokeSemanticD72(const AlchemyInstantOwnerD72& s) noexcept {return HostSemanticD75(s);}
static bool AlchemyHeldReadbackD72(const AlchemyInstantOwnerD72& s,bool held) noexcept {return HostOrdinaryHeldD75(s,held);}
static bool AlchemyEffectReadbackD72(const AlchemyInstantOwnerD72& s) noexcept {
    auto* instance=reinterpret_cast<void*>(s.receipt.instance);const auto& r=s.receipt;
    const int hand=Rd<int>(instance,0x2A0+4*r.side,-1);int source=-1,destination=-1;
    switch(r.action){
    case ai72::Action::TakeObject:return hand==r.source&&AlchemyHeldReadbackD72(s,true);
    case ai72::Action::ReturnObject:return hand==21&&AlchemyHeldReadbackD72(s,false);
    case ai72::Action::TakeIngredient:
        return hand==r.source&&AlchemyContentsD72(r.instance,r.source,source)&&source==s.sourceCount+1&&
            Rd<int>(instance,r.source<=9?0x368:0x380,-1)==s.counter+1&&(r.source<=9||AlchemyHeldReadbackD72(s,true));
    case ai72::Action::ReturnIngredient:
        return hand==21&&AlchemyContentsD72(r.instance,r.source,source)&&!source&&
            Rd<int>(instance,r.source<=9?0x368:0x380,-1)==s.counter-1&&AlchemyHeldReadbackD72(s,false);
    case ai72::Action::PourMortarD75:
        return hand==21&&Rd<int>(instance,0x2A0,-1)==2&&AlchemyMortarHeldD75(s)&&
            AlchemyContentsD72(r.instance,2,source)&&!source&&
            AlchemyContentsD72(r.instance,6,destination)&&destination==s.destinationCount+s.sourceCount;
    case ai72::Action::PlaceIngredient:case ai72::Action::PlaceBowl:
        return hand==21&&AlchemyContentsD72(r.instance,r.source,source)&&!source&&
            AlchemyContentsD72(r.instance,r.destination,destination)&&destination==s.destinationCount+s.sourceCount;
    default:return false;
    }
}
static bool AlchemyCompleteD72(const AlchemyInstantOwnerD72& s,bool success) noexcept {return HostCompleteD75(s,success);}
// A semantic helper may queue its exact attachment update. Waiting here
// does not repeat that helper, freeze presentation, or extend tracking grace.
static bool AlchemyAttachmentPendingCurrentD80(const AlchemyInstantOwnerD72& s,std::uint64_t now) noexcept {
    const auto& r=s.receipt;
    if(!s.semanticReturnedD80||!s.semanticTickD80||now<s.semanticTickD80||now-s.semanticTickD80>125||
       !kcd2vr::subsystems::alchemy::presentation_d81::attachmentAction(unsigned(r.action),r.source,r.side)||
       g_alchemy_instant_stop_d72.load()||r.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load()||
       !kcd2vr::workstation178r20c1::fullVrAlchemy63(now))return false;
    kcd2vr::workstation_sample46::Frame f{};
    const int gestureSideR93=s.input.plateBindingR93?s.input.plateGestureSideR93:r.side;
    if(gestureSideR93<0||gestureSideR93>1)return false;
    // Do not require the old pre-take selection/control state: the successful
    // semantic helper itself changes it. Actor/station identity remains checked
    // by AlchemyInstantIdentityD72 before every service of this receipt.
    return kcd2vr::frik_pose_probe::WorkstationFrame46(f)&&f.allowed&&
        kcd2vr::workstation_sample46::fresh(f,now)&&f.poseEpoch==r.poseEpoch&&f.sourceEpoch==r.sourceEpoch&&
        f.hand[gestureSideR93].valid&&kcd2vr::workstation_sample46::finite(f.hand[gestureSideR93])&&
        Rd<unsigned>(reinterpret_cast<void*>(r.instance),0x150+4*r.source,0)==s.input.entity;
}
static void TickInstantAlchemyD72(void* ctrl,std::uint64_t now) noexcept {
    if(!OnAlchemyUpdateOwnerR27()||ctrl!=g_henry_ctrl.load()||!g_owner_transaction_depth19c4)return;
    // Native content/attachment observers can synchronously re-enter this owner.
    // Do not complete or retire a parent while its original semantic helper is
    // still executing; the outer call retains the sole once-only receipt.
    static thread_local bool servicingD75=false;if(servicingD75)return;
    struct ServiceD75 {bool& value;ServiceD75(bool& v):value(v){value=true;}~ServiceD75(){value=false;}} serviceD75(servicingD75);
    auto& s=g_alchemy_instant_d72;
    if(!s.active){
        const auto instance=kcd2vr::activity_input163::witness175.active[0].load();
        if(instance){std::uintptr_t parent=0;
            // A known class is not proof that this source/action was admitted.
            // Without our live receipt an occupied native parent is fallback,
            // including unsupported types and otherwise refused acquisitions.
            const bool idle=AlchemyCurrentSlotD72(instance,parent)&&parent==0;
            ip72::Publish(ip72::Family::Crafting,!ip72::Crafting()?ip72::State::Native:
                idle&&g_alchemy_instant_ready_d72.load()?ip72::State::Available:ip72::State::Unsupported);
        }
        return;
    }
    if(!AlchemyInstantIdentityD72(s,true)){
        const auto& w=kcd2vr::activity_input163::witness175;std::uintptr_t current=0;
        // Failed/unreadable validation is not retirement. Keep the no-child
        // cleanup obligation until lifecycle or the current slot proves it ended.
        if(w.lifetime175.load()!=s.receipt.lifetime||w.active[0].load()!=s.receipt.instance||
           (AlchemyCurrentSlotD72(s.receipt.instance,current)&&current!=s.receipt.parent))s={};
        else ip72::Publish(ip72::Family::Crafting,ip72::State::Unsupported);
        return;
    }
    if(s.completionAttempted)return; // unknown partial native completion can never be repeated
    if(!s.attempted){
        const bool source=AlchemyInstantSourceD72(s,now)&&!g_alchemy_instant_stop_d72.load();
        s.attempted=true;s.receipt.entered=source; // consume before any reentrant native helper
        s.semanticTickD80=now;
        s.semanticReturnedD80=source&&AlchemyInvokeSemanticD72(s);
        s.receipt.semanticExecutedD80=s.semanticReturnedD80;
        s.receipt.effect=s.semanticReturnedD80&&AlchemyEffectReadbackD72(s);
    }else if(s.semanticReturnedD80&&!s.receipt.effect){
        // Observe only. A delayed native attachment is never a second take.
        s.receipt.effect=AlchemyEffectReadbackD72(s);
    }
    if(!s.receipt.effect&&AlchemyAttachmentPendingCurrentD80(s,now))return;
    s.receipt.attachmentWaitMsD80=s.semanticTickD80&&now>=s.semanticTickD80?now-s.semanticTickD80:0;
    // Disabling the preference after admission does not abandon the native
    // parent. A source loss/stop completes failure through its original End.
    ip72::Publish(ip72::Family::Crafting,ip72::State::Retiring);s.completionAttempted=true;
    const auto saved=s;const bool returned=AlchemyCompleteD72(saved,saved.receipt.effect);
    std::uintptr_t parent=0;const bool retired=returned&&AlchemyCurrentSlotD72(saved.receipt.instance,parent)&&parent!=saved.receipt.parent;
    auto receipt=saved.receipt;receipt.tick=now;receipt.completed=retired;(void)ai72::receipt.publish(receipt);
    (void)ai72::PublishSoundD80(receipt,saved.player,saved.controller);
    // One row at the existing once-only completion boundary. Gate first: no
    // extra readback, broad polling, per-frame formatting or feedback replay.
    namespace captureD79=kcd2vr::diagnostics::capture168;
    if(captureD79::active(captureD79::Domain::Interaction,now))
        ai72::writeOutcomeD79(receipt,captureD79::gate.session[unsigned(captureD79::Domain::Interaction)].load(std::memory_order_acquire),
            [](const char* line){kcd2vr::diagnostic_trace::Write(line);});
    if(retired)s={};
    ip72::Publish(ip72::Family::Crafting,retired?(ip72::Crafting()?ip72::State::Available:ip72::State::Native):ip72::State::Unsupported);
}
static void AlchemyFeedbackServiceD80(const native_script_t2::Context&,const kcd2vr::subsystems::alchemy::feedback_d80::Event&) noexcept;
