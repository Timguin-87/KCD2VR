#include "stereo_resource_model_v6603.h"
#include "stereo_eye_control_model_v6604.h"
#include <cassert>
#include <cstdio>
#include <cstring>

namespace pf=kcd2vr::preflight_model;
namespace rm=kcd2vr::resource_model;
namespace ec=kcd2vr::eye_control_model;
constexpr std::uintptr_t wh=0x180000000ull;
struct Fixture {
    pf::Snapshot s{};rm::Evidence e{};
    Fixture() {
        s.pipeline=0x10000;s.crv=0x20000;s.output=0x30000;s.resourceOwner=s.pipeline;
        s.frame=31;s.selector=0;s.phase=1;s.builtCount=1;
        s.qualified=s.ownerMatches=s.stateRead=s.outputRead=true;
        e.ownerRead=true;e.resourcePipeline=s.pipeline;
        e.rt.renderer=0x40000;e.rt.threadObject=0x50000;
        e.rt.readable=e.rt.matches=e.rt.stable=true;
        for(unsigned eye=0;eye<2;++eye) {
            auto& t=s.velocity[eye];auto& m=e.texture[eye];auto& v=e.view[eye];
            t.object=0x60000+eye*0x1000;t.backing=0x80000+eye*0x1000;
            t.views=0xA0000+eye*0x1000;t.width=eye?1513:1317;t.height=eye?1619:1410;
            t.objectRead=t.backingRead=t.dimensionsRead=t.viewsRead=t.stable=true;
            t.status=pf::ResourceStatus::BasicFieldsPresent;
            m.vtable=wh+rm::kTextureVtableRva;m.namePointer=0xC0000+eye*0x1000;
            m.flags=rm::kRightInitialFlags|1u;m.customId=-1;m.depth=1;m.type=1;m.mipCount=1;
            m.sourceFormat=m.deviceFormat=rm::kVelocityFormat;
            m.readable=m.stable=m.referencesRead=true;m.nativeReferences=2;
            std::snprintf(m.name,sizeof(m.name),"%s",eye?"$VelocityObjects_R_main":"$VelocityObjects_main");
            v.backingVtable=wh+rm::kDeviceTextureVtableRva;
            v.begin=t.views;v.end=v.capacity=v.begin+48;v.count=3;
            v.rtv=0xE0000+eye*0x1000;v.rtvKey=2;
            v.typeRead=v.typeStable=v.headerRead=v.bounded=v.entryRead=v.stable=true;
        }
        e.texture[0].flags=0x20041121u;
    }
};

int main() {
    const Fixture before;
    assert(rm::NeedsBackedResize(before.s));
    assert(rm::RejectBackedResize(before.s,before.e,wh)==nullptr);
    assert(rm::SameResizePrerequisites(before.s,before.e,before.s,before.e));
    // The old stereo gate must keep refusing this real recorded size mismatch.
    assert(std::strcmp(ec::RejectPersistentResources(before.s,before.e,wh,1),"velocity-size")==0);

    Fixture after=before;
    after.s.velocity[1].width=1317;after.s.velocity[1].height=1410;
    after.s.velocity[1].backing+=0x100;after.s.velocity[1].views+=0x100;
    after.e.view[1].begin+=0x100;after.e.view[1].end+=0x100;
    after.e.view[1].capacity+=0x100;after.e.view[1].rtv+=0x100;
    assert(rm::PreparedBackedResize(before.s,before.e,after.s,after.e,wh));
    assert(ec::RejectPersistentResources(after.s,after.e,wh,1)==nullptr);
    assert(!rm::NeedsBackedResize(after.s));
    assert(rm::RejectBackedResize(after.s,after.e,wh)!=nullptr);
    assert(!rm::PreparedBackedResize(before.s,before.e,before.s,before.e,wh));

    // Native allocators may reuse wrapper/descriptor addresses; dimensions and
    // ownership still have to pass. Pointer churn alone is not the acceptance.
    Fixture reused=before;reused.s.velocity[1].width=1317;reused.s.velocity[1].height=1410;
    assert(rm::PreparedBackedResize(before.s,before.e,reused.s,reused.e,wh));

    const auto refuse=[&](auto change) {
        Fixture f=before;change(f);assert(rm::RejectBackedResize(f.s,f.e,wh)!=nullptr);
    };
    refuse([](auto& f){f.s.phase=2;});
    refuse([](auto& f){f.s.selector=1;});
    refuse([](auto& f){f.s.builtCount=2;});
    refuse([](auto& f){f.s.ownerMatches=false;});
    refuse([](auto& f){f.s.scheduleEnabled=1;});
    refuse([](auto& f){f.s.output=0;});
    refuse([](auto& f){f.e.rt.matches=false;});
    refuse([](auto& f){f.e.rt.loadingMode=1;});
    refuse([](auto& f){f.s.velocity[1].backing=0;f.s.velocity[1].status=pf::ResourceStatus::NoBacking;});
    refuse([](auto& f){f.s.velocity[1].object=f.s.velocity[0].object;});
    refuse([](auto& f){f.s.velocity[1].backing=f.s.velocity[0].backing;});
    refuse([](auto& f){f.e.view[1].rtv=f.e.view[0].rtv;});
    refuse([](auto& f){f.e.view[1].backingVtable+=8;});
    refuse([](auto& f){f.e.view[1].typeStable=false;});
    refuse([](auto& f){f.e.view[1].stable=false;});
    refuse([](auto& f){f.e.texture[1].flags=f.e.texture[0].flags;});
    refuse([](auto& f){f.e.texture[1].customId=3;});
    refuse([](auto& f){f.e.texture[1].sourceFormat=0;});
    refuse([](auto& f){f.e.texture[1].deviceFormat=0;});
    refuse([](auto& f){f.e.texture[1].clearBits[0]=1;});
    refuse([](auto& f){f.e.texture[1].mipCount=2;});
    refuse([](auto& f){std::strcpy(f.e.texture[1].name,"$VelocityObjects_R_other");});
    refuse([](auto& f){f.s.velocity[0].width=0;});

    const auto rejectPost=[&](auto change) {
        Fixture f=after;change(f);
        assert(!rm::PreparedBackedResize(before.s,before.e,f.s,f.e,wh));
    };
    rejectPost([](auto& f){f.s.velocity[0].width+=1;});
    rejectPost([](auto& f){f.s.velocity[0].backing+=8;});
    rejectPost([](auto& f){f.e.texture[0].flags^=0x20;});
    rejectPost([](auto& f){f.e.view[0].rtv+=8;});
    rejectPost([](auto& f){f.s.velocity[1].object+=8;});
    rejectPost([](auto& f){f.e.texture[1].namePointer+=8;});
    rejectPost([](auto& f){f.s.frame+=1;});
    rejectPost([](auto& f){f.e.rt.renderer+=8;});
    Fixture changed=before;changed.e.view[1].rtvKey+=1;
    assert(!rm::SameResizePrerequisites(before.s,before.e,changed.s,changed.e));
    changed=before;changed.e.texture[1].clearBits[0]=1;
    assert(!rm::SameResizePrerequisites(before.s,before.e,changed.s,changed.e));
    std::puts("D85R5R4 velocity resize: valid transition, strict admission, ownership and negative cases PASS");
}
