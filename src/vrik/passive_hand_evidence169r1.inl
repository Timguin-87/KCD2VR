// Included after CoherentTransaction6689. Inspect final composed joints only.
// The torso and other-palm are explicitly the EXISTING approximations, not mesh
// or NPC colliders. No extra world query, bone write, damage or body impulse.
static void BuildPassiveHandEvidence169R1(CoherentTransaction6689& tx,bool collision,
        const bool (&freeHand)[2],const V3 (&requested)[2],const HandContact6695 (&contact)[2]) noexcept {
    namespace p=kcd2vr::passive169r1;auto& s=g_pose6689;
    ModelWorld6692 world{};const bool haveWorld=collision&&ReadModelWorld6692(s.character,world);
    tx.passiveWorldValid169r1=haveWorld;tx.passiveWorld169r1=world;
    pc::Vec3 points[2][7]{};bool geometry[2]{};int pointCountH31[2]{6,6};
    for(int side=0;side<2;++side){
        if(!haveWorld||!freeHand[side]||!s.fingerMap[side])continue;
        bool valid=true;pc::Vec3 center{};
        for(int finger=0;finger<5;++finger){const auto& chain=s.fingers[side][finger];
            if(chain.count<1||chain.count>int(chain.id.size())){valid=false;break;}
            const int root=chain.id[0],tip=chain.id[chain.count-1];
            if(root<0||tip<0||root>=int(s.count)||tip>=int(s.count)){valid=false;break;}
            const auto base=PV(FromT(s.work[root]));points[side][finger+1]=PV(FromT(s.work[tip]));
            if(!pc::finite(base)||!pc::finite(points[side][finger+1])){valid=false;break;}
            if(finger)center=center+base*.25;
        }
        points[side][0]=center;geometry[side]=valid;
        // The collision pass also tests feature6: the closed-fist knuckle bar.
        // Use its final composed proximal endpoints, without changing fingers.
        if(valid&&contact[side].featureCount==7){pc::Vec3 knuckle{};bool knuckleValid=true;
            for(int finger=1;finger<5;++finger){const auto& chain=s.fingers[side][finger];
                const int id=chain.count>=2?chain.id[1]:-1;
                if(id<0||id>=int(s.count)){knuckleValid=false;break;}
                const auto p=PV(FromT(s.work[id]));if(!pc::finite(p)){knuckleValid=false;break;}knuckle=knuckle+p*.25;}
            if(knuckleValid){points[side][6]=knuckle;pointCountH31[side]=7;}
        }
    }
    const auto worldPoint=[&](pc::Vec3 x){return PV(ModelToWorld6692(world,VP(x)));};
    const auto jointValid=[&](int id){return id>=0&&id<int(s.count);};
    const bool torsoValid=haveWorld&&jointValid(s.rig.pelvis)&&jointValid(s.rig.spine3)&&
        jointValid(s.rig.upper_r)&&jointValid(s.rig.upper_l);
    pc::Vec3 torsoA{},torsoB{};double torsoRadius=0;
    if(torsoValid){torsoA=PV(FromT(s.work[s.rig.pelvis]));torsoB=PV(FromT(s.work[s.rig.spine3]));
        torsoRadius=std::clamp(double(Len(Sub(FromT(s.work[s.rig.upper_r]),FromT(s.work[s.rig.upper_l]))))*.32,.08,.16);}
    for(int side=0;side<2;++side)for(unsigned kind=0;kind<3;++kind){
        auto& x=tx.passive169r1[kind*2+side];x={};x.source=p::Source(kind+1);x.side=side;
        x.actor=reinterpret_cast<std::uintptr_t>(s.character);x.key=0x48414e4400000000ull+side;
        x.epoch=tx.epoch;x.request=g_contact_control_epoch6695.load();x.sequence=tx.tracking.sequence;x.tick=tx.tracking.tick;
        x.eligible=collision&&freeHand[side]&&tx.tracking.allowed&&tx.tracking.head_valid&&!tx.tracking.head_held&&
            (side?tx.tracking.left_valid&&!tx.tracking.left_held:tx.tracking.right_valid&&!tx.tracking.right_held);
        x.worldCoherent=haveWorld;x.flags=freeHand[side]?1u:0u;
        x.status=x.eligible?p::Status::Unknown:p::Status::Unavailable;
        if(!x.eligible||!geometry[side]||!jointValid(side?s.rig.hand_l:s.rig.hand_r))continue;
        const auto wrist=PV(FromT(s.work[side?s.rig.hand_l:s.rig.hand_r]));
        if(kind==0){
            const auto& c=contact[side];x.status=c.fresh169r1&&c.have?p::Status::Clear:p::Status::Unknown;
            x.feature=0;x.allowed=worldPoint(points[side][0]);x.requested=worldPoint(PV(requested[side])+points[side][0]-wrist);
            unsigned fingerMask=0;for(int f=0;f<5;++f)if(c.fingerContact[f])fingerMask|=1u<<f;
            x.flags|=fingerMask<<8;
            if(!c.fresh169r1||!c.have||!c.blocked)continue;
            double minGap=1e6;int chosen=-1,feature=0;
            for(int j=0;j<c.planeCount&&j<7;++j){const auto& plane=c.planes[j];
                for(int f=0;f<pointCountH31[side];++f)if(plane.feature<0||plane.feature==f){
                    const double gap=pc::dot(worldPoint(points[side][f])-PV(plane.point),PV(plane.normal));
                    if(gap<minGap){minGap=gap;chosen=j;feature=f;}
                }
            }
            if(chosen<0){x.status=p::Status::Unknown;continue;}
            x.feature=feature;const auto& plane=c.planes[chosen];x.gap=minGap;x.point=PV(plane.point);x.normal=PV(plane.normal);x.witness=plane.witness169r1;
            // Copy the exact synchronous ray witness. Publication and measured
            // motion happen only AFTER this transaction commits successfully.
            auto& target=tx.passiveTargetsH21[side];target.contact=plane.native172;tx.passiveQueryTickR11[side]=c.lastCompleteTickH18;
            if(target.contact.body==kcd2vr::npc_contact172::Body::Rigid&&target.contact.collider){
                std::uint32_t packed=0;std::uintptr_t bodyWorld=0,foreign=0;short kind=-1;
                auto* body=reinterpret_cast<unsigned char*>(target.contact.collider);
                if(Read(body+0x44,packed)&&Read(body+0x68,bodyWorld)&&Read(body+0x20,foreign)&&
                   foreign==target.contact.owner&&Read(body+0x28,kind)&&kind==2){
                    target.physicsId=static_cast<std::int32_t>(packed<<9)>>9;target.world=bodyWorld;
                }
            }
            x.allowed=worldPoint(points[side][feature]);x.requested=worldPoint(PV(requested[side])+points[side][feature]-wrist);
            x.status=std::isfinite(minGap)&&minGap>=-.004*world.scale&&minGap<=.025*world.scale?p::Status::Contact:p::Status::Unknown;
            if(x.status==p::Status::Unknown)x.flags|=0x10000u; // final point does not certify this retained plane
        }else{
            if((kind==1&&!torsoValid)||(kind==2&&!geometry[1-side]))continue;
            const pc::Vec3 a=kind==1?torsoA:points[1-side][0],b=kind==1?torsoB:a;
            const double radius=kind==1?torsoRadius:.035;const auto axis=b-a;const double ll=pc::dot(axis,axis);
            double minGap=1e6;pc::Vec3 closest{},feature{},normal{};int chosen=-1;
            for(int f=0;f<6;++f){const auto q=points[side][f];
                const double t=ll>1e-12?std::clamp(pc::dot(q-a,axis)/ll,0.0,1.0):0;
                const auto c=a+axis*t;const auto direction=pc::unit(q-c);const double gap=pc::length(q-c)-radius;
                if(direction&&gap<minGap){chosen=f;minGap=gap;closest=c+*direction*radius;feature=q;normal=*direction;}
            }
            if(!std::isfinite(minGap)||minGap==1e6)continue;
            x.feature=chosen;x.gap=minGap*world.scale;x.point=worldPoint(closest);x.normal=normal;
            // Model-relative deltas prevent locomotion creating self-rub energy.
            x.allowed=(feature-a)*world.scale;x.requested=(PV(requested[side])+feature-wrist-a)*world.scale;
            x.status=minGap<-.004?p::Status::Unknown:minGap<=.016?p::Status::Contact:p::Status::Clear;
            if(x.status==p::Status::Unknown)x.flags|=0x10000u;
        }
    }
}
