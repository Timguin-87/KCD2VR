#pragma once
#include "ui_contextual_hud162r1.h"
#include "ui_hud_visibility175.h"

namespace kcd2vr::ui_hud_transition170 {
using namespace ui_contextual_hud162r1;
inline constexpr unsigned Preferences=HideCrosshair163|HideCompass163|HideStatus163|HideLower163;
struct Request {bool contextual=false;unsigned hidden=0;std::uint64_t lifecycle=0;bool weaponQam=false,foodQam=false,corrected=false,vitals=false,compass54=false,cursor54=false;};
struct Report {bool changed=false;unsigned reads=0,writes=0,failures=0,absent=0;};
inline Output VisibleWhileLooking(const Input& input,Output gaze){
    gaze.left=gaze.left&&input.headValid&&Looking(input,0,true);
    gaze.right=gaze.right&&input.headValid&&Looking(input,1,true);
    return gaze; // immediate dismissal outside the retained gaze cone
}
// Worker-owned. No retained native pointers, draw hook, timer or per-frame Flash
// calls. Failed/absent clips retry only on a new requested state/lifecycle event.
template<class Backend>
auto NativeMeaning175(Backend&io,unsigned i,std::uintptr_t id,bool current,bool& native,int)
    -> decltype(io.NativeVisibility175(i,id,current,native),bool()) {
    return io.NativeVisibility175(i,id,current,native);
}
template<class Backend>
bool NativeMeaning175(Backend&,unsigned,std::uintptr_t,bool,bool&,long){return false;}
template<class Backend>
auto NativeRestricted175(Backend&io,unsigned i,int)->decltype(io.NativeRestriction175(i),bool()){
    return io.NativeRestriction175(i);
}
template<class Backend>bool NativeRestricted175(Backend&,unsigned,long){return false;}
class Controller {
    struct Entry {std::uintptr_t id=0;bool base=true,owned=false;};
    std::array<Entry,Clips.size()> entries_{};
    Request last_{};std::uint64_t generation_=0;bool latched_=false;
public:
    bool Needs(Request request,std::uint64_t observedGeneration) const noexcept {
        if(request.corrected){request.weaponQam=false;request.foodQam=false;}
        return !(latched_&&generation_==observedGeneration&&last_.contextual==request.contextual&&
           last_.hidden==request.hidden&&last_.lifecycle==request.lifecycle&&
           last_.weaponQam==request.weaponQam&&last_.foodQam==request.foodQam&&last_.corrected==request.corrected&&last_.vitals==request.vitals&&last_.compass54==request.compass54&&last_.cursor54==request.cursor54);
    }
    template<class Backend> Report Update(Backend& io,Request request,std::uint64_t observedGeneration){
        Report r{};
        if(!Needs(request,observedGeneration))return r;
        if(request.corrected){request.weaponQam=false;request.foodQam=false;}
        r.changed=true;latched_=true;last_=request;
        if(!io.Acquire()){generation_=observedGeneration;++r.failures;return r;}
        const auto actualGeneration=io.Generation();
        if(generation_!=actualGeneration)entries_={};
        generation_=actualGeneration;
        for(unsigned i=0;i<Clips.size();++i){
            const auto selectedGroup=ui_hud_visibility175::ExperimentGroup36(i,request.corrected,request.contextual);
            auto group=selectedGroup;
            if(!group&&entries_[i].owned)group=Other; // release a formerly controlled selector/cursor
            if(!group)continue; // Hints, names and dialogue are not visibility owners
            auto&e=entries_[i];
            const bool controlled=selectedGroup&&(request.contextual||(group&Preferences));
            if(!controlled&&!e.owned)continue;
            std::uintptr_t id=0;bool current=false;
            if(!io.Resolve(i,Clips[i].name,id)){e={};++r.absent;continue;} // optional native absence
            ++r.reads;if(!io.Get(i,current)){++r.failures;continue;}
            if(e.id!=id){e={id,current,false};}
            if(!controlled&&!e.owned)continue;
            const bool qamOpen=(i==3&&request.weaponQam)||(i==4&&request.foodQam);
            // A user permission is never a native instruction to SHOW a clip.
            // Only restore visibility we previously hid, and only to the captured native baseline.
            // A QAM opening is owned by the game; it must not reveal a manually hidden group.
            (void)qamOpen;
            const bool hide=controlled && (request.hidden&group)!=0;
            bool native=current;
            const bool nativeKnown=NativeMeaning175(io,i,id,e.owned?e.base:current,native,0);
            if(nativeKnown){e.base=native;e.owned=e.owned||NativeRestricted175(io,i,0);}
            else if(!e.owned&&hide)e.base=current;
            const bool pin=!hide&&((request.vitals&&i==1)||(request.compass54&&i==0)||(request.cursor54&&i==19));
            const bool target=pin?true:(hide?false:(e.owned?e.base:current));
            if(current!=target){
                ++r.writes;bool read=false;
                if(!io.Set(i,target)||!io.Get(i,read)||read!=target){
                    ++r.failures;++r.writes;
                    const bool back=io.Set(i,current);e.owned=e.owned||!back;
                    continue;
                }
            }
            e.owned=hide||pin; // explicit vital display has scoped ownership; other permissions stay passive
        }
        io.Release();return r;
    }
    template<class Backend> void Restore(Backend&io){
        if(io.Acquire()){
            if(io.Generation()==generation_)for(unsigned i=0;i<Clips.size();++i)if(entries_[i].owned){
                std::uintptr_t id=0;
                if(io.Resolve(i,Clips[i].name,id)&&id==entries_[i].id){
                    bool native=entries_[i].base;
                    (void)NativeMeaning175(io,i,id,entries_[i].base,native,0);
                    io.Set(i,native);
                }
            }
            io.Release();
        }
        entries_={};latched_=false;
    }
};
}
