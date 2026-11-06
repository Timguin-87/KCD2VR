// Executes the actual station-to-pad adapter with qualified native tool leaves.
// Reuses the D82 harness; no native hit/quality/charge is simulated.
#define KCD2VR_SMITH_REHEAT_FIXTURE_ONLY_D83R1
#include "smith_reheat_d82_test.cpp"
#include <cstring>
#include <type_traits>

static void ownRelease(Driver& d){
    d.step(11,1.10f);d.step(11,1.10f);d.step(11,1.10f);
    ck(d.step(11,1.136f).strokeStarted,"deliberate lift starts one native request");
    ck(d.step(14,1.155f).requestRT,"native phase14 acknowledges the raise");
    const auto out=d.step(14,1.130f);
    ck(out.strokeReleased&&!out.requestRT,"measured reversal releases our acknowledged raise");
}

static void phase15Continuation(){
    for(unsigned dt:{8u,11u,16u,20u}){
        Driver d(dt);ownRelease(d);
        d.step(16,1.110f);d.step(16,1.130f);
        const auto passive=d.step(15,1.150f);
        ck(!passive.requestRT&&!passive.strokeStarted&&!passive.axesOwned,
           "passive phase15 cannot request a hammer press or workpiece axes");
        ck(passive.clock.fallD81.request==0,"phase15 does not acquire native fall acceleration");
        const auto next=d.step(11,1.170f);
        ck(next.requestRT&&next.strokeStarted,"phase15 preserves the current owned recovery lift");
        ck(!d.step(14,1.190f).strokeStarted,"continued raise does not replay its press edge");
    }
    for(bool recovery:{false,true}){
        Driver d; k::crafting_d67::setSmithRecovery(recovery);ownRelease(d);
        d.step(16,1.110f);d.step(15,1.105f);
        ck(!d.step(11,1.110f).requestRT,"passive completion with no recovery lift does not auto-press");
        ck(d.step(11,1.150f).strokeStarted,"fresh lift after passive completion needs no stillness pause");
    }
    Driver fresh;fresh.step(15,1.10f);
    for(int i=1;i<6;++i)ck(!fresh.step(11,1.1f+.015f*i).requestRT,
        "entering phase15 without our acknowledged cycle cannot manufacture recovery");
    Driver manual;manual.step(15,1.1f,.8f);
    ck(manual.pad.rt==.8f&&!manual.state.lastSmith.requestRT,"manual RT in phase15 remains native");
}

static void fallDiscontinuities(){
    for(int phase:{15,16,17}){
        Driver d;ownRelease(d);d.step(16,1.110f);
        const auto jump=d.step(phase,2.110f);
        ck(!jump.requestRT&&!jump.strokeStarted&&jump.reason==k::semi_smith46::Reason::Discontinuity,
           "spatial fall discontinuity rebases without work or a press");
        ck(!d.step(11,2.120f).requestRT,"jump itself cannot supply the next lift");
        ck(d.step(11,2.160f).strokeStarted,"fresh lift can follow our observed fall after spatial rebase");
    }
    // Claude D83 preserved Fell even across clock regressions/long gaps. These
    // are ownership/freshness boundaries, not just a harmless spatial rebase.
    for(int fault=0;fault<8;++fault){
        Driver d;ownRelease(d);d.step(16,1.110f);
        switch(fault){
        case 0:d.frame.tick+=81;d.frame.timeNs+=81000000;break;
        case 1:d.frame.sequence-=2;break;
        case 2:d.frame.timeNs-=50000000;break;
        case 3:++d.frame.poseEpoch;break;
        case 4:++d.frame.sourceEpoch;break;
        case 5:++d.context.lifetime;break;
        case 6:d.hammer=false;break;
        case 7:k::crafting_d67::setSmithRecovery(false);break;
        }
        const auto out=d.step(16,2.110f);
        ck(!out.requestRT&&!out.strokeStarted,"invalid fall continuation never sends an input");
        d.hammer=true;d.step(11,2.120f);
        ck(!d.step(11,2.160f).requestRT,"clock, epoch, tool loss or disabled recovery cannot preserve fall rearming");
    }
}

static void heldLtMovement(){
    k::crafting_d67::setSmithSemiAnchors(true);
    Driver d;d.frame.hand[1].trigger=1.f;d.step(11,1.1f);d.step(11,1.1f);
    d.frame.hand[1].position[2]-=.010f;
    d.frame.hand[1].position[0]+=.010f;
    const auto out=d.step(11,1.1f);
    ck(out.axesOwned&&std::fabs(d.pad.ly)>.9f,"held LT keeps actual native fore/aft workpiece input");
    ck(d.pad.lx==0.f,"held LT does not add unsupported lateral hand input");
    ck(d.pad.lt==0.f,"placement LT cannot also trigger a native LT action");
    ck(!d.step(15,1.1f).axesOwned,"passive phase15 does not synthesize workpiece movement");
    d.frame.hand[1].trigger=0.f;d.step(11,1.1f);
    ck(d.pad.ly==0.f,"LT release cannot replay movement consumed while held");
    ck(d.step(11,1.1f).y==0.f,"stationary hand cannot repeat a prior held-LT axis");
    d.frame.hand[1].position[2]+=.01f;d.step(11,1.1f);
    ck(std::fabs(d.pad.ly)>.9f,"fresh motion after release still reaches the native axis");
}

// D83R2 smith: lateral hand input reaches native lx only for a workpiece
// whose native map has a second axis (axe/horseshoe), only while LT is held.
static void publishMapD83R2(std::uintptr_t instance,std::uint64_t lifetime,float x,float y,bool valid=true){
    namespace b=k::subsystems::smithing::bridge67;b::WorkpieceMapD83R2 m{};
    m.instance=instance;m.lifetime=lifetime;m.model=40;m.size[0]=x;m.size[1]=y;m.valid=valid;
    m.refusal=valid?"NONE":"WORKPIECE_HOLDER_CLASS";b::workpieceMapD83R2.publish(m);
}
static float sidewaysLx(Driver& d,float dx){
    d.frame.hand[1].position[0]+=dx;d.step(11,1.1f);return d.pad.lx;
}
static void heldLtSidewaysD83R2(){
    k::crafting_d67::setSmithSemiAnchors(true);
    {Driver d;d.frame.hand[1].trigger=1.f;d.step(11,1.1f);d.step(11,1.1f);
        publishMapD83R2(d.context.smith,d.context.lifetime,.18f,.30f);
        ck(sidewaysLx(d,.010f)>.9f,"axe map: held LT feeds rightward hand to native lx");
        ck(sidewaysLx(d,-.010f)<-.9f,"axe map: leftward hand feeds negative native lx");
        d.frame.hand[1].position[2]-=.010f;d.step(11,1.1f);
        ck(std::fabs(d.pad.ly)>.9f&&std::fabs(d.pad.lx)<.05f,"axe map: pure fore/aft stays on ly");
        ck(sidewaysLx(d,0.f)==0.f,"axe map: stationary hand sends no lx");
        d.frame.hand[1].trigger=0.f;
        ck(sidewaysLx(d,.010f)==0.f,"axe map: LT released keeps the old fore/aft-only policy");
    }
    {Driver d;d.frame.hand[1].trigger=1.f;d.step(11,1.1f);d.step(11,1.1f);
        publishMapD83R2(d.context.smith,d.context.lifetime,.20f,.20f);
        ck(sidewaysLx(d,.010f)>.9f,"horseshoe map: held LT feeds native lx");
    }
    {Driver d;d.frame.hand[1].trigger=1.f;d.step(11,1.1f);d.step(11,1.1f);
        publishMapD83R2(d.context.smith,d.context.lifetime,.70f,0.f);
        ck(sidewaysLx(d,.010f)==0.f,"blade map (y=0): held LT never feeds lx");
        d.frame.hand[1].position[2]-=.010f;d.step(11,1.1f);
        ck(std::fabs(d.pad.ly)>.9f,"blade map: fore/aft unchanged");
    }
    {Driver d;d.frame.hand[1].trigger=1.f;d.step(11,1.1f);d.step(11,1.1f);
        publishMapD83R2(d.context.smith,d.context.lifetime+1,.18f,.30f);
        ck(sidewaysLx(d,.010f)==0.f,"map from another minigame lifetime is not used");
        publishMapD83R2(d.context.smith+8,d.context.lifetime,.18f,.30f);
        ck(sidewaysLx(d,.010f)==0.f,"map from another station instance is not used");
        publishMapD83R2(d.context.smith,d.context.lifetime,.18f,.30f,false);
        ck(sidewaysLx(d,.010f)==0.f,"refused map read keeps current behaviour");
    }
    // The production reader against copied native leaves.
    namespace b=k::subsystems::smithing::bridge67;namespace st=k::smith_tool36;
    const std::uintptr_t base=0x180000000ull,model=0x1000,holder=0x2000,def=0x3000;
    struct Mem {std::uintptr_t modelVft,holderPtr,holderVft,defPtr;float x,y;} mem{base+st::kModelVft,holder,base+st::kWorkpieceVft,def,.18f,.30f};
    const auto rd=[&](std::uintptr_t at,auto& v)->bool{
        using T=std::remove_reference_t<decltype(v)>;
        if constexpr(std::is_same_v<T,float>){if(at==def+0x40){v=mem.x;return true;}if(at==def+0x44){v=mem.y;return true;}return false;}
        else{if(at==model){v=T(mem.modelVft);return true;}if(at==model+st::kWorkpieceSlot){v=T(mem.holderPtr);return true;}
            if(at==holder){v=T(mem.holderVft);return true;}if(at==holder+0x18){v=T(mem.defPtr);return true;}return false;}
    };
    st::Binding bind{};bind.instance=0x100;bind.holder=1;bind.entity=1;bind.side=1;bind.tool=1;bind.lifetime=7;bind.model=model;
    const char* why=nullptr;b::WorkpieceMapD83R2 m{};
    ck(b::readWorkpieceMapD83R2(rd,base,bind,5,m)&&b::sidewaysD83R2(m,0x100,7,why)==1,"reader: axe record has a sideways axis");
    mem.x=.7f;mem.y=0.f;
    ck(b::readWorkpieceMapD83R2(rd,base,bind,5,m)&&b::sidewaysD83R2(m,0x100,7,why)==0,"reader: sword record is fore/aft only");
    mem.holderVft=base+st::kSlotToolVft;
    ck(!b::readWorkpieceMapD83R2(rd,base,bind,5,m)&&b::sidewaysD83R2(m,0x100,7,why)<0&&std::strcmp(why,"WORKPIECE_HOLDER_CLASS")==0,
       "reader: wrong holder class is unknown, with its reason");
    mem.holderVft=base+st::kWorkpieceVft;mem.y=std::numeric_limits<float>::quiet_NaN();
    ck(!b::readWorkpieceMapD83R2(rd,base,bind,5,m),"reader: non-finite map is refused");
    mem.y=.3f;mem.modelVft=0;
    ck(!b::readWorkpieceMapD83R2(rd,base,bind,5,m),"reader: wrong model class is refused");
    b::workpieceMapD83R2.publish({});
}

int main(int argc,char** argv){
    const auto only=[&](const char* name){return argc<2||std::strcmp(argv[1],name)==0;};
    if(only("reheat")){movingReturn();noPassiveOrForeignPress();boundaries();}
    if(only("phase15"))phase15Continuation();
    if(only("fall"))fallDiscontinuities();
    if(only("lt"))heldLtMovement();
    if(only("sideways"))heldLtSidewaysD83R2();
    std::printf("D83R1 smith station transitions: %u checks PASS; actual pad/controller, native outcome NOT_RUN\n",checks);
}
