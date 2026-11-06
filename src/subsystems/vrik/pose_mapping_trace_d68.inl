// Mapping closure is copied at the existing solve, after shared-anchor selection.
// No recalculation replaces a goal and no native field is fetched for this row.
if(tx.motionSessionD68)for(int sideD68=0;sideD68<2;++sideD68){
    namespace md=kcd2vr::diagnostics::motion_d68;md::Event e{};
    if(!MotionEventD68(tx,e,md::Mapping))break;
    e.side=sideD68;e.flags=(valid[sideD68]?1u:0u)|((sideD68?ts.left_held:ts.right_held)?2u:0u)|
        (smoothEnabledH50?4u:0u)|(stagedH50[sideD68].ready?8u:0u)|(full?64u:0u)|(tx.frame51.valid?128u:0u);
    unsigned n=0;auto v=[&](V3 x){e.v[n++]=x.x;e.v[n++]=x.y;e.v[n++]=x.z;};
    auto q=[&](Q4 x){e.v[n++]=x.x;e.v[n++]=x.y;e.v[n++]=x.z;e.v[n++]=x.w;};
    q(frame);v(trackedBase);v(headAnchor123);v(sideD68?ts.left:ts.right);v(visPH50[sideD68]);
    v(requestedGoalsRawH50[sideD68]);v(requestedGoals[sideD68]);
    const V3 manualD68=QRotateV(frame,XrToCryV(Sub(sideD68?leftXr:rightXr,visPH50[sideD68])));
    v(manualD68);v(palmOffsetD68[sideD68]);q(sideD68?ts.lq:ts.rq);q(visQH50[sideD68]);
    q(frameWorld51.q);v(frameWorld51.t);e.v[n++]=frameWorld51.scale;v(rawPalmOffsetD68[sideD68]);e.v[n++]=wristBack59c7f;
    e.count=n;(void)md::push(e);
}
