#pragma once
#include "ui_native_canvas.h"
namespace kcd2vr::ui_prompt_scale120r3 {
// Independent semantic child owners. No visibility or whole-root writes.
// Position follows the native animation; only local scale is adjusted.
template<class B> auto ChildRevision(B& io,unsigned i,int) noexcept -> decltype(io.Revision(i)){return io.Revision(i);}
template<class B> std::uint64_t ChildRevision(B&,unsigned,long) noexcept{return 0;}
template<std::size_t Count> class NamedTransaction {
    struct Entry {std::uintptr_t id=0;ui_native_canvas::Geometry base{},last{};bool owned=false,repair=false;std::uint64_t revision=0;};
    std::array<Entry,Count> entries_{};std::uint64_t generation_=0;
    std::array<const char*,Count> names_{};bool retainMissing_=false,scaleOnly_=false;
public:
    struct Report {
        unsigned resolved=0,written=0,verified=0;
        std::array<ui_native_canvas::Geometry,Count> before{},target{},after{};
    };
    const Report& LastReport() const noexcept {return report_;}
    explicit NamedTransaction(std::array<const char*,Count> names,bool retainMissing=false,bool scaleOnly=false) noexcept : names_(names),retainMissing_(retainMissing),scaleOnly_(scaleOnly) {}
private:
    Report report_{};
    bool Same(ui_native_canvas::Geometry a,ui_native_canvas::Geometry b) const noexcept {
        return scaleOnly_?(std::fabs(a.xs-b.xs)<.02f&&std::fabs(a.ys-b.ys)<.02f):ui_native_canvas::Same(a,b);
    }
public:
    bool Active() const noexcept {for(const auto& e:entries_)if(e.owned)return true;return false;}
    template<class Backend> bool Run(Backend& io,std::uint64_t generation,bool enabled,const std::array<float,Count>& scales,unsigned work=(1u<<Count)-1u) noexcept {
        if(generation_!=generation){entries_={};generation_=generation;}
        bool ok=true;report_={};
        for(unsigned i=0;i<Count;++i){
            if(!(work&(1u<<i)))continue;
            auto&e=entries_[i];std::uintptr_t id=0;ui_native_canvas::Geometry now{};
            if(!io.Resolve(i,names_[i],id)||!id||!io.Get(i,now)){if(!retainMissing_)e={};continue;} // bounded retry retains a notification baseline only
            report_.resolved|=1u<<i;report_.before[i]=report_.after[i]=now;
            const auto revision=ChildRevision(io,i,0);
            if(revision&&e.revision!=revision){e={};e.revision=revision;}
            if(!ui_native_canvas::Valid(now)||!std::isfinite(scales[i])||scales[i]<.25f||scales[i]>1.5f){ok=false;continue;}
            const bool ours=e.owned&&id==e.id&&(e.repair||(std::fabs(now.xs-e.last.xs)<.02f&&std::fabs(now.ys-e.last.ys)<.02f));
            if(scaleOnly_&&e.owned&&id==e.id&&!e.repair){
                // A native setter may reset only ONE axis. Keep the other
                // axis's unscaled baseline instead of shrinking it twice.
                const bool oursX=std::fabs(now.xs-e.last.xs)<.02f;
                const bool oursY=std::fabs(now.ys-e.last.ys)<.02f;
                if(!oursX)e.base.xs=now.xs;if(!oursY)e.base.ys=now.ys;
                e.base.x=now.x;e.base.y=now.y;e.base.rotation=now.rotation;
            }else if(!ours){e.base=now;e.id=id;e.owned=false;e.repair=false;}
            else {e.base.x=now.x;e.base.y=now.y;} // native animation can move a scaled child
            auto target=e.base;
            if(enabled){target.xs*=scales[i];target.ys*=scales[i];}
            report_.target[i]=target;
            if(!Same(now,target)){
                ui_native_canvas::Geometry read{};
                const bool wrote=io.Set(i,target);if(wrote)report_.written|=1u<<i;
                if(!wrote||!io.Get(i,read)||!Same(read,target)){
                    ui_native_canvas::Geometry back{};
                    if(!io.Set(i,now)||!io.Get(i,back)||!Same(back,now)){e.last=target;e.owned=true;e.repair=true;}report_.after[i]=back;ok=false;continue;
                }
                report_.after[i]=read;
            }
            report_.verified|=1u<<i;
            e.last=target;e.owned=enabled;e.repair=false;
        }
        return ok;
    }
};
// Keep the existing control/help owner and its public API unchanged.
class Transaction : public NamedTransaction<2> {
public:
    Transaction() noexcept : NamedTransaction({"Hints","TutorialMessage"},true,true) {}
    template<class Backend> bool Run(Backend& io,std::uint64_t generation,bool enabled,float hints,float controls) noexcept {
        return NamedTransaction::Run(io,generation,enabled,{hints,controls});
    }
};
// D59R1: halve the prior three .60 notification scales, and halve the
// previously unscaled message families. Exact HUD.xml aliases only; no whole
// cccursor/HUD, subtitle, hint, tutorial or full-field mask ownership.
class Notifications : public NamedTransaction<6> {
public:
    inline static constexpr std::array<const char*,6> Names{{"InfoText","GameLog","CommonEvent","FancyEvent","SkillCheck","ItemTransfer"}};
    inline static constexpr std::array<const char*,6> Paths{{"cccursor.infoText","tr.gameLog","tr.commonEvent","tc.fancyEvent","bc.skillCheck","bl.itemTransfer"}};
    inline static constexpr std::array<float,6> Scales{{.30f,.30f,.30f,.50f,.50f,.50f}};
    inline static constexpr unsigned All=(1u<<Names.size())-1u;
    Notifications() noexcept : NamedTransaction(Names,true,true) {}
    template<class Backend> bool Run(Backend& io,std::uint64_t generation,bool enabled,unsigned work=All) noexcept {
        return NamedTransaction::Run(io,generation,enabled,Scales,work);
    }
    // Use only x/y scale flags. Native animation owns position, alpha, visible,
    // rotation and timing, including during rollback/restoration.
    static auto PackScale(ui_native_canvas::Geometry value) noexcept {
        auto bytes=ui_native_canvas::Pack(value);const std::uint16_t flags=0x18;
        std::memcpy(bytes.data()+0x34,&flags,sizeof(flags));return bytes;
    }
};
// D69: exact world-item prompt children, controlled by the existing Controls
// size slider. Their projected parent and combat cursor remain native-owned.
class ActionCursorD69 : public NamedTransaction<2> {
public:
    inline static constexpr std::array<const char*,2> Names{{"CursorOutline","CursorCross"}};
    ActionCursorD69() noexcept : NamedTransaction(Names,true,true) {}
    template<class Backend> bool Run(Backend& io,std::uint64_t generation,bool enabled,float controls) noexcept {
        return NamedTransaction::Run(io,generation,enabled,{controls,controls});
    }
};
// R145 (Miki 8 Nov: "the yellow messages are again too big (like 'you cleaned yourself' etc. or the F2 messages)"):
// those lines are InfoText (cccursor.infoText, Game.SendInfoText). cccursor is a world-projected parent, so the
// readable HUD owner never scales it (ui_native_canvas Anchors) and no slider reached the message line: its only size
// was the D63 half-size container in the companion hud.gfx. now the line follows the "Interaction / menu icons"
// slider (VrUiHintsScale), on top of that authored size, never bigger than it (100% = the old size). scale only:
// the game keeps position, alpha, visibility and timing. the other 5 popup containers stay as they were.
class InfoTextR145 : public NamedTransaction<1> {
public:
    inline static constexpr std::array<const char*,1> Names{{"InfoText"}};
    static float Clamp(float scale) noexcept {return std::isfinite(scale)?std::clamp(scale,.25f,1.f):1.f;}
    InfoTextR145() noexcept : NamedTransaction(Names,true,true) {}
    template<class Backend> bool Run(Backend& io,std::uint64_t generation,bool enabled,float scale) noexcept {
        return NamedTransaction::Run(io,generation,enabled,{Clamp(scale)});
    }
};

}
