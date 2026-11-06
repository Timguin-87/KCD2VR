// Compiled once inside native_physics_phase_t4. The actual articulated table
// has different GetStatus/Action entries; never send it through the rigid API.
struct ArticulatedAccess {
    static constexpr std::uintptr_t table=0x3A48B30;
    static bool Pins() noexcept {
        constexpr unsigned char status[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x18};
        constexpr unsigned char action[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x50,0x10};
        constexpr unsigned char addBranch[]{0x0F,0x84,0xC5,0x5C,0x7D,0x01};
        constexpr unsigned char addDelegate[]{0xE8,0xE9,0x11,0x85,0xFE};
        constexpr unsigned char removeDelegate[]{0xE8,0x9B,0x63,0x02,0x00};
        constexpr unsigned char statusDelegate[]{0xE8,0xCB,0xCF,0xFF,0xFF};
        const auto match=[](std::uintptr_t rva,const auto& expected) noexcept {
            std::array<unsigned char,sizeof(expected)> actual{};
            return Copy(reinterpret_cast<const void*>(base+rva),actual.data(),actual.size())&&
                !std::memcmp(actual.data(),expected,actual.size());
        };
        return api.articulatedStatus&&api.articulatedAction&&
            SlotIs(table,8,0x1A72E50)&&SlotIs(table,0x10,0x5311C4)&&SlotIs(table,0x18,0x531F84)&&
            SlotIs(table,0x30,0x8A9348)&&SlotIs(table,0x38,0xA99DF0)&&SlotIs(table,0x50,0x5D6F44)&&
            match(0x8A9348,status)&&match(0xA99DF0,action)&&match(0xA99E5F,addBranch)&&
            match(0x226FB2A,addDelegate)&&match(0xA9A978,removeDelegate)&&match(0x8A9CC0,statusDelegate);
    }
    static bool Body(Context& c,void* body) noexcept {
        std::uintptr_t vt=0;void* world=nullptr;int deleting=-1;unsigned flags=0;std::int8_t sim=-1;
        return !nativeFaulted.load()&&c.Current()&&Pins()&&body&&Read(body,0,vt)&&vt==base+table&&
            Read(body,0x68,world)&&world==c.World()&&Read(body,0x50,deleting)&&deleting==0&&
            Read(body,0x84,flags)&&(flags&0x80000000u)==0&&Read(body,0x47,sim)&&sim>=0&&sim<=4;
    }
    static bool Owner(Context& c,const ArticulatedSource& s) noexcept {
        void* foreign=nullptr;std::int16_t type=-1;
        const auto before=faults.load();
        return !nativeFaulted.load()&&c.Current()&&s.physicsId>=0&&s.part>=0&&s.body&&s.foreign&&s.foreignType>=0&&s.foreignType<=32767&&
            Call(api.lookup,c.World(),s.physicsId)==s.body&&faults.load()==before&&Body(c,s.body)&&
            Read(s.body,0x20,foreign)&&foreign==s.foreign&&Read(s.body,0x28,type)&&type==s.foreignType;
    }
    static void* Handle(Context& c,ArticulatedLease& l,bool owner) noexcept {
        auto& held=l.retained_;
        if(!Access::Same(c,held)||ObserveRegistry(c,held)!=Registry::Same||!Body(c,held.body_)||
           (owner&&!Owner(c,l.source_)))return nullptr;
        return held.body_;
    }
    static bool Part(Context& c,const ArticulatedSource& s,ArticulatedSnapshot& out) noexcept {
        out={};if(!Owner(c,s))return false;
        struct Pos {
            int type=1,part=-1,index=-1;unsigned flags=0,orFlags=0,andFlags=0;
            V3 p{},bounds[2]{};Q q{};float scale=0;int sim=0;
            void* matrix33=nullptr;void* matrix34=nullptr;void* geometry=nullptr;void* proxy=nullptr;float timeBack=0;
        } p;
        static_assert(sizeof(Pos)==0x80&&offsetof(Pos,p)==0x18&&offsetof(Pos,q)==0x3C&&offsetof(Pos,scale)==0x4C&&offsetof(Pos,timeBack)==0x78);
        p.part=s.part;const float nan=std::numeric_limits<float>::quiet_NaN();p.p={nan,nan,nan};p.q={nan,nan,nan,nan};
        const auto before=faults.load();const int result=Call(api.articulatedStatus,s.body,&p);
        if(result<=0||faults.load()!=before||!Owner(c,s)||p.type!=1||p.part!=s.part||p.index!=-1||
           !finite(p.p)||!finite(p.q)||!std::isfinite(p.scale)||p.scale<.01f||p.scale>10.f)return false;
        const double norm=double(p.q.x)*p.q.x+double(p.q.y)*p.q.y+double(p.q.z)*p.q.z+double(p.q.w)*p.q.w;
        if(norm<.99||norm>1.01)return false;
        out={p.p,p.q,p.scale,p.part,p.sim};return true;
    }
    static bool Acquire(Context& c,ArticulatedSource source,ArticulatedLease& l) noexcept {
        if(l.Held()||!c.Current()||c.Kind()!=Phase::Step||source.part<0||!source.body||!source.foreign||
           source.foreignType<0||source.foreignType>32767||!IndependentReference(c.World()))return false;
        if(source.physicsId==-1){
            if(!source.body||!Body(c,source.body))return false;
            const auto before=faults.load();source.physicsId=Call(api.id,c.World(),source.body);
            if(source.physicsId<0||faults.load()!=before)return false;
        }
        ArticulatedSnapshot part{};if(!Part(c,source,part))return false;
        const auto before=faults.load();const int result=Call(api.addRef,source.body);
        if(result<=0||faults.load()!=before)return false;
        auto& held=l.retained_;held.body_=source.body;held.world_=c.World();held.generation_=c.Generation();
        held.id_=source.physicsId;held.held_=true;held.pending_=false;l.source_=source;acquisitions.fetch_add(1);
        // AddRef is a real obligation even if its readback was replaced. Do not
        // silently forget it or claim the acquisition is usable in that case.
        return Owner(c,source);
    }
    static Registry RegistryOf(Context& c,const ArticulatedLease& l) noexcept {return ObserveRegistry(c,l.retained_);}
    static bool ReadPart(Context& c,ArticulatedLease& l,ArticulatedSnapshot& out) noexcept {
        out={};return Handle(c,l,true)&&Part(c,l.source_,out);
    }
    static bool ReleaseOwner(Context& c,ArticulatedLease& l) noexcept {
        if(!Release(c,l.retained_))return false;l.source_={};return true;
    }
    static bool Abandon(Context& c,ArticulatedLease& l) noexcept {
        if(!AbandonInvalidated(c,l.retained_))return false;l.source_={};return true;
    }
    static void* RetainedBuddy(Context& c,const Lease& buddy) noexcept {
        // Opaque comparison only: an independently retained buddy can leave the
        // registry during cleanup. Never dereference or drive that old pointer.
        return Access::Same(c,buddy)?buddy.body_:nullptr;
    }
};
bool AcquireArticulated(Context& c,const ArticulatedSource& source,ArticulatedLease& out) noexcept {return ArticulatedAccess::Acquire(c,source,out);}
Registry ObserveRegistry(Context& c,const ArticulatedLease& l) noexcept {return ArticulatedAccess::RegistryOf(c,l);}
bool Release(Context& c,ArticulatedLease& l) noexcept {return ArticulatedAccess::ReleaseOwner(c,l);}
bool AbandonInvalidated(Context& c,ArticulatedLease& l) noexcept {return ArticulatedAccess::Abandon(c,l);}
bool ReadArticulatedPart(Context& c,ArticulatedLease& l,ArticulatedSnapshot& out) noexcept {
    // Source was captured by Acquire, never changed to a successor part/body.
    return ArticulatedAccess::ReadPart(c,l,out);
}
bool ReadArticulatedConstraint(Context& c,ArticulatedLease& l,int id,ConstraintSnapshot& out) noexcept {
    out={};void* body=ArticulatedAccess::Handle(c,l,false);if(!body||id<0)return false;
    struct Status {int type=26,id=0,index=-1;unsigned flags=0;V3 p[3]{};unsigned pad=0;void* buddy=nullptr;void* user=nullptr;} status;
    static_assert(sizeof(Status)==0x48&&offsetof(Status,buddy)==0x38&&offsetof(Status,user)==0x40);
    status.id=id;const auto before=faults.load();const int result=Call(api.articulatedStatus,body,&status);
    if(result<0||faults.load()!=before||!ArticulatedAccess::Handle(c,l,false)||status.type!=26||status.id!=id||status.index!=-1)return false;
    out={result>0,status.flags,result>0?status.buddy:nullptr};return true;
}
ConstraintResult AddArticulatedConstraint(Context& c,ArticulatedLease& source,Lease& buddy,const constraint_packet60::Packet& packet) noexcept {
    ConstraintResult out{};if(c.Kind()!=Phase::Step)return out;
    ArticulatedSnapshot part{};RigidSnapshot rigid{};
    if(!ReadArticulatedPart(c,source,part)||!ReadRigid(c,buddy,rigid))return out;
    void* src=ArticulatedAccess::Handle(c,source,true);void* dst=NativeHandle(c,buddy);
    if(!src||!dst||src==dst||constraint_packet60::get<std::uint32_t>(packet,0)!=5||
       constraint_packet60::get<std::uint32_t>(packet,4)!=constraint_packet60::kIntUnset||
       constraint_packet60::get<std::uintptr_t>(packet,8)!=reinterpret_cast<std::uintptr_t>(dst)||
       constraint_packet60::get<int>(packet,0x28)!=source.Part()||(constraint_packet60::get<std::uint32_t>(packet,0x60)&0x100u))return out;
    auto local=packet;const auto before=faults.load();const int result=Call(api.articulatedAction,src,&local,1);out.nativeReturn=result;
    if(faults.load()!=before){out.kind=ConstraintResult::Kind::Uncertain;return out;}
    if(result>0){out.kind=ConstraintResult::Kind::Submitted;out.id=std::uint32_t(result);}return out;
}
ConstraintResult RemoveArticulatedConstraint(Context& c,ArticulatedLease& source,Lease& buddy,std::uint32_t id) noexcept {
    ConstraintResult out{};if((c.Kind()!=Phase::Step&&c.Kind()!=Phase::Shutdown)||!id||id>=0x80000000u)return out;
    void* expected=ArticulatedAccess::RetainedBuddy(c,buddy);ConstraintSnapshot status{};
    if(!expected||!ReadArticulatedConstraint(c,source,int(id),status)||!status.exists||status.buddy!=expected)return out;
    void* src=ArticulatedAccess::Handle(c,source,false);if(!src)return out;
    auto packet=constraint_packet60::removeDefaults();constraint_packet60::setRemoveId(packet,id);
    const auto before=faults.load();const int result=Call(api.articulatedAction,src,&packet,1);out.nativeReturn=result;
    if(faults.load()!=before){out.kind=ConstraintResult::Kind::Uncertain;return out;}
    if(result>0)out.kind=ConstraintResult::Kind::Submitted;return out;
}
