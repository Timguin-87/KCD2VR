// Included inside kcd2vr::physical after the existing checked sweep contracts.
// A local, current-query constraint set, NOT native rigid-body physics. No body
// pointer, plane or correction survives this source sample. Existing sparse
// feature/chord sweeps and the shared18-ray/1ms budget remain the authority.
struct WallResponseH4 {bool handled=false,moved=false,backout=false,slid=false,limited=false;};
inline Vec3 wall_project_h4(Vec3 requested,const std::array<Vec3,3>& normal,unsigned count) noexcept {
    Vec3 best{};double cost=m::dot(requested,requested);
    const auto consider=[&](Vec3 v){
        if(!m::finite(v))return;
        for(unsigned i=0;i<count;++i)if(m::dot(v,normal[i])< -1e-9)return;
        const double e=m::dot(v-requested,v-requested);if(e<cost){cost=e;best=v;}
    };
    consider(requested);
    for(unsigned i=0;i<count;++i){
        consider(requested-normal[i]*m::dot(requested,normal[i]));
        for(unsigned j=0;j<i;++j)if(const auto crease=m::unit(m::cross(normal[i],normal[j])))
            consider(*crease*m::dot(requested,*crease));
    }
    return best;
}
template<class Sweep>
WallResponseH4 checked_wall_response_h4(const Input& in,const Profile& p,const Contact103& initial,
        Pose requested,Pose& final,CheckedPath60& path,unsigned proposals,Sweep&& sweep){
    WallResponseH4 response{};
    if(!in.physicalCombat177||!in.controllerAnchor119||!in.contact||in.contactFaultH30||in.contactSuspended59c1||
       p.side<0||p.side>1||!p.melee()||!p.geometry||!initial.known||initial.limited115||!initial.plane||
       !wall_spring_world_h31(initial)||!finite(requested)||!finite(final)||!path.valid)return response;
    const double scale=in.haveWorld?in.worldScale:1.;
    if(!std::isfinite(scale)||scale<.1||scale>10)return response;
    response.handled=true;
    std::array<Vec3,3> normals{};unsigned count=1;normals[0]=initial.normal;
    // At most two extra proposals, each accepted only along its actual checked
    // path. Decouple translation from rotation so an inward wrist turn cannot
    // veto a valid pull or tangent. A second current wall contributes a crease.
    for(unsigned attempt=0;attempt<std::min(proposals,2u)&&path.count<path.point.size();++attempt){
        const Vec3 grip=point(final,p.grip[p.side].p);
        const Vec3 remaining=point(requested,p.grip[p.side].p)-grip;
        Vec3 translation=wall_project_h4(remaining,normals,count);
        if(in.wallSlide58){
            const double friction=std::isfinite(in.wallFriction57)?std::clamp(in.wallFriction57,0.,.95):0.;
            // Optional scrape friction cannot damp the checked outward part.
            const double outward=std::max(0.,m::dot(translation,normals[0]));
            translation=normals[0]*outward+(translation-normals[0]*outward)*(1.-friction);
            translation=wall_project_h4(translation,normals,count);
        }
        Pose candidate=final;const bool translated=m::length(translation)*scale>.0001;
        if(translated)candidate.p=candidate.p+translation;
        else {
            if(m::angle(final.q,requested.q)<1e-6)break;
            candidate.q=requested.q;candidate.p=grip-m::rotate(candidate.q,p.grip[p.side].p);
        }
        if(!finite(candidate)||m::length(point(candidate,p.grip[p.side].p)-in.shoulders[p.side])>in.reach[p.side]+.001)break;
        const Pose before=final;Pose assisted{};
        const bool outward=translated&&m::dot(translation,normals[0])>.001/scale;
        const bool spring=attempt==0&&checked_wall_spring_h31(in,p,initial,final,requested,translation,outward,assisted,sweep);
        const auto checked=spring?Contact103{}:contact103(sweep(p,final,candidate,false));
        if(spring)candidate=assisted;
        if(!checked.known)break;
        if(checked.fraction>0){
            final=blend_weapon116(p,final,candidate,checked.fraction);path.append(final);
            const Vec3 actual=point(final,p.grip[p.side].p)-point(before,p.grip[p.side].p);
            response.moved=response.moved||m::length(actual)*scale>.0001||m::angle(final.q,before.q)>1e-6;
            response.backout=response.backout||m::dot(actual,normals[0])*scale>.0001;
            response.slid=response.slid||m::length(actual-normals[0]*m::dot(actual,normals[0]))*scale>.0001;
        }
        if(checked.limited115){response.limited=true;break;}
        if(checked.fraction<1.-1e-9){
            if(!checked.plane||!wall_spring_world_h31(checked)||count==normals.size())break;
            bool duplicate=false;for(unsigned i=0;i<count;++i)duplicate=duplicate||m::dot(normals[i],checked.normal)>.9999;
            if(duplicate)break;
            normals[count++]=checked.normal;
        }else if(!translated||spring||in.wallBackoutSpringH31)break;
        // After clear translation, the second proposal can rotate at the now
        // checked grip. It cannot inherit an untested requested orientation.
    }
    return response;
}
